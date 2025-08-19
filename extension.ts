import * as vscode from 'vscode';

// Define a type for our routing rules from the settings.json
type CustomRoutingRule = {
    priority: number;
    languageId?: string;
    promptPattern?: string;
    selectedTextPattern?: string;
    modelName: string;
    reason: string;
};

// Global state to track a manual model override
let manualOverrideModel: string | null = null;
let overrideReason: string | null = null;

// Function to get rich context from the active editor
function getWorkspaceContext(): { languageId?: string; selectedText?: string; } {
    const activeEditor = vscode.window.activeTextEditor;
    if (!activeEditor) {
        return {};
    }
    const languageId = activeEditor.document.languageId;
    const selectedText = activeEditor.document.getText(activeEditor.selection);
    return { languageId, selectedText };
}

// The main activation function for our extension
export function activate(context: vscode.ExtensionContext) {

    // Register our command to force GPT-4
    context.subscriptions.push(vscode.commands.registerCommand('smart-copilot-router.forceGpt4', async () => {
        manualOverrideModel = 'copilot/gpt-4-turbo';
        overrideReason = 'Manually overridden to GPT-4.';
        vscode.window.showInformationMessage('Smart Copilot Router: Forcing GPT-4 for the next chat request.');
    }));
    
    // Register our command to analyze selection with GPT-4
    context.subscriptions.push(vscode.commands.registerCommand('smart-copilot-router.analyzeWithGpt4', async () => {
        const { selectedText } = getWorkspaceContext();
        if (!selectedText) {
            vscode.window.showWarningMessage('No code selected to analyze.');
            return;
        }

        const messages: vscode.LanguageModelChatMessage[] = [
            vscode.LanguageModelChatMessage.User(
                `Analyze and provide feedback on the following code selection. Focus on potential bugs, optimization opportunities, and best practices.`
            ),
            vscode.LanguageModelChatMessage.User(`Code to analyze:\n\n\`\`\`\n${selectedText}\n\`\`\``)
        ];

        const models = await vscode.lm.selectChatModels({ family: 'copilot/gpt-4-turbo' });
        const model = models[0];

        if (!model) {
            vscode.window.showErrorMessage('GPT-4 model not available.');
            return;
        }
        
        vscode.window.withProgress({
            location: vscode.ProgressLocation.Notification,
            title: "Analyzing with GPT-4",
            cancellable: false
        }, async (progress) => {
            const chatResponse = await model.sendRequest(messages, {}, new vscode.CancellationTokenSource().token);
            let result = '';
            for await (const fragment of chatResponse.text) {
                result += fragment;
            }
            // Display the result in a new webview or output channel
            vscode.window.showInformationMessage('Analysis complete. Opening new editor.');
            const doc = await vscode.workspace.openTextDocument({
                content: result,
                language: 'markdown'
            });
            await vscode.window.showTextDocument(doc);
        });
    }));

    // Register our custom chat participant
    const participant = vscode.chat.createChatParticipant(
        'smart-copilot-router.smart-participant',
        async (request, chatContext, stream, token) => {
            console.log('Smart Copilot Router: Chat participant activated.'); // Log 1

            // 1. Check for a manual override first
            let selectedModelName = manualOverrideModel;
            let modelReason = overrideReason;
            
            // Reset the override after it's used once
            manualOverrideModel = null;
            overrideReason = null;

            // 2. If no manual override, apply the routing rules
            if (!selectedModelName) {
                const config = vscode.workspace.getConfiguration('smart-copilot-router');
                const customRules = config.get<CustomRoutingRule[]>('customRules', []);
                // Force o4-mini (Preview) as the default model for all clients
                const defaultModelName = 'copilot/o4-mini';
                const { languageId, selectedText } = getWorkspaceContext();
                const userPrompt = request.prompt;

                // Sort rules by priority (descending)
                customRules.sort((a, b) => b.priority - a.priority);

                for (const rule of customRules) {
                    let ruleMatches = true;
                    if (rule.languageId && rule.languageId !== languageId) {
                      ruleMatches = false;
                    }
                    if (rule.promptPattern && !(new RegExp(rule.promptPattern, 'i').test(userPrompt))) {
                      ruleMatches = false;
                    }
                    if (rule.selectedTextPattern && !(new RegExp(rule.selectedTextPattern, 'i').test(selectedText ?? ""))) {
                      ruleMatches = false;
                    }
                    if (ruleMatches) {
                        selectedModelName = rule.modelName;
                        modelReason = rule.reason;
                        break;
                    }
                }
                
                // Fallback to the user's configured default
                if (!selectedModelName) {
                    selectedModelName = defaultModelName;
                    modelReason = `No specific rule matched. Using the default model: ${selectedModelName}`;
                }
            }

            // 3. Get the actual language model from the API
            console.log(`Smart Copilot Router: Attempting to select model: ${selectedModelName}`); // Log 2
            const models = await vscode.lm.selectChatModels({ family: selectedModelName! });
            const selectedModel = models[0];

            if (!selectedModel) {
                stream.markdown(`Error: The model '${selectedModelName}' could not be found. Please check your settings.`);
                return;
            }

            // 4. Provide status update with response time
            const startTime = Date.now();
            console.log('Smart Copilot Router: About to stream model status message.'); // Log 3
            stream.markdown(`**Smart Copilot Router:** Using **${selectedModel.name}**. _${modelReason}_`);
            stream.markdown('\n---\n'); // Separator
            console.log('Smart Copilot Router: Model status message streamed.'); // Log 4

            // 5. Construct and send the request
            const messages: vscode.LanguageModelChatMessage[] = [];
            for (const item of chatContext.history) {
              if (item.participant === participant.id) {
                // This is a ChatResponseTurn - access the response property
                const responseTurn = item as vscode.ChatResponseTurn;
                messages.push(new vscode.LanguageModelChatMessage(vscode.LanguageModelChatMessageRole.Assistant, responseTurn.response.map(r => r.value).join('')));
              } else {
                // This is a ChatRequestTurn - access the prompt property
                const requestTurn = item as vscode.ChatRequestTurn;
                messages.push(new vscode.LanguageModelChatMessage(vscode.LanguageModelChatMessageRole.User, requestTurn.prompt));
              }
            }
            messages.push(vscode.LanguageModelChatMessage.User(request.prompt));

            const { selectedText } = getWorkspaceContext();
            if (selectedText) {
                messages.push(vscode.LanguageModelChatMessage.User(
                    `The user has also selected the following code for context:\n\n\`\`\`\n${selectedText}\n\`\`\``
                ));
            }

            console.log('Smart Copilot Router: About to send request to model.'); // Log 5
            const chatResponse = await selectedModel.sendRequest(messages, {}, token);

            // 6. Stream the response back
            for await (const fragment of chatResponse.text) {
                stream.markdown(fragment);
            }
            
            // 7. Calculate and display response time
            const endTime = Date.now();
            const responseTime = ((endTime - startTime) / 1000).toFixed(2);
            stream.markdown(`\n---\n_Response from ${selectedModel.name} in ${responseTime}s._`);
        }
    );
    participant.iconPath = vscode.Uri.joinPath(context.extensionUri, 'resources', 'smart-copilot-icon.svg');
    context.subscriptions.push(participant);
}

export function deactivate() {}