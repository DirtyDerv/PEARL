# PEARL v0.07 Calibration Guide

## Table of Contents
1. [Calibration Overview](#calibration-overview)
2. [When to Calibrate](#when-to-calibrate)
3. [Preparation](#preparation)
4. [Interactive Calibration](#interactive-calibration)
5. [Manual Calibration](#manual-calibration)
6. [Verification Procedures](#verification-procedures)
7. [Troubleshooting Calibration](#troubleshooting-calibration)
8. [Advanced Calibration](#advanced-calibration)

## Calibration Overview

### Purpose of Calibration
The calibration system determines the precise relationship between encoder counts and actual physical movement. This ensures accurate position measurements for precision machining and measurement applications.

### Calibration Method
PEARL uses a **two-point calibration system** that:
- Measures encoder counts between two known positions
- Calculates the actual thread pitch from real measurements
- Automatically corrects for mechanical variations
- Provides sub-millimeter accuracy

### Accuracy Expectations
- **Standard Calibration**: ±0.01mm typical
- **Precision Calibration**: ±0.001mm achievable
- **Long-term Stability**: ±0.1% drift per year
- **Repeatability**: ±0.005mm

## When to Calibrate

### Required Calibration
- **Initial Installation**: Always calibrate new systems
- **Hardware Changes**: After replacing encoders or mechanical components
- **Significant Drift**: When measurements consistently deviate from known standards
- **Maintenance Schedule**: Periodic recalibration per quality procedures

### Recommended Calibration
- **Monthly Check**: Quick verification with precision standard
- **Seasonal Adjustment**: Temperature-related mechanical changes
- **Process Changes**: Different operating conditions or requirements
- **Quality Audit**: Before critical measurements or inspections

### Emergency Calibration
- **Accuracy Questionable**: When measurements don't match expectations
- **After Incident**: Following mechanical impact or system malfunction
- **Customer Requirements**: Specific accuracy requirements for particular jobs

## Preparation

### Equipment Required

#### Essential Tools
- **Precision Reference**: Calibrated gage blocks, micrometer, or CMM
- **Reference Standard**: Known distance measurement (100mm+ recommended)
- **Clean Environment**: Minimize contamination and temperature variations
- **Stable Setup**: Ensure mechanical system is rigid and repeatable

#### Recommended Tools
- **Digital Caliper**: 0.001mm resolution minimum
- **Gage Blocks**: Certified precision standards
- **Dial Indicator**: For setup verification
- **Temperature Monitor**: Track environmental conditions

### Pre-Calibration Checks

#### Mechanical System
- **Backlash Check**: Verify minimal backlash in leadscrew system
- **Bearing Condition**: Ensure smooth movement without binding
- **Alignment**: Check for proper alignment of all components
- **Cleanliness**: Clean encoder and mechanical surfaces

#### Electrical System
- **Connection Security**: Verify all connections are tight
- **Power Stability**: Check for stable power supply
- **Signal Quality**: Verify clean encoder signals
- **Ground Integrity**: Ensure proper electrical grounding

#### Software Preparation
- **Backup Settings**: Save current configuration
- **Clear Errors**: Resolve any existing error conditions
- **Update Firmware**: Ensure latest software version
- **Reset Position**: Start from known zero position

### Environmental Considerations
- **Temperature Stability**: ±2°C during calibration
- **Vibration Control**: Minimize external vibrations
- **Lighting**: Adequate lighting for precision measurements
- **Documentation**: Prepare calibration record sheets

## Interactive Calibration

### Step-by-Step Procedure

#### Step 1: Access Calibration Mode
1. **Enter Engineering Menu**
   - Long press HW-040 encoder button (2+ seconds)
   - Enter password (default: 1234)
   - Navigate to **Calibration**

2. **Select Interactive Calibration**
   - Choose **Interactive Calibration** from menu
   - Read initial instructions
   - Press **OK** to begin

#### Step 2: Position 1 Setup
```
Display Shows:
┌────────────────────┐
│ Calibration Step 1 │
│ Position the       │
│ leadscrew to       │
│ first reference    │
└────────────────────┘
```

1. **Coarse Positioning**
   - Rotate main encoder to move near desired position
   - Use normal rotation speed for coarse movement
   - Dynamic velocity scaling active

2. **Fine Positioning**
   - Slow rotation for precise positioning
   - System automatically reduces sensitivity for fine adjustment
   - Velocity display shows current scaling factor

3. **Position Confirmation**
   - Position at exact reference point
   - Press **OK** when positioned accurately

#### Step 3: Position 1 Measurement Entry
```
Display Shows:
┌────────────────────┐
│ Enter Position 1:  │
│ 25.350 mm         │ ← Edit this value
│ Use encoder to edit│
│ Press OK when done │
└────────────────────┘
```

1. **Value Editing**
   - Rotate menu encoder to change value
   - Clockwise = increase, counterclockwise = decrease
   - Increment: 0.001mm per click

2. **Decimal Place Navigation**
   - Press **UP** to move between decimal places
   - Current position highlighted with cursor
   - Three decimal places available (XXX.XXX)

3. **Value Confirmation**
   - Enter precise measurement from reference tool
   - Double-check accuracy before confirming
   - Press **OK** to save position

#### Step 4: Move to Position 2
```
Display Shows:
┌────────────────────┐
│ Move to Position 2 │
│ Minimum 100mm away │
│ Distance moved:    │
│ 127.543 mm        │
└────────────────────┘
```

1. **Distance Requirements**
   - **Minimum**: 100mm separation
   - **Recommended**: 200mm+ for better accuracy
   - **Maximum**: Limited by mechanical travel

2. **Movement Direction**
   - Any direction acceptable (+ or -)
   - System tracks total distance moved
   - Real-time distance display

3. **Position Verification**
   - Ensure smooth movement without binding
   - Check for consistent encoder response
   - Press **OK** when positioned

#### Step 5: Position 2 Setup
- **Same procedure** as Position 1
- **Dynamic scaling** available for fine positioning
- **Distance tracking** shows encoder counts moved

#### Step 6: Position 2 Measurement Entry
- **Same procedure** as Position 1 entry
- **Distance validation** ensures minimum separation
- **Error checking** prevents impossible values

#### Step 7: Calculation and Results
```
Display Shows:
┌────────────────────┐
│ Calibration Results│
│ Calculated Pitch:  │
│ 2.0043 mm/rev     │
│ Apply Changes? Y/N │
└────────────────────┘
```

1. **Automatic Calculation**
   - System computes: (Distance Moved) / (Encoder Counts)
   - Accounts for encoder resolution and gear ratios
   - Displays calculated pitch value

2. **Comparison with Previous**
   - Shows old vs. new pitch values
   - Highlights significant changes
   - Calculates percentage difference

3. **Quality Check**
   - Verifies calculated pitch is reasonable
   - Checks for obvious errors
   - Warns if change is unexpected

4. **Application**
   - **Yes**: Apply new calibration immediately
   - **No**: Discard results and keep current settings
   - **Retry**: Restart calibration process

### Dynamic Velocity Control

#### How It Works
The system monitors encoder rotation speed and adjusts position sensitivity:

```
Encoder Speed → Velocity Scale → Position Movement
Slow (1-5 Hz) → 0.1x scale → Fine adjustment
Medium (5-20 Hz) → 1.0x scale → Normal movement  
Fast (20+ Hz) → 5.0x scale → Rapid positioning
```

#### Benefits
- **Efficient Positioning**: Quick movement for coarse positioning
- **Precision Control**: Fine adjustment for exact positioning
- **User Friendly**: Natural feel for experienced operators
- **Time Saving**: Reduces calibration time significantly

#### Usage Tips
- **Start Fast**: Use rapid rotation for coarse positioning
- **Slow Down**: Reduce speed as you approach target
- **Final Adjustment**: Very slow rotation for exact positioning
- **Overshoot Recovery**: Quick small movements to correct

## Manual Calibration

### When to Use Manual Calibration
- **Known Pitch**: When leadscrew pitch is precisely known
- **Quick Correction**: Minor adjustments to existing calibration
- **Maintenance**: Restoring from backup configuration
- **Verification**: Cross-checking interactive calibration results

### Procedure
1. **Access Menu**: Engineering Menu → Calibration → Manual Pitch Entry
2. **Current Value**: Display shows current pitch setting
3. **Enter New Value**: Use encoder to set precise pitch value
4. **Confirmation**: System prompts for confirmation
5. **Application**: New pitch applied immediately

### Precision Considerations
- **Manufacturer Specification**: Use certified leadscrew specifications
- **Measurement Verification**: Verify pitch with precision tools
- **Temperature Correction**: Account for thermal expansion
- **Wear Compensation**: Consider leadscrew wear over time

## Verification Procedures

### Basic Verification
1. **Known Distance Test**
   - Move to reference position
   - Reset position to zero
   - Move exactly 100.000mm using precision tools
   - Check PEARL reading: should show 100.000 ±0.010mm

2. **Repeatability Test**
   - Return to zero position
   - Repeat movement 5 times
   - Record readings
   - Calculate standard deviation (should be <0.005mm)

3. **Bidirectional Test**
   - Move +100mm, record reading
   - Move -100mm from start, record reading  
   - Values should be equal magnitude, opposite sign
   - Difference should be <0.005mm

### Advanced Verification

#### Multi-Point Verification
Test at multiple positions across full range:
- **25mm intervals**: Check linearity
- **Different speeds**: Verify velocity independence
- **Both directions**: Check for backlash compensation
- **Random positions**: Test interpolation accuracy

#### Long-Term Stability
- **Daily checks**: Monitor for drift over time
- **Temperature cycling**: Check thermal stability
- **Load variation**: Test under different loads
- **Vibration testing**: Verify performance under vibration

### Calibration Records

#### Documentation Requirements
- **Date and Time**: When calibration performed
- **Operator**: Who performed calibration
- **Equipment Used**: Reference standards and tools
- **Environmental Conditions**: Temperature, humidity
- **Results**: Before/after readings and accuracy achieved
- **Issues**: Any problems encountered during calibration

#### Quality Control
- **Calibration Certificate**: Formal calibration document
- **Traceability**: Link to national standards
- **Interval**: Next calibration due date
- **Approval**: Supervisor or quality manager signature

## Troubleshooting Calibration

### Common Issues

#### Calibration Fails to Complete
**Symptoms**: Process stops or gives error
**Possible Causes**:
- Insufficient distance between positions (<100mm)
- Encoder not responding properly
- Mechanical binding during movement
- Electrical connection problems

**Solutions**:
- Increase distance between calibration points
- Check encoder wiring and power
- Verify smooth mechanical movement
- Run encoder diagnostics

#### Inaccurate Results
**Symptoms**: Calibration gives unexpected pitch value
**Possible Causes**:
- Measurement errors in reference positions
- Mechanical backlash or play
- Encoder signal noise or errors
- Temperature effects on mechanical system

**Solutions**:
- Remeasure reference positions carefully
- Check for mechanical wear or adjustment needed
- Verify encoder signal quality
- Allow system to reach thermal equilibrium

#### Poor Repeatability
**Symptoms**: Multiple calibrations give different results
**Possible Causes**:
- Inconsistent positioning technique
- Mechanical vibration or instability
- Variable backlash or stick-slip
- Environmental changes during calibration

**Solutions**:
- Use consistent positioning technique
- Improve mechanical rigidity
- Address backlash in mechanical system
- Control environmental conditions

### Error Recovery

#### Calibration Interrupted
1. **Power Loss**: Restart calibration from beginning
2. **Button Error**: Exit to menu and restart
3. **Display Issue**: Check connections and restart
4. **Encoder Fault**: Run diagnostics and correct

#### Invalid Results
1. **Out of Range**: Check for reasonable pitch values (0.1 to 50mm)
2. **Negative Pitch**: Check encoder direction settings
3. **Zero Pitch**: Verify encoder is functioning
4. **Extreme Values**: Check reference measurements

## Advanced Calibration

### Multi-Point Calibration
For highest accuracy, use multiple calibration points:
1. **Three-Point**: Minimum for linearity check
2. **Five-Point**: Good balance of accuracy and time
3. **Ten-Point**: Maximum accuracy for critical applications

### Temperature Compensation
Account for thermal effects:
1. **Coefficient Measurement**: Determine temperature coefficient
2. **Sensor Integration**: Add temperature sensor
3. **Automatic Correction**: Real-time compensation
4. **Lookup Tables**: Discrete temperature corrections

### Automated Calibration
For production environments:
1. **Reference Fixtures**: Automated positioning
2. **Sensor Integration**: Automatic measurement
3. **Quality Gates**: Pass/fail criteria
4. **Data Logging**: Automatic record keeping

### Statistical Process Control
Monitor calibration over time:
1. **Control Charts**: Track calibration drift
2. **Trend Analysis**: Predict maintenance needs
3. **Alarm Limits**: Alert for out-of-spec conditions
4. **Batch Analysis**: Quality control for production lots

---

**Best Practices Summary**:
- Always use clean, stable reference standards
- Maintain consistent environmental conditions
- Document all calibration activities
- Verify results with independent measurements
- Schedule regular recalibration intervals

**Calibration Accuracy**: With proper procedure, PEARL can achieve ±0.001mm accuracy with appropriate reference standards and environmental control.
