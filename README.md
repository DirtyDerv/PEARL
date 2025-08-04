# Position Display System

A Raspberry Pi Pico-based position display system using a quadrature encoder and I2C LCD display.

## Features

- Real-time position monitoring using quadrature encoder
- 16x4 character LCD display via I2C
- Configurable thread pitch and encoder resolution
- Serial debug output
- 20Hz update rate for smooth display

## Hardware Requirements

- Raspberry Pi Pico
- Quadrature encoder (incremental rotary encoder)
- 16x4 I2C LCD display (with PCF8574 I2C backpack)
- Pull-up resistors (if not built into encoder)

## Wiring

### I2C LCD Display
- **VCC** → 3.3V (Pin 36)
- **GND** → GND (Pin 38)
- **SDA** → GPIO 8 (Pin 11)
- **SCL** → GPIO 9 (Pin 12)

### Quadrature Encoder
- **VCC** → 3.3V (Pin 36)
- **GND** → GND (Pin 38)
- **Channel A** → GPIO 10 (Pin 14)
- **Channel B** → GPIO 11 (Pin 15)

### Power
- Connect Pico via USB for power and programming

## Configuration

Current settings (can be modified in `Bg_Reader.cpp`):

```cpp
#define ENCODER_PITCH 1.0f       // Thread pitch (units per revolution)
#define ENCODER_RESOLUTION 500   // Pulses per revolution
```

### LCD I2C Address
Default address is `0x27`. If your LCD uses a different address (e.g., `0x3F`), modify in `lcd_i2c.h`:

```cpp
#define LCD_ADDR 0x27  // Change to your LCD's I2C address
```

## Display Layout

```
Position Display    (Line 1: Title)
Count: 1234         (Line 2: Raw encoder count)
Dist: 2.468 units   (Line 3: Calculated distance)
P:1.0 R:500         (Line 4: Pitch and Resolution)
```

## Position Calculation

The system calculates position using the formula:
```
Distance = (Encoder Count / Resolution) × Pitch
```

Where:
- **Encoder Count**: Raw pulses from the quadrature encoder
- **Resolution**: Number of pulses per complete revolution
- **Pitch**: Distance traveled per complete revolution

## Building and Flashing

1. Open the project in VS Code
2. Use the "Compile Project" task to build
3. Use the "Run Project" or "Flash" task to upload to Pico

## Troubleshooting

### LCD Not Working
1. Check I2C address using an I2C scanner
2. Verify wiring connections
3. Ensure LCD backpack is properly soldered
4. Try address `0x3F` if `0x27` doesn't work

### Encoder Not Responding
1. Check encoder wiring
2. Verify encoder type (incremental quadrature)
3. Check for proper pull-up resistors
4. Monitor serial output for position changes

### Serial Debug
Connect to the Pico's USB serial port at 115200 baud to see debug information:
- System initialization status
- Real-time position updates
- Error messages

## Customization

### Changing Encoder Resolution
Modify the `ENCODER_RESOLUTION` constant for your specific encoder:
- Common values: 100, 200, 360, 500, 1000, 2000 PPR

### Changing Thread Pitch
Modify the `ENCODER_PITCH` constant for your mechanical system:
- For lead screws: pitch = thread pitch (mm/rev)
- For rack and pinion: pitch = circumference of pinion gear
- For belt systems: pitch = belt advance per encoder revolution

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
