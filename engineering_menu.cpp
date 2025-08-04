#include "engineering_menu.h"
#include "version.h"
#include <stdio.h>
#include <string.h>
#include <cmath>

EngineeringMenu::EngineeringMenu(LCD_I2C* display, HW040Encoder* encoder, 
                                QuadratureEncoder* main_enc, StatusDisplay* status)
    : lcd(display), menu_encoder(encoder), main_encoder(main_enc), status_display(status),
      current_state(MenuState::HIDDEN), selected_item(MainMenuItems::ENCODER_SETTINGS),
      submenu_index(0), menu_active(false), menu_timeout(MENU_TIMEOUT_MS), last_activity(0) {
    load_default_config();
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
        current_state = MenuState::MAIN_MENU;
        selected_item = MainMenuItems::ENCODER_SETTINGS;
        last_activity = time_us_32();
        
        lcd->clear();
        lcd->set_cursor(0, 0);
        lcd->print("ENGINEERING MENU");
        lcd->set_cursor(0, 1);
        lcd->print("v");
        lcd->print(PROJECT_VERSION_STRING);
        lcd->print(" ACTIVATED");
        sleep_ms(1000);  // Show activation message
        
        draw_main_menu();
    }
}

void EngineeringMenu::update() {
    if (!menu_active) return;
    
    uint32_t current_time = time_us_32();
    
    // Check for timeout
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
                    exit_menu();
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
    lcd->print("ENGINEERING MENU");
    
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
    lcd->print("SYSTEM INFO");
    lcd->set_cursor(0, 1);
    lcd->print("Ver: ");
    lcd->print(PROJECT_VERSION_STRING);
    lcd->set_cursor(0, 2);
    lcd->print("Build: ");
    lcd->print(BUILD_DATE);
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
        lcd->print("Value: ");
        lcd->printf("%ld", temp_value);
        lcd->set_cursor(0, 2);
        lcd->print("Range: ");
        lcd->printf("%ld", min);
        lcd->print("-");
        lcd->printf("%ld", max);
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
        case MainMenuItems::FACTORY_RESET: return "Factory Reset";
        case MainMenuItems::SAVE_AND_EXIT: return "Save & Exit";
        case MainMenuItems::CANCEL_EXIT: return "Cancel/Exit";
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
