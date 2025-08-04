#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// LCD I2C address (typically 0x27 or 0x3F for PCF8574 I2C backpack)
#define LCD_ADDR 0x27

// LCD dimensions
#define LCD_COLS 16
#define LCD_ROWS 4

// LCD commands
#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_ENTRY_MODE_SET 0x04
#define LCD_DISPLAY_CONTROL 0x08
#define LCD_CURSOR_SHIFT 0x10
#define LCD_FUNCTION_SET 0x20
#define LCD_SET_CGRAM_ADDR 0x40
#define LCD_SET_DDRAM_ADDR 0x80

// Entry mode flags
#define LCD_ENTRY_RIGHT 0x00
#define LCD_ENTRY_LEFT 0x02
#define LCD_ENTRY_SHIFT_INCREMENT 0x01
#define LCD_ENTRY_SHIFT_DECREMENT 0x00

// Display control flags
#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_CURSOR_ON 0x02
#define LCD_CURSOR_OFF 0x00
#define LCD_BLINK_ON 0x01
#define LCD_BLINK_OFF 0x00

// Function set flags
#define LCD_8BIT_MODE 0x10
#define LCD_4BIT_MODE 0x00
#define LCD_2_LINE 0x08
#define LCD_1_LINE 0x00
#define LCD_5x10_DOTS 0x04
#define LCD_5x8_DOTS 0x00

// PCF8574 bit mapping
#define RS 0x01    // Register select bit
#define EN 0x04    // Enable bit
#define BL 0x08    // Backlight bit
#define D4 0x10    // Data bit 4
#define D5 0x20    // Data bit 5
#define D6 0x40    // Data bit 6
#define D7 0x80    // Data bit 7

class LCD_I2C {
private:
    i2c_inst_t* i2c_port;
    uint8_t lcd_addr;
    uint8_t backlight_state;
    
    void write_nibble(uint8_t nibble);
    void write_byte(uint8_t data, uint8_t mode);
    void pulse_enable(uint8_t data);
    
public:
    LCD_I2C(i2c_inst_t* i2c, uint8_t addr = LCD_ADDR);
    
    void init();
    void clear();
    void home();
    void set_cursor(uint8_t col, uint8_t row);
    void print(const char* str);
    void printf(const char* format, ...);
    void backlight_on();
    void backlight_off();
    void display_on();
    void display_off();
    void create_char(uint8_t location, const uint8_t charmap[]);
    void write(uint8_t value);
};

#endif // LCD_I2C_H
