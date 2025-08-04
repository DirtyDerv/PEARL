#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <cstdint>
#include <cstring>
#include "adaptive_config.h"

// PEARL Configuration Management System v1.0
// Flash-based persistent parameter storage with validation and backup

// Configuration flash storage settings
#define CONFIG_FLASH_OFFSET     (256 * 1024)  // 256KB offset from start of flash
#define CONFIG_MAGIC_NUMBER     0x5045524C    // "PERL" in hex
#define CONFIG_VERSION          0x0107        // v1.07
#define CONFIG_MAX_BACKUPS      3             // Number of backup copies
#define CONFIG_SECTOR_SIZE      4096          // 4KB flash sectors
#define CONFIG_PRIMARY_SECTORS  4             // Sectors for wear leveling
#define CONFIG_WRITE_DELAY_MS   2000          // Delayed write buffer (2 seconds)
#define CONFIG_SIGNIFICANT_THRESHOLD 0.001f   // Minimum change threshold for floats

// Parameter validation limits
#define MIN_ENCODER_RESOLUTION  100
#define MAX_ENCODER_RESOLUTION  10000
#define MIN_THREAD_PITCH        0.1f
#define MAX_THREAD_PITCH        50.0f
#define MIN_UPDATE_RATE         10
#define MAX_UPDATE_RATE         1000
#define MIN_PASSWORD            1
#define MAX_PASSWORD            9999

// Configuration structure for persistent storage
struct PersistentConfig {
    uint32_t magic;                    // Magic number for validation
    uint16_t version;                  // Configuration version
    uint16_t checksum;                 // CRC16 checksum
    uint32_t save_count;               // Number of times saved
    uint32_t last_save_time;           // Timestamp of last save
    uint32_t write_cycle_count;        // Total flash write cycles for wear tracking
    uint8_t primary_sector_index;      // Current primary sector for wear leveling
    uint32_t last_wear_level_time;     // Last wear leveling operation
    
    // Encoder settings
    uint32_t encoder_resolution;       // Pulses per revolution
    float thread_pitch;                // Distance per revolution
    bool pio_enabled;                  // Use PIO for encoding
    
    // Display settings
    uint32_t update_rate;              // Display update rate (ms)
    uint8_t contrast;                  // LCD contrast (0-255)
    bool backlight_enabled;            // LCD backlight on/off
    uint32_t backlight_timeout;        // Backlight timeout (ms)
    
    // Menu settings
    uint16_t engineering_password;     // 4-digit password
    uint32_t menu_timeout;             // Menu timeout (ms)
    bool splash_enabled;               // Show splash screen
    
    // Adaptive configuration
    AdaptiveConfig adaptive_config;    // Adaptive system settings
    
    // Calibration data
    float position_offset;             // Zero position offset
    float scale_factor;                // Calibration scale factor
    bool calibration_valid;            // Calibration data validity
    
    // System settings
    bool auto_save_enabled;            // Auto-save configuration changes
    uint32_t save_interval;            // Auto-save interval (ms)
    uint8_t error_log_level;           // Error logging level (0-3)
    
    // Reserved for future expansion
    uint8_t reserved[64];              // Reserved bytes for future use
};

// Configuration manager class
class ConfigManager {
private:
    PersistentConfig current_config;
    PersistentConfig factory_config;
    bool config_loaded;
    bool config_modified;
    uint32_t last_save_time;
    uint32_t last_backup_time;
    uint32_t last_change_time;         // For delayed write buffering
    uint32_t pending_changes_count;    // Number of pending changes
    bool wear_leveling_enabled;        // Wear leveling feature toggle
    uint32_t next_wear_level_check;    // Next wear leveling check time
    
    // Internal methods
    uint16_t calculate_checksum(const PersistentConfig* config);
    bool validate_config(const PersistentConfig* config);
    bool write_to_flash(const PersistentConfig* config, uint32_t offset);
    bool read_from_flash(PersistentConfig* config, uint32_t offset);
    void set_factory_defaults();
    uint32_t get_backup_offset(uint8_t backup_index);
    uint32_t get_primary_config_offset();
    bool is_significant_change(float old_val, float new_val);
    void perform_wear_leveling();
    void flush_pending_changes();
    void mark_change_time();
    bool load_config_from_flash(PersistentConfig* config);
    bool configs_equal(const PersistentConfig* config1, const PersistentConfig* config2);
    bool save_config_to_sector(uint32_t sector_addr);
    
public:
    ConfigManager();
    
    // Initialization and management
    bool init();
    bool load_config();
    bool save_config();
    bool create_backup();
    bool restore_from_backup(uint8_t backup_index);
    bool factory_reset();
    
    // Configuration access methods
    bool set_encoder_resolution(uint32_t resolution);
    bool set_thread_pitch(float pitch);
    bool set_pio_enabled(bool enabled);
    bool set_update_rate(uint32_t rate);
    bool set_engineering_password(uint16_t password);
    bool set_position_offset(float offset);
    bool set_scale_factor(float factor);
    bool set_auto_save(bool enabled);
    
    // Configuration getters
    uint32_t get_encoder_resolution() const { return current_config.encoder_resolution; }
    float get_thread_pitch() const { return current_config.thread_pitch; }
    bool get_pio_enabled() const { return current_config.pio_enabled; }
    uint32_t get_update_rate() const { return current_config.update_rate; }
    uint16_t get_engineering_password() const { return current_config.engineering_password; }
    float get_position_offset() const { return current_config.position_offset; }
    float get_scale_factor() const { return current_config.scale_factor; }
    bool get_auto_save() const { return current_config.auto_save_enabled; }
    
    // Advanced configuration methods
    PersistentConfig* get_config() { return &current_config; }
    const PersistentConfig* get_config() const { return &current_config; }
    bool import_config(const PersistentConfig* config);
    bool export_config(PersistentConfig* config) const;
    
    // Status and diagnostics
    bool is_config_loaded() const { return config_loaded; }
    bool is_config_modified() const { return config_modified; }
    uint32_t get_save_count() const { return current_config.save_count; }
    uint32_t get_last_save_time() const { return current_config.last_save_time; }
    bool validate_current_config();
    
    // Industrial-grade optimization methods
    void enable_wear_leveling(bool enable);
    uint32_t get_write_cycle_count() const { return current_config.write_cycle_count; }
    uint8_t get_current_sector() const { return current_config.primary_sector_index; }
    void force_wear_leveling();
    void set_write_delay(uint32_t delay_ms);
    
    // Auto-save functionality
    void update();  // Call regularly to handle auto-save and delayed writes
    void mark_modified() { config_modified = true; mark_change_time(); }
    void flush_immediate();  // Force immediate write of pending changes
    
    // Parameter validation
    static bool is_valid_encoder_resolution(uint32_t resolution);
    static bool is_valid_thread_pitch(float pitch);
    static bool is_valid_update_rate(uint32_t rate);
    static bool is_valid_password(uint16_t password);
};

// Global configuration manager instance
extern ConfigManager g_config_manager;

// Convenience macros for common operations
#define GET_ENCODER_RESOLUTION()    g_config_manager.get_encoder_resolution()
#define GET_THREAD_PITCH()          g_config_manager.get_thread_pitch()
#define GET_PIO_ENABLED()           g_config_manager.get_pio_enabled()
#define GET_UPDATE_RATE()           g_config_manager.get_update_rate()
#define GET_ENGINEERING_PASSWORD()  g_config_manager.get_engineering_password()
#define GET_POSITION_OFFSET()       g_config_manager.get_position_offset()
#define GET_SCALE_FACTOR()          g_config_manager.get_scale_factor()

#define SET_ENCODER_RESOLUTION(x)   g_config_manager.set_encoder_resolution(x)
#define SET_THREAD_PITCH(x)         g_config_manager.set_thread_pitch(x)
#define SET_PIO_ENABLED(x)          g_config_manager.set_pio_enabled(x)
#define SET_UPDATE_RATE(x)          g_config_manager.set_update_rate(x)
#define SET_ENGINEERING_PASSWORD(x) g_config_manager.set_engineering_password(x)
#define SET_POSITION_OFFSET(x)      g_config_manager.set_position_offset(x)
#define SET_SCALE_FACTOR(x)         g_config_manager.set_scale_factor(x)

#define SAVE_CONFIG()               g_config_manager.save_config()
#define MARK_CONFIG_MODIFIED()      g_config_manager.mark_modified()

#endif // CONFIG_MANAGER_H
