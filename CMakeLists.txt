# (Moved lcd_test target setup to after Pico SDK/project setup)
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(PEARL C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(PEARL 
    src/Bg_Reader.cpp 
    src/lcd_i2c.cpp
    src/quadrature_encoder.cpp
    src/indexed_encoder.cpp
    src/big_font.cpp
    src/status_display.cpp
    src/hw040_encoder.cpp
    src/engineering_menu.cpp
    src/splash_screen.cpp
    src/adaptive_config.cpp
    src/config_manager.cpp
)

pico_set_program_name(PEARL "PEARL")
pico_set_program_version(PEARL "0.07")

# Generate PIO headers
pico_generate_pio_header(PEARL ${CMAKE_CURRENT_LIST_DIR}/pio/blink.pio)

# Generate quadrature.pio.h for all targets
pico_generate_pio_header(PEARL ${CMAKE_CURRENT_LIST_DIR}/pio/quadrature.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(PEARL 0)
pico_enable_stdio_usb(PEARL 1)

# Add the standard library to the build
target_link_libraries(PEARL
    pico_stdlib
    pico_multicore)

# Add the standard include files to the build
target_include_directories(PEARL PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
)

# Minimal LCD test target (after Pico SDK/project setup)
add_executable(lcd_test src/lcd_test.cpp src/lcd_i2c.cpp src/big_font.cpp)
target_link_libraries(lcd_test pico_stdlib hardware_i2c)
target_include_directories(lcd_test PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/include)
pico_add_extra_outputs(lcd_test)

# Add any user requested libraries
target_link_libraries(PEARL 
    hardware_i2c
    hardware_pio
    hardware_gpio
    hardware_timer
    hardware_flash
    )

pico_add_extra_outputs(PEARL)
