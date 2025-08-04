#ifndef BIG_FONT_H
#define BIG_FONT_H

#include "lcd_i2c.h"

// Custom character definitions for big font (2x3 character display per digit)
// Each digit uses 6 custom characters arranged in 2 columns x 3 rows

class BigFont {
private:
    LCD_I2C* lcd;
    
    // Custom character patterns for big font components
    static const uint8_t custom_chars[8][8];
    
    void create_custom_characters();
    void print_big_digit(uint8_t digit, uint8_t col, uint8_t row);
    void print_big_minus(uint8_t col, uint8_t row);
    void print_big_decimal(uint8_t col, uint8_t row);
    void clear_big_digit_area(uint8_t col, uint8_t row);
    
public:
    BigFont(LCD_I2C* lcd_instance);
    
    void init();
    void display_big_number(float number, uint8_t start_col = 0, uint8_t start_row = 0);
    void clear_display_area();
    
    // Constants for layout
    static const uint8_t DIGIT_WIDTH = 2;   // Each digit is 2 characters wide
    static const uint8_t DIGIT_HEIGHT = 3;  // Each digit is 3 characters tall
    static const uint8_t MAX_DIGITS = 6;    // ####.# format (including decimal)
};

// Custom character set indices
#define CHAR_TOP_LEFT     0
#define CHAR_TOP_RIGHT    1
#define CHAR_MID_LEFT     2
#define CHAR_MID_RIGHT    3
#define CHAR_BOT_LEFT     4
#define CHAR_BOT_RIGHT    5
#define CHAR_FULL_BLOCK   6
#define CHAR_BOTTOM_BAR   7

#endif // BIG_FONT_H
