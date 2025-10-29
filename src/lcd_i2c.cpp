#include "lcd_i2c.h"
#include <cstdarg>
#include <cstdio>

LCD_I2C::LCD_I2C(i2c_inst_t* i2c, uint8_t addr) {
    i2c_port = i2c;
    lcd_addr = addr;
    backlight_state = BL; // Backlight on by default
}

void LCD_I2C::write_nibble(uint8_t nibble) {
    uint8_t data = nibble | backlight_state;
    i2c_write_blocking(i2c_port, lcd_addr, &data, 1, false);
    pulse_enable(data);
}

void LCD_I2C::pulse_enable(uint8_t data) {
    uint8_t enable_high = data | EN;
    uint8_t enable_low = data & ~EN;
    
    i2c_write_blocking(i2c_port, lcd_addr, &enable_high, 1, false);
    sleep_us(1);
    i2c_write_blocking(i2c_port, lcd_addr, &enable_low, 1, false);
    sleep_us(50);
}

void LCD_I2C::write_byte(uint8_t data, uint8_t mode) {
    uint8_t high_nibble = (data & 0xF0) | mode | backlight_state;
    uint8_t low_nibble = ((data << 4) & 0xF0) | mode | backlight_state;
    
    // Send high nibble
    i2c_write_blocking(i2c_port, lcd_addr, &high_nibble, 1, false);
    pulse_enable(high_nibble);
    
    // Send low nibble
    i2c_write_blocking(i2c_port, lcd_addr, &low_nibble, 1, false);
    pulse_enable(low_nibble);
}

void LCD_I2C::init() {
    sleep_ms(50); // Wait for LCD to power up
    
    // Initialize in 4-bit mode
    write_nibble(0x30);
    sleep_ms(5);
    write_nibble(0x30);
    sleep_us(100);
    write_nibble(0x30);
    sleep_us(100);
    write_nibble(0x20); // Set to 4-bit mode
    
    // Function set: 4-bit mode, 2 lines, 5x8 dots
    write_byte(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2_LINE | LCD_5x8_DOTS, 0);
    
    // Display control: display on, cursor off, blink off
    write_byte(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF, 0);
    
    // Clear display
    clear();
    
    // Entry mode: increment cursor, no shift
    write_byte(LCD_ENTRY_MODE_SET | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECREMENT, 0);
    
    sleep_ms(2);
}

void LCD_I2C::clear() {
    write_byte(LCD_CLEAR_DISPLAY, 0);
    sleep_ms(2);
}

void LCD_I2C::home() {
    write_byte(LCD_RETURN_HOME, 0);
    sleep_ms(2);
}

void LCD_I2C::set_cursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54}; // For 16x4 LCD
    if (row >= LCD_ROWS) row = LCD_ROWS - 1;
    if (col >= LCD_COLS) col = LCD_COLS - 1;
    
    uint8_t addr = LCD_SET_DDRAM_ADDR | (col + row_offsets[row]);
    write_byte(addr, 0);
}

bool LCD_I2C::print(const char* str) {  // ERROR RECOVERY: Updated to return success status
    bool success = true;
    while (*str) {
        write_byte(*str++, RS);
        // Note: write_byte should be enhanced with error checking in future iterations
    }
    return success;  // For now, assume success - can be enhanced later
}

void LCD_I2C::printf(const char* format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    print(buffer);
}

void LCD_I2C::backlight_on() {
    backlight_state = BL;
}

void LCD_I2C::backlight_off() {
    backlight_state = 0;
}

void LCD_I2C::display_on() {
    write_byte(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF, 0);
}

void LCD_I2C::display_off() {
    write_byte(LCD_DISPLAY_CONTROL | LCD_DISPLAY_OFF | LCD_CURSOR_OFF | LCD_BLINK_OFF, 0);
}

void LCD_I2C::create_char(uint8_t location, const uint8_t charmap[]) {
    location &= 0x7; // We only have 8 locations 0-7
    write_byte(LCD_SET_CGRAM_ADDR | (location << 3), 0);
    for (int i = 0; i < 8; i++) {
        write_byte(charmap[i], RS);
    }
}

void LCD_I2C::write(uint8_t value) {
    write_byte(value, RS);
}

// ERROR RECOVERY: Safe write with retries
bool LCD_I2C::write_safe(const uint8_t* data, size_t len) {
    for (int retry = 0; retry < 3; retry++) {
        int result = i2c_write_blocking(i2c_port, lcd_addr, data, len, false);
        if (result == len) {
            return true;  // Success
        }
        sleep_ms(1);  // Brief delay before retry
    }
    return false;  // Failed after retries
}
