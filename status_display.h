#ifndef STATUS_DISPLAY_H
#define STATUS_DISPLAY_H

#include "lcd_i2c.h"
#include "quadrature_encoder.h"

class StatusDisplay {
private:
    LCD_I2C* lcd;
    
    // Animation frame counter for moving indicators
    uint8_t animation_frame;
    uint32_t last_animation_update;
    
    // Previous values for change detection
    float last_velocity;
    bool last_direction_positive;
    
    void draw_direction_arrow(bool moving_positive, bool is_moving);
    void draw_speed_bar(float velocity, float max_velocity = 10.0f);
    void draw_static_elements();
    
public:
    StatusDisplay(LCD_I2C* lcd_instance);
    
    void init();
    void update(const QuadratureEncoder& encoder);
    void clear_status_line();
    
    // v0.04 enhancement: Performance status indicators
    void draw_performance_status(bool has_warning, uint32_t fifo_errors, uint32_t invalid_transitions);
    void draw_update_rate_indicator(uint32_t update_rate);
    void draw_system_health_bar(float cpu_load_estimate);
    
    // Configuration
    void set_max_velocity_for_display(float max_vel) { max_display_velocity = max_vel; }
    
private:
    float max_display_velocity = 10.0f; // Maximum velocity for 100% speed bar
    static const uint32_t ANIMATION_INTERVAL = 200; // Animation update interval in ms
};

#endif // STATUS_DISPLAY_H
