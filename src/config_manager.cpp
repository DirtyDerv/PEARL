#include "config_manager.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include <cstring>
#include <cmath>
#include <cstdio>

// Global configuration manager instance
ConfigManager g_config_manager;

// CRC16 calculation for checksum
static uint16_t crc16(const uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

ConfigManager::ConfigManager() : config_loaded(false), config_modified(false), 
                                 last_save_time(0), last_backup_time(0), last_change_time(0),
                                 pending_changes_count(0), wear_leveling_enabled(true),
                                 next_wear_level_check(0) {
    set_factory_defaults();
}

bool ConfigManager::init() {
    printf("[DEBUG] ConfigManager::init() - set_factory_defaults\n");
    set_factory_defaults();
    printf("[DEBUG] ConfigManager::init() - calling load_config()\n");
    // Try to load configuration from flash
    if (load_config()) {
        printf("[DEBUG] ConfigManager::init() - load_config() returned true\n");
        config_loaded = true;
        config_modified = false;
        mark_change_time();
        printf("[DEBUG] ConfigManager::init() - checking wear_leveling_enabled\n");
        // Enable wear leveling by default for industrial applications
        if (wear_leveling_enabled) {
            printf("[DEBUG] ConfigManager::init() - calling perform_wear_leveling()\n");
            perform_wear_leveling();
        }
        printf("[DEBUG] ConfigManager::init() - returning true\n");
        return true;
    } else {
        printf("[DEBUG] ConfigManager::init() - load_config() returned false -- resetting to factory defaults and saving to flash!\n");
        set_factory_defaults();
        config_loaded = true;
        config_modified = true;
        bool save_ok = save_config();
        printf("[DEBUG] ConfigManager::init() - factory defaults saved to flash, save_config returned %d\n", save_ok);
        return save_ok;
    }
}

void ConfigManager::set_factory_defaults() {
    memset(&factory_config, 0, sizeof(PersistentConfig));
    
    factory_config.magic = CONFIG_MAGIC_NUMBER;
    factory_config.version = CONFIG_VERSION;
    factory_config.save_count = 0;
    factory_config.last_save_time = 0;
    factory_config.write_cycle_count = 0;
    factory_config.primary_sector_index = 0;
    factory_config.last_wear_level_time = 0;
    
    // Encoder defaults
    factory_config.encoder_resolution = 500;
    factory_config.thread_pitch = 1.0f;
    factory_config.pio_enabled = true; // Force PIO mode by default
    
    // Display defaults
    factory_config.update_rate = UPDATE_RATE_NORMAL;
    factory_config.contrast = 128;
    factory_config.backlight_enabled = true;
    factory_config.backlight_timeout = 30000;  // 30 seconds
    
    // Menu defaults
    factory_config.engineering_password = 1234;
    factory_config.menu_timeout = 60000;  // 60 seconds
    factory_config.splash_enabled = true;
    
    // Adaptive configuration defaults
    factory_config.adaptive_config.current_update_rate = UPDATE_RATE_NORMAL;
    factory_config.adaptive_config.current_preset = PRESET_BALANCED;
    factory_config.adaptive_config.auto_adaptive_enabled = true;
    factory_config.adaptive_config.history_index = 0;
    factory_config.adaptive_config.last_adaptation_time = 0;
    memset(factory_config.adaptive_config.speed_history, 0, sizeof(factory_config.adaptive_config.speed_history));
    
    // Calibration defaults
    factory_config.position_offset = 0.0f;
    factory_config.scale_factor = 1.0f;
    factory_config.calibration_valid = false;
    
    // System defaults
    factory_config.auto_save_enabled = true;
    factory_config.save_interval = 300000;  // 5 minutes
    factory_config.error_log_level = 2;     // Warning level
    
    // Calculate checksum
    factory_config.checksum = calculate_checksum(&factory_config);
    
    // Copy factory defaults to current config
    memcpy(&current_config, &factory_config, sizeof(PersistentConfig));
}

uint16_t ConfigManager::calculate_checksum(const PersistentConfig* config) {
    // Calculate CRC16 of entire structure except the checksum field itself
    const uint8_t* data = (const uint8_t*)config;
    size_t offset = offsetof(PersistentConfig, checksum);
    size_t size1 = offset;
    size_t size2 = sizeof(PersistentConfig) - offset - sizeof(uint16_t);
    
    uint16_t crc = crc16(data, size1);
    crc = crc16(data + offset + sizeof(uint16_t), size2) ^ crc;
    
    return crc;
}

bool ConfigManager::validate_config(const PersistentConfig* config) {
    // Check magic number
    if (config->magic != CONFIG_MAGIC_NUMBER) {
        return false;
    }
    
    // Check version compatibility
    if (config->version > CONFIG_VERSION) {
        return false;  // Future version, can't handle
    }
    
    // Verify checksum
    uint16_t calculated_checksum = calculate_checksum(config);
    if (config->checksum != calculated_checksum) {
        return false;
    }
    
    // Validate critical parameters
    if (!is_valid_encoder_resolution(config->encoder_resolution) ||
        !is_valid_thread_pitch(config->thread_pitch) ||
        !is_valid_update_rate(config->update_rate) ||
        !is_valid_password(config->engineering_password)) {
        return false;
    }
    
    return true;
}

bool ConfigManager::write_to_flash(const PersistentConfig* config, uint32_t offset) {
    // Disable interrupts during flash write
    uint32_t ints = save_and_disable_interrupts();
    
    // Erase the flash sector (4KB sectors)
    flash_range_erase(offset, 4096);
    
    // Write configuration to flash
    flash_range_program(offset, (const uint8_t*)config, sizeof(PersistentConfig));
    
    restore_interrupts(ints);
    
    return true;  // Assume success as these functions don't return error codes
}

bool ConfigManager::read_from_flash(PersistentConfig* config, uint32_t offset) {
    // Read directly from flash memory
    const uint8_t* flash_data = (const uint8_t*)(XIP_BASE + offset);
    memcpy(config, flash_data, sizeof(PersistentConfig));
    
    return true;
}

uint32_t ConfigManager::get_backup_offset(uint8_t backup_index) {
    return CONFIG_FLASH_OFFSET + ((backup_index + 1) * 4096);
}

bool ConfigManager::load_config() {
    printf("[DEBUG] ConfigManager::load_config() - entered\n");
    static PersistentConfig temp_config;
    printf("[DEBUG] ConfigManager::load_config() - calling read_from_flash\n");
    // Try to load primary configuration
    if (read_from_flash(&temp_config, CONFIG_FLASH_OFFSET) && validate_config(&temp_config)) {
        printf("[DEBUG] ConfigManager::load_config() - read_from_flash and validate_config returned true\n");
        memcpy(&current_config, &temp_config, sizeof(PersistentConfig));
        return true;
    }
    printf("[DEBUG] ConfigManager::load_config() - trying backups\n");
    // Primary config failed, try backups
    for (uint8_t i = 0; i < CONFIG_MAX_BACKUPS; i++) {
        uint32_t backup_offset = get_backup_offset(i);
        printf("[DEBUG] ConfigManager::load_config() - backup %d offset: 0x%08lX\n", i, (unsigned long)backup_offset);
        printf("[DEBUG] ConfigManager::load_config() - before read_from_flash for backup %d\n", i);
        int read_result = read_from_flash(&temp_config, backup_offset);
        printf("[DEBUG] ConfigManager::load_config() - after read_from_flash for backup %d, result: %d\n", i, read_result);
        if (read_result && validate_config(&temp_config)) {
            printf("[DEBUG] ConfigManager::load_config() - backup %d valid\n", i);
            memcpy(&current_config, &temp_config, sizeof(PersistentConfig));
            // Also restore to primary location
            write_to_flash(&current_config, CONFIG_FLASH_OFFSET);
            return true;
        }
    }
    printf("[DEBUG] ConfigManager::load_config() - no valid config found\n");
    return false;  // No valid configuration found
}

bool ConfigManager::save_config() {
    printf("[DEBUG] ConfigManager::save_config() - FIRST LINE\n");
    // Update config metadata
    current_config.save_count++;
    printf("[DEBUG] ConfigManager::save_config() - save_count incremented\n");
    current_config.last_save_time = to_ms_since_boot(get_absolute_time());
    printf("[DEBUG] ConfigManager::save_config() - last_save_time set\n");
    current_config.write_cycle_count++;
    printf("[DEBUG] ConfigManager::save_config() - write_cycle_count incremented\n");
    current_config.checksum = calculate_checksum(&current_config);
    printf("[DEBUG] ConfigManager::save_config() - checksum recalculated\n");
    // Write to primary flash sector only
    printf("[DEBUG] ConfigManager::save_config() - calling write_to_flash\n");
    bool success = write_to_flash(&current_config, CONFIG_FLASH_OFFSET);
    printf("[DEBUG] ConfigManager::save_config() - write_to_flash returned %d\n", success);
    return success;
}

bool ConfigManager::create_backup() {
    if (!config_loaded) {
        return false;
    }
    
    // Find oldest backup slot by rotating through them
    static uint8_t next_backup_slot = 0;
    uint32_t backup_offset = get_backup_offset(next_backup_slot);
    
    bool success = write_to_flash(&current_config, backup_offset);
    
    if (success) {
        last_backup_time = to_ms_since_boot(get_absolute_time());
        next_backup_slot = (next_backup_slot + 1) % CONFIG_MAX_BACKUPS;
    }
    
    return success;
}

bool ConfigManager::restore_from_backup(uint8_t backup_index) {
    if (backup_index >= CONFIG_MAX_BACKUPS) {
        return false;
    }
    
    PersistentConfig temp_config;
    uint32_t backup_offset = get_backup_offset(backup_index);
    
    if (read_from_flash(&temp_config, backup_offset) && validate_config(&temp_config)) {
        memcpy(&current_config, &temp_config, sizeof(PersistentConfig));
        config_modified = true;
        return save_config();
    }
    
    return false;
}

bool ConfigManager::factory_reset() {
    memcpy(&current_config, &factory_config, sizeof(PersistentConfig));
    current_config.save_count = 0;
    current_config.last_save_time = 0;
    current_config.write_cycle_count = 0;
    current_config.primary_sector_index = 0;
    current_config.last_wear_level_time = 0;
    config_modified = true;
    mark_change_time();
    return save_config();
}

// Parameter setter methods with validation
bool ConfigManager::set_encoder_resolution(uint32_t resolution) {
    if (!is_valid_encoder_resolution(resolution)) {
        return false;
    }
    
    if (current_config.encoder_resolution != resolution) {
        current_config.encoder_resolution = resolution;
        config_modified = true;
    }
    return true;
}

bool ConfigManager::set_thread_pitch(float pitch) {
    if (!is_valid_thread_pitch(pitch)) {
        return false;
    }
    
    if (current_config.thread_pitch != pitch) {
        current_config.thread_pitch = pitch;
        config_modified = true;
    }
    return true;
}

bool ConfigManager::set_pio_enabled(bool enabled) {
    if (current_config.pio_enabled != enabled) {
        current_config.pio_enabled = enabled;
        config_modified = true;
    }
    return true;
}

bool ConfigManager::set_update_rate(uint32_t rate) {
    if (!is_valid_update_rate(rate)) {
        return false;
    }
    
    if (current_config.update_rate != rate) {
        current_config.update_rate = rate;
        config_modified = true;
    }
    return true;
}

bool ConfigManager::set_engineering_password(uint16_t password) {
    if (!is_valid_password(password)) {
        return false;
    }
    
    if (current_config.engineering_password != password) {
        current_config.engineering_password = password;
        config_modified = true;
    }
    return true;
}

bool ConfigManager::set_position_offset(float offset) {
    if (current_config.position_offset != offset) {
        current_config.position_offset = offset;
        config_modified = true;
    }
    return true;
}

bool ConfigManager::set_scale_factor(float factor) {
    if (factor <= 0.0f || factor > 10.0f) {  // Reasonable scale factor limits
        return false;
    }
    
    if (current_config.scale_factor != factor) {
        current_config.scale_factor = factor;
        config_modified = true;
    }
    return true;
}

bool ConfigManager::set_auto_save(bool enabled) {
    if (current_config.auto_save_enabled != enabled) {
        current_config.auto_save_enabled = enabled;
        config_modified = true;
    }
    return true;
}

// Update method for auto-save functionality
void ConfigManager::update() {
    absolute_time_t now = get_absolute_time();
    uint32_t current_time = to_ms_since_boot(now);
    
    // Check for pending changes and flush if delay has passed
    if (pending_changes_count > 0 && (current_time - last_change_time) >= CONFIG_WRITE_DELAY_MS) {
        flush_pending_changes();
    }
    
    // Periodic wear leveling check (every 10 minutes)
    if (wear_leveling_enabled && current_time >= next_wear_level_check) {
        perform_wear_leveling();
        next_wear_level_check = current_time + (10 * 60 * 1000); // 10 minutes
    }
    
    // Traditional auto-save for compatibility
    if (!config_loaded || !current_config.auto_save_enabled || (!config_modified && pending_changes_count == 0)) {
        return;
    }
    
    // Check if auto-save interval has elapsed (fallback for urgent saves)
    if (current_time - last_save_time >= current_config.save_interval) {
        save_config();
    }
}

bool ConfigManager::validate_current_config() {
    return validate_config(&current_config);
}

bool ConfigManager::import_config(const PersistentConfig* config) {
    if (!validate_config(config)) {
        return false;
    }
    
    memcpy(&current_config, config, sizeof(PersistentConfig));
    config_modified = true;
    return true;
}

bool ConfigManager::export_config(PersistentConfig* config) const {
    if (!config_loaded) {
        return false;
    }
    
    memcpy(config, &current_config, sizeof(PersistentConfig));
    return true;
}

// Industrial-grade optimization methods
void ConfigManager::mark_change_time() {
    last_change_time = to_ms_since_boot(get_absolute_time());
    pending_changes_count++;
}

void ConfigManager::flush_pending_changes() {
    if (pending_changes_count > 0 && config_modified) {
        save_config();
        pending_changes_count = 0;
    }
}

void ConfigManager::perform_wear_leveling() {
    if (!config_loaded || !wear_leveling_enabled) {
        return;
    }
    
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Only perform wear leveling if enough time has passed (1 hour minimum)
    if (current_time - current_config.last_wear_level_time < (60 * 60 * 1000)) {
        return;
    }
    
    // Rotate to next sector
    uint32_t next_sector = (current_config.primary_sector_index + 1) % CONFIG_PRIMARY_SECTORS;
    
    // Calculate sector addresses
    uint32_t current_sector_addr = CONFIG_FLASH_OFFSET + (current_config.primary_sector_index * FLASH_SECTOR_SIZE);
    uint32_t next_sector_addr = CONFIG_FLASH_OFFSET + (next_sector * FLASH_SECTOR_SIZE);
    
    // Erase next sector
    flash_range_erase(next_sector_addr - XIP_BASE, FLASH_SECTOR_SIZE);
    
    // Update sector index and timestamp
    current_config.primary_sector_index = next_sector;
    current_config.last_wear_level_time = current_time;
    current_config.write_cycle_count++;
    
    // Save to new sector
    save_config_to_sector(next_sector_addr);
}

bool ConfigManager::save_config_to_sector(uint32_t sector_addr) {
    // Update metadata
    current_config.save_count++;
    current_config.last_save_time = to_ms_since_boot(get_absolute_time());
    current_config.checksum = calculate_checksum(&current_config);
    
    // Prepare buffer for flash write (must be page-aligned)
    uint8_t write_buffer[FLASH_PAGE_SIZE];
    memset(write_buffer, 0xFF, FLASH_PAGE_SIZE);
    memcpy(write_buffer, &current_config, sizeof(PersistentConfig));
    
    // Disable interrupts during flash operation
    uint32_t interrupts = save_and_disable_interrupts();
    
    // Write to flash
    flash_range_program(sector_addr - XIP_BASE, write_buffer, FLASH_PAGE_SIZE);
    
    // Re-enable interrupts
    restore_interrupts(interrupts);
    
    return true;
}

void ConfigManager::enable_wear_leveling(bool enable) {
    wear_leveling_enabled = enable;
    if (enable) {
        next_wear_level_check = to_ms_since_boot(get_absolute_time()) + (10 * 60 * 1000);
    }
}

void ConfigManager::force_wear_leveling() {
    if (wear_leveling_enabled) {
        current_config.last_wear_level_time = 0; // Force immediate wear leveling
        perform_wear_leveling();
    }
}

void ConfigManager::set_write_delay(uint32_t delay_ms) {
    // This would require a configurable delay, but for now we use the compile-time constant
    // In a full implementation, you might store this in a separate variable
}

// Static validation methods
bool ConfigManager::is_valid_encoder_resolution(uint32_t resolution) {
    return (resolution >= MIN_ENCODER_RESOLUTION && resolution <= MAX_ENCODER_RESOLUTION);
}

bool ConfigManager::is_valid_thread_pitch(float pitch) {
    return (pitch >= MIN_THREAD_PITCH && pitch <= MAX_THREAD_PITCH);
}

bool ConfigManager::is_valid_update_rate(uint32_t rate) {
    return (rate >= MIN_UPDATE_RATE && rate <= MAX_UPDATE_RATE);
}

// Helper methods for industrial optimization
bool ConfigManager::load_config_from_flash(PersistentConfig* config) {
    printf("[DEBUG] ConfigManager::load_config_from_flash() - entered\n");
    if (!config) return false;
    // Read from current primary sector
    uint32_t sector_addr = CONFIG_FLASH_OFFSET;
    const PersistentConfig* flash_config = (const PersistentConfig*)sector_addr;
    printf("[DEBUG] ConfigManager::load_config_from_flash() - checking magic\n");
    // Check magic number and checksum
    if (flash_config->magic == CONFIG_MAGIC_NUMBER) {
        printf("[DEBUG] ConfigManager::load_config_from_flash() - magic ok\n");
        uint16_t calculated_checksum = calculate_checksum(flash_config);
        if (calculated_checksum == flash_config->checksum) {
            printf("[DEBUG] ConfigManager::load_config_from_flash() - checksum ok\n");
            memcpy(config, flash_config, sizeof(PersistentConfig));
            return true;
        }
    }
    printf("[DEBUG] ConfigManager::load_config_from_flash() - not valid\n");
    return false;
}

bool ConfigManager::configs_equal(const PersistentConfig* config1, const PersistentConfig* config2) {
    if (!config1 || !config2) return false;
    
    // Compare significant fields only (ignore timestamps and counters)
    return (config1->encoder_resolution == config2->encoder_resolution &&
            fabs(config1->thread_pitch - config2->thread_pitch) < CONFIG_SIGNIFICANT_THRESHOLD &&
            config1->contrast == config2->contrast &&
            config1->auto_save_enabled == config2->auto_save_enabled &&
            config1->save_interval == config2->save_interval &&
            config1->engineering_password == config2->engineering_password);
}

bool ConfigManager::is_valid_password(uint16_t password) {
    return (password >= MIN_PASSWORD && password <= MAX_PASSWORD);
}
