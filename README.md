# PEARL - Position Encoder And Readout Logic

## Project Overview

**PEARL** is a precise, elegant, and sophisticated position measurement system designed for the Raspberry Pi Pico. The system provides high-accuracy position readouts from quadrature encoders with a comprehensive engineering interface for configuration and monitoring.

### 🔍 What does PEARL stand for?
**P**osition **E**ncoder **A**nd **R**eadout **L**ogic

*Precise, tidy, and elegant — like the firmware.*

## ✨ Key Features

### Core Functionality
- **High-Precision Position Measurement**: Quadrature encoder support with sub-micron accuracy
- **Big Font LCD Display**: Clear, easy-to-read position display across 3 rows
- **Dual-Core Processing**: Hardware-accelerated PIO for encoder reading
- **Real-Time Velocity Calculation**: Speed and RPM monitoring
- **Direction Indication**: Visual indicators for motion direction

### Engineering Interface
- **HW-040 Rotary Encoder Menu**: Professional menu navigation system
- **Triple-Click Activation**: Quick access to engineering menu (3 clicks within 1 second)
- **Comprehensive Configuration**: Encoder settings, display options, system parameters
### Performance & Reliability
- **Hardware Acceleration**: PIO state machines for zero-latency encoder reading
- **Error Detection**: FIFO overflow monitoring and invalid transition detection
- **Performance Monitoring**: Real-time system load and efficiency tracking
- **Adaptive Configuration**: Dynamic system optimization based on usage patterns

## 🔧 Hardware Requirements

### Main Components
- **Raspberry Pi Pico** (RP2040)
- **16x4 Character LCD** with I2C backpack (PCF8574)
- **Quadrature Encoder** (any standard rotary encoder)
- **HW-040 Rotary Encoder Module** (for menu navigation)

### Pin Assignments
```
I2C (LCD):          SDA=GP8, SCL=GP9
Main Encoder:       A=GP10, B=GP11
Menu Encoder:       CLK=GP12, DT=GP13, SW=GP14
```

## 🚀 Quick Start

### 1. Hardware Setup
1. Connect the LCD via I2C to pins 8 (SDA) and 9 (SCL)
2. Connect your position encoder to pins 10 (A) and 11 (B)
3. Connect the HW-040 menu encoder to pins 12, 13, and 14
4. Power the Pico via USB or external 5V supply

### 2. Firmware Installation
1. Download the latest `PEARL.uf2` file from the releases
2. Hold the BOOTSEL button while connecting the Pico to USB
3. Copy `PEARL.uf2` to the RPI-RP2 drive that appears
4. The system will automatically reboot and start

### 3. Basic Operation
- **Position Display**: Large position readout appears immediately
- **Serial Commands**: Connect via USB serial for advanced commands
- **Engineering Menu**: Triple-click the HW-040 encoder button to access

## 📋 Serial Commands

| Command | Function |
|---------|----------|
| `R` | Reset position to zero |
| `S` | Scan I2C bus for devices |
| `P` | Toggle PIO/GPIO encoder mode |
| `V` | Show velocity and performance info |
| `M` | Show menu encoder status |
| `C` | Clear performance counters |
| `I` | Show version and build information |
| `H` | Show help menu |

## 🎛️ Engineering Menu

Access the comprehensive engineering menu by **triple-clicking** the HW-040 encoder button within 1 second.

### Menu Options:
1. **Encoder Settings** - Resolution, scaling, direction inversion
2. **Display Settings** - Contrast, backlight, refresh rate
3. **System Information** - Version, build info, hardware status
4. **Diagnostics Tools** - Error monitoring, performance analysis
5. **Calibration Tools** - Encoder calibration and testing
6. **Performance Monitor** - Real-time system performance
7. **Factory Reset** - Restore default settings
8. **Save & Exit** - Save configuration and return to normal operation
9. **Cancel/Exit** - Exit without saving changes

### Navigation:
- **Rotate Encoder**: Navigate up/down through menu items
- **Press Button**: Select current item or confirm changes
- **Menu Timeout**: Automatically exits after 1-2 minutes of inactivity

## 📊 Performance Specifications

- **Position Resolution**: Up to 0.25µm (depending on encoder)
- **Update Rate**: 10-100Hz (adaptive based on system load)
- **Velocity Range**: ±10,000 units/second
- **Encoder Frequency**: Up to 100kHz (PIO mode)
- **Display Refresh**: 10Hz standard, configurable
- **Menu Response**: <10ms latency for all operations

## 🛠️ Development

### Build Requirements
- **Pico SDK 2.1.1** or later
- **CMake 3.13** or later
- **ARM GCC Toolchain** for cross-compilation
- **VS Code** with Pico extension (recommended)

### Building from Source
```bash
git clone https://github.com/DirtyDerv/PICO_BG_READER.git
cd PICO_BG_READER
mkdir build && cd build
cmake ..
make -j4
```

### Version History
- **v0.06** - PEARL Rebranding & Project Identity
- **v0.05** - HW-040 Rotary Encoder Menu System
- **v0.04** - Adaptive Configuration Framework
- **v0.03** - Performance & Reliability Enhancements
- **v0.02** - Enhanced PIO Optimization
- **v0.01** - Version Management System
- **v0.00** - Initial Release

## 📄 License

This project is open source and available under the MIT License.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests, report bugs, or suggest new features.

## 📞 Support

For questions, issues, or support, please open an issue on the GitHub repository.

---

**PEARL** - Where precision meets elegance in position measurement. ✨

### Update Rate
Modify `UPDATE_INTERVAL_MS` to change display update frequency:
- Lower values = faster updates (more CPU usage)
- Higher values = slower updates (less CPU usage)

## Pin Configuration Summary

| Component | Pico Pin | GPIO | Function |
|-----------|----------|------|----------|
| LCD SDA   | 11       | 8    | I2C Data |
| LCD SCL   | 12       | 9    | I2C Clock |
| Encoder A | 14       | 10   | Quadrature A |
| Encoder B | 15       | 11   | Quadrature B |

All pins include internal pull-up resistors where appropriate.
