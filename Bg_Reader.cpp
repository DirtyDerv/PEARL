#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "config.h"
#include "lcd_i2c.h"
#include "quadrature_encoder.h"
#include "i2c_scanner.h"

void display_position_info(LCD_I2C& lcd, QuadratureEncoder& encoder) {
    // Clear display and show enhanced position information
    lcd.clear();
    
    // Line 1: Title with mode indicator
    lcd.set_cursor(0, 0);
    if (encoder.is_pio_enabled()) {
        lcd.print("POS Display [PIO]");
    } else {
        lcd.print("POS Display [GPIO]");
    }
    
    // Line 2: Raw encoder count and velocity
    lcd.set_cursor(0, 1);
    lcd.printf("Cnt:%6ld V:%+.2f", encoder.get_raw_position(), encoder.get_velocity());
    
    // Line 3: Calculated distance and RPM
    lcd.set_cursor(0, 2);
    lcd.printf("Dst:%7.4f %4.0fRPM", encoder.get_distance(), encoder.get_rpm());
    
    // Line 4: Configuration info
    lcd.set_cursor(0, 3);
    lcd.printf("P:%.1f R:%lu", encoder.get_pitch(), encoder.get_resolution());
}

int main() {
    stdio_init_all();
    
    printf("\n=== Position Display System v1.0 ===\n");
    printf("Starting initialization...\n");
    
    // Give USB time to initialize for serial output
    sleep_ms(2000);
    
    // Initialize I2C for LCD
    i2c_init(i2c0, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    printf("I2C initialized on pins %d (SDA) and %d (SCL) at %d Hz\n", 
           I2C_SDA, I2C_SCL, I2C_FREQ);
    
    // Scan I2C bus for devices
    I2CScanner scanner(i2c0);
    scanner.scan();
    
    // Test LCD address specifically
    if (scanner.test_address(LCD_ADDR)) {
        printf("LCD found at address 0x%02X ✓\n", LCD_ADDR);
    } else {
        printf("WARNING: No device found at LCD address 0x%02X\n", LCD_ADDR);
        printf("Check wiring or try different address (common: 0x27, 0x3F)\n");
    }
    
    // Initialize LCD
    LCD_I2C lcd(i2c0, LCD_ADDR);
    
    printf("Attempting LCD initialization...\n");
    lcd.init();
    lcd.backlight_on();
    
    printf("LCD initialized successfully\n");
    
    // Initialize Quadrature Encoder
    QuadratureEncoder encoder(pio0, 0, ENCODER_PIN_A, ENCODER_PIN_B, 
                             ENCODER_PITCH, ENCODER_RESOLUTION);
    encoder.init();
    
    printf("Encoder initialized on pins %d (A) and %d (B)\n", ENCODER_PIN_A, ENCODER_PIN_B);
    printf("Configuration: Pitch=%.2f, Resolution=%d PPR\n", ENCODER_PITCH, ENCODER_RESOLUTION);
    
    // Display startup message
    lcd.clear();
    lcd.set_cursor(0, 0);
    lcd.print("Position Display");
    lcd.set_cursor(0, 1);
    lcd.print("System Ready!");
    lcd.set_cursor(0, 2);
    lcd.printf("P:%.1f R:%d", ENCODER_PITCH, ENCODER_RESOLUTION);
    lcd.set_cursor(0, 3);
    lcd.print("Waiting...");
    
    printf("Displaying startup message for 3 seconds...\n");
    sleep_ms(3000); // Show startup message for 3 seconds
    
    uint32_t last_update = 0;
    int32_t last_position = 0;
    
    printf("Starting main position monitoring loop...\n");
    printf("Commands: R=reset, S=scan I2C, P=toggle PIO/GPIO, V=velocity info, H=help\n\n");
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Update encoder reading
        encoder.update();
        
        // Check for serial commands (non-blocking)
        int c = getchar_timeout_us(0);
        if (c == 'r' || c == 'R') {
            encoder.reset_position();
            printf("Position reset to zero\n");
        } else if (c == 's' || c == 'S') {
            // Scan I2C bus
            scanner.scan();
        } else if (c == 'h' || c == 'H') {
            // Show help
            printf("\n=== Commands ===\n");
            printf("R - Reset position to zero\n");
            printf("S - Scan I2C bus\n");
            printf("P - Toggle PIO/GPIO mode\n");
            printf("V - Show velocity info\n");
            printf("H - Show this help\n\n");
        } else if (c == 'p' || c == 'P') {
            // Toggle PIO mode
            encoder.enable_pio_mode(!encoder.is_pio_enabled());
            encoder.init();  // Reinitialize with new mode
            printf("Switched to %s mode\n", encoder.is_pio_enabled() ? "PIO" : "GPIO");
        } else if (c == 'v' || c == 'V') {
            // Show detailed velocity information
            printf("\n=== Velocity Info ===\n");
            printf("Current Velocity: %.4f units/sec\n", encoder.get_velocity());
            printf("Current RPM: %.2f\n", encoder.get_rpm());
            printf("Speed %%: %.1f%% (max 10 units/sec)\n", encoder.get_speed_percentage(10.0f));
            printf("Mode: %s\n\n", encoder.is_pio_enabled() ? "PIO Hardware" : "GPIO Software");
        }
        
        // Update display at specified interval or when position changes significantly
        int32_t current_position = encoder.get_raw_position();
        bool position_changed = (current_position != last_position);
        bool time_to_update = (current_time - last_update) >= UPDATE_INTERVAL_MS;
        
        if (position_changed || time_to_update) {
            display_position_info(lcd, encoder);
            last_update = current_time;
            
            // Print to serial for debugging (only when position changes)
            if (position_changed && ENABLE_SERIAL_DEBUG) {
                float update_rate = 1000.0f / (current_time - last_update + 1);
                printf("Pos: %6ld | Dist: %8.4f | Vel: %+6.3f u/s | RPM: %6.1f | Rate: %.1f Hz\n", 
                       current_position, encoder.get_distance(), encoder.get_velocity(),
                       encoder.get_rpm(), update_rate);
            }
            
            last_position = current_position;
        }
        
        // Small delay to prevent overwhelming the system
        sleep_ms(1);
    }
}
