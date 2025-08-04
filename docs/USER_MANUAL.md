# PEARL v0.07 User Manual

## Table of Contents
1. [Getting Started](#getting-started)
2. [Main Display](#main-display)
3. [Engineering Menu](#engineering-menu)
4. [Calibration System](#calibration-system)
5. [Configuration Management](#configuration-management)
6. [Daily Operation](#daily-operation)
7. [Maintenance](#maintenance)
8. [Troubleshooting](#troubleshooting)

## Getting Started

### Power-On Sequence
1. **Connect USB Power**: System displays splash screen
2. **Splash Display**: Shows PEARL v0.07 logo and system info for 3 seconds
3. **Main Display**: Transitions to position monitoring screen
4. **Ready for Use**: System is now ready for measurement

### Basic Operation Overview
- **Position Monitoring**: Continuous display of current position
- **Menu Access**: Long press encoder button for settings
- **Reset Position**: Quick reset to zero reference
- **Calibration**: Interactive precision calibration system

## Main Display

### Display Layout
```
┌────────────────────┐
│ Pos: 123.456 mm    │ ← Current Position
│ Vel: 12.3 mm/s     │ ← Current Velocity  
│ RPM: 45.6          │ ← Spindle RPM
│ PEARL v0.07 Ready  │ ← Status Line
└────────────────────┘
```

### Position Display
- **Format**: XXX.XXX mm (3 decimal places)
- **Range**: ±999.999 mm typical
- **Resolution**: 0.001 mm (with proper calibration)
- **Update Rate**: 10 Hz (100ms refresh)

### Velocity Display
- **Format**: XX.X mm/s
- **Calculation**: Real-time velocity from position changes
- **Averaging**: 8-sample moving average for stability
- **Direction**: Positive = clockwise rotation

### RPM Display
- **Format**: XX.X RPM
- **Calculation**: Based on encoder frequency and pitch
- **Range**: 0.1 to 9999.9 RPM
- **Accuracy**: ±0.1 RPM

### Status Indicators
- **"Ready"**: System operating normally
- **"Cal"**: Calibration mode active
- **"Err"**: Error condition detected
- **"Set"**: Configuration mode active

## Engineering Menu

### Accessing the Menu
1. **Long Press**: Hold HW-040 encoder button for 2+ seconds
2. **Password Entry**: Enter 4-digit password (default: 1234)
   - Rotate encoder to select digit (0-9)
   - Press button to confirm each digit
   - Press when complete
3. **Menu Navigation**: Use encoder to navigate options

### Menu Structure
```
Engineering Menu
├── Encoder Settings
│   ├── Encoder Resolution (PPR)
│   ├── Thread Pitch (mm)
│   ├── Invert Direction
│   └── Enable PIO Mode
├── Display Settings
│   ├── Update Rate (ms)
│   ├── I2C Address
│   ├── Backlight Control
│   └── Contrast Setting
├── Calibration
│   ├── Interactive Calibration
│   ├── Manual Pitch Entry
│   └── Reset Calibration
├── Diagnostics
│   ├── Encoder Test
│   ├── I2C Scanner
│   ├── Performance Monitor
│   └── System Information
├── Performance Settings
│   ├── CPU Frequency
│   ├── Memory Usage
│   └── Optimization Level
├── Factory Reset
│   ├── Reset All Settings
│   ├── Keep Calibration
│   └── Full Factory Reset
├── Save and Exit
└── Cancel (No Save)
```

### Menu Navigation
- **Rotate Encoder**: Move between menu items
- **Press Button**: Select highlighted item
- **Long Press**: Go back to previous menu
- **Auto-timeout**: Returns to main display after 5 minutes

## Calibration System

### When to Calibrate
- **Initial Setup**: Always calibrate new installations
- **After Maintenance**: Recalibrate after mechanical changes
- **Periodic Check**: Monthly verification recommended
- **Accuracy Issues**: If measurements seem incorrect

### Interactive Calibration Process

#### Step 1: Preparation
1. Access **Engineering Menu → Calibration**
2. Select **Interactive Calibration**
3. Read on-screen instructions
4. Press **OK** to begin

#### Step 2: First Position Setup
```
┌────────────────────┐
│ Set Position 1     │
│ Adjust with encoder │
│ Scale: 1.2x        │ ← Dynamic velocity scaling
│ Press OK when set  │
└────────────────────┘
```
- **Rotate Main Encoder**: Position moves to desired location
- **Dynamic Scaling**: Faster rotation = faster movement
- **Precision Mode**: Slow rotation for fine adjustment
- **Press OK**: When positioned accurately

#### Step 3: Position Entry
```
┌────────────────────┐
│ Enter position:    │
│ 25.350 mm         │ ← Edit with menu encoder
│                   │
│ Press OK to confirm│
└────────────────────┘
```
- **Rotate Menu Encoder**: Increment/decrement value
- **Decimal Navigation**: UP button moves decimal place
- **Precision**: 0.001 mm resolution
- **Confirm**: Press OK when correct

#### Step 4: Move to Second Position
```
┌────────────────────┐
│ Move leadscrew to  │
│ new position       │
│ (100mm+ away)      │
│ Press OK when set  │
└────────────────────┘
```
- **Minimum Distance**: 100mm separation required
- **Direction**: Any direction acceptable
- **Accuracy**: More separation = better calibration
- **Safety**: Ensure clear path of movement

#### Step 5: Second Position Setup
- **Same process** as Step 2
- **Dynamic scaling** available
- **Distance verification** shows encoder counts moved

#### Step 6: Final Position Entry
- **Same process** as Step 3
- **Distance validation** ensures 100mm minimum
- **Error checking** prevents invalid entries

#### Step 7: Calculation and Results
```
┌────────────────────┐
│ Calibration Results│
│ New Pitch: 2.0043mm│
│ Old Pitch: 2.0000mm│
│ Apply? (Y/N)      │
└────────────────────┘
```
- **Automatic Calculation**: System computes actual pitch
- **Comparison**: Shows old vs. new values
- **Accuracy**: Typically ±0.0001 mm precision
- **Confirmation**: User must approve changes

### Manual Calibration
1. **Measure Known Distance**: Use precision tools
2. **Menu → Calibration → Manual Pitch Entry**
3. **Enter Measured Pitch**: Direct entry in mm
4. **Save Settings**: Apply immediately

### Calibration Verification
1. **Move to Known Position**: Use precision reference
2. **Reset Position**: Set to zero
3. **Move Exact Distance**: Use calibrated measurement
4. **Check Reading**: Should match within ±0.1%

## Configuration Management

### Encoder Settings

#### Encoder Resolution (PPR)
- **Range**: 100 to 10000 PPR
- **Default**: 500 PPR
- **Common Values**:
  - 500 PPR: Standard rotary encoders
  - 1000 PPR: High-resolution encoders
  - 2500 PPR: Precision measurement
  - 4000 PPR: Ultra-high precision

#### Thread Pitch
- **Range**: 0.1 to 50.0 mm
- **Default**: 1.0 mm
- **Common Values**:
  - 1.0 mm: Metric M6 fine thread
  - 1.25 mm: Metric M8 coarse thread
  - 1.5 mm: Metric M10 coarse thread
  - 2.0 mm: Metric M12 coarse thread
  - 25.4 mm: 1 inch per revolution

#### Direction Settings
- **Normal**: Clockwise = positive movement
- **Inverted**: Counterclockwise = positive movement
- **Auto-detect**: System determines correct direction

### Display Settings

#### Update Rate
- **Range**: 50 to 1000 ms
- **Default**: 100 ms (10 Hz)
- **Recommendations**:
  - 50 ms: High-speed applications
  - 100 ms: General purpose
  - 250 ms: Battery powered
  - 500 ms: Slow processes

#### I2C Configuration
- **Address Range**: 0x20 to 0x7F
- **Default**: 0x27
- **Common Addresses**: 0x27, 0x3F
- **Scanner**: Auto-detect connected devices

### Performance Settings

#### CPU Frequency
- **Range**: 48 MHz to 266 MHz
- **Default**: 133 MHz
- **Impact**: Higher frequency = faster response, more power

#### Memory Optimization
- **Buffer Sizes**: Configurable for different applications
- **Sample Rates**: Balance between accuracy and performance
- **Overflow Protection**: Automatic buffer management

### Configuration Storage
- **Flash Memory**: Settings stored in non-volatile memory
- **Checksum Protection**: Data integrity verification
- **Backup/Restore**: Manual configuration export/import
- **Factory Reset**: Return to default settings

## Daily Operation

### Startup Routine
1. **Power On**: Connect USB power
2. **System Check**: Verify splash screen displays correctly
3. **Position Check**: Confirm encoder responds to movement
4. **Zero Reference**: Set reference position if needed

### Normal Operation
1. **Position Monitoring**: Continuous real-time display
2. **Velocity Feedback**: Monitor cutting/feed speeds
3. **Status Awareness**: Watch for error indicators
4. **Data Recording**: Manual or automatic logging

### Position Reset
1. **Quick Reset**: Short press menu encoder when in main display
2. **Confirmation**: System prompts for confirmation
3. **Zero Set**: Current position becomes new zero reference
4. **Indication**: Status shows "Zero Set" briefly

### End of Shift
1. **Final Position**: Note final position for next startup
2. **System Check**: Verify no error conditions
3. **Power Down**: Safe to disconnect USB power
4. **Documentation**: Log any issues or observations

## Maintenance

### Daily Checks
- **Display Clarity**: Clean screen if needed
- **Encoder Function**: Test rotation response
- **Connection Security**: Check for loose wires
- **Error Status**: No error indicators present

### Weekly Maintenance
- **Calibration Check**: Quick verification with known standard
- **Cleaning**: Clean encoder and display
- **Cable Inspection**: Check for wear or damage
- **Performance Review**: Check velocity and RPM readings

### Monthly Maintenance
- **Full Calibration**: Complete calibration cycle
- **Configuration Backup**: Save current settings
- **Performance Analysis**: Review diagnostic data
- **Documentation Update**: Update calibration records

### Annual Maintenance
- **Complete System Check**: Full diagnostic suite
- **Hardware Inspection**: Detailed physical examination
- **Software Update**: Check for firmware updates
- **Calibration Certification**: Professional calibration if required

### Cleaning Procedures
1. **Power Off**: Disconnect USB power
2. **Gentle Cleaning**: Use isopropyl alcohol and soft cloth
3. **Avoid Moisture**: Keep liquids away from electronics
4. **Dry Completely**: Ensure dry before reconnecting power

## Advanced Features

### Performance Monitoring
- **Real-time Diagnostics**: Encoder frequency and error rates
- **Performance Metrics**: Update rates and processing times
- **Error Logging**: Automatic error detection and logging
- **Trend Analysis**: Performance over time tracking

### Data Export
- **Position Logging**: Continuous position recording
- **CSV Export**: Compatible with spreadsheet software
- **Real-time Streaming**: USB serial data output
- **Custom Formats**: Configurable output formats

### Integration Options
- **RS485 Interface**: Multi-device networking
- **Analog Output**: 4-20mA or 0-10V position signal
- **Digital I/O**: Trigger inputs and alarm outputs
- **Protocol Support**: Modbus, CANbus integration

## Troubleshooting

### Common Issues

#### Display Problems
**Blank Display**:
- Check power connections
- Verify I2C wiring
- Try I2C address scanner

**Corrupted Characters**:
- Check I2C signal integrity
- Verify power supply stability
- Test different update rates

#### Encoder Issues
**No Response**:
- Check encoder power
- Verify signal connections
- Test with encoder diagnostics

**Wrong Direction**:
- Use direction invert setting
- Check wiring polarity
- Verify encoder type

**Incorrect Scaling**:
- Run calibration procedure
- Check encoder resolution setting
- Verify thread pitch configuration

### Error Codes
- **E01**: Encoder communication error
- **E02**: I2C communication failure
- **E03**: Configuration corruption
- **E04**: Calibration out of range
- **E05**: Hardware malfunction

### Recovery Procedures

#### Soft Reset
1. Power cycle the system
2. Check for normal startup
3. Verify basic functionality

#### Configuration Reset
1. Access Engineering Menu
2. Select Factory Reset
3. Choose reset level
4. Reconfigure as needed

#### Emergency Recovery
1. Hold menu button during power-on
2. System enters recovery mode
3. Follow on-screen instructions
4. Restore from backup if available

### Support Resources
- **Documentation**: Complete manual set
- **Video Tutorials**: Step-by-step guides
- **Community Forum**: User support community
- **Technical Support**: Professional assistance

---

**For technical support**: Contact support team with specific error descriptions and system configuration details.

**Software Version**: PEARL v0.07
**Manual Version**: 1.0
**Last Updated**: August 4, 2025
