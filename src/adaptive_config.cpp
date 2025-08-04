#include "adaptive_config.h"
#include "pico/time.h"
#include <cmath>

uint32_t calculate_optimal_update_rate(float current_speed, uint32_t fifo_errors) {
    // Start with speed-based rate
    uint32_t rate;
    
    if (current_speed < SPEED_THRESHOLD_LOW) {
        rate = UPDATE_RATE_SLOW;
    } else if (current_speed < SPEED_THRESHOLD_MED) {
        rate = UPDATE_RATE_NORMAL;
    } else if (current_speed < SPEED_THRESHOLD_HIGH) {
        rate = UPDATE_RATE_FAST;
    } else {
        rate = UPDATE_RATE_TURBO;
    }
    
    // Adjust based on performance issues
    if (fifo_errors > FIFO_WARNING_THRESHOLD) {
        // System struggling, slow down updates
        rate = rate * 2;
        if (rate > UPDATE_RATE_SLOW) rate = UPDATE_RATE_SLOW;
    }
    
    return rate;
}

EncoderPreset recommend_encoder_preset(float avg_speed, uint32_t performance_issues) {
    if (performance_issues > FIFO_WARNING_THRESHOLD) {
        return PRESET_SPEED;  // Prioritize speed over precision
    } else if (avg_speed < SPEED_THRESHOLD_LOW) {
        return PRESET_PRECISION;  // Prioritize precision for slow movements
    } else {
        return PRESET_BALANCED;   // Balanced for normal operation
    }
}

void update_speed_history(AdaptiveConfig* config, float current_speed) {
    config->speed_history[config->history_index] = fabsf(current_speed);
    config->history_index = (config->history_index + 1) % 8;
}

float get_average_speed(const AdaptiveConfig* config) {
    float sum = 0.0f;
    for (int i = 0; i < 8; i++) {
        sum += config->speed_history[i];
    }
    return sum / 8.0f;
}
