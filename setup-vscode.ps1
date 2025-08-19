# PowerShell script to install all extensions from vscode-extensions.txt
Get-Content vscode-extensions.txt | ForEach-Object { code --install-extension $_ }

# Copy your extension file (edit the path as needed)
# Copy-Item -Path .\src\extension.ts -Destination <target-path>
# Or install a VSIX if you have one:
# code --install-extension <your-extension>.vsix
