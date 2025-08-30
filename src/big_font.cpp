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

// New big font data
const uint8_t BigFont::big_font_shape_table[8][8] = {
    {0b01111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111}, // LT
    {0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // UB
    {0b11110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111}, // RT
    {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b01111}, // LL
    {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111}, // LB
    {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11110}, // LR
    {0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111}, // UMB
    {0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111}  // LMB
};

const uint8_t BigFont::big_font_ascii_table[] = {
    // Characters from ASCII 32 to 57 (space to '9')
    // Each character is 6 bytes (3 for top row, 3 for bottom row)
    // 255 is a full block, 32 is a space
    32, 32, 32, 32, 32, 32, // 32: space
    32, 32, 32, 32, 32, 32, // 33: !
    32, 32, 32, 32, 32, 32, // 34: "
    32, 32, 32, 32, 32, 32, // 35: #
    32, 32, 32, 32, 32, 32, // 36: $
    32, 32, 32, 32, 32, 32, // 37: %
    32, 32, 32, 32, 32, 32, // 38: &
    32, 32, 32, 32, 32, 32, // 39: '
    32, 32, 32, 32, 32, 32, // 40: (
    32, 32, 32, 32, 32, 32, // 41: )
    32, 32, 32, 32, 32, 32, // 42: *
    32, 32, 32, 32, 32, 32, // 43: +
    32, 32, 32, 32, 32, 32, // 44: ,
    32, 32, 32, 32, 32, 32, // 45: -
    32, 32, 32, 32, 4, 32,  // 46: .
    32, 32, 32, 32, 32, 32, // 47: /
    0, 1, 2, 3, 4, 5,       // 48: 0
    1, 2, 32, 32, 5, 32,    // 49: 1
    6, 6, 2, 3, 7, 7,       // 50: 2
    6, 6, 2, 7, 7, 5,       // 51: 3
    3, 4, 2, 32, 32, 5,     // 52: 4
    0, 6, 6, 7, 7, 5,       // 53: 5
    0, 6, 6, 3, 7, 5,       // 54: 6
    1, 1, 2, 32, 0, 32,     // 55: 7
    0, 6, 2, 3, 7, 5,       // 56: 8
    0, 6, 2, 32, 32, 5      // 57: 9
};

void BigFont::init_matrix_font() {
    create_matrix_characters();
}

void BigFont::create_matrix_characters() {
    for (int i = 0; i < 8; i++) {
        lcd->create_char(i, big_font_shape_table[i]);
    }
}

void BigFont::display_big_char(char ch, uint8_t col, uint8_t row) {
    if (ch < 32 || ch > 57) {
        return; // Character not in table
    }

    int offset = (ch - 32) * 6;

    lcd->set_cursor(col, row);
    for (int i = 0; i < 3; i++) {
        uint8_t shape_index = big_font_ascii_table[offset + i];
        lcd->write(shape_index);
    }

    lcd->set_cursor(col, row + 1);
    for (int i = 0; i < 3; i++) {
        uint8_t shape_index = big_font_ascii_table[offset + 3 + i];
        lcd->write(shape_index);
    }
}
