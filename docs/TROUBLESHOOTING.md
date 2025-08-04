# PEARL v0.07 Troubleshooting Guide

## Table of Contents
1. [Quick Diagnostics](#quick-diagnostics)
2. [Hardware Issues](#hardware-issues)
3. [Software Problems](#software-problems)
4. [Calibration Issues](#calibration-issues)
5. [Performance Problems](#performance-problems)
6. [Error Codes](#error-codes)
7. [Recovery Procedures](#recovery-procedures)
8. [Preventive Maintenance](#preventive-maintenance)

## Quick Diagnostics

### 30-Second System Check
1. **Power LED**: Check Pico power LED is on
2. **Display**: LCD should show position/status
3. **Encoder Response**: Rotate main encoder, position should change
4. **Menu Access**: Long press should enter engineering menu
5. **No Error Messages**: Status line should show "Ready"

### Common Quick Fixes
- **Reboot**: Disconnect and reconnect USB power
- **Menu Reset**: Hold menu button during power-on
- **Cable Check**: Verify all connections are secure
- **Clean Contacts**: Clean encoder and display connections

## Hardware Issues

### Display Problems

#### Blank LCD Display
**Symptoms**: No characters visible, backlight may or may not work

**Possible Causes**:
- No power to LCD
- I2C communication failure
- Wrong I2C address
- Damaged LCD module

**Troubleshooting Steps**:
1. **Check Power**:
   - Verify 5V on LCD VCC pin
   - Check ground connection
   - Measure voltage with multimeter

2. **I2C Communication**:
   - Use I2C scanner in diagnostics menu
   - Try alternate I2C address (0x3F instead of 0x27)
   - Check SDA/SCL wiring (pins 21/22)

3. **Wiring Verification**:
   ```
   LCD → Pico
   VCC → VBUS (Pin 40, 5V)
   GND → GND (Pin 38)
   SDA → GP21 (Pin 31)
   SCL → GP22 (Pin 32)
   ```

4. **LCD Module Test**:
   - Try different LCD module
   - Check I2C backpack functionality
   - Verify LCD contrast adjustment

#### Corrupted Characters
**Symptoms**: Garbled text, random characters, flickering

**Possible Causes**:
- I2C signal interference
- Power supply instability
- Update rate too fast
- Ground loops

**Solutions**:
1. **Signal Quality**:
   - Add pull-up resistors (4.7kΩ) on SDA/SCL
   - Shorten I2C wire length
   - Use shielded cables in noisy environments

2. **Power Stability**:
   - Add decoupling capacitors near LCD
   - Use separate power supply for LCD
   - Check for voltage drops under load

3. **Software Settings**:
   - Increase display update rate (200ms+)
   - Reduce I2C clock frequency
   - Enable adaptive display mode

#### Backlight Issues
**Symptoms**: Text visible but no backlight, or can't control backlight

**Solutions**:
- Check I2C backpack jumper settings
- Verify backlight power connections
- Use software backlight control in menu
- Replace I2C backpack if faulty

### Encoder Problems

#### Main Encoder Not Responding
**Symptoms**: Position doesn't change when rotating encoder

**Troubleshooting**:
1. **Power Check**:
   - Verify 3.3V on encoder VCC
   - Check ground connection
   - Ensure clean connections

2. **Signal Verification**:
   - Check encoder A/B signal wiring
   - Verify signals toggle with rotation
   - Test with oscilloscope if available

3. **Encoder Type**:
   - Confirm quadrature encoder (not simple rotary)
   - Check PPR specification matches settings
   - Verify voltage levels (3.3V logic)

4. **Wiring Check**:
   ```
   Encoder → Pico
   VCC → 3V3 (Pin 37)
   GND → GND (Pin 4)
   A → GP0 (Pin 2)
   B → GP1 (Pin 3)
   ```

#### Wrong Direction
**Symptoms**: Encoder rotates but wrong direction

**Solutions**:
- Use direction invert in engineering menu
- Swap encoder A/B connections
- Check encoder wiring polarity

#### Erratic Position Reading
**Symptoms**: Position jumps, inconsistent readings

**Possible Causes**:
- Loose connections
- Electrical noise
- Mechanical vibration
- Encoder quality issues

**Solutions**:
1. **Connection Security**:
   - Tighten all connections
   - Use proper connectors
   - Secure cables against movement

2. **Noise Reduction**:
   - Add pull-up resistors
   - Use shielded cables
   - Filter power supply
   - Separate signal/power wiring

3. **Mechanical Issues**:
   - Check encoder mounting
   - Verify smooth rotation
   - Eliminate vibration sources
   - Check for backlash

#### Menu Encoder Problems
**Symptoms**: Cannot navigate menus, button not working

**Troubleshooting**:
1. **Button Function**:
   - Check SW pin connection (GP18)
   - Verify button makes contact when pressed
   - Test with multimeter for continuity

2. **Encoder Rotation**:
   - Check CLK/DT connections (GP19/GP20)
   - Verify rotation direction
   - Test encoder mechanically

3. **Wiring Verification**:
   ```
   HW-040 → Pico
   VCC → 3V3 (Pin 36)
   GND → GND (Pin 33)
   CLK → GP19 (Pin 29)
   DT → GP20 (Pin 30)
   SW → GP18 (Pin 28)
   ```

### Power Supply Issues

#### System Instability
**Symptoms**: Random resets, erratic behavior, display corruption

**Solutions**:
- Use quality USB power supply (minimum 1A)
- Add bulk capacitors for power filtering
- Check for voltage drops during operation
- Use shorter, thicker USB cables

#### Undervoltage Detection
**Symptoms**: System works but performance degraded

**Diagnostic**:
- Monitor VSYS voltage (should be >4.5V)
- Check power consumption under load
- Verify USB cable integrity
- Test with different power source

## Software Problems

### Boot Issues

#### System Won't Start
**Symptoms**: No splash screen, no display activity

**Troubleshooting**:
1. **Firmware Check**:
   - Verify PEARL.uf2 was flashed correctly
   - Try reflashing firmware
   - Check file integrity

2. **Hardware Reset**:
   - Hold BOOTSEL and power cycle
   - Flash known good firmware
   - Check for hardware damage

#### Stuck at Splash Screen
**Symptoms**: Splash screen appears but doesn't progress

**Possible Causes**:
- I2C initialization failure
- Encoder initialization failure
- Configuration corruption

**Solutions**:
- Try factory reset (hold menu button during power-on)
- Check all hardware connections
- Reflash firmware

### Menu System Issues

#### Cannot Enter Engineering Menu
**Symptoms**: Long press doesn't activate menu

**Solutions**:
- Check menu encoder button connection
- Try different button press duration
- Verify button polarity and pull-up
- Test button with multimeter

#### Wrong Password
**Symptoms**: Cannot pass password entry

**Solutions**:
- Default password is "1234"
- Check encoder rotation direction
- Try manual password reset procedure
- Factory reset if necessary

#### Menu Navigation Problems
**Symptoms**: Cannot navigate or select items

**Solutions**:
- Check encoder rotation response
- Verify button press registration
- Check for mechanical encoder issues
- Update encoder sensitivity settings

### Configuration Issues

#### Settings Not Saving
**Symptoms**: Configuration changes don't persist after reboot

**Possible Causes**:
- Flash memory write failure
- Configuration validation failure
- File system corruption

**Solutions**:
1. **Manual Save**:
   - Use "Save and Exit" from menu
   - Wait for confirmation message
   - Don't power off during save

2. **Flash Memory**:
   - Check available flash space
   - Try factory reset to clear corruption
   - Reflash firmware if necessary

#### Invalid Configuration
**Symptoms**: System reverts to defaults, error messages

**Solutions**:
- Check parameter ranges in menu
- Verify encoder settings match hardware
- Use configuration validation in diagnostics
- Restore from backup if available

## Calibration Issues

### Calibration Fails

#### Cannot Complete Interactive Calibration
**Symptoms**: Process stops or gives errors

**Common Causes**:
1. **Distance Too Small**: Must be ≥100mm between positions
2. **Encoder Not Responding**: Check main encoder functionality
3. **Invalid Measurements**: Check entered position values

**Solutions**:
- Increase distance between calibration points
- Verify encoder working properly
- Double-check measurement accuracy
- Use manual calibration as backup

#### Inaccurate Calibration Results
**Symptoms**: Calculated pitch seems wrong

**Troubleshooting**:
1. **Measurement Accuracy**:
   - Use precision measurement tools
   - Verify reference standard accuracy
   - Account for temperature effects

2. **Mechanical Issues**:
   - Check for backlash
   - Verify smooth movement
   - Eliminate binding or friction

3. **Systematic Errors**:
   - Check encoder resolution setting
   - Verify thread pitch specification
   - Consider leadscrew wear

### Position Accuracy Issues

#### Poor Repeatability
**Symptoms**: Measurements vary between readings

**Causes and Solutions**:
1. **Mechanical**:
   - Eliminate backlash in system
   - Improve mechanical rigidity
   - Check for worn components

2. **Electrical**:
   - Verify clean encoder signals
   - Check for interference
   - Improve grounding

3. **Environmental**:
   - Control temperature variations
   - Reduce vibration
   - Maintain clean environment

#### Systematic Offset
**Symptoms**: Consistent error in measurements

**Solutions**:
- Recalibrate with certified standards
- Check for setup errors
- Verify reference measurements
- Consider temperature compensation

## Performance Problems

### Slow Response

#### Sluggish Display Updates
**Symptoms**: Display updates slowly or appears to lag

**Solutions**:
- Reduce display update rate
- Disable unnecessary display features
- Check I2C communication speed
- Optimize software settings

#### Encoder Response Lag
**Symptoms**: Position changes lag behind encoder rotation

**Causes**:
- High encoder frequency overload
- Insufficient CPU performance
- PIO FIFO overflow

**Solutions**:
- Reduce maximum encoder speed
- Enable PIO mode for hardware acceleration
- Increase CPU frequency
- Check performance monitoring

### High Error Rates

#### FIFO Overflow Errors
**Symptoms**: Performance warnings, lost encoder counts

**Solutions**:
- Reduce encoder input frequency
- Enable hardware PIO processing
- Increase CPU frequency
- Add encoder frequency limiting

#### Communication Errors
**Symptoms**: I2C errors, display corruption

**Solutions**:
- Reduce I2C clock frequency
- Add pull-up resistors
- Check cable integrity
- Reduce electrical interference

## Error Codes

### System Error Codes

| Code | Description | Possible Causes | Solutions |
|------|-------------|-----------------|-----------|
| E01 | Encoder Communication Error | Wiring, power, encoder fault | Check connections, test encoder |
| E02 | I2C Communication Failure | Display wiring, address conflict | Verify I2C setup, scan addresses |
| E03 | Configuration Corruption | Flash memory error, validation fail | Factory reset, reflash firmware |
| E04 | Calibration Out of Range | Invalid measurements, hardware fault | Recalibrate, check mechanical system |
| E05 | Hardware Malfunction | Component failure, power issue | Check hardware, replace if needed |
| E06 | Memory Error | Insufficient RAM, corruption | Reboot, reduce memory usage |
| E07 | Validation Failed | Invalid settings, range error | Check configuration parameters |

### Performance Warnings

| Warning | Meaning | Action Required |
|---------|---------|-----------------|
| FIFO_OVERFLOW | Encoder data lost | Reduce encoder speed, enable PIO |
| HIGH_FREQUENCY | Encoder frequency too high | Limit rotation speed |
| CPU_OVERLOAD | Processor overloaded | Reduce update rates, optimize |
| TEMPERATURE | Temperature extreme | Check environment, add cooling |

## Recovery Procedures

### Emergency Recovery

#### System Unresponsive
1. **Power Cycle**: Disconnect USB, wait 10 seconds, reconnect
2. **Hardware Reset**: Hold BOOTSEL during power-on
3. **Factory Reset**: Hold menu button during power-on
4. **Firmware Reflash**: Load fresh PEARL.uf2 file

#### Configuration Recovery
1. **Load Defaults**: Use factory reset in engineering menu
2. **Backup Restore**: Use backup configuration if available
3. **Manual Reconfiguration**: Enter settings manually
4. **Calibration Recovery**: Redo calibration process

### Data Recovery

#### Configuration Backup
- Always backup working configuration
- Document calibration values
- Keep record of custom settings
- Export configuration before changes

#### Calibration Backup
- Record calibrated pitch value
- Document calibration procedure
- Keep measurement standards available
- Maintain calibration certificates

## Preventive Maintenance

### Daily Checks
- Verify display clarity and function
- Check encoder response to rotation
- Monitor for error messages
- Confirm position accuracy

### Weekly Maintenance
- Clean encoder and display
- Check connection security
- Verify calibration with standard
- Review performance metrics

### Monthly Maintenance
- Full calibration verification
- Configuration backup
- Performance analysis
- Documentation update

### Annual Maintenance
- Complete system inspection
- Certified calibration check
- Hardware replacement if needed
- Software update review

## Getting Help

### Self-Help Resources
1. **Built-in Diagnostics**: Use engineering menu diagnostics
2. **Performance Monitor**: Check real-time system status
3. **Error Logs**: Review error history in diagnostics
4. **Documentation**: Refer to complete manual set

### Professional Support
1. **Technical Documentation**: API reference and troubleshooting
2. **Community Support**: User forums and knowledge base
3. **Professional Service**: Certified calibration and repair
4. **Emergency Support**: Critical system issues

### Information to Provide
When seeking help, provide:
- **System Version**: PEARL v0.07 with build date
- **Error Codes**: Any error messages or codes
- **Configuration**: Current system settings
- **Problem Description**: Detailed symptoms and conditions
- **Hardware Setup**: Encoder types, display model, connections

---

**Remember**: Most issues are caused by simple wiring problems or configuration errors. Always check basic connections first before assuming hardware failure.

**Emergency Contact**: For critical system failures, document all symptoms and configuration before making changes.
