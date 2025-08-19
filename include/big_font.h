#ifndef BIG_FONT_H
#define BIG_FONT_H

#include "lcd_i2c.h"

// 7-segment style big font (3x3 character display per digit)
// Each digit uses 7 segments (A-G) mapped to custom chars, minus and decimal are 1x1

class BigFont {
private:
    LCD_I2C* lcd;
    
    // Custom character patterns for big font components
    static const uint8_t custom_chars[8][8]; // 7 segments + decimal/minus
    
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
    static const uint8_t DIGIT_WIDTH = 3;   // Each digit is 3 characters wide
    static const uint8_t DIGIT_HEIGHT = 3;  // Each digit is 3 characters tall
    static const uint8_t MAX_DIGITS = 6;    // ####.# format (including decimal)
};


// 7-segment indices for 3x3 grid
#define SEG_A 0 // Top horizontal
#define SEG_B 1 // Upper right vertical
#define SEG_C 2 // Lower right vertical
#define SEG_D 3 // Bottom horizontal
#define SEG_E 4 // Lower left vertical
#define SEG_F 5 // Upper left vertical
#define SEG_G 6 // Middle horizontal
#define SEG_DP 7 // Decimal point/minus

#endif // BIG_FONT_H
