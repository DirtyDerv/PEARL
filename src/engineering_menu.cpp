
#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include "engineering_menu.h"
#include "version.h"

// --- Calibration Flow Implementation (now uses CalibrationState) ---
// NOTE: The old handle_calibration_flow with static variables has been removed
// This simple wrapper routes to the new handle_calibration_state system
void EngineeringMenu::handle_calibration_flow(MenuDirection direction) {
    if (!calibration.active) {
        start_calibration();
    }
    handle_calibration_state(direction);
}

void EngineeringMenu::draw_calibration_set_pos1(float pos_mm) {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Step 2: Set Pos 1");
    lcd->set_cursor(0, 1);
    lcd->print("Edit position (mm):");
    lcd->set_cursor(0, 2);
    char buf[20];
    snprintf(buf, sizeof(buf), "%.2f", pos_mm);
    lcd->print(buf);
    lcd->set_cursor(0, 3);
    lcd->print("Enter=OK Back=Menu");
}

void EngineeringMenu::draw_calibration_move_prompt() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Step 3: Move");
    lcd->set_cursor(0, 1);
    lcd->print("Move to end pos");
    lcd->set_cursor(0, 2);
    lcd->print("Press Enter...");
}

void EngineeringMenu::draw_calibration_set_pos2(float pos_mm) {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Step 4: Set Pos 2");
    lcd->set_cursor(0, 1);
    lcd->print("Edit position (mm):");
    lcd->set_cursor(0, 2);
    char buf[20];
    snprintf(buf, sizeof(buf), "%.2f", pos_mm);
    lcd->print(buf);
    lcd->set_cursor(0, 3);
    lcd->print("Enter=OK Back=Menu");
}

void EngineeringMenu::draw_calibration_calculate() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Step 5: Calculate");
    lcd->set_cursor(0, 1);
    lcd->print("Calculating...");
    lcd->set_cursor(0, 2);
    char buf[32];
    snprintf(buf, sizeof(buf), "Pitch: %.3f", calibration.calculated_pitch);
    lcd->print(buf);
    lcd->set_cursor(0, 3);
    lcd->print("Enter=OK Back=Edit");
}

void EngineeringMenu::draw_calibration_review() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Review Calibration");
    lcd->set_cursor(0, 1);
    char buf[32];
    snprintf(buf, sizeof(buf), "Pitch: %.3f", calibration.calculated_pitch);
    lcd->print(buf);
    lcd->set_cursor(0, 2);
    lcd->print("Enter=Accept Back=Undo");
}

void EngineeringMenu::draw_calibration_complete(bool saved) {
    lcd->clear();
    lcd->set_cursor(0, 0);
    if (saved) {
        lcd->print("Calibration Saved!");
    } else {
        lcd->print("Calibration Canceled");
    }
    lcd->set_cursor(0, 1);
    lcd->print("Returning...");
}

void EngineeringMenu::start_value_editor(const char* name, long* value, long min, long max) {
    // TODO: Implement non-blocking value editor logic here
}
// Show the About submenu: project name, build date, version, author
void EngineeringMenu::show_about_submenu() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("PEARL Project");
    lcd->set_cursor(0, 1);
    char build_str[32];
    snprintf(build_str, sizeof(build_str), "Built: %s", BUILD_DATE);
    lcd->print(build_str);
    lcd->set_cursor(0, 2);
    char ver_str[32];
    snprintf(ver_str, sizeof(ver_str), "Ver: %s", PROJECT_VERSION_STRING);
    lcd->print(ver_str);
    lcd->set_cursor(0, 3);
    lcd->print("By: Saen Wood");
    sleep_ms(2000);
}

// ...existing code...

// ...existing code...


EngineeringMenu::EngineeringMenu(LCD_I2C* display, HW040Encoder* encoder,
                                QuadratureEncoder* main_enc, StatusDisplay* status)
    : lcd(display), menu_encoder(encoder), main_encoder(main_enc), status_display(status),
      current_state(MenuState::HIDDEN), selected_item(MainMenuItems::ENCODER_SETTINGS),
      selected_user_item(UserMenuItems::RESET_POSITION),
      submenu_index(0), menu_active(false), menu_timeout(MENU_TIMEOUT_MS), last_activity(0),
      password_digit_index(0), password_timeout(0), password_attempts(0),
      cal_step(0), temp_pos1(0.0f), temp_pos2(0.0f), last_encoder(0),
      eng_selected(MainMenuItems::ENCODER_SETTINGS), set_params_selected(0),
      factory_reset_confirm(false), password_change_digit_index(0), password_confirm_mode(false) {
    for (int i = 0; i < 4; ++i) {
        password_entry[i] = 0;
        new_password[i] = 0;
        confirm_password[i] = 0;
    }
    float_edit_ctx.active = false;
    bool_edit_ctx.active = false;
    position_edit_ctx.active = false;
    reset_ctx.waiting = false;
    load_default_config();
    reset_password_entry();
    reset_calibration();
}

void EngineeringMenu::reset_menu_state() {
    // Reset all state variables to prevent corruption between menu sessions
    cal_step = 0;
    temp_pos1 = 0.0f;
    temp_pos2 = 0.0f;
    last_encoder = 0;
    eng_selected = MainMenuItems::ENCODER_SETTINGS;
    set_params_selected = 0;
    factory_reset_confirm = false;
    password_change_digit_index = 0;
    password_confirm_mode = false;

    for (int i = 0; i < 4; ++i) {
        new_password[i] = 0;
        confirm_password[i] = 0;
    }

    float_edit_ctx.active = false;
    bool_edit_ctx.active = false;
    position_edit_ctx.active = false;
    reset_ctx.waiting = false;

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
        current_state = MenuState::PASSWORD_ENTRY;  // Always start with password entry
        selected_item = MainMenuItems::ENCODER_SETTINGS;
        last_activity = time_us_32();
        start_password_entry();
    }
}

void EngineeringMenu::update(bool button_pressed, bool button_held) {
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
        }
    }

    // Map encoder/button input to menu direction
    MenuDirection direction = MenuDirection::NONE;
    int32_t delta = menu_encoder->get_delta();
    if (current_state == MenuState::PASSWORD_ENTRY) {
        // Only button press sets ENTER, never rotation
        if (button_pressed) {
            direction = MenuDirection::ENTER;
        } else if (button_held) {
            direction = MenuDirection::BACK;
        } else if (delta > 0) {
            direction = MenuDirection::UP;
        } else if (delta < 0) {
            direction = MenuDirection::DOWN;
        }
    } else {
        if (delta > 0) direction = MenuDirection::UP;
        else if (delta < 0) direction = MenuDirection::DOWN;
        else if (button_pressed) direction = MenuDirection::ENTER;
        else if (button_held) direction = MenuDirection::BACK;
    }

    if (direction != MenuDirection::NONE) {
        handle_menu_input(direction);
    }
}

void EngineeringMenu::handle_menu_input(MenuDirection direction) {
    // Check if any non-blocking editor is active and route input to it
    if (float_edit_ctx.active) {
        update_float_editor(direction);
        return;
    }
    if (bool_edit_ctx.active) {
        update_bool_editor(direction);
        return;
    }
    if (position_edit_ctx.active) {
        update_position_editor(direction);
        return;
    }
    if (reset_ctx.waiting) {
        update_position_reset(direction);
        return;
    }

    switch (current_state) {
        case MenuState::PASSWORD_ENTRY:
            handle_password_entry(direction);
            break;
        case MenuState::USER_MENU:
            handle_user_menu(direction);
            break;
        case MenuState::MAIN_MENU:
            handle_engineer_main_menu(direction);
            break;
        case MenuState::ENG_SET_PARAMS:
            handle_set_params_menu(direction);
            break;
        case MenuState::ENG_CALIBRATE:
            handle_calibration_flow(direction);
            break;
        case MenuState::PASSWORD_CHANGE:
            handle_password_change(direction);
            break;
        // ...existing code...
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
        case MenuState::CALIBRATION:
            handle_calibration(direction);
            break;
        case MenuState::PERFORMANCE:
            handle_performance(direction);
            break;
        case MenuState::FACTORY_RESET:
            handle_factory_reset_confirm(direction);
            break;
        default:
            break;
    }
}
// Engineer Main Menu handler using MainMenuItems
void EngineeringMenu::handle_engineer_main_menu(MenuDirection direction) {
    int max_item = static_cast<int>(MainMenuItems::ITEM_COUNT) - 1;
    switch (direction) {
        case MenuDirection::UP:
            if (eng_selected > MainMenuItems::ENCODER_SETTINGS) eng_selected = static_cast<MainMenuItems>(static_cast<int>(eng_selected) - 1);
            break;
        case MenuDirection::DOWN:
            if (static_cast<int>(eng_selected) < max_item) eng_selected = static_cast<MainMenuItems>(static_cast<int>(eng_selected) + 1);
            break;
        case MenuDirection::ENTER:
            switch (eng_selected) {
                case MainMenuItems::ENCODER_SETTINGS:
                    current_state = MenuState::ENG_CALIBRATE;
                    draw_calibration_intro();
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
                case MainMenuItems::SAVE_AND_EXIT:
                    current_state = MenuState::SAVE_AND_EXIT_CONFIRM;
                    break;
                case MainMenuItems::CANCEL_EXIT:
                    start_user_menu();
                    break;
                default:
                    break;
            }
            break;
        case MenuDirection::BACK:
            start_user_menu();
            break;
        default:
            break;
    }
    if (current_state == MenuState::MAIN_MENU)
        draw_engineer_main_menu(eng_selected);
}

// Draw calibration intro screen (step 1 of calibration flow)
void EngineeringMenu::draw_calibration_intro() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("CALIBRATION");
    lcd->set_cursor(0, 1);
    lcd->print("Step 1: Setup");
    lcd->set_cursor(0, 2);
    lcd->print("Follow on-screen");
    lcd->set_cursor(0, 3);
    lcd->print("instructions...");
    // In a real flow, advance on ENTER, BACK returns to menu
}

void EngineeringMenu::draw_engineer_main_menu(MainMenuItems selected) {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("ENGINEER MENU");
    for (int i = 0; i < static_cast<int>(MainMenuItems::ITEM_COUNT); ++i) {
        lcd->set_cursor(0, i+1);
        if (selected == static_cast<MainMenuItems>(i)) lcd->print("> ");
        else lcd->print("  ");
        switch (static_cast<MainMenuItems>(i)) {
            case MainMenuItems::ENCODER_SETTINGS: lcd->print("Encoder Settings"); break;
            case MainMenuItems::DISPLAY_SETTINGS: lcd->print("Display Settings"); break;
            case MainMenuItems::SYSTEM_INFORMATION: lcd->print("System Info"); break;
            case MainMenuItems::DIAGNOSTICS_TOOLS: lcd->print("Diagnostics"); break;
            case MainMenuItems::CALIBRATION_TOOLS: lcd->print("Calibration"); break;
            case MainMenuItems::PERFORMANCE_MONITOR: lcd->print("Performance"); break;
            case MainMenuItems::CHANGE_PASSWORD: lcd->print("Change Password"); break;
            case MainMenuItems::FACTORY_RESET: lcd->print("Factory Reset"); break;
            case MainMenuItems::SAVE_AND_EXIT: lcd->print("Save & Exit"); break;
            case MainMenuItems::CANCEL_EXIT: lcd->print("Cancel/Exit"); break;
            default: break;
        }
    }
}
// End of draw_engineer_main_menu

// Set Params submenu handler with grouping and descriptions
void EngineeringMenu::handle_set_params_menu(MenuDirection direction) {
    const int set_params_count = 5; // Encoder, Display, System, Security, Back
    switch (direction) {
        case MenuDirection::UP:
            if (set_params_selected > 0) set_params_selected--;
            break;
        case MenuDirection::DOWN:
            if (set_params_selected < set_params_count - 1) set_params_selected++;
            break;
        case MenuDirection::ENTER:
            switch (set_params_selected) {
                case 0: // Encoder
                    handle_encoder_config(MenuDirection::NONE);
                    break;
                case 1: // Display
                    handle_display_config(MenuDirection::NONE);
                    break;
                case 2: // System
                    // Placeholder: could add system settings here
                    lcd->clear();
                    lcd->set_cursor(0, 0);
                    lcd->print("System Settings\nComing soon");
                    sleep_ms(1000);
                    break;
                case 3: // Security
                    current_state = MenuState::PASSWORD_CHANGE;
                    break;
                case 4: // Back
                    current_state = MenuState::MAIN_MENU;
                    break;
                default:
                    break;
            }
            break;
        case MenuDirection::BACK:
            current_state = MenuState::MAIN_MENU;
            break;
        default:
            break;
    }
    draw_set_params_menu(set_params_selected);
}

void EngineeringMenu::draw_set_params_menu(int selected) {
    const char* items[] = {
        "Encoder Settings",
        "Display Settings",
        "System Settings",
        "Security (Password)",
        "< Back"
    };
    const char* descs[] = {
        "Resolution, scale, dir",
        "Contrast, backlight",
        "Timeouts, debug, etc.",
        "Change access password",
        "Return to menu"
    };
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("SET PARAMS");
    for (int i = 0; i < 5; ++i) {
        lcd->set_cursor(0, i+1);
        if (selected == i) lcd->print("> ");
        else lcd->print("  ");
        lcd->print(items[i]);
    }
    // Show description for selected item
    lcd->set_cursor(0, 4);
    lcd->print(descs[selected]);
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
        case MenuDirection::BACK:
            exit_to_main();
            return;
        default:
            break;
    }
    draw_encoder_config();

// --- Confirmation Dialogs for Critical Actions ---
}

void EngineeringMenu::handle_factory_reset_confirm(MenuDirection direction) {
    if (!factory_reset_confirm) {
        lcd->clear();
        lcd->set_cursor(0, 0);
        lcd->print("Confirm Factory Reset?");
        lcd->set_cursor(0, 1);
        lcd->print("Enter=Yes Back=No");
        factory_reset_confirm = true;
        return;
    }
    if (direction == MenuDirection::ENTER) {
        factory_reset();
        lcd->clear();
        lcd->set_cursor(0, 0);
        lcd->print("Factory Reset Done");
        sleep_ms(1500);
        factory_reset_confirm = false;
        current_state = MenuState::MAIN_MENU;
        handle_engineer_main_menu(MenuDirection::NONE);
    } else if (direction == MenuDirection::BACK) {
        factory_reset_confirm = false;
        current_state = MenuState::MAIN_MENU;
        handle_engineer_main_menu(MenuDirection::NONE);
    }
}
void EngineeringMenu::draw_encoder_config() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("ENCODER SETUP");
    // Show each setting, highlight current selection
    for (int i = 0; i < 4; ++i) {
        lcd->set_cursor(0, i+1);
        if (submenu_index == i) lcd->print("> ");
        else lcd->print("  ");
        char buf[16];
        switch (i) {
            case 0:
                snprintf(buf, sizeof(buf), "Res: %d", config.encoder_resolution);
                lcd->print(buf);
                break;
            case 1:
                snprintf(buf, sizeof(buf), "Scale: %.2f", config.encoder_scale_factor);
                lcd->print(buf);
                break;
            case 2:
                lcd->print("Dir: ");
                lcd->print(config.encoder_direction_invert ? "Inv" : "Norm");
                break;
            case 3:
                snprintf(buf, sizeof(buf), "Filt: %dms", config.velocity_filter_ms);
                lcd->print(buf);
                break;
        }
    }
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



void EngineeringMenu::exit_to_main() {
    current_state = MenuState::MAIN_MENU;
    menu_timeout = MENU_TIMEOUT_MS;
    draw_main_menu();
}

void EngineeringMenu::exit_menu() {
    reset_menu_state();  // Clear all state to prevent corruption
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
    case UserMenuItems::ABOUT: return "About";
    case UserMenuItems::EXIT_MENU: return "Exit Menu";
        default: return "Unknown";
    }
}

// Placeholder implementations for remaining methods
void EngineeringMenu::draw_submenu() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("No options here");
    lcd->set_cursor(0, 2);
    lcd->print("Press to return");
}

void EngineeringMenu::handle_display_config(MenuDirection direction) {
    if (direction == MenuDirection::BACK) {
        exit_to_main();
        return;
    }
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("DISPLAY SETTINGS");
    lcd->set_cursor(0, 1);
    char buf[20];
    snprintf(buf, sizeof(buf), "Contrast: %d", config.contrast);
    lcd->print(buf);
    lcd->set_cursor(0, 2);
    snprintf(buf, sizeof(buf), "Backlight: %d", config.backlight);
    lcd->print(buf);
    lcd->set_cursor(0, 3);
    lcd->print("Press to return");
}

void EngineeringMenu::handle_diagnostics(MenuDirection direction) {
    if (direction == MenuDirection::BACK) {
        exit_to_main();
        return;
    }
    if (current_state == MenuState::PASSWORD_ENTRY) return; // Do not update diagnostics during password entry
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("DIAGNOSTICS");
    lcd->set_cursor(0, 1);
    char buf[20];
    snprintf(buf, sizeof(buf), "Enc: %ld", main_encoder ? main_encoder->get_raw_position() : 0L);
    lcd->print(buf);
    lcd->set_cursor(0, 2);
    // Error and FIFO usage display removed: StatusDisplay has no such members.
    // Optionally, add more live stats here
    sleep_ms(250); // Slow refresh for readability
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
    if (direction == MenuDirection::BACK) {
        exit_to_main();
        return;
    }

    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("PERFORMANCE");
    lcd->set_cursor(0, 1);
    char buf[20];
    snprintf(buf, sizeof(buf), "Max FIFO: %d%%", config.max_fifo_usage);
    lcd->print(buf);
    lcd->set_cursor(0, 2);
    snprintf(buf, sizeof(buf), "Err Thresh: %d", config.error_threshold);
    lcd->print(buf);
    lcd->set_cursor(0, 3);
    lcd->print("Press to return");
}

void EngineeringMenu::handle_factory_reset(MenuDirection direction) {
    if (direction == MenuDirection::BACK) {
        exit_to_main();
        return;
    }
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("FACTORY RESET");
    lcd->set_cursor(0, 1);
    lcd->print("Hold to confirm");
    extern bool g_menu_button_held;
    if (g_menu_button_held) {
        load_default_config();
        apply_config();
        lcd->set_cursor(0, 2);
        lcd->print("Reset done!");
        sleep_ms(1000);
        exit_to_main();
    }
}

// ============================================================================
// NON-BLOCKING EDITOR IMPLEMENTATIONS
// ============================================================================
// NOTE: Old blocking show_float_editor and show_bool_editor have been removed
// Use start_float_editor/update_float_editor and start_bool_editor/update_bool_editor instead

void EngineeringMenu::start_float_editor(const char* name, float* value, float min, float max, float step) {
    float_edit_ctx.label = name;
    float_edit_ctx.value_ptr = value;
    float_edit_ctx.min = min;
    float_edit_ctx.max = max;
    float_edit_ctx.step = step;
    float_edit_ctx.temp_value = *value;
    float_edit_ctx.active = true;
    draw_float_editor();
}

void EngineeringMenu::update_float_editor(MenuDirection direction) {
    if (!float_edit_ctx.active) return;

    switch (direction) {
        case MenuDirection::UP:
            float_edit_ctx.temp_value += float_edit_ctx.step;
            if (float_edit_ctx.temp_value > float_edit_ctx.max)
                float_edit_ctx.temp_value = float_edit_ctx.max;
            draw_float_editor();
            break;

        case MenuDirection::DOWN:
            float_edit_ctx.temp_value -= float_edit_ctx.step;
            if (float_edit_ctx.temp_value < float_edit_ctx.min)
                float_edit_ctx.temp_value = float_edit_ctx.min;
            draw_float_editor();
            break;

        case MenuDirection::ENTER:
            // Save value and exit editor
            *float_edit_ctx.value_ptr = float_edit_ctx.temp_value;
            float_edit_ctx.active = false;
            draw_submenu();
            break;

        case MenuDirection::BACK:
            // Cancel without saving
            float_edit_ctx.active = false;
            draw_submenu();
            break;

        default:
            break;
    }
}

void EngineeringMenu::draw_float_editor() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("EDIT: ");
    lcd->print(float_edit_ctx.label);
    lcd->set_cursor(0, 1);
    char value_str[20];
    snprintf(value_str, sizeof(value_str), "Value: %.2f", float_edit_ctx.temp_value);
    lcd->print(value_str);
    lcd->set_cursor(0, 2);
    char range_str[20];
    snprintf(range_str, sizeof(range_str), "%.2f-%.2f", float_edit_ctx.min, float_edit_ctx.max);
    lcd->print(range_str);
    lcd->set_cursor(0, 3);
    lcd->print("Enter:OK Back:Cancel");
}

void EngineeringMenu::start_bool_editor(const char* name, bool* value) {
    bool_edit_ctx.label = name;
    bool_edit_ctx.value_ptr = value;
    bool_edit_ctx.temp_value = *value;
    bool_edit_ctx.active = true;
    draw_bool_editor();
}

void EngineeringMenu::update_bool_editor(MenuDirection direction) {
    if (!bool_edit_ctx.active) return;

    switch (direction) {
        case MenuDirection::UP:
        case MenuDirection::DOWN:
            // Toggle value
            bool_edit_ctx.temp_value = !bool_edit_ctx.temp_value;
            draw_bool_editor();
            break;

        case MenuDirection::ENTER:
            // Save value and exit editor
            *bool_edit_ctx.value_ptr = bool_edit_ctx.temp_value;
            bool_edit_ctx.active = false;
            draw_submenu();
            break;

        case MenuDirection::BACK:
            // Cancel without saving
            bool_edit_ctx.active = false;
            draw_submenu();
            break;

        default:
            break;
    }
}

void EngineeringMenu::draw_bool_editor() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("EDIT: ");
    lcd->print(bool_edit_ctx.label);
    lcd->set_cursor(0, 1);
    lcd->print(bool_edit_ctx.temp_value ? "ON " : "OFF");
    lcd->set_cursor(0, 2);
    lcd->print("Turn:Toggle");
    lcd->set_cursor(0, 3);
    lcd->print("Enter:OK Back:Cancel");
}

void EngineeringMenu::start_position_editor(const char* title, int32_t current_pos) {
    position_edit_ctx.title = title;
    position_edit_ctx.current_pos = current_pos;
    position_edit_ctx.new_position = current_pos;
    position_edit_ctx.active = true;
    draw_position_editor();
}

void EngineeringMenu::update_position_editor(MenuDirection direction) {
    if (!position_edit_ctx.active) return;

    switch (direction) {
        case MenuDirection::UP:
            position_edit_ctx.new_position += 100;  // Increment by 100
            draw_position_editor();
            break;

        case MenuDirection::DOWN:
            position_edit_ctx.new_position -= 100;  // Decrement by 100
            draw_position_editor();
            break;

        case MenuDirection::ENTER:
            // Apply new position
            main_encoder->set_position(position_edit_ctx.new_position);
            lcd->clear();
            lcd->set_cursor(0, 0);
            lcd->print("POSITION SET TO");
            lcd->set_cursor(0, 1);
            char pos_str[20];
            sprintf(pos_str, "%ld", position_edit_ctx.new_position);
            lcd->print(pos_str);
            sleep_ms(1500);
            position_edit_ctx.active = false;
            draw_user_menu();
            break;

        case MenuDirection::BACK:
            // Cancel
            position_edit_ctx.active = false;
            draw_user_menu();
            break;

        default:
            break;
    }
}

void EngineeringMenu::draw_position_editor() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print(position_edit_ctx.title);
    lcd->set_cursor(0, 1);
    char current_str[20];
    sprintf(current_str, "Current: %ld", position_edit_ctx.current_pos);
    lcd->print(current_str);
    lcd->set_cursor(0, 2);
    char new_str[20];
    sprintf(new_str, "New: %ld", position_edit_ctx.new_position);
    lcd->print(new_str);
    lcd->set_cursor(0, 3);
    lcd->print("Enter:OK Back:Cancel");
}

void EngineeringMenu::start_position_reset() {
    reset_ctx.start_time = time_us_32();
    reset_ctx.waiting = true;
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("RESET POSITION");
    lcd->set_cursor(0, 1);
    lcd->print("Are you sure?");
    lcd->set_cursor(0, 2);
    lcd->print("Press: Confirm");
    lcd->set_cursor(0, 3);
    lcd->print("Turn: Cancel");
}

void EngineeringMenu::update_position_reset(MenuDirection direction) {
    if (!reset_ctx.waiting) return;

    // Check for timeout (5 seconds)
    if (time_us_32() - reset_ctx.start_time > 5000000) {
        lcd->clear();
        lcd->set_cursor(0, 0);
        lcd->print("RESET TIMEOUT");
        sleep_ms(1000);
        reset_ctx.waiting = false;
        draw_user_menu();
        return;
    }

    switch (direction) {
        case MenuDirection::ENTER:
            // Confirm reset
            main_encoder->set_position(100000); // Set to 100mm
            lcd->clear();
            lcd->set_cursor(0, 0);
            lcd->print("POSITION RESET");
            lcd->set_cursor(0, 1);
            lcd->print("TO 100mm");
            sleep_ms(1500);
            reset_ctx.waiting = false;
            draw_user_menu();
            break;

        case MenuDirection::UP:
        case MenuDirection::DOWN:
            // Cancel
            lcd->clear();
            lcd->set_cursor(0, 0);
            lcd->print("RESET CANCELLED");
            sleep_ms(1000);
            reset_ctx.waiting = false;
            draw_user_menu();
            break;

        default:
            break;
    }
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
            if (direction == MenuDirection::BACK) {
                exit_to_main();
                return;
            }
            lcd->clear();
            lcd->set_cursor(0, 0);
            lcd->print("FACTORY RESET");
            lcd->set_cursor(0, 1);
            lcd->print("Hold to confirm");
            extern bool g_menu_button_held;
            if (g_menu_button_held) {
    switch (calibration.state) {
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
}
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
    password_digit_index = 0; // Always start at first digit
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
        case MenuDirection::DOWN:
            // Only allow digit value change, not cursor movement
            if (direction == MenuDirection::UP) {
                password_entry[password_digit_index]++;
                if (password_entry[password_digit_index] > 9) {
                    password_entry[password_digit_index] = 0;
                }
            } else {
                if (password_entry[password_digit_index] == 0) {
                    password_entry[password_digit_index] = 9;
                } else {
                    password_entry[password_digit_index]--;
                }
            }
            draw_password_screen();
            break;

        case MenuDirection::ENTER:
            if (password_digit_index < 3) {
                // Move to next digit only
                password_digit_index++;
                draw_password_screen();
            } else if (password_digit_index == 3) {
                // Only submit when already at the last digit
                if (verify_password()) {
                    // Correct password - enter new engineer menu
                    current_state = MenuState::MAIN_MENU;
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
                    handle_engineer_main_menu(MenuDirection::NONE);
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
                        password_digit_index = 0; // Always reset to first digit after fail
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
    lcd->clear();
    // Row 0: Blank
    lcd->set_cursor(0, 0);
    lcd->print("");
    // Row 1: Prompt
    lcd->set_cursor(0, 1);
    lcd->print("Enter Password");
    // Row 2: 4 digits, highlight current
    lcd->set_cursor(0, 2);
    for (int i = 0; i < 4; ++i) {
        char buf[4];
        if (i == password_digit_index) {
            snprintf(buf, sizeof(buf), "[%d]", password_entry[i]);
        } else {
            snprintf(buf, sizeof(buf), " %d ", password_entry[i]);
        }
        lcd->print(buf);
        lcd->print(" ");
    }
    // Row 3: Blank
    lcd->set_cursor(0, 3);
    lcd->print("");
    // Prevent any other display updates during password entry
}

bool EngineeringMenu::verify_password() {
    // Extract digits from config.access_password (e.g., 1234)
    uint16_t pw = config.access_password;
    uint8_t correct_password[4];
    correct_password[0] = (pw / 1000) % 10;
    correct_password[1] = (pw / 100) % 10;
    correct_password[2] = (pw / 10) % 10;
    correct_password[3] = pw % 10;

    for (int i = 0; i < 4; ++i) {
        if (password_entry[i] != correct_password[i]) {
            return false;
        }
    }
    return true;
}

void EngineeringMenu::reset_password_entry() {
    password_digit_index = 0;
    for (int i = 0; i < 4; ++i) {
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
    menu_active = false;
    current_state = MenuState::HIDDEN;
    // Optionally, redraw the main position screen here if needed
}

void EngineeringMenu::handle_password_change(MenuDirection direction) {
    switch (direction) {
        case MenuDirection::UP:
            if (password_confirm_mode) {
                confirm_password[password_change_digit_index]++;
                if (confirm_password[password_change_digit_index] > 9) confirm_password[password_change_digit_index] = 0;
                draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
            } else {
                new_password[password_change_digit_index]++;
                if (new_password[password_change_digit_index] > 9) new_password[password_change_digit_index] = 0;
                draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
            }
            break;
        case MenuDirection::DOWN:
            if (password_confirm_mode) {
                if (confirm_password[password_change_digit_index] == 0) confirm_password[password_change_digit_index] = 9;
                else confirm_password[password_change_digit_index]--;
                draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
            } else {
                if (new_password[password_change_digit_index] == 0) new_password[password_change_digit_index] = 9;
                else new_password[password_change_digit_index]--;
                draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
            }
            break;
        case MenuDirection::ENTER:
            if (password_change_digit_index < 3) {
                password_change_digit_index++;
                draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
            } else if (!password_confirm_mode) {
                // Move to confirm mode
                password_confirm_mode = true;
                password_change_digit_index = 0;
                draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
            } else {
                // Compare new_password and confirm_password
                bool match = true;
                for (int i = 0; i < 4; ++i) {
                    if (new_password[i] != confirm_password[i]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    config.access_password = new_password[0] * 1000 + new_password[1] * 100 + new_password[2] * 10 + new_password[3];
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
                    password_confirm_mode = false;
                    password_change_digit_index = 0;
                    for (int i = 0; i < 4; i++) {
                        new_password[i] = 0;
                        confirm_password[i] = 0;
                    }
                    draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
                }
            }
            break;

        case MenuDirection::BACK:
            if (password_change_digit_index > 0) {
                password_change_digit_index--;
                draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
            } else if (password_confirm_mode) {
                password_confirm_mode = false;
                password_change_digit_index = 3;  // Go back to last digit of new password
                draw_password_change_screen(new_password, confirm_password, password_change_digit_index, password_confirm_mode);
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
    menu_active = true;
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
                    current_state = MenuState::USER_RESET_CONFIRM;
                    draw_user_reset_confirm();
                    break;
                case UserMenuItems::ABOUT:
                    this->show_about_submenu();
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

void EngineeringMenu::draw_user_reset_confirm() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Click to reset to");
    lcd->set_cursor(0, 1);
    lcd->print("100 MMM");
    lcd->set_cursor(0, 2);
    lcd->print("Are you sure?");
    lcd->set_cursor(0, 3);
    lcd->print("Yes < > No");
    // Default to Yes selected
    // user_reset_confirm_yes logic removed: variable not declared.
}

void EngineeringMenu::handle_user_reset_confirm(MenuDirection direction) {
    switch (direction) {
        case MenuDirection::ENTER:
            // Always perform reset for now (user_reset_confirm_yes removed)
            main_encoder->set_position(100000); // 100mm
            current_state = MenuState::USER_RESET_DONE;
            draw_user_reset_done();
            break;
        case MenuDirection::BACK:
            current_state = MenuState::USER_MENU;
            draw_user_menu();
            break;
        default:
            break;
    }
}

void EngineeringMenu::draw_user_reset_done() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print("Position reset!");
    lcd->set_cursor(0, 1);
    lcd->print("to 100 MMM");
    sleep_ms(1500);
    current_state = MenuState::USER_MENU;
    draw_user_menu();
}

void EngineeringMenu::handle_position_reset() {
    // Non-blocking implementation - just start the reset process
    // The actual handling is done in update_position_reset called from handle_menu_input
    start_position_reset();
}

void EngineeringMenu::handle_position_set() {
    int32_t current_pos = main_encoder->get_raw_position();
    start_position_editor("SET POSITION", current_pos);
}

void EngineeringMenu::start_engineering_access() {
    menu_active = true;
    current_state = MenuState::PASSWORD_ENTRY;
    password_attempts = 0;
    password_timeout = time_us_32();
    start_password_entry();
}
