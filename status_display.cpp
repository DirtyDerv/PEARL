#include "status_display.h"
#include "pico/time.h"
#include <cmath>

StatusDisplay::StatusDisplay(LCD_I2C* lcd_instance) {
    lcd = lcd_instance;
    animation_frame = 0;
    last_animation_update = 0;
    last_velocity = 0.0f;
    last_direction_positive = true;
    max_display_velocity = 10.0f;
}

void StatusDisplay::init() {
    // Draw static elements that don't change
    draw_static_elements();
}

void StatusDisplay::update(const QuadratureEncoder& encoder) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    float current_velocity = encoder.get_velocity();
    bool is_moving = (fabsf(current_velocity) > 0.01f); // Threshold for considering "moving"
    bool direction_positive = (current_velocity >= 0);
    
    // Update animation frame
    if (current_time - last_animation_update >= ANIMATION_INTERVAL) {
        animation_frame = (animation_frame + 1) % 4; // 4-frame animation
        last_animation_update = current_time;
    }
    
    // Update direction indicator
    if (direction_positive != last_direction_positive || 
        (is_moving != (fabsf(last_velocity) > 0.01f))) {
        draw_direction_arrow(direction_positive, is_moving);
        last_direction_positive = direction_positive;
    }
    
    // Update speed bar (update more frequently for smooth display)
    if (fabsf(current_velocity - last_velocity) > 0.1f || 
        current_time - last_animation_update < ANIMATION_INTERVAL) {
        draw_speed_bar(current_velocity);
        last_velocity = current_velocity;
    }
}

void StatusDisplay::draw_static_elements() {
    // Draw labels and static parts on the bottom row
    lcd->set_cursor(0, 3);
    lcd->print("Dir:     Spd:");
}

void StatusDisplay::draw_direction_arrow(bool moving_positive, bool is_moving) {
    lcd->set_cursor(4, 3);
    
    if (!is_moving) {
        lcd->print("---"); // Stopped indicator
    } else if (moving_positive) {
        // Animate forward arrow
        switch (animation_frame) {
            case 0: lcd->print(">>>");  break;
            case 1: lcd->print(" >>>");  break;
            case 2: lcd->print(">>>");  break;
            case 3: lcd->print(">> ");  break;
        }
    } else {
        // Animate backward arrow  
        switch (animation_frame) {
            case 0: lcd->print("<<<"); break;
            case 1: lcd->print("<<< "); break;
            case 2: lcd->print("<<<"); break;
            case 3: lcd->print(" <<<"); break;
        }
    }
}

void StatusDisplay::draw_speed_bar(float velocity, float max_velocity) {
    // Calculate speed as percentage of max
    float speed_percent = fabsf(velocity) / max_velocity;
    if (speed_percent > 1.0f) speed_percent = 1.0f;
    
    // Convert to bar segments (7 characters available: positions 9-15)
    uint8_t bar_length = (uint8_t)(speed_percent * 7.0f);
    
    lcd->set_cursor(9, 3);
    
    // Draw speed bar with different characters for different levels
    for (int i = 0; i < 7; i++) {
        if (i < bar_length) {
            if (speed_percent < 0.3f) {
                lcd->print("."); // Low speed
            } else if (speed_percent < 0.7f) {
                lcd->print("="); // Medium speed
            } else {
                lcd->print("#"); // High speed
            }
        } else {
            lcd->print(" ");
        }
    }
}

void StatusDisplay::clear_status_line() {
    lcd->set_cursor(0, 3);
    lcd->print("                "); // Clear entire bottom line
}

// v0.04 enhancement: Performance status indicators
void StatusDisplay::draw_performance_status(bool has_warning, uint32_t fifo_errors, uint32_t invalid_transitions) {
    lcd->set_cursor(0, 3);
    
    if (has_warning || fifo_errors > 0 || invalid_transitions > 0) {
        // Show warning indicator
        lcd->print("!");
        if (fifo_errors > 0) {
            lcd->printf("F%lu", fifo_errors);
        }
        if (invalid_transitions > 0) {
            lcd->printf("E%lu", invalid_transitions);
        }
    } else {
        // Show OK status
        lcd->print("OK");
    }
}

void StatusDisplay::draw_update_rate_indicator(uint32_t update_rate) {
    lcd->set_cursor(4, 3);
    
    if (update_rate <= 20) {
        lcd->print("T");  // Turbo
    } else if (update_rate <= 50) {
        lcd->print("F");  // Fast
    } else if (update_rate <= 100) {
        lcd->print("N");  // Normal
    } else {
        lcd->print("S");  // Slow
    }
}

void StatusDisplay::draw_system_health_bar(float cpu_load_estimate) {
    lcd->set_cursor(13, 3);
    
    // Simple health indicator (3 characters)
    if (cpu_load_estimate < 0.3f) {
        lcd->print("|||");  // Excellent
    } else if (cpu_load_estimate < 0.6f) {
        lcd->print("|| ");  // Good
    } else if (cpu_load_estimate < 0.8f) {
        lcd->print("|  ");  // Fair
    } else {
        lcd->print("   ");  // Poor
    }
}
