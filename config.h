#ifndef CONFIG_H
#define CONFIG_H

// Pin Configuration
#define I2C_SDA 8
#define I2C_SCL 9
#define ENCODER_PIN_A 10
#define ENCODER_PIN_B 11

// System Configuration
#define ENCODER_PITCH 1.0f       // Thread pitch (units per revolution)
#define ENCODER_RESOLUTION 500   // Pulses per revolution
#define UPDATE_INTERVAL_MS 50    // Display update rate (20Hz)

// LCD Configuration
#define LCD_ADDR 0x27           // I2C address of LCD (try 0x3F if this doesn't work)
#define I2C_FREQ 400000         // I2C frequency (400kHz)

// Debug Configuration
#define ENABLE_SERIAL_DEBUG 1   // Enable/disable serial debug output

#endif // CONFIG_H
