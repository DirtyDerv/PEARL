#ifndef ENGINEERING_MENU_H
#define ENGINEERING_MENU_H

#include "pico/stdlib.h"
#include "lcd_i2c.h"
#include "hw040_encoder.h"
#include "quadrature_encoder.h"
#include "status_display.h"

// Engineering Menu System v0.05
// Comprehensive configuration and diagnostic interface

enum class MenuState {
    HIDDEN,         // Menu not active
    MAIN_MENU,      // Main menu selection
    ENCODER_CONFIG, // Encoder configuration
    DISPLAY_CONFIG, // Display settings
    SYSTEM_INFO,    // System information
    DIAGNOSTICS,    // System diagnostics
    CALIBRATION,    // Calibration routines
    PERFORMANCE,    // Performance monitoring
    FACTORY_RESET,  // Factory reset options
    SAVE_CONFIG     // Save configuration
};

enum class MainMenuItems {
    ENCODER_SETTINGS = 0,
    DISPLAY_SETTINGS,
    SYSTEM_INFORMATION,
    DIAGNOSTICS_TOOLS,
    CALIBRATION_TOOLS,
    PERFORMANCE_MONITOR,
    FACTORY_RESET,
    SAVE_AND_EXIT,
    CANCEL_EXIT,
    ITEM_COUNT
};

struct EngineeringConfig {
    // Encoder settings
    int32_t encoder_resolution;     // Pulses per revolution
    float encoder_scale_factor;     // Scaling multiplier
    bool encoder_direction_invert;  // Invert direction
    uint32_t velocity_filter_ms;    // Velocity filter time constant
    
    // Display settings
    uint8_t contrast;               // LCD contrast (if supported)
    uint8_t backlight;              // Backlight level
    uint32_t refresh_rate_ms;       // Display refresh rate
    bool show_velocity;             // Show velocity on display
    bool show_diagnostics;          // Show diagnostic info
    
    // System settings
    uint32_t error_reset_interval;  // Error counter reset interval
    bool debug_mode;                // Enable debug output
    uint8_t pio_frequency_div;      // PIO clock divider
    
    // Performance thresholds
    uint32_t max_fifo_usage;        // Maximum FIFO usage before warning
    uint32_t error_threshold;       // Error count threshold
    float velocity_threshold;       // Velocity change threshold
};

class EngineeringMenu {
private:
    LCD_I2C* lcd;
    HW040Encoder* menu_encoder;
    QuadratureEncoder* main_encoder;
    StatusDisplay* status_display;
    
    MenuState current_state;
    MainMenuItems selected_item;
    uint8_t submenu_index;
    bool menu_active;
    uint32_t menu_timeout;
    uint32_t last_activity;
    
    EngineeringConfig config;
    EngineeringConfig default_config;
    
    // Menu timeout settings
    static const uint32_t MENU_TIMEOUT_MS = 60000;  // 1 minute timeout
    static const uint32_t SUBMENU_TIMEOUT_MS = 120000; // 2 minute timeout for submenus
    
    // Menu navigation
    void handle_menu_input(MenuDirection direction);
    void draw_main_menu();
    void draw_submenu();
    void enter_submenu();
    void exit_to_main();
    void exit_menu();
    
    // Submenu handlers
    void handle_encoder_config(MenuDirection direction);
    void handle_display_config(MenuDirection direction);
    void handle_system_info(MenuDirection direction);
    void handle_diagnostics(MenuDirection direction);
    void handle_calibration(MenuDirection direction);
    void handle_performance(MenuDirection direction);
    void handle_factory_reset(MenuDirection direction);
    
    // Configuration management
    void load_default_config();
    void save_config_to_flash();
    bool load_config_from_flash();
    void apply_config();
    
    // Utility functions
    const char* get_menu_item_text(MainMenuItems item);
    void show_confirmation_dialog(const char* message);
    void show_value_editor(const char* name, int32_t* value, int32_t min, int32_t max);
    void show_float_editor(const char* name, float* value, float min, float max, float step);
    void show_bool_editor(const char* name, bool* value);

public:
    EngineeringMenu(LCD_I2C* display, HW040Encoder* encoder, 
                   QuadratureEncoder* main_enc, StatusDisplay* status);
    
    // Menu lifecycle
    bool init();
    void update();  // Call this regularly in main loop
    
    // Menu activation
    void activate_menu();
    void deactivate_menu();
    bool is_menu_active() const { return menu_active; }
    
    // Configuration access
    const EngineeringConfig& get_config() const { return config; }
    void set_config(const EngineeringConfig& new_config);
    
    // Status and diagnostics
    void show_system_status();
    void run_diagnostics();
    void show_performance_stats();
    void calibrate_encoder();
    
    // Factory reset
    void factory_reset();
    
    // Menu item descriptions for help
    const char* get_item_description(MainMenuItems item);
};

#endif // ENGINEERING_MENU_H
