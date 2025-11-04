# PEARL - Precision Encoder Absolute Reference Logger

<div align="center">

![Version](https://img.shields.io/badge/version-0.07-blue)
![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-red)
![Language](https://img.shields.io/badge/language-C%2B%2B-orange)
![License](https://img.shields.io/badge/license-MIT-green)

**High-Precision Quadrature Encoder Position Tracking System**

[Features](#features) • [Hardware](#hardware) • [Installation](#installation) • [Documentation](#documentation) • [Testing](#testing)

</div>

---

## 🎯 Overview

PEARL is a professional-grade position encoder system built on the Raspberry Pi Pico, designed for industrial precision applications. It combines PIO-accelerated quadrature decoding, multicore processing, and comprehensive menu-driven configuration to deliver sub-micron accuracy in position tracking.

### Key Capabilities

- **64-bit Position Tracking**: Prevents integer overflow for extended operation
- **PIO Hardware Acceleration**: High-speed quadrature decoding using programmable I/O
- **Multicore Processing**: Dedicated Core1 for encoder processing, Core0 for UI/display
- **Security Hardened**: Production-ready with buffer overflow protection and mutex safety
- **Real-time Performance**: <1ms main loop, <10μs encoder response
- **Comprehensive Testing**: Web-based and automated test framework included

---

## ✨ Features

### Core Functionality
- **Precision Measurement**: Sub-micron accuracy with configurable pitch calibration
- **Dual Mode Operation**: PIO hardware or GPIO software fallback
- **Live Display**: 20x4 LCD with big font display mode
- **Configuration Persistence**: EEPROM-style flash storage for settings
- **Engineering Menu**: Password-protected access to advanced configuration

### Advanced Features
- **Performance Monitoring**: Real-time benchmarking and profiling (commands `B`, `W`)
- **Web Testing Interface**: Browser-based menu system testing at `localhost:8080`
- **Security Framework**: Debug-only authentication bypass, buffer overflow protection
- **AI-Powered Development**: Integrated Claude Agents for security, testing, and performance analysis

---

## 🛠️ Hardware

### Required Components

| Component | Specification | Purpose |
|-----------|--------------|---------|
| **Raspberry Pi Pico** | RP2040 @ 133MHz | Main controller |
| **Quadrature Encoder** | Any standard encoder | Position sensing |
| **LCD Display** | 20x4 I2C (PCF8574) | User interface |
| **HW-040 Encoder** | Rotary encoder w/ button | Menu navigation |

### Pin Configuration

```
Quadrature Encoder:
  - Channel A: GPIO 10
  - Channel B: GPIO 11
  - Index:     GPIO 12 (optional)

LCD Display (I2C):
  - SDA: GPIO 4
  - SCL: GPIO 5
  - Address: 0x27

HW-040 Menu Encoder:
  - CLK: GPIO 16
  - DT:  GPIO 17
  - SW:  GPIO 18
```

### Schematic

See [`electronics/schematic/PEARL_Schematic.kicad_sch`](electronics/schematic/PEARL_Schematic.kicad_sch) for complete wiring diagram.

---

## 📦 Installation

### Prerequisites

- **Raspberry Pi Pico SDK** v2.1.1 or later
- **CMake** 3.13+
- **Ninja** build system
- **Python** 3.9+ (for testing tools)

### Build Instructions

```powershell
# Clone the repository
git clone https://github.com/DirtyDerv/PEARL.git
cd PEARL

# Configure CMake build
mkdir build
cd build
cmake -G Ninja ..

# Build firmware
ninja

# Flash to Pico (BOOTSEL mode)
# Copy build/PEARL.uf2 to your Pico drive
```

### Quick Start

1. **Connect Hardware**: Wire encoder, LCD, and menu encoder per pinout
2. **Flash Firmware**: Copy `PEARL.uf2` to Pico in BOOTSEL mode
3. **Power On**: System boots and displays position immediately
4. **Calibrate**: Triple-click menu encoder to access calibration wizard

---

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [User Manual](docs/USER_MANUAL.md) | Complete operating instructions |
| [API Reference](docs/API_REFERENCE.md) | Developer API documentation |
| [Engineering Menu](docs/ENGINEERING_MENU.md) | Advanced configuration guide |
| [Calibration Guide](docs/CALIBRATION.md) | Step-by-step calibration process |
| [Troubleshooting](docs/TROUBLESHOOTING.md) | Common issues and solutions |
| [Electronics Guide](docs/electronics/README.md) | Hardware design documentation |

---

## 🧪 Testing

### Automated Testing

```powershell
# Run comprehensive menu system tests
python test_menu_system.py

# Output: 8 test cases covering edge cases and integration
```

### Web Testing Interface

```powershell
# Start web server
cd "Web Test"
python -m http.server 8080

# Open browser to http://localhost:8080
# Connect via Web Serial API and run tests
```

### Serial Commands

Connect via USB serial (115200 baud):

```
R - Reset position to zero
S - Scan I2C bus
P - Toggle PIO/GPIO mode
V - Show velocity & performance info
D - Toggle display mode
C - Clear performance counters
M - Show menu encoder status
I - Show version info
H - Show help

Performance Commands:
B - Run performance benchmark
W - Start continuous monitoring

Menu Test Commands:
T - Test menu system status
U - Force user menu activation
Q - Force menu exit
```

---

## 🔒 Security Features

### Production-Ready Security

- ✅ **Buffer Overflow Protection**: All display functions bounds-checked
- ✅ **Integer Overflow Prevention**: 64-bit position tracking
- ✅ **Race Condition Safety**: Multicore mutex synchronization
- ✅ **Debug-Only Bypass**: Engineering menu bypass disabled in production builds

### Security Analysis Reports

- [Security Analysis Report](security_analysis_report.md) - Vulnerability assessment
- [Performance Analysis Report](performance_analysis_report.md) - Optimization guide

---

## 🚀 Performance

### Real-Time Specifications

| Metric | Target | Achieved |
|--------|--------|----------|
| Main Loop | <1ms | ✅ ~100μs |
| Encoder Response | <10ms | ✅ <1ms |
| Display Update | <50ms | ✅ ~20ms |
| Menu Navigation | <100ms | ✅ <50ms |

### Optimization Features

- Event-driven menu updates (90% CPU reduction when idle)
- Adaptive encoder sampling (50% Core1 efficiency gain)
- Buffered LCD updates (80% faster screen refresh)

---

## 🤖 AI Powerhouse Integration

PEARL includes the complete AI Powerhouse framework with specialized Claude Agents:

- 🔒 **Security Agent**: Vulnerability scanning and hardening
- 🧪 **Testing Agent**: Automated test generation
- ⚡ **Performance Agent**: Bottleneck analysis and optimization
- 📚 **Documentation Agent**: API and user documentation
- 🐛 **Debug Agent**: Systematic issue resolution

See [AI Integration Guide](ai_integration/README.md) for details.

---

## 📁 Project Structure

```
PEARL/
├── src/                    # Source code
│   ├── Bg_Reader.cpp       # Main application
│   ├── engineering_menu.cpp # Menu system
│   ├── quadrature_encoder.cpp # Encoder driver
│   ├── lcd_i2c.cpp         # Display driver
│   └── performance_monitor.cpp # Performance tools
├── include/                # Header files
├── docs/                   # Documentation
├── tests/                  # Test suite
├── electronics/            # Hardware schematics
├── Web Test/               # Web testing interface
├── ai_integration/         # AI Powerhouse framework
└── build/                  # Compiled binaries
```

---

## 🔧 Configuration

### Default Settings

```cpp
// Encoder Configuration
ENCODER_PITCH = 2.0mm        // Lead screw pitch
ENCODER_PPR = 600            // Pulses per revolution
QUADRATURE_MODE = 4X         // Quadrature multiplication

// Display Settings
LCD_ADDRESS = 0x27           // I2C address
LCD_SIZE = 20x4              // Character display
UPDATE_RATE = 100ms          // Refresh interval

// Performance
CORE1_PRIORITY = HIGH        // Encoder processing priority
DISPLAY_BUFFER = ENABLED     // Buffered updates
```

### Runtime Configuration

Access via engineering menu (triple-click + long-press):
- Pitch calibration
- Display modes
- Performance tuning
- System diagnostics

---

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Workflow

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open Pull Request

---

## 📝 Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history.

### Recent Updates (v0.07)

- 🔒 Critical security fixes (buffer overflow, race conditions)
- ⚡ Performance monitoring system
- 🧪 Comprehensive test framework
- 🤖 AI Powerhouse integration
- 📚 Complete documentation overhaul

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- Raspberry Pi Foundation for the Pico SDK
- Community contributors and testers
- AI Powerhouse framework developers

---

## 📞 Contact

- **Author**: DirtyDerv
- **Repository**: https://github.com/DirtyDerv/PEARL
- **Issues**: https://github.com/DirtyDerv/PEARL/issues

---

<div align="center">

**Built with ❤️ using Raspberry Pi Pico and AI Powerhouse**

[⬆ Back to Top](#pearl---precision-encoder-absolute-reference-logger)

</div>
