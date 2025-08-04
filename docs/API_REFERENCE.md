# PEARL v0.07 API Reference

## Table of Contents
1. [System Architecture](#system-architecture)
2. [Core Classes](#core-classes)
3. [Configuration System](#configuration-system)
4. [Encoder Interface](#encoder-interface)
5. [Display System](#display-system)
6. [Calibration API](#calibration-api)
7. [Data Structures](#data-structures)
8. [Constants and Enums](#constants-and-enums)

## System Architecture

### Component Overview
```
┌─────────────────────────────────────┐
│              Main Application        │
│            (Bg_Reader.cpp)          │
├─────────────────────────────────────┤
│  Status Display  │  Engineering Menu│
│  (status_display)│  (engineering_menu)│ 
├─────────────────────────────────────┤
│  Config Manager  │  Adaptive Config │
│  (config_manager)│  (adaptive_config)│
├─────────────────────────────────────┤
│  Quadrature Enc  │  HW040 Encoder   │
│  (main position) │  (menu control)  │
├─────────────────────────────────────┤
│     LCD I2C      │   Splash Screen  │
│    (display)     │   (startup)      │
└─────────────────────────────────────┘
```

### File Structure
```
src/
├── Bg_Reader.cpp          # Main application
├── lcd_i2c.cpp           # Display driver
├── quadrature_encoder.cpp # Position encoder
├── indexed_encoder.cpp    # Enhanced encoder
├── hw040_encoder.cpp     # Menu encoder
├── engineering_menu.cpp  # Configuration UI
├── status_display.cpp    # Main display
├── splash_screen.cpp     # Startup screen
├── adaptive_config.cpp   # Runtime config
├── config_manager.cpp    # Config persistence
└── big_font.cpp          # Large font support

include/
├── lcd_i2c.h
├── quadrature_encoder.h
├── indexed_encoder.h
├── hw040_encoder.h
├── engineering_menu.h
├── status_display.h
├── splash_screen.h
├── adaptive_config.h
├── config_manager.h
└── big_font.h
```

## Core Classes

### QuadratureEncoder Class

#### Description
Core position measurement class providing high-precision quadrature encoder interface with PIO-based hardware acceleration.

#### Constructor
```cpp
QuadratureEncoder(PIO pio_instance, uint state_machine, 
                 uint encoder_pin_a, uint encoder_pin_b, 
                 float thread_pitch = 1.0f, 
                 uint32_t encoder_resolution = 500, 
                 bool enable_pio = true);
```

#### Public Methods

##### Position Methods
```cpp
int32_t get_raw_position() const;
float get_distance() const;
void reset_position();
void set_position(int32_t new_position);
```

##### Velocity Methods
```cpp
float get_velocity() const;      // Distance per second
float get_rpm() const;           // Revolutions per minute
float get_speed_percentage(float max_speed) const;
```

##### Configuration Methods
```cpp
void set_pitch(float new_pitch);
void set_resolution(uint32_t new_resolution);
void enable_pio_mode(bool enable);
float get_pitch() const;
uint32_t get_resolution() const;
bool is_pio_enabled() const;
```

##### Performance Monitoring
```cpp
float get_encoder_frequency() const;
uint32_t get_transitions_per_second() const;
float get_max_theoretical_rpm() const;
uint32_t get_fifo_overflow_count() const;
uint32_t get_invalid_transition_count() const;
bool has_performance_warning() const;
void reset_performance_counters();
```

### HW040Encoder Class

#### Description
Menu navigation encoder with button interface for user interaction.

#### Constructor
```cpp
HW040Encoder(uint pin_clk, uint pin_dt, uint pin_sw);
```

#### Public Methods
```cpp
void init();
void update();
int32_t get_position() const;
void reset_position();
bool is_button_pressed();
bool is_button_released();
MenuDirection get_direction();
bool has_movement() const;
uint32_t get_last_movement_time() const;
```

### LCD_I2C Class

#### Description
I2C LCD display driver with 20x4 character support and custom character capabilities.

#### Constructor
```cpp
LCD_I2C(i2c_inst_t* i2c_instance, uint8_t addr, uint8_t sda_pin, uint8_t scl_pin);
```

#### Public Methods

##### Basic Display
```cpp
void init();
void clear();
void home();
void set_cursor(uint8_t col, uint8_t row);
void print(const char* str);
void print(int value);
void print(float value, int decimals = 2);
```

##### Advanced Features
```cpp
void backlight_on();
void backlight_off();
void cursor_on();
void cursor_off();
void blink_on();
void blink_off();
void create_char(uint8_t location, uint8_t charmap[]);
void write_char(uint8_t data);
```

### EngineeringMenu Class

#### Description
Comprehensive configuration and calibration interface with hierarchical menu system.

#### Constructor
```cpp
EngineeringMenu(LCD_I2C* display, HW040Encoder* encoder, 
               QuadratureEncoder* main_enc, ConfigManager* config);
```

#### Public Methods

##### Menu Control
```cpp
void init();
void show();
void hide();
void update();
bool is_visible() const;
void handle_input(MenuDirection direction);
```

##### Calibration System
```cpp
void start_calibration();
void handle_calibration(MenuDirection direction);
void calculate_pitch_from_positions();
void apply_calibration_results();
void reset_calibration();
```

##### Configuration
```cpp
void load_configuration();
void save_configuration();
void reset_to_defaults();
void validate_settings();
```

## Configuration System

### ConfigManager Class

#### Description
Persistent configuration storage with validation and backup capabilities.

#### Public Methods

##### Storage Operations
```cpp
bool save_config(const SystemConfig& config);
bool load_config(SystemConfig& config);
bool reset_to_factory_defaults();
bool is_config_valid() const;
```

##### Backup and Restore
```cpp
bool backup_config();
bool restore_from_backup();
bool export_config(char* buffer, size_t buffer_size);
bool import_config(const char* config_data);
```

### AdaptiveConfig Class

#### Description
Runtime configuration optimization based on performance monitoring.

#### Public Methods
```cpp
void init(SystemConfig* config);
void update_performance_metrics();
void optimize_settings();
bool should_adjust_update_rate() const;
bool should_enable_pio_mode() const;
float get_recommended_cpu_frequency() const;
```

## Encoder Interface

### Encoder Data Structures

#### EncoderConfig
```cpp
struct EncoderConfig {
    uint32_t resolution;        // PPR (pulses per revolution)
    float pitch;               // Thread pitch in mm
    bool invert_direction;     // Direction inversion
    bool enable_pio;          // Hardware PIO acceleration
    uint8_t pin_a;            // Encoder A channel pin
    uint8_t pin_b;            // Encoder B channel pin
    float velocity_smoothing;  // Velocity filter coefficient
};
```

#### EncoderStatus
```cpp
struct EncoderStatus {
    int32_t position;         // Current position (counts)
    float distance;           // Distance in mm
    float velocity;           // Velocity in mm/s
    float rpm;               // RPM calculation
    uint32_t frequency;       // Encoder frequency Hz
    bool performance_warning; // Performance issues
    uint32_t error_count;     // Error counter
};
```

### Performance Monitoring

#### PerformanceMetrics
```cpp
struct PerformanceMetrics {
    uint32_t fifo_overflows;     // PIO FIFO overflow count
    uint32_t invalid_transitions; // Invalid state transitions
    float encoder_frequency;      // Current frequency Hz
    uint32_t max_frequency;       // Maximum recorded frequency
    float cpu_utilization;        // CPU usage percentage
    uint32_t memory_usage;        // Memory usage bytes
};
```

## Display System

### Display Configuration

#### DisplayConfig
```cpp
struct DisplayConfig {
    uint8_t i2c_address;      // I2C device address
    uint16_t update_rate_ms;  // Display update rate
    bool backlight_enabled;   // Backlight control
    uint8_t contrast;         // Display contrast
    bool large_font_enabled;  // Big font mode
    uint8_t decimal_places;   // Position decimal places
};
```

### Status Display Layouts

#### MainDisplayLayout
```cpp
enum class DisplayMode {
    STANDARD,     // Position, velocity, status
    DETAILED,     // Position, velocity, RPM, frequency
    LARGE_FONT,   // Large position display only
    DIAGNOSTIC    // System information and diagnostics
};
```

#### Display Update API
```cpp
class StatusDisplay {
public:
    void update_position(float position);
    void update_velocity(float velocity);
    void update_rpm(float rpm);
    void update_status(const char* status);
    void set_display_mode(DisplayMode mode);
    void show_error(const char* error_msg);
    void show_warning(const char* warning_msg);
};
```

## Calibration API

### Calibration State Machine

#### CalibrationState Enum
```cpp
enum class CalibrationState {
    INSTRUCTIONS,  // Show calibration instructions
    SETUP_POS1,   // Setup first position
    ADJUSTING_POS1, // Fine adjustment of position 1
    CONFIRM_POS1, // Enter measured position 1
    MOVE_PROMPT,  // Prompt to move to position 2
    SETUP_POS2,   // Setup second position
    ADJUSTING_POS2, // Fine adjustment of position 2
    CONFIRM_POS2, // Enter measured position 2
    CALCULATING,  // Perform pitch calculation
    RESULTS,      // Show calibration results
    APPLYING,     // Apply new settings
    COMPLETE      // Calibration finished
};
```

#### CalibrationData Structure
```cpp
struct CalibrationData {
    bool active;                    // Calibration in progress
    CalibrationState state;         // Current state
    
    // Position data
    int32_t encoder_pos1;          // First encoder position
    int32_t encoder_pos2;          // Second encoder position
    float measured_pos1_mm;        // First measured position
    float measured_pos2_mm;        // Second measured position
    
    // Calculation results
    float calculated_pitch;        // Calculated thread pitch
    float old_pitch;              // Previous pitch value
    float accuracy_estimate;       // Estimated accuracy
    
    // UI state
    uint8_t decimal_place;         // Current decimal place
    bool show_cursor;             // Cursor visibility
    float velocity_scale;         // Dynamic velocity scaling
    uint32_t last_update_time;    // Last update timestamp
    int32_t last_encoder_pos;     // Last encoder position
};
```

### Calibration Methods

#### Interactive Calibration
```cpp
class EngineeringMenu {
private:
    void start_calibration();
    void handle_calibration_state(MenuDirection direction);
    void calculate_pitch_from_positions();
    void apply_calibration_results();
    void reset_calibration();
    
    // Position editing
    float edit_position_value(float current_value, int32_t delta);
    void update_velocity_scaling();
    void draw_calibration_screen();
    void show_calibration_results();
};
```

## Data Structures

### System Configuration

#### SystemConfig Structure
```cpp
struct SystemConfig {
    // Version and validation
    uint32_t config_version;
    uint32_t checksum;
    
    // Encoder settings
    EncoderConfig main_encoder;
    EncoderConfig menu_encoder;
    
    // Display settings
    DisplayConfig display;
    
    // Performance settings
    uint32_t cpu_frequency_mhz;
    uint16_t update_rate_ms;
    bool adaptive_mode_enabled;
    
    // Calibration data
    float calibrated_pitch;
    uint32_t calibration_date;
    bool calibration_valid;
    
    // System settings
    char password[5];
    bool debug_mode_enabled;
    uint8_t log_level;
};
```

### Error Handling

#### ErrorCode Enum
```cpp
enum class ErrorCode {
    NO_ERROR = 0,
    ENCODER_FAULT = 1,
    I2C_ERROR = 2,
    CONFIG_CORRUPT = 3,
    CALIBRATION_FAILED = 4,
    HARDWARE_FAULT = 5,
    MEMORY_ERROR = 6,
    VALIDATION_FAILED = 7
};
```

#### Error Information
```cpp
struct ErrorInfo {
    ErrorCode code;
    const char* description;
    uint32_t timestamp;
    bool recoverable;
    uint16_t error_count;
};
```

## Constants and Enums

### Hardware Constants
```cpp
// Pin definitions
#define MAIN_ENCODER_PIN_A    0
#define MAIN_ENCODER_PIN_B    1
#define MENU_ENCODER_PIN_CLK  19
#define MENU_ENCODER_PIN_DT   20
#define MENU_ENCODER_PIN_SW   18
#define LCD_SDA_PIN          21
#define LCD_SCL_PIN          22

// I2C configuration
#define I2C_INSTANCE         i2c1
#define I2C_FREQUENCY        100000
#define LCD_I2C_ADDRESS      0x27

// Performance limits
#define MAX_ENCODER_FREQUENCY 50000  // Hz
#define MIN_UPDATE_RATE      50      // ms
#define MAX_UPDATE_RATE      1000    // ms
#define VELOCITY_SAMPLE_COUNT 8      // samples
```

### Menu System
```cpp
enum class MenuState {
    HIDDEN,
    PASSWORD_ENTRY,
    MAIN_MENU,
    ENCODER_SETTINGS,
    DISPLAY_SETTINGS,
    CALIBRATION,
    DIAGNOSTICS,
    PERFORMANCE,
    FACTORY_RESET
};

enum class MenuDirection {
    NONE,
    UP,
    DOWN,
    ENTER,
    BACK
};

enum class MainMenuItems {
    ENCODER_SETTINGS,
    DISPLAY_SETTINGS,
    CALIBRATION,
    DIAGNOSTICS,
    PERFORMANCE,
    FACTORY_RESET,
    SAVE_AND_EXIT,
    CANCEL_EXIT
};
```

### Version Information
```cpp
#define PEARL_VERSION_MAJOR  0
#define PEARL_VERSION_MINOR  7
#define PEARL_VERSION_PATCH  0
#define PEARL_VERSION_STRING "0.07"
#define BUILD_DATE          __DATE__
#define BUILD_TIME          __TIME__
```

## Usage Examples

### Basic Position Reading
```cpp
// Initialize encoder
QuadratureEncoder encoder(pio0, 0, 0, 1, 2.0f, 1000);
encoder.init();

// Read position
int32_t raw_pos = encoder.get_raw_position();
float distance = encoder.get_distance();
float velocity = encoder.get_velocity();
```

### Display Management
```cpp
// Initialize display
LCD_I2C lcd(i2c1, 0x27, 21, 22);
lcd.init();

// Update display
lcd.clear();
lcd.set_cursor(0, 0);
lcd.print("Position: ");
lcd.print(distance, 3);
```

### Configuration Handling
```cpp
// Load configuration
ConfigManager config_mgr;
SystemConfig config;
if (config_mgr.load_config(config)) {
    // Apply settings
    encoder.set_pitch(config.main_encoder.pitch);
    encoder.set_resolution(config.main_encoder.resolution);
}
```

### Error Handling
```cpp
// Check for errors
if (encoder.has_performance_warning()) {
    ErrorCode error = ErrorCode::ENCODER_FAULT;
    // Handle error condition
    display.show_error("Encoder Error");
}
```

---

**API Version**: v0.07
**Documentation Version**: 1.0
**Last Updated**: August 4, 2025

**Note**: This API reference covers the core functionality. For complete implementation details, refer to the source code and header files.
