# VS Code Setup Instructions

1. Install all extensions listed in `vscode-extensions.txt`:
   ```powershell
   Get-Content vscode-extensions.txt | ForEach-Object { code --install-extension $_ }
   ```

2. Copy your custom command file (extension source or VSIX) to the new VS Code instance.
   - If you have a built VSIX, install it with:
     ```powershell
     code --install-extension path\to\your-extension.vsix
     ```
   - If you want to copy the source, place the `extension.ts` file in the appropriate directory and reload VS Code.

3. (Optional) If you have settings or keybindings to transfer, copy your `settings.json` and `keybindings.json` from your old VS Code profile.

---

This script and the extension list will help you replicate your VS Code environment on another machine.
