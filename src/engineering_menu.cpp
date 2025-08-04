#include "engineering_menu.h"
#include "version.h"
#include <stdio.h>
#include <string.h>
#include <cmath>

EngineeringMenu::EngineeringMenu(LCD_I2C* display, HW040Encoder* encoder, 
                                QuadratureEncoder* main_enc, StatusDisplay* status)
    : lcd(display), menu_encoder(encoder), main_encoder(main_enc), status_display(status),
      current_state(MenuState::HIDDEN), selected_item(MainMenuItems::ENCODER_SETTINGS),
      selected_user_item(UserMenuItems::RESET_POSITION),
      submenu_index(0), menu_active(false), menu_timeout(MENU_TIMEOUT_MS), last_activity(0),
      password_digit_index(0), password_timeout(0), password_attempts(0) {
    load_default_config();
    reset_password_entry();
    reset_calibration();
}

bool EngineeringMenu::init() {
    // Try to load saved configuration
    if (!load_config_from_flash()) {
        load_default_config();
    }
    apply_config();
    return true;
}

void EngineeringMenu::load_default_config() {
    // Security settings - default password 1234
    config.access_password = 1234;
    
    // Set reasonable defaults
    config.encoder_resolution = 600;           // Typical encoder resolution
    config.encoder_scale_factor = 1.0f;        // No scaling by default
    config.encoder_direction_invert = false;   // Normal direction
    config.velocity_filter_ms = 100;           // 100ms filter
    
    config.contrast = 50;                      // 50% contrast
    config.backlight = 80;                     // 80% backlight
    config.refresh_rate_ms = 100;              // 10Hz refresh
    config.show_velocity = true;               // Show velocity
    config.show_diagnostics = false;           // Hide diagnostics by default
    
    config.error_reset_interval = 10000;       // 10 second error reset
    config.debug_mode = false;                 // Debug off by default
    config.pio_frequency_div = 1;              // No PIO frequency division
    
    config.max_fifo_usage = 75;                // 75% FIFO usage warning
    config.error_threshold = 10;               // 10 errors per interval
    config.velocity_threshold = 100.0f;        // 100 units/sec threshold
    
    default_config = config;  // Save default for factory reset
}

void EngineeringMenu::activate_menu() {
    if (!menu_active) {
        menu_active = true;
        current_state = MenuState::USER_MENU;  // Start with user menu (no password required)
        selected_user_item = UserMenuItems::RESET_POSITION;
        last_activity = time_us_32();
        
        start_user_menu();
    }
}

void EngineeringMenu::update() {
    if (!menu_active) return;
    
    uint32_t current_time = time_us_32();
    
    // Update calibration system if active
    if (calibration.active) {
        update_calibration();
    }
    
    // Check for password timeout
    if (current_state == MenuState::PASSWORD_ENTRY) {
        if (current_time - password_timeout > (PASSWORD_TIMEOUT_MS * 1000)) {
            handle_password_timeout();
            return;
        }
    }
    
    // Check for general menu timeout (but not during calibration)
    if (!calibration.active && current_time - last_activity > (menu_timeout * 1000)) {
        exit_menu();
        return;
    }
    
    // Get encoder input
    int32_t delta = menu_encoder->get_delta();
    bool button_pressed = menu_encoder->is_button_pressed();
    
    MenuDirection direction = encoder_to_menu_direction(delta, button_pressed);
    
    if (direction != MenuDirection::NONE) {
        last_activity = current_time;
        handle_menu_input(direction);
    }
    
    menu_encoder->update();
}

void EngineeringMenu::handle_menu_input(MenuDirection direction) {
    switch (current_state) {
        case MenuState::USER_MENU:
            handle_user_menu(direction);
            break;
            
        case MenuState::PASSWORD_ENTRY:
            handle_password_entry(direction);
            break;
            
        case MenuState::MAIN_MENU:
            switch (direction) {
                case MenuDirection::UP:
                    if (selected_item == MainMenuItems::ENCODER_SETTINGS) {
                        selected_item = MainMenuItems::CANCEL_EXIT;
                    } else {
                        selected_item = static_cast<MainMenuItems>(
                            static_cast<int>(selected_item) - 1);
                    }
                    draw_main_menu();
                    break;
                    
                case MenuDirection::DOWN:
                    if (selected_item == MainMenuItems::CANCEL_EXIT) {
                        selected_item = MainMenuItems::ENCODER_SETTINGS;
                    } else {
                        selected_item = static_cast<MainMenuItems>(
                            static_cast<int>(selected_item) + 1);
                    }
                    draw_main_menu();
                    break;
                    
                case MenuDirection::ENTER:
                    if (selected_item == MainMenuItems::CANCEL_EXIT) {
                        exit_menu();
                    } else if (selected_item == MainMenuItems::SAVE_AND_EXIT) {
                        save_config_to_flash();
                        apply_config();
                        exit_menu();
                    } else {
                        enter_submenu();
                    }
                    break;
                    
                case MenuDirection::BACK:
                    // Return to user menu instead of exiting completely
                    start_user_menu();
                    break;
            }
            break;
            
        case MenuState::ENCODER_CONFIG:
            handle_encoder_config(direction);
            break;
            
        case MenuState::DISPLAY_CONFIG:
            handle_display_config(direction);
            break;
            
        case MenuState::SYSTEM_INFO:
            handle_system_info(direction);
            break;
            
        case MenuState::DIAGNOSTICS:
            handle_diagnostics(direction);
            break;
            
        case MenuState::PASSWORD_CHANGE:
            handle_password_change(direction);
            break;
            
        case MenuState::CALIBRATION:
            handle_calibration(direction);
            break;
            
        case MenuState::PERFORMANCE:
            handle_performance(direction);
            break;
            
        case MenuState::FACTORY_RESET:
            handle_factory_reset(direction);
            break;
            
        default:
            break;
    }
}

void EngineeringMenu::draw_main_menu() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("PEARL MENU");
    
    // Show current selection
    lcd->set_cursor(0, 1);
    lcd->print(">");
    lcd->print(get_menu_item_text(selected_item));
    
    // Show navigation hint
    lcd->set_cursor(0, 2);
    lcd->print("Turn:Nav Enter:Sel");
    
    // Show position indicator
    lcd->set_cursor(0, 3);
    char pos_str[20];
    snprintf(pos_str, sizeof(pos_str), "%d/%d", 
             static_cast<int>(selected_item) + 1, 
             static_cast<int>(MainMenuItems::ITEM_COUNT));
    lcd->print(pos_str);
}

void EngineeringMenu::enter_submenu() {
    submenu_index = 0;
    menu_timeout = SUBMENU_TIMEOUT_MS;  // Longer timeout for submenus
    
    switch (selected_item) {
        case MainMenuItems::ENCODER_SETTINGS:
            current_state = MenuState::ENCODER_CONFIG;
            break;
        case MainMenuItems::DISPLAY_SETTINGS:
            current_state = MenuState::DISPLAY_CONFIG;
            break;
        case MainMenuItems::SYSTEM_INFORMATION:
            current_state = MenuState::SYSTEM_INFO;
            break;
        case MainMenuItems::DIAGNOSTICS_TOOLS:
            current_state = MenuState::DIAGNOSTICS;
            break;
        case MainMenuItems::CALIBRATION_TOOLS:
            current_state = MenuState::CALIBRATION;
            break;
        case MainMenuItems::PERFORMANCE_MONITOR:
            current_state = MenuState::PERFORMANCE;
            break;
        case MainMenuItems::CHANGE_PASSWORD:
            current_state = MenuState::PASSWORD_CHANGE;
            break;
        case MainMenuItems::FACTORY_RESET:
            current_state = MenuState::FACTORY_RESET;
            break;
        default:
            return;  // Invalid selection
    }
    
    draw_submenu();
}

void EngineeringMenu::handle_encoder_config(MenuDirection direction) {
    switch (direction) {
        case MenuDirection::UP:
            if (submenu_index > 0) submenu_index--;
            break;
        case MenuDirection::DOWN:
            if (submenu_index < 3) submenu_index++;  // 4 encoder settings
            break;
        case MenuDirection::ENTER:
            // Edit the selected parameter
            switch (submenu_index) {
                case 0:
                    show_value_editor("Resolution", &config.encoder_resolution, 100, 10000);
                    break;
                case 1:
                    show_float_editor("Scale Factor", &config.encoder_scale_factor, 0.1f, 10.0f, 0.1f);
                    break;
                case 2:
                    show_bool_editor("Invert Direction", &config.encoder_direction_invert);
                    break;
                case 3:
                    show_value_editor("Velocity Filter (ms)", 
                                    reinterpret_cast<int32_t*>(&config.velocity_filter_ms), 10, 1000);
                    break;
            }
            break;
        case MenuDirection::BACK:
            exit_to_main();
            return;
    }
    draw_submenu();
}

void EngineeringMenu::handle_system_info(MenuDirection direction) {
    if (direction == MenuDirection::BACK) {
        exit_to_main();
        return;
    }
    
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("PEARL INFO");
    lcd->set_cursor(0, 1);
    char version_str[20];
    sprintf(version_str, "Ver: %s", PROJECT_VERSION_STRING);
    lcd->print(version_str);
    lcd->set_cursor(0, 2);
    char build_str[20];
    sprintf(build_str, "Build: %s", BUILD_DATE);
    lcd->print(build_str);
    lcd->set_cursor(0, 3);
    lcd->print("Press to return");
}

void EngineeringMenu::show_value_editor(const char* name, int32_t* value, int32_t min, int32_t max) {
    int32_t temp_value = *value;
    bool editing = true;
    
    while (editing) {
        lcd->clear();
        lcd->set_cursor(0, 0);
        lcd->print("EDIT: ");
        lcd->print(name);
        lcd->set_cursor(0, 1);
        char value_str[20];
        sprintf(value_str, "Value: %ld", temp_value);
        lcd->print(value_str);
        lcd->set_cursor(0, 2);
        char range_str[20];
        sprintf(range_str, "Range: %ld-%ld", min, max);
        lcd->print(range_str);
        lcd->set_cursor(0, 3);
        lcd->print("Turn:Edit Hold:Save");
        
        sleep_ms(100);  // Prevent too fast updates
        
        int32_t delta = menu_encoder->get_delta();
        if (delta != 0) {
            temp_value += delta * ((max - min) / 100 + 1);  // Adaptive step size
            if (temp_value < min) temp_value = min;
            if (temp_value > max) temp_value = max;
        }
        
        if (menu_encoder->is_button_pressed()) {
            *value = temp_value;
            editing = false;
            sleep_ms(300);  // Debounce
        }
    }
    
    draw_submenu();
}

void EngineeringMenu::exit_to_main() {
    current_state = MenuState::MAIN_MENU;
    menu_timeout = MENU_TIMEOUT_MS;
    draw_main_menu();
}

void EngineeringMenu::exit_menu() {
    menu_active = false;
    current_state = MenuState::HIDDEN;
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("MENU CLOSED");
    sleep_ms(500);
    
    // Return to normal display
    // Status will be updated in the main loop
}

const char* EngineeringMenu::get_menu_item_text(MainMenuItems item) {
    switch (item) {
        case MainMenuItems::ENCODER_SETTINGS: return "Encoder Setup";
        case MainMenuItems::DISPLAY_SETTINGS: return "Display Setup";
        case MainMenuItems::SYSTEM_INFORMATION: return "System Info";
        case MainMenuItems::DIAGNOSTICS_TOOLS: return "Diagnostics";
        case MainMenuItems::CALIBRATION_TOOLS: return "Calibration";
        case MainMenuItems::PERFORMANCE_MONITOR: return "Performance";
        case MainMenuItems::CHANGE_PASSWORD: return "Change Password";
        case MainMenuItems::FACTORY_RESET: return "Factory Reset";
        case MainMenuItems::SAVE_AND_EXIT: return "Save & Exit";
        case MainMenuItems::CANCEL_EXIT: return "Cancel/Exit";
        default: return "Unknown";
    }
}

const char* EngineeringMenu::get_user_menu_item_text(UserMenuItems item) {
    switch (item) {
        case UserMenuItems::RESET_POSITION: return "Reset Position";
        case UserMenuItems::SET_POSITION: return "Set Position";
        case UserMenuItems::ENGINEERING_ACCESS: return "Engineering Menu";
        case UserMenuItems::EXIT_MENU: return "Exit Menu";
        default: return "Unknown";
    }
}

// Placeholder implementations for remaining methods
void EngineeringMenu::draw_submenu() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("SUBMENU");
    lcd->set_cursor(0, 1);
    lcd->print("Under construction");
    lcd->set_cursor(0, 3);
    lcd->print("Press to return");
}

void EngineeringMenu::handle_display_config(MenuDirection direction) {
    if (direction == MenuDirection::BACK) exit_to_main();
    else draw_submenu();
}

void EngineeringMenu::handle_diagnostics(MenuDirection direction) {
    if (direction == MenuDirection::BACK) exit_to_main();
    else draw_submenu();
}

void EngineeringMenu::handle_calibration(MenuDirection direction) {
    if (direction == MenuDirection::BACK) {
        if (calibration.active) {
            reset_calibration();
        }
        exit_to_main();
    } else if (direction == MenuDirection::ENTER) {
        if (!calibration.active) {
            start_calibration();
        } else {
            handle_calibration_state(direction);
        }
    } else {
        if (calibration.active) {
            handle_calibration_state(direction);
        } else {
            draw_submenu();
        }
    }
}

void EngineeringMenu::handle_performance(MenuDirection direction) {
    if (direction == MenuDirection::BACK) exit_to_main();
    else draw_submenu();
}

void EngineeringMenu::handle_factory_reset(MenuDirection direction) {
    if (direction == MenuDirection::BACK) exit_to_main();
    else draw_submenu();
}

void EngineeringMenu::show_float_editor(const char* name, float* value, float min, float max, float step) {
    // Simplified implementation
    lcd->clear();
    lcd->print("Float editor TODO");
    sleep_ms(1000);
    draw_submenu();
}

void EngineeringMenu::show_bool_editor(const char* name, bool* value) {
    // Simplified implementation
    lcd->clear();
    lcd->print("Bool editor TODO");
    sleep_ms(1000);
    draw_submenu();
}

void EngineeringMenu::apply_config() {
    // Apply configuration to system components
    // This will be implemented as needed
}

bool EngineeringMenu::load_config_from_flash() {
    // Flash storage implementation
    return false;  // Not implemented yet
}

void EngineeringMenu::save_config_to_flash() {
    // Flash storage implementation
}

void EngineeringMenu::set_config(const EngineeringConfig& new_config) {
    config = new_config;
}

// ============================================================================
// CALIBRATION SYSTEM IMPLEMENTATION
// ============================================================================

void EngineeringMenu::start_calibration() {
    calibration.active = true;
    calibration.state = CalibrationState::INSTRUCTIONS;
    calibration.blink_timer = 0;
    calibration.show_cursor = true;
    calibration.velocity_scale = 1.0f;
    calibration.last_encoder_time = to_ms_since_boot(get_absolute_time());
    calibration.last_encoder_pos = menu_encoder->get_position();
    
    // Store current pitch for comparison
    calibration.old_pitch = config.encoder_scale_factor;
    
    lcd->clear();
    draw_calibration_screen();
}

void EngineeringMenu::handle_calibration_state(MenuDirection direction) {
    switch (calibration.state) {
        case CalibrationState::INSTRUCTIONS:
            if (direction == MenuDirection::ENTER) {
                calibration.state = CalibrationState::SETUP_POS1;
            }
            break;
            
        case CalibrationState::SETUP_POS1:
            if (direction == MenuDirection::ENTER) {
                calibration.state = CalibrationState::ADJUSTING_POS1;
                calibration.encoder_pos1 = main_encoder->get_raw_position();
            }
            break;
            
        case CalibrationState::ADJUSTING_POS1:
            if (direction == MenuDirection::ENTER) {
                calibration.state = CalibrationState::CONFIRM_POS1;
                calibration.measured_pos1_mm = 0.0f; // Start with 0, user will adjust
                calibration.decimal_place = 0;
            }
            break;
            
        case CalibrationState::CONFIRM_POS1:
            if (direction == MenuDirection::ENTER) {
                calibration.state = CalibrationState::MOVE_PROMPT;
            } else if (direction == MenuDirection::UP || direction == MenuDirection::DOWN) {
                int32_t delta = (direction == MenuDirection::UP) ? 1 : -1;
                calibration.measured_pos1_mm = edit_position_value(calibration.measured_pos1_mm, delta);
            } else if (direction == MenuDirection::UP) { // Use UP as next digit
                calibration.decimal_place = (calibration.decimal_place + 1) % 3; // 0, 1, 2 decimal places
            }
            break;
            
        case CalibrationState::MOVE_PROMPT:
            if (direction == MenuDirection::ENTER) {
                calibration.state = CalibrationState::SETUP_POS2;
            }
            break;
            
        case CalibrationState::SETUP_POS2:
            if (direction == MenuDirection::ENTER) {
                calibration.state = CalibrationState::ADJUSTING_POS2;
                calibration.encoder_pos2 = main_encoder->get_raw_position();
            }
            break;
            
        case CalibrationState::ADJUSTING_POS2:
            if (direction == MenuDirection::ENTER) {
                calibration.state = CalibrationState::CONFIRM_POS2;
                calibration.measured_pos2_mm = calibration.measured_pos1_mm + 100.0f; // Default 100mm away
                calibration.decimal_place = 0;
            }
            break;
            
        case CalibrationState::CONFIRM_POS2:
            if (direction == MenuDirection::ENTER) {
                // Check minimum distance
                float distance = fabs(calibration.measured_pos2_mm - calibration.measured_pos1_mm);
                if (distance >= 100.0f) {
                    calibration.state = CalibrationState::CALCULATING;
                    calculate_pitch_from_positions();
                } else {
                    // Show error - need at least 100mm separation
                    lcd->clear();
                    lcd->set_cursor(0, 0);
                    lcd->print("ERROR: Need 100mm");
                    lcd->set_cursor(0, 1);
                    lcd->print("minimum distance");
                    sleep_ms(2000);
                }
            } else if (direction == MenuDirection::UP || direction == MenuDirection::DOWN) {
                int32_t delta = (direction == MenuDirection::UP) ? 1 : -1;
                calibration.measured_pos2_mm = edit_position_value(calibration.measured_pos2_mm, delta);
            } else if (direction == MenuDirection::UP) { // Use UP as next digit
                calibration.decimal_place = (calibration.decimal_place + 1) % 3;
            }
            break;
            
        case CalibrationState::RESULTS:
            if (direction == MenuDirection::ENTER) {
                calibration.state = CalibrationState::APPLYING;
                apply_calibration_results();
            } else if (direction == MenuDirection::BACK) {
                reset_calibration();
                exit_to_main();
            }
            break;
            
        case CalibrationState::COMPLETE:
            if (direction == MenuDirection::ENTER || direction == MenuDirection::BACK) {
                reset_calibration();
                exit_to_main();
            }
            break;
            
        default:
            break;
    }
    
    draw_calibration_screen();
}

void EngineeringMenu::update_calibration() {
    if (!calibration.active) return;
    
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Update blink timer for cursor
    if (current_time - calibration.blink_timer > 500) {
        calibration.show_cursor = !calibration.show_cursor;
        calibration.blink_timer = current_time;
    }
    
    // Handle dynamic velocity during position adjustment
    if (calibration.state == CalibrationState::ADJUSTING_POS1 || 
        calibration.state == CalibrationState::ADJUSTING_POS2) {
        calculate_dynamic_velocity();
        apply_velocity_to_position();
    }
}

void EngineeringMenu::calculate_dynamic_velocity() {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    int32_t current_encoder_pos = menu_encoder->get_position();
    
    uint32_t time_delta = current_time - calibration.last_encoder_time;
    int32_t pos_delta = current_encoder_pos - calibration.last_encoder_pos;
    
    if (time_delta > 50) { // Update every 50ms
        // Calculate velocity (steps per second)
        float velocity = fabs((float)pos_delta * 1000.0f / (float)time_delta);
        
        // Dynamic scaling based on velocity
        if (velocity > 20.0f) {
            calibration.velocity_scale = 10.0f; // Fast movement
        } else if (velocity > 10.0f) {
            calibration.velocity_scale = 5.0f;  // Medium movement
        } else if (velocity > 5.0f) {
            calibration.velocity_scale = 2.0f;  // Slow movement
        } else {
            calibration.velocity_scale = 1.0f;  // Precision movement
        }
        
        calibration.last_encoder_time = current_time;
        calibration.last_encoder_pos = current_encoder_pos;
    }
}

void EngineeringMenu::apply_velocity_to_position() {
    // This would be used if we had motor control
    // For now, we just simulate position changes based on encoder
    // In a real implementation, this would control stepper motors
    // or other actuators to move the backgauge
}

void EngineeringMenu::calculate_pitch_from_positions() {
    // Calculate the actual distance moved
    float actual_distance = calibration.measured_pos2_mm - calibration.measured_pos1_mm;
    
    // Calculate encoder counts difference
    int32_t encoder_counts = calibration.encoder_pos2 - calibration.encoder_pos1;
    
    if (encoder_counts != 0) {
        // Calculate pitch (distance per encoder count)
        calibration.calculated_pitch = actual_distance / (float)abs(encoder_counts);
        
        // Calculate position error with current settings
        float current_calculated_distance = (float)abs(encoder_counts) * calibration.old_pitch;
        calibration.position_error = current_calculated_distance - actual_distance;
        
        calibration.state = CalibrationState::RESULTS;
    } else {
        // Error - no movement detected
        lcd->clear();
        lcd->set_cursor(0, 0);
        lcd->print("ERROR: No movement");
        lcd->set_cursor(0, 1);
        lcd->print("detected!");
        sleep_ms(2000);
        calibration.state = CalibrationState::MOVE_PROMPT;
    }
}

void EngineeringMenu::draw_calibration_screen() {
    lcd->clear();
    
    switch (calibration.state) {
        case CalibrationState::INSTRUCTIONS:
            lcd->set_cursor(0, 0);
            lcd->print("LEADSCREW CALIB");
            lcd->set_cursor(0, 1);
            lcd->print("Press OK to start");
            break;
            
        case CalibrationState::SETUP_POS1:
            lcd->set_cursor(0, 0);
            lcd->print("STEP 1: Position");
            lcd->set_cursor(0, 1);
            lcd->print("Press OK when set");
            break;
            
        case CalibrationState::ADJUSTING_POS1:
            lcd->set_cursor(0, 0);
            lcd->print("Use HW040 to move");
            lcd->set_cursor(0, 1);
            char scale_str[20];
            sprintf(scale_str, "Scale: %.1fx", calibration.velocity_scale);
            lcd->print(scale_str);
            
            // Show current encoder position
            lcd->set_cursor(0, 2);
            char pos_str[20];
            sprintf(pos_str, "Pos: %ld", main_encoder->get_raw_position());
            lcd->print(pos_str);
            
            lcd->set_cursor(0, 3);
            lcd->print("Press OK when done");
            break;
            
        case CalibrationState::CONFIRM_POS1:
            lcd->set_cursor(0, 0);
            lcd->print("Enter position:");
            lcd->set_cursor(0, 1);
            char pos1_str[20];
            sprintf(pos1_str, "%.3f mm", calibration.measured_pos1_mm);
            lcd->print(pos1_str);
            
            // Show cursor at decimal place
            if (calibration.show_cursor) {
                int cursor_pos = strlen(pos1_str) - 4 + calibration.decimal_place;
                if (calibration.decimal_place == 0) cursor_pos = strlen(pos1_str) - 7; // Before decimal
                lcd->set_cursor(cursor_pos, 1);
                lcd->print("_");
            }
            
            lcd->set_cursor(0, 2);
            lcd->print("UP/DN: adjust");
            lcd->set_cursor(0, 3);
            lcd->print("RIGHT: next digit");
            break;
            
        case CalibrationState::MOVE_PROMPT:
            lcd->set_cursor(0, 0);
            lcd->print("STEP 2: Move to");
            lcd->set_cursor(0, 1);
            lcd->print("new position");
            lcd->set_cursor(0, 2);
            lcd->print("(100mm+ away)");
            lcd->set_cursor(0, 3);
            lcd->print("Press OK when set");
            break;
            
        case CalibrationState::SETUP_POS2: {
            lcd->set_cursor(0, 0);
            lcd->print("Ready for pos 2?");
            lcd->set_cursor(0, 1);
            char distance_str[20];
            int32_t encoder_diff = abs(main_encoder->get_raw_position() - calibration.encoder_pos1);
            sprintf(distance_str, "Moved: %ld counts", encoder_diff);
            lcd->print(distance_str);
            lcd->set_cursor(0, 2);
            lcd->print("Press OK to set");
            break;
        }
            
        case CalibrationState::ADJUSTING_POS2: {
            lcd->set_cursor(0, 0);
            lcd->print("Final position");
            lcd->set_cursor(0, 1);
            char scale_str[20];
            sprintf(scale_str, "Scale: %.1fx", calibration.velocity_scale);
            lcd->print(scale_str);
            
            lcd->set_cursor(0, 2);
            char pos_str[20];
            sprintf(pos_str, "Pos: %ld", main_encoder->get_raw_position());
            lcd->print(pos_str);
            
            lcd->set_cursor(0, 3);
            lcd->print("Press OK when done");
            break;
        }
            
        case CalibrationState::CONFIRM_POS2: {
            lcd->set_cursor(0, 0);
            lcd->print("Enter position:");
            lcd->set_cursor(0, 1);
            char pos2_str[20];
            sprintf(pos2_str, "%.3f mm", calibration.measured_pos2_mm);
            lcd->print(pos2_str);
            
            if (calibration.show_cursor) {
                int cursor_pos = strlen(pos2_str) - 4 + calibration.decimal_place;
                if (calibration.decimal_place == 0) cursor_pos = strlen(pos2_str) - 7;
                lcd->set_cursor(cursor_pos, 1);
                lcd->print("_");
            }
            
            lcd->set_cursor(0, 2);
            float distance = fabs(calibration.measured_pos2_mm - calibration.measured_pos1_mm);
            char dist_str[20];
            sprintf(dist_str, "Distance: %.1f mm", distance);
            lcd->print(dist_str);
            
            lcd->set_cursor(0, 3);
            if (distance >= 100.0f) {
                lcd->print("OK to calculate");
            } else {
                lcd->print("Need 100mm min!");
            }
            break;
        }
            
        case CalibrationState::CALCULATING: {
            lcd->set_cursor(0, 0);
            lcd->print("Calculating...");
            lcd->set_cursor(0, 1);
            lcd->print("Please wait");
            break;
        }
            
        case CalibrationState::RESULTS: {
            show_calibration_results();
            break;
        }
            
        case CalibrationState::APPLYING: {
            lcd->set_cursor(0, 0);
            lcd->print("Applying new");
            lcd->set_cursor(0, 1);
            lcd->print("calibration...");
            break;
        }
            
        case CalibrationState::COMPLETE: {
            lcd->set_cursor(0, 0);
            lcd->print("Calibration");
            lcd->set_cursor(0, 1);
            lcd->print("Complete!");
            lcd->set_cursor(0, 2);
            char new_pitch_str[20];
            sprintf(new_pitch_str, "Pitch: %.4f", calibration.calculated_pitch);
            lcd->print(new_pitch_str);
            lcd->set_cursor(0, 3);
            lcd->print("Press OK to exit");
            break;
        }
    }
}

void EngineeringMenu::show_calibration_results() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("CALIBRATION RESULTS");
    
    lcd->set_cursor(0, 1);
    char old_str[20];
    sprintf(old_str, "Old: %.4f", calibration.old_pitch);
    lcd->print(old_str);
    
    lcd->set_cursor(0, 2);
    char new_str[20];
    sprintf(new_str, "New: %.4f", calibration.calculated_pitch);
    lcd->print(new_str);
    
    lcd->set_cursor(0, 3);
    char error_str[20];
    sprintf(error_str, "Err: %.2fmm", calibration.position_error);
    lcd->print(error_str);
    
    // Show percentage improvement
    float improvement = fabs(calibration.position_error / (calibration.measured_pos2_mm - calibration.measured_pos1_mm)) * 100.0f;
    
    sleep_ms(3000); // Show results for 3 seconds
    
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Improvement:");
    lcd->set_cursor(0, 1);
    char imp_str[20];
    sprintf(imp_str, "%.1f%% error", improvement);
    lcd->print(imp_str);
    lcd->set_cursor(0, 2);
    lcd->print("Apply changes?");
    lcd->set_cursor(0, 3);
    lcd->print("OK=Yes BACK=No");
}

void EngineeringMenu::apply_calibration_results() {
    // Update the encoder scale factor with the new pitch
    config.encoder_scale_factor = calibration.calculated_pitch;
    
    // Save configuration to flash
    save_config_to_flash();
    
    // Apply the new configuration
    apply_config();
    
    calibration.state = CalibrationState::COMPLETE;
    
    // Brief confirmation
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Calibration");
    lcd->set_cursor(0, 1);
    lcd->print("Applied!");
    sleep_ms(1500);
}

void EngineeringMenu::reset_calibration() {
    calibration.active = false;
    calibration.state = CalibrationState::INACTIVE;
    calibration.encoder_pos1 = 0;
    calibration.encoder_pos2 = 0;
    calibration.measured_pos1_mm = 0.0f;
    calibration.measured_pos2_mm = 0.0f;
    calibration.calculated_pitch = 0.0f;
    calibration.position_error = 0.0f;
    calibration.velocity_scale = 1.0f;
}

float EngineeringMenu::edit_position_value(float current_value, int32_t encoder_delta) {
    float step_size;
    
    switch (calibration.decimal_place) {
        case 0: step_size = 10.0f; break;    // 10mm steps
        case 1: step_size = 1.0f; break;     // 1mm steps  
        case 2: step_size = 0.1f; break;     // 0.1mm steps
        default: step_size = 0.01f; break;   // 0.01mm steps
    }
    
    return current_value + (encoder_delta * step_size);
}

void EngineeringMenu::factory_reset() {
    config = default_config;
}

// ===== PASSWORD SYSTEM IMPLEMENTATION =====

void EngineeringMenu::start_password_entry() {
    reset_password_entry();
    current_state = MenuState::PASSWORD_ENTRY;
    password_timeout = time_us_32();
    
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("PEARL ACCESS");
    lcd->set_cursor(0, 1);
    lcd->print("Enter Password:");
    
    draw_password_screen();
}

void EngineeringMenu::handle_password_entry(MenuDirection direction) {
    switch (direction) {
        case MenuDirection::UP:
            password_entry[password_digit_index]++;
            if (password_entry[password_digit_index] > 9) {
                password_entry[password_digit_index] = 0;
            }
            draw_password_screen();
            break;
            
        case MenuDirection::DOWN:
            if (password_entry[password_digit_index] == 0) {
                password_entry[password_digit_index] = 9;
            } else {
                password_entry[password_digit_index]--;
            }
            draw_password_screen();
            break;
            
        case MenuDirection::ENTER:
            if (password_digit_index < 3) {
                // Move to next digit
                password_digit_index++;
                draw_password_screen();
            } else {
                // All 4 digits entered, verify password
                if (verify_password()) {
                    // Correct password - enter engineering menu
                    current_state = MenuState::MAIN_MENU;
                    selected_item = MainMenuItems::ENCODER_SETTINGS;
                    lcd->clear();
                    lcd->set_cursor(0, 0);
                    lcd->print("ACCESS GRANTED");
                    lcd->set_cursor(0, 1);
                    lcd->print("ENGINEERING MODE");
                    lcd->set_cursor(0, 2);
                    char version_str[20];
                    sprintf(version_str, "PEARL v%s", PROJECT_VERSION_STRING);
                    lcd->print(version_str);
                    sleep_ms(1500);
                    draw_main_menu();
                } else {
                    // Wrong password
                    password_attempts++;
                    lcd->clear();
                    lcd->set_cursor(0, 0);
                    lcd->print("INVALID PASSWORD");
                    lcd->set_cursor(0, 1);
                    if (password_attempts >= MAX_PASSWORD_ATTEMPTS) {
                        lcd->print("ACCESS LOCKED!");
                        sleep_ms(2000);
                        start_user_menu();  // Return to user menu after lockout
                        return;
                    } else {
                        char msg[20];
                        sprintf(msg, "Attempt %d/%d", password_attempts, MAX_PASSWORD_ATTEMPTS);
                        lcd->print(msg);
                        sleep_ms(1500);
                        reset_password_entry();
                        draw_password_screen();
                    }
                }
            }
            break;
            
        case MenuDirection::BACK:
            if (password_digit_index > 0) {
                // Go back to previous digit
                password_digit_index--;
                draw_password_screen();
            } else {
                // Cancel password entry - return to user menu
                start_user_menu();
            }
            break;
    }
}

void EngineeringMenu::draw_password_screen() {
    lcd->set_cursor(0, 1);
    lcd->print("Pass: ");
    
    char display_str[20] = "";
    for (int i = 0; i < 4; i++) {
        char digit_str[8];
        if (i == password_digit_index) {
            // Highlight current digit
            sprintf(digit_str, "[%d]", password_entry[i]);
        } else {
            sprintf(digit_str, "%d", password_entry[i]);
            if (i < 3) strcat(digit_str, " ");
        }
        strcat(display_str, digit_str);
    }
    lcd->print(display_str);
    
    // Show remaining time
    uint32_t elapsed = (time_us_32() - password_timeout) / 1000000;
    uint32_t remaining = (PASSWORD_TIMEOUT_MS / 1000) - elapsed;
    
    lcd->set_cursor(15, 1);
    char time_str[8];
    sprintf(time_str, "%2d", (int)remaining);
    lcd->print(time_str);
}

bool EngineeringMenu::verify_password() {
    uint16_t entered_password = password_entry[0] * 1000 + 
                               password_entry[1] * 100 + 
                               password_entry[2] * 10 + 
                               password_entry[3];
    return (entered_password == config.access_password);
}

void EngineeringMenu::reset_password_entry() {
    password_digit_index = 0;
    for (int i = 0; i < 4; i++) {
        password_entry[i] = 0;
    }
}

void EngineeringMenu::handle_password_timeout() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("PASSWORD TIMEOUT");
    lcd->set_cursor(0, 1);
    lcd->print("ACCESS DENIED");
    sleep_ms(1500);
    start_user_menu();  // Return to user menu instead of exiting
}

void EngineeringMenu::handle_password_change(MenuDirection direction) {
    static uint16_t new_password[4] = {0, 0, 0, 0};
    static uint8_t digit_index = 0;
    static bool confirm_mode = false;
    static uint16_t confirm_password[4] = {0, 0, 0, 0};
    
    switch (direction) {
        case MenuDirection::UP:
            if (confirm_mode) {
                confirm_password[digit_index]++;
                if (confirm_password[digit_index] > 9) {
                    confirm_password[digit_index] = 0;
                }
            } else {
                new_password[digit_index]++;
                if (new_password[digit_index] > 9) {
                    new_password[digit_index] = 0;
                }
            }
            draw_password_change_screen(new_password, confirm_password, digit_index, confirm_mode);
            break;
            
        case MenuDirection::DOWN:
            if (confirm_mode) {
                if (confirm_password[digit_index] == 0) {
                    confirm_password[digit_index] = 9;
                } else {
                    confirm_password[digit_index]--;
                }
            } else {
                if (new_password[digit_index] == 0) {
                    new_password[digit_index] = 9;
                } else {
                    new_password[digit_index]--;
                }
            }
            draw_password_change_screen(new_password, confirm_password, digit_index, confirm_mode);
            break;
            
        case MenuDirection::ENTER:
            if (digit_index < 3) {
                digit_index++;
                draw_password_change_screen(new_password, confirm_password, digit_index, confirm_mode);
            } else {
                if (!confirm_mode) {
                    // Switch to confirm mode
                    confirm_mode = true;
                    digit_index = 0;
                    for (int i = 0; i < 4; i++) confirm_password[i] = 0;
                    draw_password_change_screen(new_password, confirm_password, digit_index, confirm_mode);
                } else {
                    // Check if passwords match
                    bool match = true;
                    for (int i = 0; i < 4; i++) {
                        if (new_password[i] != confirm_password[i]) {
                            match = false;
                            break;
                        }
                    }
                    
                    if (match) {
                        // Update password
                        config.access_password = new_password[0] * 1000 + 
                                               new_password[1] * 100 + 
                                               new_password[2] * 10 + 
                                               new_password[3];
                        lcd->clear();
                        lcd->set_cursor(0, 0);
                        lcd->print("PASSWORD CHANGED");
                        lcd->set_cursor(0, 1);
                        lcd->print("Successfully!");
                        sleep_ms(2000);
                        exit_to_main();
                    } else {
                        lcd->clear();
                        lcd->set_cursor(0, 0);
                        lcd->print("PASSWORDS DO NOT");
                        lcd->set_cursor(0, 1);
                        lcd->print("MATCH! Try again");
                        sleep_ms(2000);
                        // Reset and start over
                        confirm_mode = false;
                        digit_index = 0;
                        for (int i = 0; i < 4; i++) {
                            new_password[i] = 0;
                            confirm_password[i] = 0;
                        }
                        draw_password_change_screen(new_password, confirm_password, digit_index, confirm_mode);
                    }
                }
            }
            break;
            
        case MenuDirection::BACK:
            if (digit_index > 0) {
                digit_index--;
                draw_password_change_screen(new_password, confirm_password, digit_index, confirm_mode);
            } else if (confirm_mode) {
                confirm_mode = false;
                digit_index = 3;  // Go back to last digit of new password
                draw_password_change_screen(new_password, confirm_password, digit_index, confirm_mode);
            } else {
                exit_to_main();
            }
            break;
    }
}

void EngineeringMenu::draw_password_change_screen(uint16_t* new_pass, uint16_t* confirm_pass, 
                                                 uint8_t digit_idx, bool confirm_mode) {
    lcd->clear();
    lcd->set_cursor(0, 0);
    
    if (confirm_mode) {
        lcd->print("Confirm New:");
        lcd->set_cursor(0, 1);
        lcd->print("Pass: ");
        
        char display_str[20] = "";
        for (int i = 0; i < 4; i++) {
            char digit_str[8];
            if (i == digit_idx) {
                sprintf(digit_str, "[%d]", confirm_pass[i]);
            } else {
                sprintf(digit_str, "%d", confirm_pass[i]);
                if (i < 3) strcat(digit_str, " ");
            }
            strcat(display_str, digit_str);
        }
        lcd->print(display_str);
    } else {
        lcd->print("Enter New:");
        lcd->set_cursor(0, 1);
        lcd->print("Pass: ");
        
        char display_str[20] = "";
        for (int i = 0; i < 4; i++) {
            char digit_str[8];
            if (i == digit_idx) {
                sprintf(digit_str, "[%d]", new_pass[i]);
            } else {
                sprintf(digit_str, "%d", new_pass[i]);
                if (i < 3) strcat(digit_str, " ");
            }
            strcat(display_str, digit_str);
        }
        lcd->print(display_str);
    }
}

// ===== USER MENU SYSTEM =====

void EngineeringMenu::start_user_menu() {
    current_state = MenuState::USER_MENU;
    selected_user_item = UserMenuItems::RESET_POSITION;
    
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("PEARL USER MENU");
    lcd->set_cursor(0, 1);
    char version_str[20];
    sprintf(version_str, "v%s", PROJECT_VERSION_STRING);
    lcd->print(version_str);
    sleep_ms(1000);
    
    draw_user_menu();
}

void EngineeringMenu::draw_user_menu() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("USER MENU");
    
    // Show current selection
    lcd->set_cursor(0, 1);
    lcd->print(">");
    lcd->print(get_user_menu_item_text(selected_user_item));
    
    // Show navigation hint
    lcd->set_cursor(0, 2);
    lcd->print("Turn:Nav Enter:Sel");
    
    // Show position indicator
    lcd->set_cursor(0, 3);
    char pos_str[20];
    snprintf(pos_str, sizeof(pos_str), "%d/%d", 
             static_cast<int>(selected_user_item) + 1, 
             static_cast<int>(UserMenuItems::ITEM_COUNT));
    lcd->print(pos_str);
}

void EngineeringMenu::handle_user_menu(MenuDirection direction) {
    switch (direction) {
        case MenuDirection::UP:
            if (selected_user_item == UserMenuItems::RESET_POSITION) {
                selected_user_item = UserMenuItems::EXIT_MENU;
            } else {
                selected_user_item = static_cast<UserMenuItems>(
                    static_cast<int>(selected_user_item) - 1);
            }
            draw_user_menu();
            break;
            
        case MenuDirection::DOWN:
            if (selected_user_item == UserMenuItems::EXIT_MENU) {
                selected_user_item = UserMenuItems::RESET_POSITION;
            } else {
                selected_user_item = static_cast<UserMenuItems>(
                    static_cast<int>(selected_user_item) + 1);
            }
            draw_user_menu();
            break;
            
        case MenuDirection::ENTER:
            switch (selected_user_item) {
                case UserMenuItems::RESET_POSITION:
                    handle_position_reset();
                    break;
                case UserMenuItems::SET_POSITION:
                    handle_position_set();
                    break;
                case UserMenuItems::ENGINEERING_ACCESS:
                    start_engineering_access();
                    break;
                case UserMenuItems::EXIT_MENU:
                    exit_menu();
                    break;
            }
            break;
            
        case MenuDirection::BACK:
            exit_menu();
            break;
    }
}

void EngineeringMenu::handle_position_reset() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("RESET POSITION");
    lcd->set_cursor(0, 1);
    lcd->print("Are you sure?");
    lcd->set_cursor(0, 2);
    lcd->print("Press: Confirm");
    lcd->set_cursor(0, 3);
    lcd->print("Turn: Cancel");
    
    // Wait for user response
    uint32_t start_time = time_us_32();
    bool waiting = true;
    
    while (waiting && (time_us_32() - start_time < 5000000)) { // 5 second timeout
        menu_encoder->update();
        
        if (menu_encoder->is_button_pressed()) {
            // Confirm reset
            main_encoder->reset_position();
            lcd->clear();
            lcd->set_cursor(0, 0);
            lcd->print("POSITION RESET");
            lcd->set_cursor(0, 1);
            lcd->print("TO ZERO");
            sleep_ms(1500);
            waiting = false;
        }
        
        if (menu_encoder->get_delta() != 0) {
            // Cancel
            lcd->clear();
            lcd->set_cursor(0, 0);
            lcd->print("RESET CANCELLED");
            sleep_ms(1000);
            waiting = false;
        }
        
        sleep_ms(50);
    }
    
    if (waiting) {
        // Timeout
        lcd->clear();
        lcd->set_cursor(0, 0);
        lcd->print("RESET TIMEOUT");
        sleep_ms(1000);
    }
    
    draw_user_menu();
}

void EngineeringMenu::handle_position_set() {
    int32_t current_pos = main_encoder->get_raw_position();
    show_position_editor("SET POSITION", current_pos);
    draw_user_menu();
}

void EngineeringMenu::show_position_editor(const char* title, int32_t current_pos) {
    int32_t new_position = current_pos;
    bool editing = true;
    
    while (editing) {
        lcd->clear();
        lcd->set_cursor(0, 0);
        lcd->print(title);
        lcd->set_cursor(0, 1);
        char current_str[20];
        sprintf(current_str, "Current: %ld", current_pos);
        lcd->print(current_str);
        lcd->set_cursor(0, 2);
        char new_str[20];
        sprintf(new_str, "New: %ld", new_position);
        lcd->print(new_str);
        lcd->set_cursor(0, 3);
        lcd->print("Turn:Edit Hold:Set");
        
        sleep_ms(100);
        
        int32_t delta = menu_encoder->get_delta();
        if (delta != 0) {
            new_position += delta;
        }
        
        if (menu_encoder->is_button_pressed()) {
            // Set new position
            main_encoder->set_position(new_position);
            lcd->clear();
            lcd->set_cursor(0, 0);
            lcd->print("POSITION SET TO");
            lcd->set_cursor(0, 1);
            char pos_str[20];
            sprintf(pos_str, "%ld", new_position);
            lcd->print(pos_str);
            sleep_ms(1500);
            editing = false;
        }
        
        menu_encoder->update();
    }
}

void EngineeringMenu::start_engineering_access() {
    current_state = MenuState::PASSWORD_ENTRY;
    password_attempts = 0;
    password_timeout = time_us_32();
    
    start_password_entry();
}
