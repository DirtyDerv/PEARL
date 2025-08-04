# Contributing to PEARL

## Welcome Contributors!

We welcome contributions to the PEARL (Precision Encoder-based Automatic Reading Logger) project! This document provides guidelines for contributing to the codebase, documentation, and overall project.

## Table of Contents
1. [Getting Started](#getting-started)
2. [Development Environment](#development-environment)
3. [Coding Standards](#coding-standards)
4. [Contribution Process](#contribution-process)
5. [Testing Guidelines](#testing-guidelines)
6. [Documentation](#documentation)
7. [Community Guidelines](#community-guidelines)

## Getting Started

### Prerequisites
- **Development Board**: Raspberry Pi Pico or compatible RP2040 board
- **Development Environment**: VS Code with Raspberry Pi Pico extension
- **Hardware**: LCD display, encoders, and test setup as per installation guide
- **Git Knowledge**: Basic understanding of Git and GitHub workflows

### Repository Structure
```
PICO_BG_READER/
├── src/                 # Source code
├── include/             # Header files
├── docs/                # Documentation
├── pio/                 # PIO assembly files
├── tests/               # Unit tests
├── examples/            # Example code
├── tools/               # Development tools
└── CMakeLists.txt       # Build configuration
```

## Development Environment

### Setup Instructions
1. **Clone Repository**:
   ```bash
   git clone https://github.com/DirtyDerv/PICO_BG_READER.git
   cd PICO_BG_READER
   git checkout development
   ```

2. **Install Dependencies**:
   - VS Code with Raspberry Pi Pico extension
   - CMake and Ninja build system
   - ARM GCC toolchain (installed with Pico extension)

3. **Build Project**:
   ```bash
   mkdir build
   cd build
   cmake -G "Ninja" ..
   ninja
   ```

### Development Workflow
1. **Create Feature Branch**: `git checkout -b feature/your-feature-name`
2. **Make Changes**: Implement your feature or fix
3. **Test Thoroughly**: Verify functionality on hardware
4. **Commit Changes**: Use descriptive commit messages
5. **Submit Pull Request**: Target the `development` branch

## Coding Standards

### C++ Style Guidelines

#### General Principles
- **Clarity over Cleverness**: Code should be readable and maintainable
- **Consistent Style**: Follow existing code patterns
- **Industrial Robustness**: Error handling and validation throughout
- **Performance Awareness**: Consider real-time constraints

#### Naming Conventions
```cpp
// Classes: PascalCase
class QuadratureEncoder {

// Functions/Methods: snake_case
void update_position();

// Variables: snake_case
int32_t encoder_position;

// Constants: UPPER_SNAKE_CASE
#define MAX_ENCODER_FREQUENCY 50000

// Private members: trailing underscore
private:
    float velocity_;
    bool is_initialized_;
};
```

#### File Organization
```cpp
// Header file structure
#ifndef CLASS_NAME_H
#define CLASS_NAME_H

#include "system_headers.h"
#include "project_headers.h"

// Constants and enums
#define CONSTANT_VALUE 123

enum class EnumName {
    VALUE_ONE,
    VALUE_TWO
};

// Class declaration
class ClassName {
private:
    // Private members first
    
protected:
    // Protected members
    
public:
    // Public interface
    // Constructor/destructor
    // Core methods
    // Getters/setters
};

#endif // CLASS_NAME_H
```

#### Error Handling
```cpp
// Always check return values
bool result = initialize_hardware();
if (!result) {
    log_error("Hardware initialization failed");
    return false;
}

// Use exceptions sparingly (embedded constraints)
// Prefer error codes and validation

// Validate parameters
bool set_encoder_resolution(uint32_t resolution) {
    if (resolution < MIN_RESOLUTION || resolution > MAX_RESOLUTION) {
        return false;
    }
    // Implementation
    return true;
}
```

### Documentation Standards

#### Code Comments
```cpp
/**
 * @brief Brief description of the function
 * @param param1 Description of parameter 1
 * @param param2 Description of parameter 2
 * @return Description of return value
 * @note Any important notes or limitations
 */
bool function_name(int param1, float param2);

// Inline comments for complex logic
void complex_function() {
    // Step 1: Initialize encoder state machine
    init_state_machine();
    
    // Step 2: Configure PIO for quadrature decoding
    configure_pio_quadrature();
}
```

#### Header Documentation
```cpp
/**
 * @file quadrature_encoder.h
 * @brief High-precision quadrature encoder interface
 * @version 0.07
 * @date 2025-08-04
 * 
 * Provides hardware-accelerated quadrature decoding with
 * velocity calculation and performance monitoring.
 * 
 * Features:
 * - PIO-based hardware decoding
 * - Real-time velocity calculation
 * - Performance monitoring and diagnostics
 * - Industrial-grade error handling
 */
```

## Contribution Process

### Types of Contributions

#### Bug Fixes
1. **Create Issue**: Document the bug with steps to reproduce
2. **Fix Implementation**: Minimal changes to resolve issue
3. **Test Verification**: Confirm fix works on hardware
4. **Regression Testing**: Ensure no new issues introduced

#### New Features
1. **Feature Request**: Discuss proposed feature in issues
2. **Design Review**: Get feedback on implementation approach
3. **Implementation**: Follow coding standards and test thoroughly
4. **Documentation**: Update relevant documentation

#### Documentation Improvements
1. **Identify Need**: Areas needing better documentation
2. **Research**: Ensure accuracy and completeness
3. **Write/Update**: Clear, comprehensive documentation
4. **Review**: Get feedback from other contributors

### Pull Request Guidelines

#### Before Submitting
- [ ] Code follows project style guidelines
- [ ] All tests pass on hardware
- [ ] Documentation updated as needed
- [ ] Commit messages are descriptive
- [ ] No merge conflicts with development branch

#### Pull Request Template
```markdown
## Description
Brief description of changes made.

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Refactoring

## Testing
- [ ] Tested on hardware
- [ ] All existing functionality works
- [ ] New tests added if applicable

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No breaking changes (or documented)
```

#### Review Process
1. **Automated Checks**: Code style and basic validation
2. **Peer Review**: Code review by maintainers
3. **Testing**: Hardware testing if needed
4. **Integration**: Merge into development branch

## Testing Guidelines

### Hardware Testing
Since PEARL is an embedded system, hardware testing is crucial:

#### Basic Functionality Tests
```cpp
// Example test procedure
void test_encoder_basic_functionality() {
    // Initialize encoder
    QuadratureEncoder encoder(pio0, 0, 0, 1);
    assert(encoder.init() == true);
    
    // Test position reading
    int32_t initial_pos = encoder.get_raw_position();
    
    // Manual rotation test (requires operator)
    printf("Rotate encoder 10 clicks clockwise\n");
    wait_for_input();
    
    int32_t final_pos = encoder.get_raw_position();
    assert(final_pos > initial_pos);
    
    printf("Basic encoder test: PASSED\n");
}
```

#### Performance Testing
```cpp
void test_encoder_performance() {
    QuadratureEncoder encoder(pio0, 0, 0, 1);
    encoder.init();
    
    // Test high-frequency operation
    uint32_t start_time = time_us_32();
    for (int i = 0; i < 10000; i++) {
        encoder.update();
    }
    uint32_t end_time = time_us_32();
    
    float update_rate = 10000.0f / ((end_time - start_time) / 1000000.0f);
    printf("Update rate: %.1f Hz\n", update_rate);
    
    assert(update_rate > 1000.0f); // Should exceed 1kHz
}
```

#### Integration Testing
- Test complete system functionality
- Verify menu system operation
- Test calibration procedure
- Check configuration persistence

### Software Testing
For testable components:

#### Unit Tests
```cpp
// Test configuration validation
void test_config_validation() {
    SystemConfig config = {};
    
    // Test valid configuration
    config.main_encoder.resolution = 1000;
    config.main_encoder.pitch = 2.0f;
    assert(validate_encoder_config(&config.main_encoder) == true);
    
    // Test invalid configuration
    config.main_encoder.resolution = 0;
    assert(validate_encoder_config(&config.main_encoder) == false);
}
```

### Test Documentation
Document test procedures and expected results:
```markdown
## Test: Basic Encoder Functionality
**Objective**: Verify encoder responds to rotation
**Procedure**:
1. Flash test firmware
2. Connect encoder to pins 0,1
3. Rotate encoder 10 clicks clockwise
4. Verify position increases by 10

**Expected Result**: Position reading increases
**Pass Criteria**: Final position > initial position
```

## Documentation

### Documentation Types

#### User Documentation
- **Installation Guide**: Hardware setup and software installation
- **User Manual**: Complete operating instructions
- **Troubleshooting**: Common issues and solutions

#### Developer Documentation
- **API Reference**: Complete class and method documentation
- **Architecture Guide**: System design and component interaction
- **Contribution Guide**: This document

#### Technical Documentation
- **Hardware Specifications**: Pin assignments, electrical requirements
- **Calibration Procedures**: Detailed calibration methodology
- **Performance Benchmarks**: System capabilities and limitations

### Documentation Standards
- **Markdown Format**: Use standard Markdown for all documentation
- **Clear Structure**: Use headers, lists, and tables effectively
- **Code Examples**: Include practical examples where applicable
- **Version Control**: Keep documentation in sync with code
- **Review Process**: Documentation changes go through same review as code

## Community Guidelines

### Communication
- **Be Respectful**: Professional and courteous communication
- **Be Constructive**: Focus on improving the project
- **Be Patient**: Allow time for review and feedback
- **Be Helpful**: Share knowledge and assist other contributors

### Issue Reporting
When reporting issues:
1. **Search Existing Issues**: Avoid duplicates
2. **Use Templates**: Follow issue templates when available
3. **Provide Details**: Include version, hardware, steps to reproduce
4. **Be Responsive**: Reply to requests for additional information

### Code of Conduct
- Respect all contributors regardless of experience level
- Focus on technical merit of contributions
- Provide constructive feedback
- Help create a welcoming environment for new contributors

## Release Process

### Version Numbering
- **Major.Minor.Patch** format (e.g., v0.07.1)
- **Major**: Breaking changes, major new features
- **Minor**: New features, significant improvements
- **Patch**: Bug fixes, minor improvements

### Development Branches
- **main**: Stable release branch
- **development**: Active development branch
- **feature/***: Individual feature development
- **hotfix/***: Critical bug fixes

### Release Checklist
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Version numbers updated
- [ ] Release notes prepared
- [ ] Hardware testing completed
- [ ] Performance benchmarks verified

## Getting Help

### Resources
- **Documentation**: Start with user manual and API reference
- **Issues**: Search existing issues for similar problems
- **Discussions**: Use GitHub discussions for general questions
- **Code Review**: Learn from existing pull requests

### Contact
- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For general questions and ideas
- **Pull Request Comments**: For specific code questions

---

**Thank you for contributing to PEARL!** Your efforts help make precision measurement accessible to more users and applications.

**Project Maintainers**: See MAINTAINERS.md for current project maintainers and contact information.
