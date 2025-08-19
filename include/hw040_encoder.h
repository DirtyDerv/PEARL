#ifndef HW040_ENCODER_H
#define HW040_ENCODER_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// HW-040 Rotary Encoder with Push Button
// v0.05 - Engineering Menu Interface

class HW040Encoder {
private:
    uint pin_clk;           // Clock pin (A)
    uint pin_dt;            // Data pin (B)
    uint pin_sw;            // Switch pin (push button)
    
    // State tracking
    volatile bool last_clk;
    volatile int32_t position;
    volatile bool button_pressed;
    volatile uint32_t button_press_time;
    volatile uint32_t click_count;
    volatile uint32_t last_click_time;
    
    // Triple-click detection for menu access
    static const uint32_t CLICK_TIMEOUT_US = 1000000;  // 1 second
    static const uint32_t DEBOUNCE_TIME_US = 50000;    // 50ms debounce (button)
    static const uint32_t ROTARY_DEBOUNCE_US = 2000;   // 2ms debounce (rotary)

    volatile uint32_t last_rotary_time;
    
    // Interrupt handlers
    static void gpio_irq_handler(uint gpio, uint32_t events);
    
    // Static instance for interrupt handling
    static HW040Encoder* instance;

public:
    HW040Encoder(uint clk_pin, uint dt_pin, uint sw_pin);
    ~HW040Encoder();
    
    // Initialization
    bool init();
    
    // Position and movement
    int32_t get_position() const { return position; }
    int32_t get_delta();  // Get position change since last call
    void reset_position() { position = 0; }
    
    // Button functionality
    bool is_button_pressed() const { return button_pressed; }
    bool check_triple_click();  // Returns true if triple-click detected
    uint32_t get_click_count() const { return click_count; }
    
    // Status and debugging
    void update();  // Call this regularly in main loop
    const char* get_status() const;
};

// Menu navigation directions
enum class MenuDirection {
    NONE,
    UP,
    DOWN,
    ENTER,
    BACK
};

// Convert encoder movement to menu navigation
MenuDirection encoder_to_menu_direction(int32_t delta, bool button_pressed);

#endif // HW040_ENCODER_H
