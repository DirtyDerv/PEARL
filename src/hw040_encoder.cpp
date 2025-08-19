
#include "hw040_encoder.h"
#include <stdio.h>
#include <string.h>


// Debounce time for push button in microseconds (20ms)
#define DEBOUNCE_TIME_US 20000
// Triple-click timeout in microseconds (0.5s)
#define CLICK_TIMEOUT_US 500000
// Rotary debounce time in microseconds (3ms)
#define ROTARY_DEBOUNCE_US 3000

// Static instance pointer for interrupt handling
HW040Encoder* HW040Encoder::instance = nullptr;

HW040Encoder::HW040Encoder(uint clk_pin, uint dt_pin, uint sw_pin)
    : pin_clk(clk_pin), pin_dt(dt_pin), pin_sw(sw_pin),
    last_clk(false), position(0), button_pressed(false),
    button_press_time(0), click_count(0), last_click_time(0), last_rotary_time(0) {
    instance = this;
}

HW040Encoder::~HW040Encoder() {
    if (instance == this) {
        instance = nullptr;
    }
}

bool HW040Encoder::init() {
    printf("HW040: init() called, pin_sw=%u\n", pin_sw);
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
    gpio_set_irq_enabled(pin_sw, GPIO_IRQ_EDGE_FALL, true); // Only falling edge initially
    
    return true;
}

void HW040Encoder::gpio_irq_handler(uint gpio, uint32_t events) {
        // Debug: print when button is pressed
        if (!instance->button_pressed && !gpio_get(instance->pin_sw)) {
            printf("HW040: Button pressed\n");
        }
    if (instance == nullptr) return;
    
    if (gpio == instance->pin_clk) {
        printf("[DEBUG] ROTARY IRQ: CLK triggered\n");
        // Handle CLK pin interrupt with debounce
        uint32_t now = time_us_32();
        if (now - instance->last_rotary_time < ROTARY_DEBOUNCE_US) {
            instance->last_clk = gpio_get(instance->pin_clk); // update state
            return;
        }
        bool clk_state = gpio_get(instance->pin_clk);
        bool dt_state = gpio_get(instance->pin_dt);
        // Only process on falling edge of CLK for reliable operation
        if (!clk_state && instance->last_clk) {
            if (dt_state) {
                instance->position++;  // Clockwise
            } else {
                instance->position--;  // Counter-clockwise
            }
            instance->last_rotary_time = now;
        }
        instance->last_clk = clk_state;
    // Do NOT set button_pressed or click_count here; rotation is not a click
    printf("[DEBUG] ROTARY IRQ: position=%ld, click_count=%lu\n", instance->position, instance->click_count);
    }
    else if (gpio == instance->pin_sw) {
        printf("[DEBUG] BUTTON IRQ: SW triggered\n");
        // Handle button interrupt (falling edge only, active low)
        uint32_t current_time = time_us_32();
        // Only register a click if button is not already pressed (prevents double count)
        if (!gpio_get(instance->pin_sw)) {
            // Software filter: only count as click if not rotating (CLK and DT both HIGH)
            if (gpio_get(instance->pin_clk) && gpio_get(instance->pin_dt)) {
                // Debounce check
                if (current_time - instance->button_press_time >= DEBOUNCE_TIME_US) {
                    instance->button_pressed = true;
                    instance->button_press_time = current_time;
                    // Check for multiple clicks within timeout
                    if (current_time - instance->last_click_time < CLICK_TIMEOUT_US) {
                        instance->click_count++;
                    } else {
                        instance->click_count = 1;  // First click or timeout expired
                    }
                    instance->last_click_time = current_time;
                    printf("[DEBUG] BUTTON IRQ: click_count=%lu\n", instance->click_count);
                }
            } else {
                printf("[DEBUG] BUTTON IRQ IGNORED: CLK or DT LOW (rotation in progress)\n");
            }
        }
        // Always re-enable interrupt after handling (both press and release)
        if (gpio_get(instance->pin_sw)) {
            instance->button_pressed = false;
        }
        gpio_set_irq_enabled(instance->pin_sw, GPIO_IRQ_EDGE_FALL, true);
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
    // Reset button pressed flag immediately on release
    if (!current_button && button_pressed) {
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
    // Only button press should ever return ENTER, never from rotation
    if (button_pressed) {
        return MenuDirection::ENTER;
    }
    if (delta > 0) {
        return MenuDirection::DOWN;
    } else if (delta < 0) {
        return MenuDirection::UP;
    }
    return MenuDirection::NONE;
}
