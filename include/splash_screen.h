#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include "lcd_i2c.h"
#include "pico/stdlib.h"

class SplashScreen {
public:
    SplashScreen(LCD_I2C* display);
    
    // Show initial splash screen
    void show_startup();
    
    // Show initialization progress
    void show_progress(const char* step, uint8_t percentage);
    
    // Show completion message
    void show_ready();
    
    // Show version and build info
    void show_version_info();
    
    // Show hardware initialization status
    void show_hardware_status(const char* component, bool status);
    
    // Animated loading indicator
    void update_loading_animation();
    
private:
    LCD_I2C* lcd;
    uint8_t animation_frame;
    uint32_t last_animation_time;
    
    // Animation characters for progress indication
    static const char* SPINNER_CHARS;
    static const uint32_t ANIMATION_INTERVAL_MS = 200;
    
    void center_text(const char* text, uint8_t row);
    void draw_progress_bar(uint8_t percentage, uint8_t row);
};

// Initialization step enumeration for tracking progress
enum class InitStep {
    STARTING = 0,
    GPIO_INIT = 20,
    LCD_INIT = 40,
    ENCODER_INIT = 60,
    MENU_INIT = 80,
    COMPLETE = 100
};

#endif // SPLASH_SCREEN_H
