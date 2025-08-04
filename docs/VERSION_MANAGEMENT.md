# BG Reader - Version Management

## Version Numbering System

This project uses a systematic version numbering approach:

- **Format**: `MAJOR.MINOR` (e.g., 0.01, 0.02, 1.00)
- **Starting Point**: 0.00
- **Increment**: 0.01 for each iteration/improvement
- **Release Ready**: When ready for release, increment to next whole number (1.00, 2.00, etc.)

## Current Version: 0.01

### Version History
- **0.01** (2025-08-04) - Test version increment system functionality
- **0.00** (2025-08-04) - Initial version with new numbering system
  - Big font LCD display system
  - Quadrature encoder with PIO support  
  - Direction and speed indicators
  - Custom character generation

## Version Management Tools

### Manual Version Updates
Edit these files when incrementing version:
1. `version.h` - Update version constants and history
2. `CMakeLists.txt` - Update program version string

### Automated Scripts (Future Use)
- `increment_version.ps1` - PowerShell script for Windows
- `increment_version.sh` - Bash script for Linux/Mac

*Note: PowerShell execution policy may need adjustment on Windows*

## Version Display Features

The current version is displayed in:
- **LCD Startup**: Shows version in header "BG Reader v0.01"
- **Serial Console**: Shows version in startup banner
- **'I' Command**: Displays detailed version and build information

## Build Information
Each build includes:
- Version number from version.h
- Build date and time (compile-time)
- Feature list and hardware information

Use the **'I'** command during runtime to view complete version details.
