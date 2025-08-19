# PEARL v0.06 - Folder Structure Documentation

## Project Organization

This document describes the clean, professional folder structure implemented for the PEARL (Pico Ball Screw Encoder Control System) project.

## Directory Structure


```
PEARL/
├── src/                        # Source code files (.cpp)
├── include/                    # Header files (.h)
├── pio/                        # PIO assembly files (.pio)
├── scripts/                    # Build and utility scripts
├── docs/                       # Documentation files
│   ├── electronics/            # Electronics docs, guides, block diagrams, schematic
│   │   ├── schematic/          # KiCad schematic files
│   │   └── ...                 # Block diagrams, protection guides, walkthroughs
│   └── ...                     # Other documentation
├── build/                      # Build output directory (generated)
├── .vscode/                    # VS Code configuration
├── .git/                       # Git repository data
├── CMakeLists.txt              # CMake build configuration
├── pico_sdk_import.cmake       # Pico SDK integration
├── .gitignore                  # Git ignore rules
└── README.md                   # Main project documentation
```

## Directory Details

### `src/` - Source Code
Contains all C++ implementation files:
- `Bg_Reader.cpp` - Main application entry point
- `lcd_i2c.cpp` - LCD display driver implementation
- `quadrature_encoder.cpp` - Main encoder logic
- `indexed_encoder.cpp` - Indexed encoder specialization
- `big_font.cpp` - Large font display system
- `status_display.cpp` - Status information display
- `hw040_encoder.cpp` - HW040 menu encoder driver
- `engineering_menu.cpp` - Two-level menu system
- `splash_screen.cpp` - Professional startup sequence
- `adaptive_config.cpp` - Dynamic configuration system

### `include/` - Header Files
Contains all C++ header files defining interfaces and classes:
- `config.h` - System configuration constants
- `version.h` - Version information and build metadata
- `lcd_i2c.h` - LCD display driver interface
- `quadrature_encoder.h` - Main encoder class definition
- `indexed_encoder.h` - Indexed encoder interface
- `big_font.h` - Large font system interface
- `status_display.h` - Status display interface
- `hw040_encoder.h` - Menu encoder interface
- `engineering_menu.h` - Menu system interface
- `splash_screen.h` - Startup sequence interface
- `adaptive_config.h` - Configuration system interface
- `i2c_scanner.h` - I2C device scanning utilities

### `pio/` - PIO Assembly
Contains Programmable I/O assembly files for hardware interfacing:
- `blink.pio` - LED blinking PIO program
- `quadrature.pio` - High-speed encoder reading PIO program

### `scripts/` - Automation Scripts
Contains build and utility scripts:
- `increment_version.ps1` - PowerShell version incrementing
- `increment_version.sh` - Bash version incrementing

### `docs/` - Documentation

Contains project documentation:
- `README.md` - Main project overview
- `PROJECT_SUMMARY.md` - Detailed project specifications
- `BRANCHING_STRATEGY.md` - Git workflow documentation
- `VERSION_MANAGEMENT.md` - Version control procedures
- `FOLDER_STRUCTURE.md` - This file
- `electronics/` - All electronics documentation, block diagrams, protection guides, and schematic files
   - `BLOCK_DIAGRAM.md`, `BLOCK_DIAGRAM_PROTECTED.md` - System block diagrams
   - `PROTECTION_GUIDE.md` - Industrial protection guide
   - `SCHEMATIC_WALKTHROUGH.md` - Schematic build walkthrough
   - `KiCad_Getting_Started.md` - KiCad beginner's guide
   - `schematic/` - KiCad schematic files (e.g., `PEARL_Schematic.kicad_sch`)

### `build/` - Build Output
Generated directory containing:
- Compiled object files
- Linked executables (.elf, .uf2)
- CMake cache and configuration
- Build system files

## Build System Integration

The CMakeLists.txt has been updated to work with this structure:

```cmake
# Source files from src/ directory
add_executable(PEARL 
    src/Bg_Reader.cpp 
    src/lcd_i2c.cpp
    src/quadrature_encoder.cpp
    # ... other source files
)

# Include directories
target_include_directories(PEARL PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# PIO files from pio/ directory
pico_generate_pio_header(PEARL ${CMAKE_CURRENT_LIST_DIR}/pio/blink.pio)
pico_generate_pio_header(PEARL ${CMAKE_CURRENT_LIST_DIR}/pio/quadrature.pio)
```

## Benefits of This Structure

1. **Clear Separation**: Source and header files are clearly separated
2. **Professional Layout**: Follows embedded C++ project conventions
3. **Build System Friendly**: CMake easily finds all required files
4. **Scalable**: Easy to add new modules without cluttering root directory
5. **IDE Integration**: VS Code and other IDEs work better with organized structure
6. **Version Control**: .gitignore can be more specific about build artifacts
7. **Documentation**: Centralized documentation in docs/ folder

## Development Workflow

1. **Adding New Features**:
   - Add .cpp file to `src/`
   - Add .h file to `include/`
   - Update CMakeLists.txt to include new source file

2. **Building**:
   ```bash
   mkdir build && cd build
   cmake -G "Ninja" ..
   ninja
   ```

3. **Cleaning**:
   ```bash
   rm -rf build/
   ```

## File Movement Summary

The following files were reorganized:
- **Source files** (.cpp) → `src/`
- **Header files** (.h) → `include/`
- **PIO files** (.pio) → `pio/`
- **Script files** (.ps1, .sh) → `scripts/`
- **Documentation** (.md) → `docs/`

## Compatibility

This structure maintains full compatibility with:
- Raspberry Pi Pico SDK
- VS Code Pico extension
- CMake build system
- Git version control
- All existing functionality

The project successfully builds and maintains all features with the new organization.
