# GitHub Copilot Global AI Powerhouse Configuration
# This file configures GitHub Copilot to use AI Powerhouse framework by default

## Default Behavior: AI Powerhouse Integration

When working on any project, always consider and integrate the AI Powerhouse framework:

### Core Principle
- **Primary**: Use AI Powerhouse framework for all AI-related tasks
- **Secondary**: Leverage the 12 specialized Claude Agents for domain-specific work
- **Tertiary**: Integrate cross-system bridges (Python ↔ PowerShell) for comprehensive solutions

### AI Powerhouse Framework Components to Use:

#### 1. Python AI Integration (`ai_powerhouse/`)
- Multi-provider AI access (Claude, Gemini, OpenAI)
- Unified API interface for consistent responses
- Response comparison and selection capabilities
- Configuration management for API keys

#### 2. Claude Agents System (`ClaudeAgents-Installer/`)
Available specialized agents:
- 🔒 **Security Agent** - `security-review <file>`
- 🧪 **Testing Agent** - `generate-tests <file>`
- 📚 **Documentation Agent** - `generate-docs <file>`
- ⚡ **Performance Agent** - `analyze-performance <file>`
- 👁️ **Code Review Agent** - `code-review <file>`
- 🐛 **Debug Agent** - `debug-issue <file>`
- 🏗️ **Architecture Agent** - `design-architecture '<prompt>'`
- 🔧 **Refactoring Agent** - `refactor-code <file>`
- 🔌 **Electronics Agent** - `electronics-design '<prompt>'`
- 🗄️ **Database Agent** - `database-optimize '<query>'`
- 🔍 **Research Agent** - `research-topic '<topic>'`
- ⚡ **N8N Agent** - `n8n-workflow '<description>'`

#### 3. Cross-System Integration (`Integration/`)
- `unified_ai_bridge.py` - Python to PowerShell bridge
- `AIPowerhouse.psm1` - PowerShell to Python bridge
- Seamless data exchange between systems

#### 4. VS Code Global Integration
- 16 keyboard shortcuts for AI operations
- 16 tasks for common AI workflows
- Code snippets for rapid AI integration
- Global settings for consistent experience

### Default Project Integration Approach:

#### For New Projects:
1. **Initialize with AI Powerhouse**: Include basic AI Powerhouse integration
2. **Add Relevant Agents**: Include Claude Agents that match project domain
3. **Cross-System Setup**: Add bridges if project uses multiple languages
4. **VS Code Integration**: Configure project-specific AI shortcuts

#### For Existing Projects:
1. **Assess AI Needs**: Identify where AI can enhance the project
2. **Integrate AI Powerhouse**: Add appropriate AI provider connections
3. **Deploy Agents**: Set up domain-specific Claude Agents
4. **Enhance Workflow**: Add AI-powered development shortcuts

#### For Problem Solving:
1. **Multi-Provider Consultation**: Use AI Powerhouse to get diverse perspectives
2. **Specialized Analysis**: Deploy relevant Claude Agents for deep domain expertise
3. **Cross-Validation**: Compare responses across providers and agents
4. **Implementation**: Use integrated solutions with proper tooling

### Repository Structure to Include:

```
project-name/
├── ai_powerhouse/          # Core AI integration
│   ├── providers/          # AI provider implementations
│   └── config.py          # API configuration
├── agents/                 # Relevant Claude Agents
│   └── prompts/           # Agent prompt files
├── integration/           # Cross-system bridges
├── .vscode/              # VS Code AI configuration
│   ├── tasks.json        # AI-powered tasks
│   └── keybindings.json  # AI shortcuts
├── .env.example          # API key template
└── setup_ai.py          # AI environment setup
```

### Code Integration Patterns:

#### Python Projects:
```python
from ai_powerhouse import AIPowerhouse

# Always include multi-provider AI access
ai = AIPowerhouse()

# Use for code generation, review, optimization
def get_ai_assistance(prompt, provider='all'):
    return ai.ask(prompt, provider)
```

#### PowerShell Projects:
```powershell
# Import AI Powerhouse module
Import-Module AIPowerhouse

# Use Claude Agents for specialized tasks
security-review $FilePath
code-review $FilePath
database-optimize $Query
```

#### JavaScript/TypeScript Projects:
```javascript
// Include AI Powerhouse bridge
const aiPowerhouse = require('./integration/ai_bridge');

// Use for development assistance
async function getAIHelp(prompt) {
    return await aiPowerhouse.ask(prompt);
}
```

### Default Commands to Include:

#### Project Setup:
- `setup-ai-environment` - Initialize AI Powerhouse for project
- `install-agents` - Add relevant Claude Agents
- `configure-vscode-ai` - Set up VS Code AI integration

#### Development Workflow:
- `ai-help` - General AI assistance
- `ai-review` - Code review with multiple agents
- `ai-test` - Generate tests using Testing Agent
- `ai-docs` - Generate documentation
- `ai-debug` - Debug issues with Debug Agent
- `ai-optimize` - Performance optimization
- `ai-secure` - Security analysis

#### Cross-System Operations:
- `python-to-powershell` - Execute PowerShell from Python
- `powershell-to-python` - Execute Python from PowerShell
- `unified-ai-query` - Query across all systems and providers

### Quality Standards:

#### Always Include:
- Multi-provider AI integration for diverse perspectives
- Specialized agents for domain expertise
- Cross-system compatibility for comprehensive solutions
- Proper error handling and fallback mechanisms
- Configuration management for API keys and settings
- Documentation with AI assistance examples

#### Best Practices:
- Use appropriate agents for specific tasks
- Compare responses from multiple providers
- Validate AI suggestions with domain expertise
- Maintain human oversight for critical decisions
- Document AI-assisted development processes
- Keep AI configurations version controlled

### Implementation Priority:

1. **High Priority**: Core AI Powerhouse integration
2. **Medium Priority**: Relevant Claude Agents for project domain
3. **Low Priority**: Full cross-system integration (unless needed)
4. **Optional**: Advanced VS Code integration and shortcuts

This configuration ensures that AI Powerhouse framework becomes the standard approach for all development work, providing consistent, powerful AI assistance across all projects and technologies.