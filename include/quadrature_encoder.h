#ifndef QUADRATURE_ENCODER_H
#define QUADRATURE_ENCODER_H

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "quadrature.pio.h"

#define VELOCITY_SAMPLE_COUNT 8  // Number of samples for velocity averaging

class QuadratureEncoder {
protected:  // Changed from private to allow inheritance
    PIO pio;
    uint sm;
    uint pin_a, pin_b;
    int64_t position;      // SAFETY FIX: Changed from int32_t to prevent overflow
    uint8_t last_state;
    float pitch;           // Thread pitch (distance per revolution)
    uint32_t resolution;   // Encoder pulses per revolution
    
private:
    // Velocity calculation variables
    int32_t velocity_samples[VELOCITY_SAMPLE_COUNT];
    uint32_t time_samples[VELOCITY_SAMPLE_COUNT];
    uint8_t sample_index;
    float current_velocity;
    
    // Frequency monitoring (v0.02 enhancement)
    uint32_t last_frequency_check;
    uint32_t transitions_per_second;
    float encoder_frequency_hz;
    
    // Performance monitoring (v0.03 enhancement)
    uint32_t fifo_overflow_count;
    uint32_t invalid_transition_count;
    uint32_t performance_check_time;
    bool performance_warning;
    uint32_t last_update_time;
    bool use_pio;
    
    void update_position();
    void update_velocity();
    int8_t decode_transition(uint8_t current_state);
    void add_velocity_sample(int32_t position_delta, uint32_t time_delta);
    
public:
    QuadratureEncoder(PIO pio_instance, uint state_machine, uint encoder_pin_a, uint encoder_pin_b, 
                     float thread_pitch = 1.0f, uint32_t encoder_resolution = 500, bool enable_pio = true);
    
    virtual void init();
    virtual void update();
    
    // Position methods
    int64_t get_raw_position() const { return position; }  // SAFETY FIX: Updated return type
    float get_distance() const;
    void reset_position() { position = 0; }
    void set_position(int64_t new_position) { position = new_position; }  // SAFETY FIX: Updated parameter type
    
    // Velocity methods
    float get_velocity() const { return current_velocity; }      // Distance per second
    float get_rpm() const;                                       // Revolutions per minute
    float get_speed_percentage(float max_speed) const;           // Percentage of max speed
    
    // Frequency monitoring methods (v0.02 enhancement)
    float get_encoder_frequency() const { return encoder_frequency_hz; }  // Hz
    uint32_t get_transitions_per_second() const { return transitions_per_second; }
    float get_max_theoretical_rpm() const;                       // Based on encoder frequency
    
    // Performance monitoring methods (v0.03 enhancement)
    uint32_t get_fifo_overflow_count() const { return fifo_overflow_count; }
    uint32_t get_invalid_transition_count() const { return invalid_transition_count; }
    bool has_performance_warning() const { return performance_warning; }
    void reset_performance_counters();
    
    // Configuration methods
    void set_pitch(float new_pitch) { pitch = new_pitch; }
    void set_resolution(uint32_t new_resolution) { resolution = new_resolution; }
    void enable_pio_mode(bool enable) { use_pio = enable; }
    
    // Getters
    float get_pitch() const { return pitch; }
    uint32_t get_resolution() const { return resolution; }
    bool is_pio_enabled() const { return use_pio; }
};

#endif // QUADRATURE_ENCODER_H
