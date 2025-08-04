#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "config.h"
#include "lcd_i2c.h"
#include "quadrature_encoder.h"
#include "i2c_scanner.h"
#include "big_font.h"
#include "status_display.h"
#include "version.h"
#include "hw040_encoder.h"
#include "engineering_menu.h"

void display_position_info(LCD_I2C& lcd, QuadratureEncoder& encoder, BigFont& big_font, StatusDisplay& status) {
    // Get current position as distance
    float position = encoder.get_distance();
    
    // Display the big number in ####.# format across top 3 rows
    big_font.display_big_number(position, 0, 0);
    
    // Enhanced status display with performance indicators (v0.05)
    float velocity = encoder.get_velocity();
    
    // Calculate CPU load estimate (simplified)
    float cpu_load = (encoder.get_fifo_overflow_count() + encoder.get_invalid_transition_count()) / 100.0f;
    if (cpu_load > 1.0f) cpu_load = 1.0f;
    
    // Draw enhanced status line
    status.draw_performance_status(encoder.has_performance_warning(), 
                                 encoder.get_fifo_overflow_count(), 
                                 encoder.get_invalid_transition_count());
    
    // Update standard status (direction and speed)
    status.update(encoder);
    
    // Add system health indicator
    status.draw_system_health_bar(cpu_load);
}

int main() {
    stdio_init_all();
    
    printf("\n=== PEARL v%s ===\n", get_version_string());
    printf("Position Encoder And Readout Logic\n");
    printf("Build: %s %s\n", BUILD_DATE, BUILD_TIME);
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
    
    // Initialize HW-040 rotary encoder for menu control (v0.05)
    HW040Encoder menu_encoder(MENU_ENCODER_CLK, MENU_ENCODER_DT, MENU_ENCODER_SW);
    menu_encoder.init();
    
    printf("HW-040 menu encoder initialized on pins %d (CLK), %d (DT), %d (SW)\n", 
           MENU_ENCODER_CLK, MENU_ENCODER_DT, MENU_ENCODER_SW);
    
    // Initialize big font display system
    BigFont big_font(&lcd);
    big_font.init();
    
    // Initialize status display for direction and speed
    StatusDisplay status(&lcd);
    status.set_max_velocity_for_display(10.0f); // Set max velocity for speed bar
    status.init();
    
    printf("Big font and status display initialized\n");
    
    // Initialize Quadrature Encoder
    QuadratureEncoder encoder(pio0, 0, ENCODER_PIN_A, ENCODER_PIN_B, 
                             ENCODER_PITCH, ENCODER_RESOLUTION);
    encoder.init();
    
    printf("Encoder initialized on pins %d (A) and %d (B)\n", ENCODER_PIN_A, ENCODER_PIN_B);
    printf("Configuration: Pitch=%.2f, Resolution=%d PPR\n", ENCODER_PITCH, ENCODER_RESOLUTION);
    
    // Initialize Engineering Menu System (v0.05)
    EngineeringMenu eng_menu(&lcd, &menu_encoder, &encoder, &status);
    eng_menu.init();
    
    printf("Engineering menu system initialized\n");
    
    // Display startup message
    lcd.clear();
    lcd.set_cursor(0, 0);
    lcd.printf("PEARL v%s", get_version_string());
    lcd.set_cursor(0, 1);
    lcd.print("Pos Enc & Readout");
    lcd.set_cursor(0, 2);
    lcd.printf("P:%.1f R:%d %s", ENCODER_PITCH, ENCODER_RESOLUTION, 
               encoder.is_pio_enabled() ? "PIO" : "GPIO");
    status.clear_status_line();
    lcd.set_cursor(0, 3);
    lcd.print("Ready...");
    
    printf("Displaying startup message for 3 seconds...\n");
    sleep_ms(3000); // Show startup message for 3 seconds
    
    uint32_t last_update = 0;
    int32_t last_position = 0;
    uint32_t update_rate_ms = 100;  // Default 100ms update rate
    
    printf("Starting main position monitoring loop...\n");
    printf("Commands: R=reset, S=scan, P=PIO toggle, V=velocity/perf, D=display, C=clear counters, I=version, H=help\n");
    printf("Engineering Menu: Triple-click the HW-040 encoder button within 1 second\n\n");
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Update encoder reading
        encoder.update();
        
        // Update menu encoder and check for triple-click
        menu_encoder.update();
        
        // Check for engineering menu activation (triple-click)
        if (menu_encoder.check_triple_click() && !eng_menu.is_menu_active()) {
            printf("Triple-click detected - Activating engineering menu\n");
            eng_menu.activate_menu();
        }
        
        // Update engineering menu if active
        if (eng_menu.is_menu_active()) {
            eng_menu.update();
            // Skip normal display updates while menu is active
            sleep_ms(50);  // Reduce loop rate while in menu
            continue;
        }
        
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
            printf("V - Show velocity & performance info\n");
            printf("D - Toggle display mode\n");
            printf("C - Clear performance counters\n");
            printf("M - Show menu encoder status\n");
            printf("I - Show version info\n");
            printf("H - Show this help\n");
            printf("\n=== Engineering Menu ===\n");
            printf("Triple-click HW-040 encoder to access\n\n");
        } else if (c == 'p' || c == 'P') {
            // Toggle PIO mode
            encoder.enable_pio_mode(!encoder.is_pio_enabled());
            encoder.init();  // Reinitialize with new mode
            printf("Switched to %s mode\n", encoder.is_pio_enabled() ? "PIO" : "GPIO");
        } else if (c == 'v' || c == 'V') {
            // Show detailed velocity and performance information
            printf("\n=== Velocity & Performance Info ===\n");
            printf("Current Velocity: %.4f units/sec\n", encoder.get_velocity());
            printf("Current RPM: %.2f\n", encoder.get_rpm());
            printf("Speed %%: %.1f%% (max 10 units/sec)\n", encoder.get_speed_percentage(10.0f));
            printf("Encoder Frequency: %.2f Hz\n", encoder.get_encoder_frequency());
            printf("Transitions/sec: %lu\n", encoder.get_transitions_per_second());
            printf("Max Theoretical RPM: %.1f\n", encoder.get_max_theoretical_rpm());
            printf("\n--- Performance Stats ---\n");
            printf("FIFO Overflows: %lu\n", encoder.get_fifo_overflow_count());
            printf("Invalid Transitions: %lu\n", encoder.get_invalid_transition_count());
            printf("Performance Warning: %s\n", encoder.has_performance_warning() ? "YES" : "No");
            printf("Mode: %s\n\n", encoder.is_pio_enabled() ? "PIO Hardware" : "GPIO Software");
        } else if (c == 'i' || c == 'I') {
            // Show version and build information
            printf("\n=== PEARL Version Information ===\n");
            printf("Project: Position Encoder And Readout Logic\n");
            printf("Version: %s\n", get_version_string());
            printf("Build Date: %s\n", BUILD_DATE);
            printf("Build Time: %s\n", BUILD_TIME);
            printf("Features: Big Font Display, PIO Support, Engineering Menu\n");
            printf("Hardware: Quadrature Encoder + 16x4 I2C LCD + HW-040 Menu\n\n");
        } else if (c == 'c' || c == 'C') {
            // Clear performance counters
            encoder.reset_performance_counters();
            printf("Performance counters cleared\n");
        } else if (c == 'm' || c == 'M') {
            // Show menu encoder status
            printf("\n=== HW-040 Menu Encoder Status ===\n");
            printf("Status: %s\n", menu_encoder.get_status());
            printf("Position: %ld\n", menu_encoder.get_position());
            printf("Button: %s\n", menu_encoder.is_button_pressed() ? "PRESSED" : "Released");
            printf("Click Count: %lu\n", menu_encoder.get_click_count());
            printf("Menu Active: %s\n", eng_menu.is_menu_active() ? "YES" : "No");
            printf("Hint: Triple-click to enter engineering menu\n\n");
        } else if (c == 'd' || c == 'D') {
            // Toggle display mode (future enhancement placeholder)
            printf("Big font display mode (additional modes coming soon)\n");
        }
        
        // Update display at regular interval or when position changes significantly
        int32_t current_position = encoder.get_raw_position();
        bool position_changed = (current_position != last_position);
        
        bool time_to_update = (current_time - last_update) >= update_rate_ms;
        
        if (position_changed || time_to_update) {
            display_position_info(lcd, encoder, big_font, status);
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
