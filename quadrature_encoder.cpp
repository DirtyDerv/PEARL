#include "quadrature_encoder.h"
#include "hardware/gpio.h"

QuadratureEncoder::QuadratureEncoder(PIO pio_instance, uint state_machine, 
                                   uint encoder_pin_a, uint encoder_pin_b,
                                   float thread_pitch, uint32_t encoder_resolution) {
    pio = pio_instance;
    sm = state_machine;
    pin_a = encoder_pin_a;
    pin_b = encoder_pin_b;
    position = 0;
    last_state = 0;
    pitch = thread_pitch;
    resolution = encoder_resolution;
}

void QuadratureEncoder::init() {
    // Set up GPIO pins
    gpio_init(pin_a);
    gpio_init(pin_b);
    gpio_set_dir(pin_a, GPIO_IN);
    gpio_set_dir(pin_b, GPIO_IN);
    gpio_pull_up(pin_a);
    gpio_pull_up(pin_b);
    
    // Initialize PIO program (we'll use a simpler approach than the .pio file for now)
    // Read initial state
    last_state = (gpio_get(pin_b) << 1) | gpio_get(pin_a);
}

void QuadratureEncoder::update() {
    // Read current state of both encoder pins
    uint8_t current_state = (gpio_get(pin_b) << 1) | gpio_get(pin_a);
    
    // Only process if state has changed
    if (current_state != last_state) {
        int8_t direction = decode_transition(current_state);
        position += direction;
        last_state = current_state;
    }
}

int8_t QuadratureEncoder::decode_transition(uint8_t current_state) {
    // Quadrature decoder lookup table
    // Based on previous state and current state, determine direction
    static const int8_t transition_table[16] = {
        0,  -1,   1,   0,   // 00 -> 00,01,10,11
        1,   0,   0,  -1,   // 01 -> 00,01,10,11  
       -1,   0,   0,   1,   // 10 -> 00,01,10,11
        0,   1,  -1,   0    // 11 -> 00,01,10,11
    };
    
    uint8_t table_index = (last_state << 2) | current_state;
    return transition_table[table_index];
}

float QuadratureEncoder::get_distance() const {
    // Calculate distance based on position, pitch, and resolution
    // Distance = (position / resolution) * pitch
    return (float)position * pitch / (float)resolution;
}
