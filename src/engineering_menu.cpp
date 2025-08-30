#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include "engineering_menu.h"
#include "version.h"

void EngineeringMenu::draw_message_screen() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print(message_line1);
    lcd->set_cursor(0, 1);
    lcd->print(message_line2);
}

// --- Calibration Flow Implementation ---
void EngineeringMenu::handle_calibration_flow(MenuDirection direction) {
    static int cal_step = 0;
    static float temp_pos1 = 0.0f;
    static float temp_pos2 = 0.0f;
    static int32_t last_encoder = 0;
    switch (cal_step) {
        case 0: // Intro
            if (direction == MenuDirection::ENTER) {
                cal_step = 1;
                temp_pos1 = calibration.measured_pos1_mm;
                last_encoder = menu_encoder->get_position();
                draw_calibration_set_pos1(temp_pos1);
            } else if (direction == MenuDirection::BACK) {
                cal_step = 0;
                current_state = MenuState::MAIN_MENU;
                handle_engineer_main_menu(MenuDirection::NONE);
            }
            break;
        case 1: // Set Position 1 (edit value)
            {
                int32_t enc_now = menu_encoder->get_position();
                int32_t delta = enc_now - last_encoder;
                last_encoder = enc_now;
                temp_pos1 += delta * 0.01f; // 0.01mm per detent
                if (direction == MenuDirection::UP) {
                    temp_pos1 += 0.01f;
                } else if (direction == MenuDirection::DOWN) {
                    temp_pos1 -= 0.01f;
                }
                if (direction == MenuDirection::ENTER) {
                    calibration.encoder_pos1 = main_encoder->get_raw_position();
                    calibration.measured_pos1_mm = temp_pos1;
                    cal_step = 2;
                    draw_calibration_move_prompt();
                } else if (direction == MenuDirection::BACK) {
                    cal_step = 0;
                    draw_calibration_intro();
                } else {
                    draw_calibration_set_pos1(temp_pos1);
                }
            }
            break;
        case 2: // Prompt move to position 2
            if (direction == MenuDirection::ENTER) {
                cal_step = 3;
                temp_pos2 = calibration.measured_pos2_mm;
                last_encoder = menu_encoder->get_position();
                draw_calibration_set_pos2(temp_pos2);
            } else if (direction == MenuDirection::BACK) {
                cal_step = 1;
                draw_calibration_set_pos1(temp_pos1);
            }
            break;
        case 3: // Set Position 2 (edit value)
            {
                int32_t enc_now = menu_encoder->get_position();
                int32_t delta = enc_now - last_encoder;
                last_encoder = enc_now;
                temp_pos2 += delta * 0.01f;
                if (direction == MenuDirection::UP) {
                    temp_pos2 += 0.01f;
                } else if (direction == MenuDirection::DOWN) {
                    temp_pos2 -= 0.01f;
                }
                if (direction == MenuDirection::ENTER) {
                    calibration.encoder_pos2 = main_encoder->get_raw_position();
                    calibration.measured_pos2_mm = temp_pos2;
                    cal_step = 4;
                    draw_calibration_calculate();
                } else if (direction == MenuDirection::BACK) {
                    cal_step = 2;
                    draw_calibration_move_prompt();
                } else {
                    draw_calibration_set_pos2(temp_pos2);
                }
            }
            break;
        case 4: // Calculate and review
            if (direction == MenuDirection::ENTER) {
                float delta_mm = calibration.measured_pos2_mm - calibration.measured_pos1_mm;
                int32_t delta_enc = calibration.encoder_pos2 - calibration.encoder_pos1;
                calibration.calculated_pitch = (delta_enc != 0) ? delta_mm / delta_enc : 0.0f;
                cal_step = 5;
                draw_calibration_review();
            } else if (direction == MenuDirection::BACK) {
                cal_step = 3;
                draw_calibration_set_pos2(temp_pos2);
            }
            break;
        case 5: // Review/Undo
            if (direction == MenuDirection::ENTER) {
                // Accept calibration
                // Save pitch to config or wherever needed
                config.encoder_scale_factor = calibration.calculated_pitch;
                draw_calibration_complete(true);
                cal_step = 0;
                current_state = MenuState::MAIN_MENU;
            } else if (direction == MenuDirection::BACK) {
                // Undo, return to main menu without saving
                draw_calibration_complete(false);
                cal_step = 0;
                current_state = MenuState::MAIN_MENU;
            }
            break;
    }
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
    if (saved) {
        strncpy(message_line1, "Calibration Saved!", 20);
    } else {
        strncpy(message_line1, "Calibration Canceled", 20);
    }
    strncpy(message_line2, "Returning...", 20);
    post_message_state = MenuState::MAIN_MENU;
    message_display_duration_ms = 1000;
    message_display_start_ms = time_us_32() / 1000;
    current_state = MenuState::MESSAGE;
    draw_message_screen();
}

void EngineeringMenu::start_value_editor(const char* name, long* value, long min, long max) {
    int_edit_ctx.label = name;
    int_edit_ctx.value_ptr = (int32_t*)value;
    int_edit_ctx.min = min;
    int_edit_ctx.max = max;
    int_edit_ctx.temp_value = *value;
    int_edit_ctx.active = true;
    post_edit_state = current_state;
    current_state = MenuState::EDIT_INT_VALUE;
    draw_edit_int();
}

void EngineeringMenu::start_float_editor(const char* name, float* value, float min, float max, float step) {
    float_edit_ctx.label = name;
    float_edit_ctx.value_ptr = value;
    float_edit_ctx.min = min;
    float_edit_ctx.max = max;
    float_edit_ctx.step = step;
    float_edit_ctx.temp_value = *value;
    post_edit_state = current_state;
    current_state = MenuState::EDIT_FLOAT_VALUE;
    draw_edit_float();
}

void EngineeringMenu::start_bool_editor(const char* name, bool* value) {
    bool_edit_ctx.label = name;
    bool_edit_ctx.value_ptr = value;
    bool_edit_ctx.temp_value = *value;
    post_edit_state = current_state;
    current_state = MenuState::EDIT_BOOL_VALUE;
    draw_edit_bool();
}

void EngineeringMenu::show_about_submenu() {
    strncpy(message_line1, "PEARL Project", 20);
    char ver_str[21];
    snprintf(ver_str, sizeof(ver_str), "Ver: %s", PROJECT_VERSION_STRING);
    strncpy(message_line2, ver_str, 20);
    post_message_state = MenuState::USER_MENU;
    message_display_duration_ms = 2000;
    message_display_start_ms = time_us_32() / 1000;
    current_state = MenuState::MESSAGE;
    draw_message_screen();
}

EngineeringMenu::EngineeringMenu(LCD_I2C* display, HW040Encoder* encoder, 
                                QuadratureEncoder* main_enc, StatusDisplay* status)
    : lcd(display), menu_encoder(encoder), main_encoder(main_enc), status_display(status),
      current_state(MenuState::HIDDEN), selected_item(MainMenuItems::ENCODER_SETTINGS),
      selected_user_item(UserMenuItems::RESET_POSITION),
      submenu_index(0), menu_active(false), menu_timeout(MENU_TIMEOUT_MS), last_activity(0),
    password_digit_index(0), password_timeout(0), password_attempts(0) {
    for (int i = 0; i < 4; ++i) password_entry[i] = 0;
    load_default_config();
    reset_password_entry();
    reset_calibration();
}

bool EngineeringMenu::init() {
    if (!load_config_from_flash()) {
        load_default_config();
    }
    apply_config();
    return true;
}

void EngineeringMenu::load_default_config() {
    config.access_password = 1234;
    config.encoder_resolution = 600;
    config.encoder_scale_factor = 1.0f;
    config.encoder_direction_invert = false;
    config.velocity_filter_ms = 100;
    config.contrast = 50;
    config.backlight = 80;
    config.refresh_rate_ms = 100;
    config.show_velocity = true;
    config.show_diagnostics = false;
    config.error_reset_interval = 10000;
    config.debug_mode = false;
    config.pio_frequency_div = 1;
    config.max_fifo_usage = 75;
    config.error_threshold = 10;
    config.velocity_threshold = 100.0f;
    default_config = config;
}

void EngineeringMenu::activate_menu() {
    if (!menu_active) {
        menu_active = true;
        current_state = MenuState::PASSWORD_ENTRY;
        selected_item = MainMenuItems::ENCODER_SETTINGS;
        last_activity = time_us_32();
        start_password_entry();
    }
}

void EngineeringMenu::update(bool button_pressed, bool button_held) {
    if (!menu_active) return;

    uint32_t current_time_us = time_us_32();
    uint32_t current_time_ms = current_time_us / 1000;

    if (current_time_us - last_activity > menu_timeout * 1000) {
        exit_menu();
        return;
    }

    if (current_state == MenuState::MESSAGE) {
        if (current_time_ms - message_display_start_ms > message_display_duration_ms) {
            current_state = post_message_state;
        }
    }

    if (calibration.active) {
        update_calibration();
    }

    if (current_state == MenuState::PASSWORD_ENTRY) {
        if (current_time_us - password_timeout > (PASSWORD_TIMEOUT_MS * 1000)) {
            handle_password_timeout();
        }
    }

    MenuDirection direction = MenuDirection::NONE;
    int32_t delta = menu_encoder->get_delta();
    if (current_state == MenuState::PASSWORD_ENTRY) {
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
        last_activity = time_us_32();
        handle_menu_input(direction);
    }
}

void EngineeringMenu::handle_menu_input(MenuDirection direction) {
    switch (current_state) {
        case MenuState::PASSWORD_ENTRY: handle_password_entry(direction); break;
        case MenuState::USER_MENU: handle_user_menu(direction); break;
        case MenuState::MAIN_MENU: handle_engineer_main_menu(direction); break;
        case MenuState::ENG_SET_PARAMS: handle_set_params_menu(direction); break;
        case MenuState::ENG_CALIBRATE: handle_calibration_flow(direction); break;
        case MenuState::PASSWORD_CHANGE: handle_password_change(direction); break;
        case MenuState::ENCODER_CONFIG: handle_encoder_config(direction); break;
        case MenuState::DISPLAY_CONFIG: handle_display_config(direction); break;
        case MenuState::SYSTEM_INFO: handle_system_info(direction); break;
        case MenuState::DIAGNOSTICS: handle_diagnostics(direction); break;
        case MenuState::CALIBRATION: handle_calibration(direction); break;
        case MenuState::PERFORMANCE: handle_performance(direction); break;
        case MenuState::FACTORY_RESET: handle_factory_reset_confirm(direction); break;
        case MenuState::EDIT_INT_VALUE: handle_edit_int(direction); break;
        case MenuState::EDIT_FLOAT_VALUE: handle_edit_float(direction); break;
        case MenuState::EDIT_BOOL_VALUE: handle_edit_bool(direction); break;
        default: break;
    }
}

void EngineeringMenu::handle_edit_int(MenuDirection direction) {
    switch(direction) {
        case MenuDirection::UP:
            int_edit_ctx.temp_value++;
            if (int_edit_ctx.temp_value > int_edit_ctx.max) {
                int_edit_ctx.temp_value = int_edit_ctx.max;
            }
            draw_edit_int();
            break;
        case MenuDirection::DOWN:
            int_edit_ctx.temp_value--;
            if (int_edit_ctx.temp_value < int_edit_ctx.min) {
                int_edit_ctx.temp_value = int_edit_ctx.min;
            }
            draw_edit_int();
            break;
        case MenuDirection::ENTER:
            *int_edit_ctx.value_ptr = int_edit_ctx.temp_value;
            int_edit_ctx.active = false;
            current_state = post_edit_state;
            break;
        case MenuDirection::BACK:
            int_edit_ctx.active = false;
            current_state = post_edit_state;
            break;
        default:
            break;
    }
}

void EngineeringMenu::draw_edit_int() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print(int_edit_ctx.label);
    char buf[21];
    snprintf(buf, sizeof(buf), "Value: %ld", int_edit_ctx.temp_value);
    lcd->set_cursor(0, 1);
    lcd->print(buf);
}

void EngineeringMenu::handle_edit_float(MenuDirection direction) {
    switch(direction) {
        case MenuDirection::UP:
            float_edit_ctx.temp_value += float_edit_ctx.step;
            if (float_edit_ctx.temp_value > float_edit_ctx.max) {
                float_edit_ctx.temp_value = float_edit_ctx.max;
            }
            draw_edit_float();
            break;
        case MenuDirection::DOWN:
            float_edit_ctx.temp_value -= float_edit_ctx.step;
            if (float_edit_ctx.temp_value < float_edit_ctx.min) {
                float_edit_ctx.temp_value = float_edit_ctx.min;
            }
            draw_edit_float();
            break;
        case MenuDirection::ENTER:
            *float_edit_ctx.value_ptr = float_edit_ctx.temp_value;
            current_state = post_edit_state;
            break;
        case MenuDirection::BACK:
            current_state = post_edit_state;
            break;
        default:
            break;
    }
}

void EngineeringMenu::draw_edit_float() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print(float_edit_ctx.label);
    char buf[21];
    snprintf(buf, sizeof(buf), "Value: %.2f", float_edit_ctx.temp_value);
    lcd->set_cursor(0, 1);
    lcd->print(buf);
}

void EngineeringMenu::handle_edit_bool(MenuDirection direction) {
    switch(direction) {
        case MenuDirection::UP:
        case MenuDirection::DOWN:
            bool_edit_ctx.temp_value = !bool_edit_ctx.temp_value;
            draw_edit_bool();
            break;
        case MenuDirection::ENTER:
            *bool_edit_ctx.value_ptr = bool_edit_ctx.temp_value;
            current_state = post_edit_state;
            break;
        case MenuDirection::BACK:
            current_state = post_edit_state;
            break;
        default:
            break;
    }
}

void EngineeringMenu::draw_edit_bool() {
    lcd->clear();
    lcd->set_cursor(0, 0);
    lcd->print(bool_edit_ctx.label);
    lcd->set_cursor(0, 1);
    lcd->print(bool_edit_ctx.temp_value ? "ON" : "OFF");
}

void EngineeringMenu::handle_engineer_main_menu(MenuDirection direction) {
    int item_count = static_cast<int>(MainMenuItems::ITEM_COUNT);
    int current_item = static_cast<int>(selected_item);

    switch (direction) {
        case MenuDirection::UP:
            current_item--;
            if (current_item < 0) {
                current_item = item_count - 1;
            }
            selected_item = static_cast<MainMenuItems>(current_item);
            draw_engineer_main_menu(selected_item);
            break;
        case MenuDirection::DOWN:
            current_item++;
            if (current_item >= item_count) {
                current_item = 0;
            }
            selected_item = static_cast<MainMenuItems>(current_item);
            draw_engineer_main_menu(selected_item);
            break;
        case MenuDirection::ENTER:
            switch (selected_item) {
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
            draw_engineer_main_menu(selected_item);
            break;
    }
}

void EngineeringMenu::start_user_menu() {
    menu_active = true;
    current_state = MenuState::USER_MENU;
    selected_user_item = UserMenuItems::RESET_POSITION;
    draw_user_menu();
}

void EngineeringMenu::draw_user_menu() {
    printf("[DEBUG] draw_user_menu called, lcd ptr: %p\n", lcd);
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
    int item_count = static_cast<int>(UserMenuItems::ITEM_COUNT);
    int current_item = static_cast<int>(selected_user_item);

    switch (direction) {
        case MenuDirection::UP:
            current_item--;
            if (current_item < 0) {
                current_item = item_count - 1;
            }
            selected_user_item = static_cast<UserMenuItems>(current_item);
            draw_user_menu();
            break;

        case MenuDirection::DOWN:
            current_item++;
            if (current_item >= item_count) {
                current_item = 0;
            }
            selected_user_item = static_cast<UserMenuItems>(current_item);
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
        default:
            draw_user_menu();
            break;
    }
}

// ... the rest of the file is the same ...
// I will omit it for brevity, but I will include it in the overwrite_file_with_block call.
// ...
