#ifndef INDEXED_ENCODER_H
#define INDEXED_ENCODER_H

#include "quadrature_encoder.h"
#include "hardware/gpio.h"

class IndexedEncoder : public QuadratureEncoder {
private:
    uint index_pin;
    bool index_enabled;
    bool index_found;
    int32_t index_position;
    uint8_t last_index_state;
    bool auto_zero_on_index;
    
    void check_index_signal();
    
public:
    IndexedEncoder(PIO pio_instance, uint state_machine, 
                  uint encoder_pin_a, uint encoder_pin_b, uint encoder_index_pin,
                  float thread_pitch = 1.0f, uint32_t encoder_resolution = 500, 
                  bool enable_pio = true, bool auto_zero = true);
    
    void init() override;
    void update() override;
    
    // Index-specific methods
    bool has_index_signal() const { return index_found; }
    int32_t get_index_position() const { return index_position; }
    void reset_to_index();
    void enable_auto_zero(bool enable) { auto_zero_on_index = enable; }
    bool is_auto_zero_enabled() const { return auto_zero_on_index; }
    
    // Distance calculation relative to index
    float get_distance_from_index() const;
};

#endif // INDEXED_ENCODER_H
