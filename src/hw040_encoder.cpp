#include "hw040_encoder.h"
#include <stdio.h>
#include <string.h>

// Static instance pointer for interrupt handling
HW040Encoder* HW040Encoder::instance = nullptr;

HW040Encoder::HW040Encoder(uint clk_pin, uint dt_pin, uint sw_pin)
    : pin_clk(clk_pin), pin_dt(dt_pin), pin_sw(sw_pin),
      last_clk(false), position(0), button_pressed(false),
      button_press_time(0), click_count(0), last_click_time(0) {
    instance = this;
}

HW040Encoder::~HW040Encoder() {
    if (instance == this) {
        instance = nullptr;
    }
}

bool HW040Encoder::init() {
    // Initialize GPIO pins
    gpio_init(pin_clk);
    gpio_init(pin_dt);
    gpio_init(pin_sw);
    
    // Set pin directions
    gpio_set_dir(pin_clk, GPIO_IN);
    gpio_set_dir(pin_dt, GPIO_IN);
    gpio_set_dir(pin_sw, GPIO_IN);
    
    // Enable pull-ups (HW-040 typically needs pull-ups)
    gpio_pull_up(pin_clk);
    gpio_pull_up(pin_dt);
    gpio_pull_up(pin_sw);
    
    // Read initial state
    last_clk = gpio_get(pin_clk);
    
    // Set up interrupts
    gpio_set_irq_enabled_with_callback(pin_clk, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, 
                                      true, &HW040Encoder::gpio_irq_handler);
    gpio_set_irq_enabled(pin_sw, GPIO_IRQ_EDGE_FALL, true);
    
    return true;
}

void HW040Encoder::gpio_irq_handler(uint gpio, uint32_t events) {
    if (instance == nullptr) return;
    
    if (gpio == instance->pin_clk) {
        // Handle CLK pin interrupt
        bool clk_state = gpio_get(instance->pin_clk);
        bool dt_state = gpio_get(instance->pin_dt);
        
        // Only process on falling edge of CLK for reliable operation
        if (!clk_state && instance->last_clk) {
            if (dt_state) {
                instance->position++;  // Clockwise
            } else {
                instance->position--;  // Counter-clockwise
            }
        }
        
        instance->last_clk = clk_state;
    }
    else if (gpio == instance->pin_sw) {
        // Handle button interrupt
        uint32_t current_time = time_us_32();
        
        // Debounce check
        if (current_time - instance->button_press_time < DEBOUNCE_TIME_US) {
            return;
        }
        
        if (!gpio_get(instance->pin_sw)) {  // Button pressed (active low)
            instance->button_pressed = true;
            instance->button_press_time = current_time;
            
            // Check for multiple clicks within timeout
            if (current_time - instance->last_click_time < CLICK_TIMEOUT_US) {
                instance->click_count++;
            } else {
                instance->click_count = 1;  // First click or timeout expired
            }
            
            instance->last_click_time = current_time;
        }
    }
}

int32_t HW040Encoder::get_delta() {
    static int32_t last_position = 0;
    int32_t current_pos = position;
    int32_t delta = current_pos - last_position;
    last_position = current_pos;
    return delta;
}

bool HW040Encoder::check_triple_click() {
    uint32_t current_time = time_us_32();
    
    // Check if we have 3 clicks within the timeout period
    if (click_count >= 3 && (current_time - last_click_time < CLICK_TIMEOUT_US)) {
        click_count = 0;  // Reset counter
        return true;
    }
    
    // Reset click count if timeout expired
    if (current_time - last_click_time > CLICK_TIMEOUT_US) {
        click_count = 0;
    }
    
    return false;
}

void HW040Encoder::update() {
    // Check button state (for continuous press detection)
    bool current_button = !gpio_get(pin_sw);  // Active low
    uint32_t current_time = time_us_32();
    
    // Reset button pressed flag if button released
    if (!current_button) {
        button_pressed = false;
    }
    
    // Auto-reset click count after timeout
    if (current_time - last_click_time > CLICK_TIMEOUT_US && click_count > 0) {
        click_count = 0;
    }
}

const char* HW040Encoder::get_status() const {
    static char status_buffer[100];
    snprintf(status_buffer, sizeof(status_buffer),
             "Pos:%ld Btn:%s Clicks:%lu",
             position,
             button_pressed ? "ON" : "OFF",
             click_count);
    return status_buffer;
}

// Helper function to convert encoder movement to menu navigation
MenuDirection encoder_to_menu_direction(int32_t delta, bool button_pressed) {
    if (button_pressed) {
        return MenuDirection::ENTER;
    }
    
    if (delta > 0) {
        return MenuDirection::DOWN;  // Clockwise = move down in menu
    } else if (delta < 0) {
        return MenuDirection::UP;    // Counter-clockwise = move up in menu
    }
    
    return MenuDirection::NONE;
}
