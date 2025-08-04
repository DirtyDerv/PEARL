#ifndef ADAPTIVE_CONFIG_H
#define ADAPTIVE_CONFIG_H

#include <cstdint>

// Adaptive Configuration System v0.04
// Dynamic performance tuning based on encoder speed and system load

// Display update rates (milliseconds)
#define UPDATE_RATE_SLOW     500    // Low speed operation
#define UPDATE_RATE_NORMAL   100    // Normal operation  
#define UPDATE_RATE_FAST     50     // High speed operation
#define UPDATE_RATE_TURBO    20     // Maximum performance

// Speed thresholds for adaptive update rates
#define SPEED_THRESHOLD_LOW   1.0f   // units/sec
#define SPEED_THRESHOLD_MED   5.0f   // units/sec  
#define SPEED_THRESHOLD_HIGH  15.0f  // units/sec

// Performance thresholds
#define FIFO_WARNING_THRESHOLD  5    // FIFO overflows per minute
#define TRANSITION_ERROR_THRESHOLD 10 // Invalid transitions per minute

// Encoder configuration presets
enum EncoderPreset {
    PRESET_PRECISION,    // High resolution, low speed
    PRESET_BALANCED,     // Default balanced mode
    PRESET_SPEED,        // High speed, lower resolution
    PRESET_CUSTOM        // User-defined settings
};

// Adaptive configuration structure
struct AdaptiveConfig {
    uint32_t current_update_rate;
    EncoderPreset current_preset;
    bool auto_adaptive_enabled;
    float speed_history[8];
    uint8_t history_index;  // Renamed to avoid potential conflicts
    uint32_t last_adaptation_time;
};

// Function declarations
uint32_t calculate_optimal_update_rate(float current_speed, uint32_t fifo_errors);
EncoderPreset recommend_encoder_preset(float avg_speed, uint32_t performance_issues);
void update_speed_history(AdaptiveConfig* config, float current_speed);
float get_average_speed(const AdaptiveConfig* config);

#endif // ADAPTIVE_CONFIG_H
