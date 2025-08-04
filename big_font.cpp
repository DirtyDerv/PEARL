#include "big_font.h"
#include <cstdio>
#include <cstring>

// Custom character patterns for creating big digits
const uint8_t BigFont::custom_chars[8][8] = {
    // CHAR_TOP_LEFT (0) - Top-left corner and vertical line
    {
        0b11111,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000
    },
    // CHAR_TOP_RIGHT (1) - Top-right corner and vertical line
    {
        0b11111,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001
    },
    // CHAR_MID_LEFT (2) - Middle-left for numbers like 8, B, etc.
    {
        0b10000,
        0b10000,
        0b10000,
        0b11111,
        0b10000,
        0b10000,
        0b10000,
        0b10000
    },
    // CHAR_MID_RIGHT (3) - Middle-right for numbers like 8, B, etc.
    {
        0b00001,
        0b00001,
        0b00001,
        0b11111,
        0b00001,
        0b00001,
        0b00001,
        0b00001
    },
    // CHAR_BOT_LEFT (4) - Bottom-left corner
    {
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b11111
    },
    // CHAR_BOT_RIGHT (5) - Bottom-right corner
    {
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b00001,
        0b11111
    },
    // CHAR_FULL_BLOCK (6) - Solid block
    {
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111
    },
    // CHAR_BOTTOM_BAR (7) - Bottom horizontal line only
    {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111
    }
};

BigFont::BigFont(LCD_I2C* lcd_instance) {
    lcd = lcd_instance;
}

void BigFont::init() {
    create_custom_characters();
}

void BigFont::create_custom_characters() {
    // Load all custom characters into LCD memory
    for (int i = 0; i < 8; i++) {
        lcd->create_char(i, custom_chars[i]);
    }
}

void BigFont::display_big_number(float number, uint8_t start_col, uint8_t start_row) {
    // Clear the display area first
    clear_display_area();
    
    // Format number to ####.# format
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "%6.1f", number);
    
    uint8_t col = start_col;
    uint8_t row = start_row;
    
    // Process each character in the formatted string
    for (int i = 0; i < 6; i++) {
        char c = buffer[i];
        
        if (c == ' ') {
            // Skip spaces but advance position
            col += DIGIT_WIDTH;
        } else if (c == '-') {
            print_big_minus(col, row);
            col += DIGIT_WIDTH;
        } else if (c == '.') {
            print_big_decimal(col, row);
            col += 1; // Decimal point is only 1 character wide
        } else if (c >= '0' && c <= '9') {
            print_big_digit(c - '0', col, row);
            col += DIGIT_WIDTH;
        }
        
        // Check if we're running out of space
        if (col >= 16) break;
    }
}

void BigFont::print_big_digit(uint8_t digit, uint8_t col, uint8_t row) {
    // Define digit patterns using custom characters
    // Each digit is 2 characters wide, 3 characters tall
    
    switch (digit) {
        case 0:
            lcd->set_cursor(col, row);     lcd->write(CHAR_TOP_LEFT);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_TOP_RIGHT);
            lcd->set_cursor(col, row+1);   lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col, row+2);   lcd->write(CHAR_BOT_LEFT);
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_BOT_RIGHT);
            break;
            
        case 1:
            lcd->set_cursor(col, row);     lcd->print(" ");
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col, row+1);   lcd->print(" ");
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col, row+2);   lcd->print(" ");
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_FULL_BLOCK);
            break;
            
        case 2:
            lcd->set_cursor(col, row);     lcd->write(CHAR_TOP_LEFT);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_TOP_RIGHT);
            lcd->set_cursor(col, row+1);   lcd->write(CHAR_BOTTOM_BAR);
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_MID_LEFT);
            lcd->set_cursor(col, row+2);   lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_BOTTOM_BAR);
            break;
            
        case 3:
            lcd->set_cursor(col, row);     lcd->write(CHAR_TOP_LEFT);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_TOP_RIGHT);
            lcd->set_cursor(col, row+1);   lcd->write(CHAR_BOTTOM_BAR);
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_MID_RIGHT);
            lcd->set_cursor(col, row+2);   lcd->write(CHAR_BOTTOM_BAR);
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_BOT_RIGHT);
            break;
            
        case 4:
            lcd->set_cursor(col, row);     lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col, row+1);   lcd->write(CHAR_BOT_LEFT);
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_MID_RIGHT);
            lcd->set_cursor(col, row+2);   lcd->print(" ");
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_FULL_BLOCK);
            break;
            
        case 5:
            lcd->set_cursor(col, row);     lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_BOTTOM_BAR);
            lcd->set_cursor(col, row+1);   lcd->write(CHAR_MID_LEFT);
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_BOTTOM_BAR);
            lcd->set_cursor(col, row+2);   lcd->write(CHAR_BOTTOM_BAR);
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_BOT_RIGHT);
            break;
            
        case 6:
            lcd->set_cursor(col, row);     lcd->write(CHAR_TOP_LEFT);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_BOTTOM_BAR);
            lcd->set_cursor(col, row+1);   lcd->write(CHAR_MID_LEFT);
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_TOP_RIGHT);
            lcd->set_cursor(col, row+2);   lcd->write(CHAR_BOT_LEFT);
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_BOT_RIGHT);
            break;
            
        case 7:
            lcd->set_cursor(col, row);     lcd->write(CHAR_TOP_LEFT);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_TOP_RIGHT);
            lcd->set_cursor(col, row+1);   lcd->print(" ");
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_FULL_BLOCK);
            lcd->set_cursor(col, row+2);   lcd->print(" ");
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_FULL_BLOCK);
            break;
            
        case 8:
            lcd->set_cursor(col, row);     lcd->write(CHAR_TOP_LEFT);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_TOP_RIGHT);
            lcd->set_cursor(col, row+1);   lcd->write(CHAR_MID_LEFT);
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_MID_RIGHT);
            lcd->set_cursor(col, row+2);   lcd->write(CHAR_BOT_LEFT);
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_BOT_RIGHT);
            break;
            
        case 9:
            lcd->set_cursor(col, row);     lcd->write(CHAR_TOP_LEFT);
            lcd->set_cursor(col+1, row);   lcd->write(CHAR_TOP_RIGHT);
            lcd->set_cursor(col, row+1);   lcd->write(CHAR_BOT_LEFT);
            lcd->set_cursor(col+1, row+1); lcd->write(CHAR_MID_RIGHT);
            lcd->set_cursor(col, row+2);   lcd->write(CHAR_BOTTOM_BAR);
            lcd->set_cursor(col+1, row+2); lcd->write(CHAR_BOT_RIGHT);
            break;
    }
}

void BigFont::print_big_minus(uint8_t col, uint8_t row) {
    // Print a big minus sign
    lcd->set_cursor(col, row);     lcd->print(" ");
    lcd->set_cursor(col+1, row);   lcd->print(" ");
    lcd->set_cursor(col, row+1);   lcd->write(CHAR_BOTTOM_BAR);
    lcd->set_cursor(col+1, row+1); lcd->write(CHAR_BOTTOM_BAR);
    lcd->set_cursor(col, row+2);   lcd->print(" ");
    lcd->set_cursor(col+1, row+2); lcd->print(" ");
}

void BigFont::print_big_decimal(uint8_t col, uint8_t row) {
    // Print a decimal point
    lcd->set_cursor(col, row);     lcd->print(" ");
    lcd->set_cursor(col, row+1);   lcd->print(" ");
    lcd->set_cursor(col, row+2);   lcd->write(CHAR_FULL_BLOCK);
}

void BigFont::clear_display_area() {
    // Clear the top 3 rows (where big numbers are displayed)
    for (int row = 0; row < 3; row++) {
        lcd->set_cursor(0, row);
        lcd->print("                "); // 16 spaces
    }
}

void BigFont::clear_big_digit_area(uint8_t col, uint8_t row) {
    // Clear a 2x3 area for a digit
    for (int r = 0; r < 3; r++) {
        lcd->set_cursor(col, row + r);
        lcd->print("  "); // 2 spaces
    }
}
