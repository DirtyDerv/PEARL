#include "big_font.h"
#include <cstdio>
#include <cstring>

// 7-segment custom characters for 3x3 grid (each char is a segment or dot/minus)
const uint8_t BigFont::custom_chars[8][8] = {
    // SEG_A: Top horizontal (full row)
    {0b11111,0b11111,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000},
    // SEG_B: Upper right vertical (right col, top 3 rows)
    {0b00001,0b00001,0b00001,0b00000,0b00000,0b00000,0b00000,0b00000},
    // SEG_C: Lower right vertical (right col, bottom 3 rows)
    {0b00000,0b00000,0b00000,0b00001,0b00001,0b00001,0b00000,0b00000},
    // SEG_D: Bottom horizontal (full row at bottom)
    {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111},
    // SEG_E: Lower left vertical (left col, bottom 3 rows)
    {0b00000,0b00000,0b00000,0b10000,0b10000,0b10000,0b00000,0b00000},
    // SEG_F: Upper left vertical (left col, top 3 rows)
    {0b10000,0b10000,0b10000,0b00000,0b00000,0b00000,0b00000,0b00000},
    // SEG_G: Middle horizontal (full row in the middle)
    {0b00000,0b00000,0b11111,0b11111,0b00000,0b00000,0b00000,0b00000},
    // SEG_DP: Used for both minus and decimal
    // For minus: row 3 (middle), for decimal: 2x2 dot in bottom right
    {0b00000,0b00000,0b00000,0b01100,0b00000,0b00000,0b01100,0b01100}
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

// 7-segment digit segment map: {A,B,C,D,E,F,G}
const uint8_t segment_map[10][7] = {
    {1,1,1,1,1,1,0}, // 0
    {0,1,1,0,0,0,0}, // 1
    {1,1,0,1,1,0,1}, // 2
    {1,1,1,1,0,0,1}, // 3
    {0,1,1,0,0,1,1}, // 4
    {1,0,1,1,0,1,1}, // 5
    {1,0,1,1,1,1,1}, // 6
    {1,1,1,0,0,0,0}, // 7
    {1,1,1,1,1,1,1}, // 8
    {1,1,1,1,0,1,1}  // 9
};

void BigFont::print_big_digit(uint8_t digit, uint8_t col, uint8_t row) {
    // Each digit is 3x3 chars: (col,row) is top-left
    // Top row: SEG_A (all 3 cols)
    for (int i = 0; i < 3; ++i) {
        lcd->set_cursor(col+i, row);
        if (segment_map[digit][0]) lcd->write(SEG_A); else lcd->print(" ");
    }
    // Middle row: SEG_F (left), SEG_G (center), SEG_B (right)
    lcd->set_cursor(col, row+1);
    if (segment_map[digit][5]) lcd->write(SEG_F); else lcd->print(" ");
    lcd->set_cursor(col+1, row+1);
    if (segment_map[digit][6]) lcd->write(SEG_G); else lcd->print(" ");
    lcd->set_cursor(col+2, row+1);
    if (segment_map[digit][1]) lcd->write(SEG_B); else lcd->print(" ");
    // Bottom row: SEG_E (left), SEG_D (center), SEG_C (right)
    lcd->set_cursor(col, row+2);
    if (segment_map[digit][4]) lcd->write(SEG_E); else lcd->print(" ");
    lcd->set_cursor(col+1, row+2);
    if (segment_map[digit][3]) lcd->write(SEG_D); else lcd->print(" ");
    lcd->set_cursor(col+2, row+2);
    if (segment_map[digit][2]) lcd->write(SEG_C); else lcd->print(" ");
}

void BigFont::print_big_minus(uint8_t col, uint8_t row) {
    // Print a minus sign in the middle row (row+1) using SEG_DP
    lcd->set_cursor(col, row+1);
    lcd->write(SEG_DP);
}

void BigFont::print_big_decimal(uint8_t col, uint8_t row) {
    // Print a 2x2 pixel decimal point in the bottom row (row+2) using SEG_DP
    lcd->set_cursor(col, row+2);
    lcd->write(SEG_DP);
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
