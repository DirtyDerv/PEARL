#ifndef VERSION_H
#define VERSION_H

// Project Version System
// Starting at 0.00, increment by 0.01 for each iteration
// Major releases will use whole numbers (1.00, 2.00, etc.)

#define PROJECT_VERSION_MAJOR 0
#define PROJECT_VERSION_MINOR 2
#define PROJECT_VERSION_STRING "0.02"

// Version History (for reference)
// 0.02 - 2025-08-04 - Development branch setup + Enhanced PIO optimization
// 0.01 - 2025-08-04 - Test version increment system functionality
// 0.00 - Initial version with new numbering system
//        - Big font LCD display system
//        - Quadrature encoder with PIO support
//        - Direction and speed indicators
//        - Custom character generation

// Build information
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

// Version display helper
inline const char* get_version_string() {
    return PROJECT_VERSION_STRING;
}

// Version comparison helpers
inline bool is_version_at_least(int major, int minor) {
    return (PROJECT_VERSION_MAJOR > major) || 
           (PROJECT_VERSION_MAJOR == major && PROJECT_VERSION_MINOR >= minor);
}

#endif // VERSION_H
