# PEARL v0.07 Installation Guide

## Table of Contents
1. [Hardware Requirements](#hardware-requirements)
2. [Wiring Diagram](#wiring-diagram)
3. [Software Installation](#software-installation)
4. [Initial Setup](#initial-setup)
5. [Verification](#verification)
6. [Troubleshooting](#troubleshooting)

## Hardware Requirements

### Essential Components
- **Raspberry Pi Pico** (RP2040-based microcontroller)
- **20x4 I2C LCD Display** (HD44780 compatible with I2C backpack)
- **HW-040 Rotary Encoder** (with push button)
- **Quadrature Encoder** (for main position measurement)
- **USB Cable** (USB-A to Micro-USB for Pico)

### Optional Components
- **Breadboard or PCB** for connections
- **Jumper Wires** (male-to-male, male-to-female)
- **Pull-up Resistors** (10kΩ, if not included in encoder modules)
- **Power Supply** (5V for LCD, 3.3V for logic)

### Tools Required
- Soldering iron and solder (if making permanent connections)
- Wire strippers
- Multimeter (for troubleshooting)
- Computer with USB port

## Wiring Diagram

### Raspberry Pi Pico Pinout

```
                    Raspberry Pi Pico
                   ┌─────────────────┐
               3V3 │1              40│ VBUS
               GP0 │2              39│ VSYS
               GP1 │3              38│ GND
               GND │4              37│ 3V3_EN
               GP2 │5              36│ GP28
               GP3 │6              35│ GP27
               GP4 │7              34│ GP26
               GP5 │8              33│ GND
               GND │9              32│ GP22
               GP6 │10             31│ GP21
               GP7 │11             30│ GP20
               GP8 │12             29│ GP19
               GP9 │13             28│ GP18
               GND │14             27│ GP17
              GP10 │15             26│ GP16
              GP11 │16             25│ GP15
              GP12 │17             24│ GP14
              GP13 │18             23│ GND
               GND │19             22│ GP28
              GP14 │20             21│ GP15
                   └─────────────────┘
```

### Connection Table

| Component | Pico Pin | GPIO | Description |
|-----------|----------|------|-------------|
| **I2C LCD** | | | |
| VCC | VBUS (40) | - | 5V Power |
| GND | GND (38) | - | Ground |
| SDA | Pin 31 | GP21 | I2C Data |
| SCL | Pin 32 | GP22 | I2C Clock |
| **HW-040 Menu Encoder** | | | |
| VCC | 3V3 (36) | - | 3.3V Power |
| GND | GND (33) | - | Ground |
| CLK | Pin 29 | GP19 | Encoder A |
| DT | Pin 30 | GP20 | Encoder B |
| SW | Pin 28 | GP18 | Button |
| **Main Quadrature Encoder** | | | |
| VCC | 3V3 (37) | - | 3.3V Power |
| GND | GND (4) | - | Ground |
| A | Pin 2 | GP0 | Encoder A |
| B | Pin 3 | GP1 | Encoder B |

### Detailed Wiring Instructions

#### 1. I2C LCD Display
```
LCD Module → Pico
VCC → VBUS (Pin 40, 5V)
GND → GND (Pin 38)
SDA → GP21 (Pin 31)
SCL → GP22 (Pin 32)
```

#### 2. HW-040 Menu Encoder
```
HW-040 → Pico
VCC → 3V3 (Pin 36)
GND → GND (Pin 33)
CLK → GP19 (Pin 29)
DT → GP20 (Pin 30)
SW → GP18 (Pin 28)
```

#### 3. Main Quadrature Encoder
```
Main Encoder → Pico
VCC → 3V3 (Pin 37)
GND → GND (Pin 4)
Channel A → GP0 (Pin 2)
Channel B → GP1 (Pin 3)
```

### Circuit Diagram
```
                    ┌─────────────────┐
                    │   Raspberry     │
                    │   Pi Pico       │
                    │                 │
   ┌─────────────── │ GP21 (SDA)      │
   │            ┌── │ GP22 (SCL)      │
   │            │   │                 │
   │            │   │ GP19 ←──────────┼─── HW-040 CLK
   │            │   │ GP20 ←──────────┼─── HW-040 DT
   │            │   │ GP18 ←──────────┼─── HW-040 SW
   │            │   │                 │
   │            │   │ GP0 ←───────────┼─── Main Enc A
   │            │   │ GP1 ←───────────┼─── Main Enc B
   │            │   │                 │
   │            │   │ VBUS (5V) ──────┼─── LCD VCC
   │            │   │ 3V3 ────────────┼─── Encoders VCC
   │            │   │ GND ────────────┼─── Common Ground
   │            │   └─────────────────┘
   │            │
   │  ┌─────────┴──────────────┐
   │  │     20x4 I2C LCD       │
   └──┤ SDA               VCC  │
      │ SCL               GND  │
      └────────────────────────┘
```

## Software Installation

### Prerequisites

#### Development Environment Setup
1. **Install VS Code**
   - Download from [https://code.visualstudio.com/](https://code.visualstudio.com/)
   - Install the Raspberry Pi Pico extension

2. **Install Pico SDK**
   - The VS Code extension will automatically install the Pico SDK
   - Location: `%USERPROFILE%\.pico-sdk\`

#### Alternative: Pre-built Binary Installation

1. **Download PEARL.uf2**
   - Get the latest release from GitHub releases
   - Or compile from source using the instructions below

2. **Flash to Pico**
   - Hold BOOTSEL button while connecting USB
   - Pico appears as mass storage device
   - Copy `PEARL.uf2` to the Pico drive
   - Pico will automatically reboot and run PEARL

### Building from Source

#### 1. Clone Repository
```bash
git clone https://github.com/DirtyDerv/PICO_BG_READER.git
cd PICO_BG_READER
git checkout development
```

#### 2. Open in VS Code
```bash
code .
```

#### 3. Build Project
- Open VS Code terminal: `Ctrl+Shift+`
- Run build task: `Ctrl+Shift+P` → "Tasks: Run Task" → "Compile Project"
- Or use terminal:
```bash
cd build
cmake -G "Ninja" ..
ninja
```

#### 4. Flash to Pico
- Connect Pico in BOOTSEL mode
- Copy `build/PEARL.uf2` to Pico drive
- Or use VS Code task: "Run Project"

### Compilation Requirements

#### Windows (Recommended)
- **VS Code** with Raspberry Pi Pico extension
- **CMake** (installed with Pico SDK)
- **Ninja Build System** (installed with Pico SDK)
- **ARM GCC Toolchain** (installed with Pico SDK)

#### Linux/macOS
```bash
# Install dependencies
sudo apt install cmake gcc-arm-none-eabi build-essential

# Clone Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
export PICO_SDK_PATH=/path/to/pico-sdk

# Build
mkdir build && cd build
cmake ..
make -j4
```

## Initial Setup

### 1. Power-On Sequence
1. Connect all wiring according to diagram
2. Connect USB cable to Pico
3. System should display splash screen
4. After 3 seconds, main position display appears

### 2. First Boot Configuration
The system will boot with default settings:
- **Encoder Resolution**: 500 PPR
- **Thread Pitch**: 1.0mm
- **Display Update**: 100ms
- **I2C Address**: 0x27

### 3. Access Engineering Menu
1. **Long Press** the HW-040 encoder button (2+ seconds)
2. Enter password using encoder:
   - Rotate to select numbers (0-9)
   - Press to confirm each digit
   - Default password: `1234`
3. Navigate menu with encoder rotation
4. Press to select options

### 4. Essential Configuration

#### Configure Main Encoder
1. Engineering Menu → **Encoder Settings**
2. Set **Encoder Resolution** (PPR of your encoder)
3. Set **Thread Pitch** (mm per revolution of leadscrew)
4. Save settings: **Save and Exit**

#### Run Initial Calibration
1. Engineering Menu → **Calibration**
2. Follow on-screen instructions:
   - Set first position
   - Move at least 100mm
   - Set second position
   - System calculates accurate pitch
3. Accept calibration results

## Verification

### 1. Display Test
- **LCD should show**: Position, velocity, and status
- **Characters should be**: Clear and properly aligned
- **Updates should be**: Smooth without flicker

### 2. Encoder Test
- **Rotate main encoder**: Position should change
- **Direction should be**: Correct (CW = positive)
- **Resolution should match**: Expected counts per revolution

### 3. Menu Navigation Test
- **Long press**: Should enter engineering menu
- **Rotation**: Should navigate menu items
- **Press**: Should select items
- **Back navigation**: Should work properly

### 4. Calibration Verification
1. Move to known position
2. Reset position to zero
3. Move exactly 100mm
4. Reading should show 100.000mm ±0.1mm

## Common Installation Issues

### 1. LCD Not Working
**Symptoms**: Blank display or corrupted characters
**Solutions**:
- Check I2C wiring (SDA/SCL swapped?)
- Verify 5V power to LCD
- Check I2C address (try 0x27 or 0x3F)
- Test with I2C scanner

### 2. Encoder Not Responding
**Symptoms**: No position changes when rotating
**Solutions**:
- Check encoder wiring
- Verify 3.3V power to encoder
- Check for loose connections
- Verify encoder type (quadrature vs. simple)

### 3. Menu Button Not Working
**Symptoms**: Cannot access engineering menu
**Solutions**:
- Check HW-040 wiring
- Verify button connection (SW pin)
- Try different GPIO pin
- Check for pull-up resistor

### 4. Compilation Errors
**Symptoms**: Build fails with errors
**Solutions**:
- Clean build directory: `rm -rf build`
- Regenerate CMake: `cmake -G "Ninja" ..`
- Check Pico SDK installation
- Verify all source files present

### 5. Flash Memory Issues
**Symptoms**: Settings not saving
**Solutions**:
- Check flash permissions
- Verify configuration validation
- Reset to factory defaults
- Re-flash firmware

## Safety Considerations

### Electrical Safety
- **Double-check wiring** before applying power
- **Use proper voltage levels** (3.3V for logic, 5V for LCD)
- **Avoid short circuits** with proper insulation
- **Disconnect power** when making wiring changes

### Mechanical Safety
- **Secure all connections** to prevent intermittent operation
- **Mount properly** to avoid vibration issues
- **Protect from contamination** in industrial environments
- **Provide emergency stops** in automated systems

### Data Safety
- **Backup configurations** before major changes
- **Test thoroughly** before production use
- **Document all modifications** for maintenance
- **Implement regular calibration** schedule

## Advanced Installation Options

### Custom Hardware Integration
- **PCB Design**: Create custom PCB for permanent installation
- **Enclosure**: Use industrial enclosure for harsh environments
- **Cable Management**: Use proper cable routing and strain relief
- **EMI Protection**: Add filtering for noisy industrial environments

### Multi-Unit Installation
- **Address Configuration**: Use unique I2C addresses for multiple LCDs
- **Network Integration**: Consider RS485 or CAN bus for multiple units
- **Centralized Monitoring**: Implement data logging and remote monitoring
- **Synchronization**: Coordinate multiple measurement points

---

**Next Steps**: After successful installation, proceed to the [User Manual](USER_MANUAL.md) for complete operating instructions.
