# PEARL v0.07 - Configuration Management System

## 🔧 Enhanced Configuration Management

PEARL now includes a comprehensive flash-based configuration management system that provides persistent storage, validation, and backup capabilities for all system parameters.

## ✨ Features

### **Persistent Flash Storage**
- **Flash-based Storage**: Configuration saved to dedicated flash memory region
- **CRC16 Validation**: Data integrity protection with checksum verification
- **Magic Number Protection**: Prevents corruption from invalid data
- **Version Compatibility**: Handles configuration version upgrades gracefully

### **Backup & Recovery**
- **Multiple Backups**: Automatic creation of 3 rolling backup copies
- **Backup Restoration**: Restore from any backup if primary config corrupts
- **Factory Reset**: Complete reset to factory defaults
- **Auto-Recovery**: Automatic fallback to backups if primary config fails

### **Parameter Validation**
- **Range Checking**: All parameters validated against safe operating limits
- **Type Safety**: Strong typing prevents configuration errors
- **Boundary Enforcement**: Automatic clamping to safe ranges

### **Auto-Save Functionality**
- **Configurable Auto-Save**: Automatic saving at configurable intervals
- **Change Tracking**: Only saves when configuration actually changes
- **Manual Save Control**: Immediate save capability when needed

## 📊 Managed Parameters

### **Encoder Configuration**
```cpp
- encoder_resolution: 100-10000 PPR (default: 500)
- thread_pitch: 0.1-50.0 mm (default: 1.0)
- pio_enabled: true/false (default: true)
```

### **Display Settings**
```cpp
- update_rate: 10-1000 ms (default: 100)
- contrast: 0-255 (default: 128)
- backlight_enabled: true/false (default: true)
- backlight_timeout: milliseconds (default: 30000)
```

### **Menu & Security**
```cpp
- engineering_password: 1-9999 (default: 1234)
- menu_timeout: milliseconds (default: 60000)
- splash_enabled: true/false (default: true)
```

### **Calibration & Positioning**
```cpp
- position_offset: float (default: 0.0)
- scale_factor: 0.1-10.0 (default: 1.0)
- calibration_valid: true/false (default: false)
```

### **System Settings**
```cpp
- auto_save_enabled: true/false (default: true)
- save_interval: milliseconds (default: 300000)
- error_log_level: 0-3 (default: 2)
```

## 🔧 API Usage

### **Basic Parameter Access**
```cpp
// Reading parameters
uint32_t resolution = GET_ENCODER_RESOLUTION();
float pitch = GET_THREAD_PITCH();
bool pio_enabled = GET_PIO_ENABLED();

// Setting parameters (with validation)
SET_ENCODER_RESOLUTION(1000);
SET_THREAD_PITCH(2.5f);
SET_PIO_ENABLED(false);

// Save changes
SAVE_CONFIG();
```

### **Advanced Configuration Management**
```cpp
// Direct access to configuration manager
ConfigManager& config = g_config_manager;

// Check if configuration is loaded and valid
if (config.is_config_loaded()) {
    printf("Config loaded, save count: %d\n", config.get_save_count());
}

// Manual backup creation
config.create_backup();

// Factory reset
config.factory_reset();

// Restore from specific backup
config.restore_from_backup(0);  // Restore from backup slot 0
```

### **Parameter Validation**
```cpp
// Validate before setting
if (ConfigManager::is_valid_encoder_resolution(2000)) {
    SET_ENCODER_RESOLUTION(2000);
} else {
    printf("Invalid encoder resolution\n");
}

// Check current config validity
if (g_config_manager.validate_current_config()) {
    printf("Configuration is valid\n");
}
```

## 💾 Flash Memory Layout

```
Flash Memory Map:
┌─────────────────────────────────────┐
│ Application Code                     │  0x00000000
├─────────────────────────────────────┤
│ ...                                 │
├─────────────────────────────────────┤
│ Primary Configuration               │  0x00040000 (256KB)
├─────────────────────────────────────┤
│ Backup Configuration #1             │  0x00041000 (260KB)  
├─────────────────────────────────────┤
│ Backup Configuration #2             │  0x00042000 (264KB)
├─────────────────────────────────────┤
│ Backup Configuration #3             │  0x00043000 (268KB)
└─────────────────────────────────────┘
```

- **Primary Config**: 4KB sector at 256KB offset
- **Backup Configs**: 3 additional 4KB sectors
- **Total Usage**: 16KB for configuration system

## ⚙️ Configuration Structure

```cpp
struct PersistentConfig {
    uint32_t magic;                    // "PERL" validation
    uint16_t version;                  // Version compatibility
    uint16_t checksum;                 // CRC16 data integrity
    uint32_t save_count;               // Save operation counter
    uint32_t last_save_time;           // Timestamp tracking
    
    // System parameters...
    uint32_t encoder_resolution;
    float thread_pitch;
    bool pio_enabled;
    uint32_t update_rate;
    // ... all configurable parameters
    
    uint8_t reserved[64];              // Future expansion
};
```

## 🛡️ Safety Features

### **Data Integrity**
- **CRC16 Checksums**: Every configuration write includes integrity check
- **Magic Number Validation**: Prevents accidental corruption
- **Version Checking**: Handles firmware upgrades gracefully

### **Failure Recovery**
- **Automatic Backup Recovery**: Falls back to backups if primary config fails
- **Factory Default Fallback**: Uses safe defaults if all configs corrupt
- **Validation on Load**: Comprehensive parameter checking on startup

### **Write Protection**
- **Parameter Validation**: All values checked before writing
- **Range Enforcement**: Automatic clamping to safe operating ranges
- **Type Safety**: Strong typing prevents configuration errors

## 🔄 Integration Points

### **Startup Sequence**
1. **Configuration System Init**: Load/validate configuration first
2. **Parameter Application**: Apply loaded parameters to hardware
3. **Splash Screen Control**: Configuration controls splash display
4. **System Initialization**: Use configured values throughout startup

### **Runtime Operation**
1. **Parameter Updates**: Live configuration changes via engineering menu
2. **Auto-Save Monitoring**: Background auto-save when parameters change
3. **Validation Checking**: Continuous parameter validation
4. **Status Reporting**: Configuration status in system diagnostics

### **Engineering Menu Integration**
- **Configuration Management Menu**: Dedicated config management section
- **Parameter Editing**: Live parameter modification with validation
- **Backup Management**: Create/restore backups from menu
- **Factory Reset**: Menu-driven factory reset capability

## 📈 Performance Impact

### **Memory Usage**
- **Flash Storage**: 16KB total (primary + 3 backups)
- **RAM Usage**: ~1KB for runtime configuration structure
- **Minimal Overhead**: Efficient CRC and validation algorithms

### **Timing**
- **Startup Time**: ~50ms additional for configuration loading
- **Save Operations**: ~10ms for flash write with interrupts disabled
- **Background Updates**: Minimal impact, only when parameters change

## 🎯 Benefits

1. **Persistence**: Settings survive power cycles and firmware updates
2. **Reliability**: Multiple backup layers prevent configuration loss
3. **Safety**: Comprehensive validation prevents invalid configurations
4. **Convenience**: Automatic parameter application at startup
5. **Professional**: Industrial-grade configuration management

## 🚀 Future Enhancements

1. **Remote Configuration**: Network-based parameter management
2. **Configuration Profiles**: Multiple named configuration sets
3. **Export/Import**: Configuration file backup to external storage
4. **Audit Trail**: Detailed logging of all configuration changes
5. **Live Monitoring**: Real-time configuration status display

The configuration management system provides PEARL with professional-grade parameter persistence and management capabilities, ensuring reliable operation and easy maintenance.
