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
    
    // Check for password timeout
    if (current_state == MenuState::PASSWORD_ENTRY) {
        if (current_time - password_timeout > (PASSWORD_TIMEOUT_MS * 1000)) {
            handle_password_timeout();
            return;
        }
    }
    
    // Check for general menu timeout
    if (current_time - last_activity > (menu_timeout * 1000)) {
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
    if (direction == MenuDirection::BACK) exit_to_main();
    else draw_submenu();
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
