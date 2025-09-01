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
#include "quadrature_encoder.h"
#include "i2c_scanner.h"
#include "status_display.h"
#include "version.h"
#include "graphical_display.h"
#include "hw040_encoder.h"
#include "engineering_menu.h"
#include "splash_screen.h"
#include "config_manager.h"

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
    
    // Initialize SPI for LCD
    spi_init(spi1, 5000000); // 5 MHz
    gpio_set_function(11, GPIO_FUNC_SPI);
    gpio_set_function(10, GPIO_FUNC_SPI);

    // Initialize I2C for touch
    i2c_init(i2c0, 100000); // 100 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ST7796S lcd(spi1, 9, 25, 27, 18);
    FT6336U touch(i2c0);
    GraphicalDisplay display(&lcd, &touch);
    display.init();

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
    
    display.draw_main_ui();
    
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
    
    while (true) {
        watchdog_update();

        long count;
        double distance, velocity;
        int rpm;

        mutex_enter_blocking(&encoder_mutex);
        count = shared_position;
        distance = shared_distance;
        velocity = encoder.get_velocity();
        rpm = encoder.get_rpm();
        mutex_exit(&encoder_mutex);

        display.update_values(count, distance, velocity, rpm);
        display.handle_touch();

        sleep_ms(20);
    }
}
