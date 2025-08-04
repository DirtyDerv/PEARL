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

project(Bg_Reader C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Bg_Reader 
    Bg_Reader.cpp 
    lcd_i2c.cpp
    quadrature_encoder.cpp
    indexed_encoder.cpp
    big_font.cpp
    status_display.cpp
)

pico_set_program_name(Bg_Reader "Bg_Reader")
pico_set_program_version(Bg_Reader "0.01")

# Generate PIO headers
pico_generate_pio_header(Bg_Reader ${CMAKE_CURRENT_LIST_DIR}/blink.pio)
pico_generate_pio_header(Bg_Reader ${CMAKE_CURRENT_LIST_DIR}/quadrature.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Bg_Reader 0)
pico_enable_stdio_usb(Bg_Reader 1)

# Add the standard library to the build
target_link_libraries(Bg_Reader
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Bg_Reader PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(Bg_Reader 
        hardware_i2c
        hardware_pio
        hardware_gpio
        )

pico_add_extra_outputs(Bg_Reader)

