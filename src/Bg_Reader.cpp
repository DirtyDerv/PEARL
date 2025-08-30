#include "engineering_menu.h"
#include "hardware/watchdog.h"
#ifndef SRAM_END
#define SRAM_END 0x20042000
#endif

// Global flag for menu button hold (used by menu editors)
bool g_menu_button_held = false;
#include <stdio.h>
#include "pico/multicore.h"
#include "pico/sync.h"
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
#include "splash_screen.h"
#include "config_manager.h"

void display_position_info(LCD_I2C& lcd, QuadratureEncoder& encoder, BigFont& big_font, StatusDisplay& status) {
    // Minimal main display: clear LCD and print position as plain number
    lcd.clear();
    float position = encoder.get_distance();
    lcd.set_cursor(0, 0);
    char buf[16];
    snprintf(buf, sizeof(buf), "Pos: %7.2f", position);
    lcd.print(buf);
}

int main() {
    // HW-040 button polling variables
    uint32_t button_last_time = 0;
    uint32_t button_last_click_time = 0;
    uint8_t button_last_state = 1; // pull-up, so HIGH = not pressed
    uint8_t button_click_count = 0;
    const uint32_t BUTTON_DEBOUNCE_MS = 10;
    const uint32_t TRIPLE_CLICK_TIMEOUT_MS = 500;
    stdio_init_all();
    
    printf("\n=== PEARL v%s ===\n", get_version_string());
    printf("Position Encoder And Readout Logic\n");
    printf("Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    printf("Starting initialization...\n");
    
    // Give USB time to initialize for serial output
    sleep_ms(2000);

    // Enable watchdog timer
    watchdog_enable(1000, 1);
    
    // LCD/I2C must be initialized before config manager
    
    // Initialize I2C for LCD
    i2c_init(i2c0, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    printf("I2C initialized on pins %d (SDA) and %d (SCL) at %d Hz\n", 
           I2C_SDA, I2C_SCL, I2C_FREQ);

    // Initialize LCD and run test output
    LCD_I2C lcd(i2c0, LCD_ADDR);
    printf("Attempting LCD initialization...\n");
    lcd.init();
    lcd.backlight_on();
    lcd.set_cursor(0, 0);
    lcd.print("HELLO");
    sleep_ms(2000); // Show test message for 2 seconds

    // Now safe to initialize config manager
    printf("Initializing configuration system...\n");
    if (!g_config_manager.init()) {
        printf("WARNING: Configuration system failed to initialize, using defaults\n");
    } else {
        printf("Configuration system initialized successfully\n");
    // Encoder debug print removed
    }

    // ...existing code...

    // ...existing code...
    // (Restore all main firmware logic after config manager initialization)
    // ...existing code...
    
    printf("LCD initialized successfully\n");
    
    // Initialize splash screen (if enabled in config)
    SplashScreen splash(&lcd);
    if (g_config_manager.get_config()->splash_enabled) {
        splash.show_startup();
        splash.show_version_info();
        
        // Now continue with detailed initialization with progress feedback
        splash.show_progress("Config Loaded", static_cast<uint8_t>(InitStep::CONFIG_INIT));
        sleep_ms(500);
        splash.show_progress("I2C Bus Scan", static_cast<uint8_t>(InitStep::I2C_SCAN));
    }
    
    // Scan I2C bus for devices
    I2CScanner scanner(i2c0);
    scanner.scan();
    
    // Test LCD address specifically
    if (scanner.test_address(LCD_ADDR)) {
        printf("LCD found at address 0x%02X ✓\n", LCD_ADDR);
        if (g_config_manager.get_config()->splash_enabled) {
            splash.show_hardware_status("LCD", true);
        }
    } else {
        printf("WARNING: No device found at LCD address 0x%02X\n", LCD_ADDR);
        printf("Check wiring or try different address (common: 0x27, 0x3F)\n");
        if (g_config_manager.get_config()->splash_enabled) {
            splash.show_hardware_status("LCD", false);
        }
    }
    
    if (g_config_manager.get_config()->splash_enabled) {
        splash.show_progress("GPIO Setup", static_cast<uint8_t>(InitStep::GPIO_INIT));
    }
    
    printf("Attempting LCD initialization...\n");
    lcd.init();
    lcd.backlight_on();
    
    if (g_config_manager.get_config()->splash_enabled) {
        splash.show_progress("GPIO Setup", static_cast<uint8_t>(InitStep::GPIO_INIT));
    }
    
    // Initialize HW-040 rotary encoder for menu control (v0.05)
    HW040Encoder menu_encoder(MENU_ENCODER_CLK, MENU_ENCODER_DT, MENU_ENCODER_SW);
    menu_encoder.init();
    
    // Menu encoder debug print removed
    
    if (g_config_manager.get_config()->splash_enabled) {
        splash.show_hardware_status("Menu Encoder", true);
        splash.show_progress("Display Setup", static_cast<uint8_t>(InitStep::LCD_INIT));
    }
    
    // Initialize big font display system
    BigFont big_font(&lcd);
    big_font.init();
    
    // Initialize status display for direction and speed
    StatusDisplay status(&lcd, &big_font);
    status.set_max_velocity_for_display(10.0f); // Set max velocity for speed bar
    status.init();
    
    printf("Big font and status display initialized\n");
    
    if (g_config_manager.get_config()->splash_enabled) {
        splash.show_hardware_status("Display Sys", true);
        splash.show_progress("Encoder Init", static_cast<uint8_t>(InitStep::ENCODER_INIT));
    }
    
    // Initialize Quadrature Encoder with configuration values
    static QuadratureEncoder encoder(pio0, 0, ENCODER_PIN_A, ENCODER_PIN_B, 
                             GET_THREAD_PITCH(), GET_ENCODER_RESOLUTION(), GET_PIO_ENABLED());
    encoder.init();

    // Shared variables for multicore
    static volatile int32_t shared_position = 0;
    static volatile float shared_distance = 0.0f;
    static volatile uint32_t shared_fifo_ovf = 0;
    static volatile uint32_t shared_inv_trans = 0;
    static mutex_t encoder_mutex;
    mutex_init(&encoder_mutex);

    // Core 1 function: update encoder as fast as possible
    auto core1_encoder_task = []() {
        while (true) {
            encoder.update();
            mutex_enter_blocking(&encoder_mutex);
            shared_position = encoder.get_raw_position();
            shared_distance = encoder.get_distance();
            shared_fifo_ovf = encoder.get_fifo_overflow_count();
            shared_inv_trans = encoder.get_invalid_transition_count();
            mutex_exit(&encoder_mutex);
        }
    };
    multicore_launch_core1(core1_encoder_task);
    
    // Main encoder debug print removed
    
    if (g_config_manager.get_config()->splash_enabled) {
        splash.show_hardware_status("Main Encoder", true);
        splash.show_progress("Menu System", static_cast<uint8_t>(InitStep::MENU_INIT));
    }
    
    // Initialize Engineering Menu System (v0.05)
    EngineeringMenu eng_menu(&lcd, &menu_encoder, &encoder, &status);
    eng_menu.init();
    
    printf("Engineering menu system initialized\n");
    
    if (g_config_manager.get_config()->splash_enabled) {
        splash.show_hardware_status("Menu System", true);
        splash.show_progress("Startup Complete", static_cast<uint8_t>(InitStep::COMPLETE));
        
        // Show ready screen
        splash.show_ready();
    }
    
    uint32_t last_update = 0;
    int32_t last_position = 0;
    uint32_t update_rate_ms = GET_UPDATE_RATE();  // Use configured update rate
    uint32_t last_movement_time = 0;
    
    printf("Starting main position monitoring loop...\n");
    printf("Commands: R=reset, S=scan, P=PIO toggle, V=velocity/perf, D=display, C=clear counters, I=version, H=help\n");
    printf("Engineering Menu: Triple-click the HW-040 encoder button within 1 second\n\n");
    
    while (true) {
        watchdog_update();
        uint32_t current_time = to_ms_since_boot(get_absolute_time());

        // Update configuration manager (handles auto-save)
        g_config_manager.update();

        // --- Use HW040Encoder for button and click detection ---
        menu_encoder.update();

        // Triple-click for engineering menu
        if (menu_encoder.check_triple_click() && !eng_menu.is_menu_active()) {
            printf("Triple-click detected - Engineering menu password entry\n");
            eng_menu.start_engineering_access();
        }

        // Long-press for user menu
        static uint32_t button_press_start = 0;
        static bool long_press_fired = false;
        const uint32_t BUTTON_HOLD_MS = 1000;
        bool button_pressed = menu_encoder.is_button_pressed();
        bool button_held = false;
        if (button_pressed && button_press_start == 0) {
            button_press_start = current_time;
            long_press_fired = false;
        }
        if (!button_pressed) {
            button_press_start = 0;
            long_press_fired = false;
        }
        if (button_pressed && button_press_start > 0 && !long_press_fired && (current_time - button_press_start > BUTTON_HOLD_MS)) {
            long_press_fired = true;
            button_held = true;
            if (!eng_menu.is_menu_active()) {
                printf("Long press detected - Activating user menu\n");
                eng_menu.start_user_menu();
            }
        }

        // Update engineering menu if active
        if (eng_menu.is_menu_active()) {
            eng_menu.update(button_pressed, button_held);
        }

        // Check for serial commands (non-blocking)
        int c = getchar_timeout_us(0);
        if (c == 'b' || c == 'B') {
            // Reboot into BOOTSEL mode (USB mass storage)
            printf("Rebooting into BOOTSEL mode...\n");
            sleep_ms(100);
            // Use watchdog to reset into BOOTSEL
            watchdog_reboot(0, SRAM_END, 1);
            while (1) { tight_loop_contents(); }
        } else if (c == 'r' || c == 'R') {
            mutex_enter_blocking(&encoder_mutex);
            encoder.reset_position();
            shared_position = 0;
            shared_distance = 0.0f;
            mutex_exit(&encoder_mutex);
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
            mutex_enter_blocking(&encoder_mutex);
            encoder.enable_pio_mode(!encoder.is_pio_enabled());
            encoder.init();  // Reinitialize with new mode
            mutex_exit(&encoder_mutex);
            printf("Switched to %s mode\n", encoder.is_pio_enabled() ? "PIO" : "GPIO");
        } else if (c == 'v' || c == 'V') {
            mutex_enter_blocking(&encoder_mutex);
            printf("\n=== Velocity & Performance Info ===\n");
            printf("Current Velocity: %.4f units/sec\n", encoder.get_velocity());
            printf("Current RPM: %.2f\n", encoder.get_rpm());
            printf("Speed %%: %.1f%% (max 10 units/sec)\n", encoder.get_speed_percentage(10.0f));
            printf("Encoder Frequency: %.2f Hz\n", encoder.get_encoder_frequency());
            printf("Transitions/sec: %lu\n", encoder.get_transitions_per_second());
            printf("Max Theoretical RPM: %.1f\n", encoder.get_max_theoretical_rpm());
            printf("\n--- Performance Stats ---\n");
            printf("FIFO Overflows: %lu\n", shared_fifo_ovf);
            printf("Invalid Transitions: %lu\n", shared_inv_trans);
            printf("Performance Warning: %s\n", encoder.has_performance_warning() ? "YES" : "No");
            printf("Mode: %s\n\n", encoder.is_pio_enabled() ? "PIO Hardware" : "GPIO Software");
            mutex_exit(&encoder_mutex);
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
            mutex_enter_blocking(&encoder_mutex);
            encoder.reset_performance_counters();
            shared_fifo_ovf = 0;
            shared_inv_trans = 0;
            mutex_exit(&encoder_mutex);
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
        } else if (c == 'f' || c == 'F') {
            static bool big_font_mode = false;
            big_font_mode = !big_font_mode;
            if (big_font_mode) {
                status.set_display_mode(DisplayMode::BIG_FONT);
                printf("Big font display mode enabled\n");
            } else {
                status.set_display_mode(DisplayMode::STANDARD);
                printf("Standard display mode enabled\n");
            }
        }

        // Update display at regular interval or when position changes significantly
        int32_t current_position;
        float current_distance;
        uint32_t current_fifo_ovf, current_inv_trans;
        mutex_enter_blocking(&encoder_mutex);
        current_position = shared_position;
        current_distance = shared_distance;
        current_fifo_ovf = shared_fifo_ovf;
        current_inv_trans = shared_inv_trans;
        mutex_exit(&encoder_mutex);

        bool position_changed = (current_position != last_position);

        if (position_changed) {
            last_movement_time = current_time;
            lcd.backlight_on();
        }

        if (current_time - last_movement_time > 15 * 60 * 1000) {
            lcd.backlight_off();
        }

        if (position_changed) {
            // Only update display if menu is NOT active
            if (!eng_menu.is_menu_active()) {
                status.update(encoder);
                last_position = current_position;
            }
        }

        // Small delay to prevent overwhelming the system
        sleep_ms(1);
    }
}
