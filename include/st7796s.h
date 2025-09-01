#ifndef ST7796S_H
#define ST7796S_H

#include "pico/stdlib.h"
#include "hardware/spi.h"

class ST7796S {
public:
    ST7796S(spi_inst_t* spi, uint8_t cs, uint8_t dc, uint8_t rst, uint8_t bl);
    void init();
    void clear(uint16_t color);
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void drawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg_color);
    void drawBitmap(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color, uint16_t bg_color);


private:
    void write_cmd(uint8_t cmd);
    void write_data(uint8_t data);
    void reset();
    void set_windows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

    spi_inst_t* _spi;
    uint8_t _cs;
    uint8_t _dc;
    uint8_t _rst;
    uint8_t _bl;
};

#endif // ST7796S_H
