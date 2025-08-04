#ifndef QUADRATURE_ENCODER_H
#define QUADRATURE_ENCODER_H

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "quadrature.pio.h"

class QuadratureEncoder {
private:
    PIO pio;
    uint sm;
    uint pin_a, pin_b;
    int32_t position;
    uint8_t last_state;
    float pitch;           // Thread pitch (distance per revolution)
    uint32_t resolution;   // Encoder pulses per revolution
    
    void update_position();
    int8_t decode_transition(uint8_t current_state);
    
public:
    QuadratureEncoder(PIO pio_instance, uint state_machine, uint encoder_pin_a, uint encoder_pin_b, 
                     float thread_pitch = 1.0f, uint32_t encoder_resolution = 500);
    
    void init();
    void update();
    
    int32_t get_raw_position() const { return position; }
    float get_distance() const;
    void reset_position() { position = 0; }
    
    void set_pitch(float new_pitch) { pitch = new_pitch; }
    void set_resolution(uint32_t new_resolution) { resolution = new_resolution; }
    
    float get_pitch() const { return pitch; }
    uint32_t get_resolution() const { return resolution; }
};

#endif // QUADRATURE_ENCODER_H
