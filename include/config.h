#ifndef CONFIG_H
#define CONFIG_H

// Pin Configuration
#define I2C_SDA 8               // CONFIGURATION FIX: Updated to match docs
#define I2C_SCL 9               // CONFIGURATION FIX: Updated to match docs  
#define ENCODER_PIN_A 10
#define ENCODER_PIN_B 11

// HW-040 Rotary Encoder Menu Control (v0.05)
#define MENU_ENCODER_CLK 12     // HW-040 CLK pin (A)
#define MENU_ENCODER_DT 13      // HW-040 DT pin (B)  
#define MENU_ENCODER_SW 14      // HW-040 SW pin (Push button)

// System Configuration
#define ENCODER_PITCH 1.0f       // Thread pitch (units per revolution)
#define ENCODER_RESOLUTION 500   // Pulses per revolution
#define UPDATE_INTERVAL_MS 10    // Display update rate (100Hz)

// LCD Configuration
#define LCD_ADDR 0x27           // I2C address of LCD (try 0x3F if this doesn't work)
#define I2C_FREQ 400000         // I2C frequency (400kHz)

// Debug Configuration
#define ENABLE_SERIAL_DEBUG 1   // Enable/disable serial debug output

#endif // CONFIG_H
