#ifndef ENGINEERING_MENU_H
#define ENGINEERING_MENU_H

#include "pico/stdlib.h"
#include "lcd_i2c.h"
#include "hw040_encoder.h"
#include "quadrature_encoder.h"
#include "status_display.h"
#include "config_manager.h"

// Engineering Menu System v0.05
// Comprehensive configuration and diagnostic interface

enum class MenuState {
    HIDDEN,         // Menu not active
    USER_MENU,      // User-level menu (position control only)
    PASSWORD_ENTRY, // Password entry screen
    MAIN_MENU,      // Main engineering menu selection
    ENCODER_CONFIG, // Encoder configuration
    DISPLAY_CONFIG, // Display settings
    CONFIG_MGMT,    // Configuration management
    SYSTEM_INFO,    // System information
    DIAGNOSTICS,    // System diagnostics
    CALIBRATION,    // Calibration routines
    PERFORMANCE,    // Performance monitoring
    FACTORY_RESET,  // Factory reset options
    SAVE_CONFIG,    // Save configuration
    PASSWORD_CHANGE, // Change password
    CAL_SETUP,      // Calibration setup instructions
    CAL_POSITION1,  // Set first calibration position
    CAL_MOVE_TO_POS2, // Move to second position
    CAL_POSITION2,  // Set second calibration position
    CAL_CALCULATE,  // Calculate and confirm calibration
    CAL_COMPLETE    // Calibration complete
};

enum class CalibrationState {
    INACTIVE,
    INSTRUCTIONS,   // Show calibration instructions
    SETUP_POS1,     // Setting up first position
    ADJUSTING_POS1, // User adjusting to desired position
    CONFIRM_POS1,   // Confirm first position
    MOVE_PROMPT,    // Prompt to move to second position
    SETUP_POS2,     // Setting up second position  
    ADJUSTING_POS2, // User adjusting to second position
    CONFIRM_POS2,   // Confirm second position
    CALCULATING,    // Calculating pitch
    RESULTS,        // Show calibration results
    APPLYING,       // Apply new calibration
    COMPLETE        // Calibration finished
};

struct CalibrationData {
    bool active;
    CalibrationState state;
    
    // Position data
    int32_t encoder_pos1;    // First encoder position
    float measured_pos1_mm;  // First measured position in mm
    int32_t encoder_pos2;    // Second encoder position
    float measured_pos2_mm;  // Second measured position in mm
    
    // Calculated values
    float calculated_pitch;  // Calculated thread pitch
    float old_pitch;         // Previous pitch for comparison
    float position_error;    // Error in current measurement
    
    // User interface
    uint32_t blink_timer;    // For blinking prompts
    bool show_cursor;        // Cursor visibility
    float temp_value;        // Temporary value during editing
    uint8_t decimal_place;   // Current decimal place being edited
    
    // Velocity control for position adjustment
    uint32_t last_encoder_time;
    int32_t last_encoder_pos;
    float velocity_scale;    // Dynamic velocity scaling
};

enum class UserMenuItems {
    RESET_POSITION = 0,
    SET_POSITION,
    ENGINEERING_ACCESS,
    EXIT_MENU,
    ITEM_COUNT
};

enum class MainMenuItems {
    ENCODER_SETTINGS = 0,
    DISPLAY_SETTINGS,
    CONFIGURATION_MGMT,
    SYSTEM_INFORMATION,
    DIAGNOSTICS_TOOLS,
    CALIBRATION_TOOLS,
    PERFORMANCE_MONITOR,
    CHANGE_PASSWORD,
    FACTORY_RESET,
    SAVE_AND_EXIT,
    CANCEL_EXIT,
    ITEM_COUNT
};

struct EngineeringConfig {
    // Security settings
    uint16_t access_password;       // 4-digit password (0000-9999)
    
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
    UserMenuItems selected_user_item;
    uint8_t submenu_index;
    bool menu_active;
    uint32_t menu_timeout;
    uint32_t last_activity;
    
    EngineeringConfig config;
    EngineeringConfig default_config;
    
    // Calibration system
    CalibrationData calibration;
    
    // Password entry state
    uint16_t password_entry[4];     // Current password being entered
    uint8_t password_digit_index;   // Current digit being edited (0-3)
    uint32_t password_timeout;      // Password entry timeout
    uint8_t password_attempts;      // Failed password attempts
    
    // Menu timeout settings
    static const uint32_t MENU_TIMEOUT_MS = 60000;  // 1 minute timeout
    static const uint32_t SUBMENU_TIMEOUT_MS = 120000; // 2 minute timeout for submenus
    static const uint32_t PASSWORD_TIMEOUT_MS = 30000;  // 30 second password timeout
    static const uint8_t MAX_PASSWORD_ATTEMPTS = 3;     // Lock after 3 failed attempts
    
    // Menu navigation
    void handle_menu_input(MenuDirection direction);
    void draw_main_menu();
    void draw_user_menu();
    void draw_submenu();
    void enter_submenu();
    void exit_to_main();
    void exit_menu();
    void start_user_menu();
    void start_engineering_access();
    
    // User menu handlers
    void handle_user_menu(MenuDirection direction);
    void handle_position_reset();
    void handle_position_set();
    
    // Password system
    void start_password_entry();
    void handle_password_entry(MenuDirection direction);
    void draw_password_screen();
    bool verify_password();
    void reset_password_entry();
    void handle_password_timeout();
    void draw_password_change_screen(uint16_t* new_pass, uint16_t* confirm_pass, 
                                   uint8_t digit_idx, bool confirm_mode);
    
    // Submenu handlers
    void handle_encoder_config(MenuDirection direction);
    void handle_display_config(MenuDirection direction);
    void handle_system_info(MenuDirection direction);
    void handle_diagnostics(MenuDirection direction);
    void handle_calibration(MenuDirection direction);
    void handle_performance(MenuDirection direction);
    void handle_factory_reset(MenuDirection direction);
    void handle_password_change(MenuDirection direction);
    
    // Configuration management
    void load_default_config();
    void save_config_to_flash();
    bool load_config_from_flash();
    void apply_config();
    
    // Utility functions
    const char* get_menu_item_text(MainMenuItems item);
    const char* get_user_menu_item_text(UserMenuItems item);
    void show_confirmation_dialog(const char* message);
    void show_value_editor(const char* name, int32_t* value, int32_t min, int32_t max);
    void show_float_editor(const char* name, float* value, float min, float max, float step);
    void show_bool_editor(const char* name, bool* value);
    void show_position_editor(const char* title, int32_t current_pos);
    
    // Calibration system methods
    void start_calibration();
    void handle_calibration_state(MenuDirection direction);
    void update_calibration();
    void draw_calibration_screen();
    void calculate_dynamic_velocity();
    void apply_velocity_to_position();
    void calculate_pitch_from_positions();
    void show_calibration_results();
    void apply_calibration_results();
    void reset_calibration();
    float edit_position_value(float current_value, int32_t encoder_delta);

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
