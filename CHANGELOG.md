# Change Log

All notable changes to the PEARL (Precision Encoder-based Automatic Reading Logger) project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned Features
- Temperature compensation system
- RS485 communication interface
- Multi-channel encoder support
- Web-based configuration interface
- Data logging and export capabilities

## [0.07.0] - 2025-08-04

### Added - Interactive Calibration System
- **Complete two-point calibration workflow** with step-by-step user guidance
- **Dynamic velocity scaling** - encoder movement speed affects position adjustment speed
- **Precision position entry** with decimal place navigation for accurate measurements
- **Automatic pitch calculation** from measured positions with validation
- **100mm minimum distance** requirement for accurate calibration
- **Real-time distance tracking** during calibration process
- **Configuration persistence** with automatic saving and validation

### Enhanced - Engineering Menu System
- **12-state calibration state machine** for robust interactive workflow
- **Comprehensive user feedback** with clear instructions and status display
- **Error handling and validation** throughout calibration process
- **Professional UI/UX** with industrial-grade interface design
- **Position editing interface** with cursor navigation and real-time updates

### Improved - Industrial Features
- **Production-grade configuration management** with validation and backup
- **Enhanced error recovery** mechanisms for robust operation
- **Performance monitoring** with real-time diagnostics
- **Advanced encoder interface** with hardware acceleration options
- **Comprehensive documentation** suite for professional deployment

### Technical Improvements
- **Memory optimization** for efficient embedded operation
- **Real-time performance** enhancements for responsive user interface
- **Code structure** improvements for maintainability and extensibility
- **Build system** optimization with Ninja generator support

## [0.06.0] - 2025-07-15

### Added - Enhanced User Interface
- **Splash screen system** with professional startup display
- **System information** display with version and hardware details
- **Enhanced status display** with velocity and RPM monitoring
- **Improved menu navigation** with better feedback and responsiveness

### Enhanced - Encoder Interface
- **HW-040 encoder integration** for menu navigation and control
- **Advanced position tracking** with improved accuracy and stability
- **Velocity calculation enhancements** with moving average filtering
- **Performance monitoring** for encoder frequency and error detection

### Improved - Display System
- **20x4 LCD support** with full character set utilization
- **Real-time updates** with configurable refresh rates
- **Custom character support** for enhanced visual presentation
- **Backlight control** with software configuration

## [0.05.0] - 2025-06-20

### Added - Industrial Configuration System
- **Comprehensive configuration management** with persistent storage
- **Parameter validation** to prevent invalid settings
- **Factory reset capability** with multiple reset levels
- **Configuration backup and restore** functionality

### Enhanced - Engineering Menu
- **Hierarchical menu structure** for organized settings access
- **Password protection** for engineering functions
- **Real-time parameter editing** with immediate feedback
- **Input validation** with range checking and error prevention

### Improved - System Reliability
- **Enhanced error handling** throughout the system
- **Automatic recovery** from configuration errors
- **Data integrity protection** with checksum validation
- **Performance optimization** for embedded operation

## [0.04.0] - 2025-05-25

### Added - Advanced Engineering Features
- **Engineering menu system** with comprehensive configuration options
- **Real-time diagnostics** for system health monitoring
- **Performance tuning** capabilities for optimization
- **Advanced encoder settings** with resolution and pitch configuration

### Enhanced - Core Functionality
- **Improved position accuracy** with enhanced algorithms
- **Better velocity calculation** with temporal filtering
- **Enhanced display features** with multiple information modes
- **Robust error handling** for production environments

### Technical Improvements
- **Memory management** optimization for stable operation
- **Code organization** improvements for maintainability
- **Documentation** enhancements with detailed guides
- **Testing framework** for quality assurance

## [0.03.0] - 2025-04-10

### Added - Performance Monitoring
- **Encoder frequency monitoring** for performance analysis
- **Error counting and reporting** for diagnostics
- **Performance warning system** for proactive maintenance
- **Real-time performance metrics** display

### Enhanced - Encoder System
- **PIO-based hardware acceleration** for high-speed operation
- **Improved signal processing** for noise immunity
- **Enhanced resolution support** up to 10,000 PPR
- **Bidirectional tracking** with direction detection

### Improved - Reliability
- **FIFO overflow protection** for high-frequency operation
- **Invalid transition detection** for error prevention
- **Automatic error recovery** mechanisms
- **Performance counter management** for long-term operation

## [0.02.0] - 2025-03-15

### Added - Frequency Monitoring
- **Real-time frequency tracking** of encoder signals
- **Maximum frequency recording** for performance analysis
- **Transitions per second** calculation for diagnostics
- **Theoretical RPM calculation** based on encoder specifications

### Enhanced - Velocity System
- **8-sample moving average** for stable velocity readings
- **Temporal filtering** for smooth velocity display
- **Speed percentage calculation** relative to maximum speed
- **Enhanced RPM calculation** with frequency-based validation

### Improved - Display Features
- **Multi-line status display** with comprehensive information
- **Real-time velocity updates** with configurable precision
- **RPM display** for rotational speed monitoring
- **Status indicators** for system operation feedback

## [0.01.0] - 2025-02-01

### Added - Initial Release
- **Basic quadrature encoder interface** for position measurement
- **LCD display support** for 20x4 character displays
- **Simple position tracking** with configurable resolution
- **Basic configuration system** for encoder parameters

### Core Features
- **Position measurement** in engineering units (mm)
- **Real-time display** of position and status
- **Encoder resolution configuration** from 100 to 10,000 PPR
- **Thread pitch configuration** for linear measurement conversion

### Hardware Support
- **Raspberry Pi Pico** microcontroller platform
- **I2C LCD displays** with standard HD44780 interface
- **Quadrature encoders** with differential signal support
- **USB power and communication** for development and operation

## Development Milestones

### Hardware Validation (2025-01-15)
- Raspberry Pi Pico hardware platform selected
- I2C LCD interface implementation completed
- Quadrature encoder signal processing validated
- Basic system architecture established

### Software Foundation (2025-01-20)
- CMake build system configured
- Core class structure designed
- Basic encoder and display drivers implemented
- Initial testing framework established

### Feature Development (2025-02-01 to 2025-08-04)
- Iterative feature addition with user feedback
- Performance optimization and reliability improvements
- Comprehensive testing on actual hardware
- Documentation development and refinement

## Technical Evolution

### Architecture Improvements
- **v0.01-0.02**: Basic functionality and monitoring
- **v0.03-0.04**: Performance optimization and advanced features
- **v0.05-0.06**: Industrial-grade reliability and user interface
- **v0.07**: Complete calibration system and production readiness

### Code Quality Metrics
- **Lines of Code**: ~3,500 (v0.07)
- **Test Coverage**: Hardware validation required
- **Documentation**: Comprehensive user and developer guides
- **Performance**: 1kHz+ position update rate capability

### Hardware Compatibility
- **Raspberry Pi Pico**: Full support with all features
- **Compatible RP2040 boards**: Tested with various manufacturers
- **Encoder compatibility**: 100-10,000 PPR quadrature encoders
- **Display compatibility**: Standard I2C LCD modules (20x4, 16x2)

## Migration Notes

### Upgrading from v0.06 to v0.07
- **Configuration compatibility**: Existing settings preserved
- **New calibration system**: Accessible via engineering menu
- **Enhanced features**: All previous functionality retained
- **Documentation**: Updated user manual with calibration procedures

### Upgrading from Earlier Versions
- **v0.05 and later**: Direct upgrade path with configuration migration
- **v0.04 and earlier**: Factory reset recommended for optimal operation
- **Hardware changes**: No hardware modifications required
- **Configuration**: May need to reconfigure encoder parameters

## Known Issues and Limitations

### Current Limitations
- **Single encoder support**: Multi-channel planned for future release
- **Temperature compensation**: Manual calibration required for extreme temperatures
- **Communication interfaces**: USB only, RS485 planned for v0.08
- **Data logging**: Basic logging only, enhanced features planned

### Resolved Issues
- **v0.06**: Display flickering under high update rates (fixed in v0.07)
- **v0.05**: Configuration corruption on power loss (robust saving in v0.06+)
- **v0.04**: Menu navigation inconsistency (redesigned interface in v0.05+)
- **v0.03**: Encoder overflow at high speeds (PIO acceleration in v0.04+)

## Future Roadmap

### Version 0.08 (Planned Q4 2025)
- **Multi-channel encoder support** for synchronized measurements
- **RS485 communication interface** for networked installations
- **Enhanced data logging** with timestamp and export capabilities
- **Temperature compensation** with automatic sensor integration

### Version 0.09 (Planned Q1 2026)
- **Web-based configuration** interface for remote setup
- **Wireless communication** options (WiFi, Bluetooth)
- **Advanced analytics** with trend analysis and reporting
- **Industrial protocol support** (Modbus, CANbus)

### Version 1.0 (Planned Q2 2026)
- **Production release** with full feature set
- **Certified calibration** capabilities for metrology applications
- **Complete documentation** suite for industrial deployment
- **Long-term support** commitment for production users

---

**Versioning Policy**: PEARL follows semantic versioning with focus on backward compatibility and reliable upgrade paths for production deployments.

**Release Schedule**: Regular releases every 6-8 weeks with continuous integration and hardware validation.
