#include "status_display.h"
#include <cstdio>
#include <cmath>

StatusDisplay::StatusDisplay(LCD_I2C* lcd_instance, BigFont* big_font_instance) {
    lcd = lcd_instance;
    big_font = big_font_instance;
    animation_frame = 0;
    last_animation_update = 0;
    last_distance = -999.0f;
    last_velocity = -999.0f;
    last_rpm = -999.0f;
    last_direction_positive = true;
    current_mode = DisplayMode::STANDARD;
}

void StatusDisplay::init() {
    draw_static_elements();
}

void StatusDisplay::set_display_mode(DisplayMode mode) {
    current_mode = mode;
    lcd->clear();
}

void StatusDisplay::update(const QuadratureEncoder& encoder) {
    if (current_mode == DisplayMode::BIG_FONT) {
        display_big_font(encoder);
        return;
    }

    char buf[21];
    float distance = encoder.get_distance();
    if (abs(distance - last_distance) > 0.0005f) {
        snprintf(buf, sizeof(buf), "Pos: %8.3f mm", distance);
        lcd->set_cursor(0, 0);
        lcd->print(buf);
        last_distance = distance;
    }

    float velocity = encoder.get_velocity();
    if (abs(velocity - last_velocity) > 0.005f) {
        snprintf(buf, sizeof(buf), "Vel: %8.2f mm/s", velocity);
        lcd->set_cursor(0, 1);
        lcd->print(buf);
        last_velocity = velocity;
    }

    float rpm = encoder.get_rpm();
    if (abs(rpm - last_rpm) > 0.05f) {
        snprintf(buf, sizeof(buf), "RPM: %9.1f", rpm);
        lcd->set_cursor(0, 2);
        lcd->print(buf);
        last_rpm = rpm;
    }

    // Update animation and dynamic elements
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_animation_update > ANIMATION_INTERVAL) {
        animation_frame++;
        if (animation_frame > 3) {
            animation_frame = 0;
        }
        last_animation_update = now;
    }

    bool is_moving = (velocity != 0.0f);
    bool is_positive = (velocity >= 0.0f);
    draw_direction_arrow(is_positive, is_moving);
    draw_speed_bar(velocity, max_display_velocity);
}

void StatusDisplay::display_big_font(const QuadratureEncoder& encoder) {
    big_font->init_matrix_font();
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%7.2f", encoder.get_distance());

    int col = 0;
    for (int i = 0; i < 7; i++) {
        if (buffer[i] == ' ') {
            col += 3;
        } else if (buffer[i] == '.') {
            big_font->display_big_char('.', col, 0);
            col += 1;
        }
        else {
            big_font->display_big_char(buffer[i], col, 0);
            col += 4;
        }
    }
}

void StatusDisplay::draw_static_elements() {
    // This could be used to draw a border or other static elements
}

void StatusDisplay::clear_status_line() {
    lcd->set_cursor(0, 3);
    lcd->print("                    ");
}

void StatusDisplay::draw_direction_arrow(bool moving_positive, bool is_moving) {
    lcd->set_cursor(19, 0);
    if (!is_moving) {
        lcd->write(' ');
    } else {
        if (moving_positive) {
            lcd->write('^');
        } else {
            lcd->write('v');
        }
    }
}

void StatusDisplay::draw_speed_bar(float velocity, float max_velocity) {
    uint8_t bar_width = 10;
    float speed_ratio = abs(velocity) / max_velocity;
    if (speed_ratio > 1.0) {
        speed_ratio = 1.0;
    }
    uint8_t filled_segments = speed_ratio * bar_width;

    lcd->set_cursor(5, 3);
    for (uint8_t i = 0; i < bar_width; ++i) {
        if (i < filled_segments) {
            lcd->write(0xFF); // Solid block character
        } else {
            lcd->write('-');
        }
    }
}

void StatusDisplay::draw_performance_status(bool has_warning, uint32_t fifo_errors, uint32_t invalid_transitions) {
    lcd->set_cursor(0, 3);
    if (has_warning) {
        lcd->print("PERF WARN!");
    } else {
        lcd->print("PERF OK   ");
    }
    char buf[10];
    snprintf(buf, sizeof(buf), "F:%d T:%d", fifo_errors, invalid_transitions);
    lcd->set_cursor(11, 3);
    lcd->print(buf);
}
