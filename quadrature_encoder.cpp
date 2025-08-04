#include "quadrature_encoder.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include <cmath>

QuadratureEncoder::QuadratureEncoder(PIO pio_instance, uint state_machine, 
                                   uint encoder_pin_a, uint encoder_pin_b,
                                   float thread_pitch, uint32_t encoder_resolution, bool enable_pio) {
    pio = pio_instance;
    sm = state_machine;
    pin_a = encoder_pin_a;
    pin_b = encoder_pin_b;
    position = 0;
    last_state = 0;
    pitch = thread_pitch;
    resolution = encoder_resolution;
    use_pio = enable_pio;
    
    // Initialize velocity tracking
    current_velocity = 0.0f;
    sample_index = 0;
    last_update_time = to_ms_since_boot(get_absolute_time());
    
    // Clear velocity samples
    for (int i = 0; i < VELOCITY_SAMPLE_COUNT; i++) {
        velocity_samples[i] = 0;
        time_samples[i] = 0;
    }
}

void QuadratureEncoder::init() {
    if (use_pio) {
        // Load and start PIO program
        uint offset = pio_add_program(pio, &quadrature_program);
        quadrature_program_init(pio, sm, offset, pin_a, pin_b);
    } else {
        // Set up GPIO pins for manual reading
        gpio_init(pin_a);
        gpio_init(pin_b);
        gpio_set_dir(pin_a, GPIO_IN);
        gpio_set_dir(pin_b, GPIO_IN);
        gpio_pull_up(pin_a);
        gpio_pull_up(pin_b);
        
        // Read initial state
        last_state = (gpio_get(pin_b) << 1) | gpio_get(pin_a);
    }
}

void QuadratureEncoder::update() {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    int32_t old_position = position;
    
    if (use_pio) {
        // Read from PIO FIFO
        int32_t delta;
        while (quadrature_get_count(pio, sm, &delta)) {
            position += delta;
        }
    } else {
        // Manual GPIO reading with software decoding
        uint8_t current_state = (gpio_get(pin_b) << 1) | gpio_get(pin_a);
        
        if (current_state != last_state) {
            int8_t direction = decode_transition(current_state);
            position += direction;
            last_state = current_state;
        }
    }
    
    // Update velocity if position changed
    if (position != old_position) {
        int32_t position_delta = position - old_position;
        uint32_t time_delta = current_time - last_update_time;
        add_velocity_sample(position_delta, time_delta);
        last_update_time = current_time;
    }
    
    // Update velocity calculation
    update_velocity();
}

void QuadratureEncoder::add_velocity_sample(int32_t position_delta, uint32_t time_delta) {
    velocity_samples[sample_index] = position_delta;
    time_samples[sample_index] = time_delta;
    sample_index = (sample_index + 1) % VELOCITY_SAMPLE_COUNT;
}

void QuadratureEncoder::update_velocity() {
    int32_t total_position_delta = 0;
    uint32_t total_time_delta = 0;
    
    // Sum all samples for averaging
    for (int i = 0; i < VELOCITY_SAMPLE_COUNT; i++) {
        total_position_delta += velocity_samples[i];
        total_time_delta += time_samples[i];
    }
    
    // Calculate velocity in distance units per second
    if (total_time_delta > 0) {
        float pulses_per_ms = (float)total_position_delta / (float)total_time_delta;
        float pulses_per_sec = pulses_per_ms * 1000.0f;
        current_velocity = (pulses_per_sec * pitch) / (float)resolution;
    } else {
        current_velocity = 0.0f;
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

float QuadratureEncoder::get_rpm() const {
    // Convert velocity to RPM
    // velocity is in distance units per second
    // RPM = (velocity / pitch) * 60
    if (pitch > 0) {
        return (current_velocity / pitch) * 60.0f;
    }
    return 0.0f;
}

float QuadratureEncoder::get_speed_percentage(float max_speed) const {
    if (max_speed > 0) {
        return (fabsf(current_velocity) / max_speed) * 100.0f;
    }
    return 0.0f;
}
