#include "indexed_encoder.h"

IndexedEncoder::IndexedEncoder(PIO pio_instance, uint state_machine,
                              uint encoder_pin_a, uint encoder_pin_b, uint encoder_index_pin,
                              float thread_pitch, uint32_t encoder_resolution,
                              bool enable_pio, bool auto_zero)
    : QuadratureEncoder(pio_instance, state_machine, encoder_pin_a, encoder_pin_b,
                       thread_pitch, encoder_resolution, enable_pio) {
    
    index_pin = encoder_index_pin;
    index_enabled = true;
    index_found = false;
    index_position = 0;
    last_index_state = 0;
    auto_zero_on_index = auto_zero;
}

void IndexedEncoder::init() {
    // Initialize base quadrature encoder
    QuadratureEncoder::init();
    
    // Set up index pin
    if (index_enabled) {
        gpio_init(index_pin);
        gpio_set_dir(index_pin, GPIO_IN);
        gpio_pull_up(index_pin);
        last_index_state = gpio_get(index_pin);
    }
}

void IndexedEncoder::update() {
    // Update base encoder
    QuadratureEncoder::update();
    
    // Check index signal
    if (index_enabled) {
        check_index_signal();
    }
}

void IndexedEncoder::check_index_signal() {
    uint8_t current_index_state = gpio_get(index_pin);
    
    // Detect rising edge of index signal (assuming active high)
    if (current_index_state && !last_index_state) {
        index_position = get_raw_position();
        index_found = true;
        
        // Auto-zero if enabled
        if (auto_zero_on_index) {
            reset_position();
        }
    }
    
    last_index_state = current_index_state;
}

void IndexedEncoder::reset_to_index() {
    if (index_found) {
        // Reset position relative to last index position
        int32_t current_pos = get_raw_position();
        int32_t offset = current_pos - index_position;
        position = -offset;  // Direct access to protected member
    }
}

float IndexedEncoder::get_distance_from_index() const {
    if (index_found) {
        int32_t relative_position = get_raw_position() - index_position;
        return (float)relative_position * get_pitch() / (float)get_resolution();
    }
    return 0.0f;
}
