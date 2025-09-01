#include "st7796s.h"
#include "font.h"

ST7796S::ST7796S(spi_inst_t* spi, uint8_t cs, uint8_t dc, uint8_t rst, uint8_t bl) {
    _spi = spi;
    _cs = cs;
    _dc = dc;
    _rst = rst;
    _bl = bl;
}

void ST7796S::write_cmd(uint8_t cmd) {
    gpio_put(_cs, 0);
    gpio_put(_dc, 0);
    spi_write_blocking(_spi, &cmd, 1);
    gpio_put(_cs, 1);
}

void ST7796S::write_data(uint8_t data) {
    gpio_put(_cs, 0);
    gpio_put(_dc, 1);
    spi_write_blocking(_spi, &data, 1);
    gpio_put(_cs, 1);
}

void ST7796S::reset() {
    gpio_put(_rst, 0);
    sleep_ms(200);
    gpio_put(_rst, 1);
    sleep_ms(200);
}

void ST7796S::init() {
    gpio_init(_cs);
    gpio_init(_dc);
    gpio_init(_rst);
    gpio_init(_bl);
    gpio_set_dir(_cs, GPIO_OUT);
    gpio_set_dir(_dc, GPIO_OUT);
    gpio_set_dir(_rst, GPIO_OUT);
    gpio_set_dir(_bl, GPIO_OUT);

    gpio_put(_cs, 1);
    gpio_put(_dc, 1);
    gpio_put(_rst, 1);
    gpio_put(_bl, 1);

    reset();

    write_cmd(0x11);
    sleep_ms(120);

    write_cmd(0x36);
    write_data(0x08);

    write_cmd(0x3A);
    write_data(0x05);

    write_cmd(0xF0);
    write_data(0xC3);

    write_cmd(0xF0);
    write_data(0x96);

    write_cmd(0xB4);
    write_data(0x01);

    write_cmd(0xB7);
    write_data(0xC6);

    write_cmd(0xC0);
    write_data(0x80);
    write_data(0x45);

    write_cmd(0xC1);
    write_data(0x13);

    write_cmd(0xC2);
    write_data(0xA7);

    write_cmd(0xC5);
    write_data(0x0A);

    write_cmd(0xE8);
    write_data(0x40);
    write_data(0x8A);
    write_data(0x00);
    write_data(0x00);
    write_data(0x29);
    write_data(0x19);
    write_data(0xA5);
    write_data(0x33);

    write_cmd(0xE0);
    write_data(0xD0);
    write_data(0x08);
    write_data(0x0F);
    write_data(0x06);
    write_data(0x06);
    write_data(0x33);
    write_data(0x30);
    write_data(0x33);
    write_data(0x47);
    write_data(0x17);
    write_data(0x13);
    write_data(0x13);
    write_data(0x2B);
    write_data(0x31);

    write_cmd(0xE1);
    write_data(0xD0);
    write_data(0x0A);
    write_data(0x11);
    write_data(0x0B);
    write_data(0x09);
    write_data(0x07);
    write_data(0x2F);
    write_data(0x33);
    write_data(0x47);
    write_data(0x38);
    write_data(0x15);
    write_data(0x16);
    write_data(0x2C);
    write_data(0x32);

    write_cmd(0xF0);
    write_data(0x3C);

    write_cmd(0xF0);
    write_data(0x69);

    sleep_ms(120);

    write_cmd(0x21);
    write_cmd(0x29);
}

void ST7796S::set_windows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
    write_cmd(0x2a);
    write_data(x_start >> 8);
    write_data(x_start & 0xff);
    write_data(x_end >> 8);
    write_data(x_end & 0xff);

    write_cmd(0x2b);
    write_data(y_start >> 8);
    write_data(y_start & 0xff);
    write_data(y_end >> 8);
    write_data(y_end & 0xff);

    write_cmd(0x2c);
}


void ST7796S::clear(uint16_t color) {
    uint16_t i, j;
    set_windows(0, 0, 320 - 1, 480 - 1);
    for (i = 0; i < 480; i++) {
        for (j = 0; j < 320; j++) {
            write_data(color >> 8);
            write_data(color);
        }
    }
}

void ST7796S::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    set_windows(x, y, x, y);
    write_data(color >> 8);
    write_data(color);
}

void ST7796S::drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    set_windows(x, y, x + w - 1, y + h - 1);
    for (uint32_t i = 0; i < w * h; i++) {
        write_data(color >> 8);
        write_data(color);
    }
}

void ST7796S::drawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg_color) {
    while (*str) {
        if (x > 320 - 8) {
            x = 0;
            y += 8;
        }
        if (y > 480 - 8) {
            return;
        }

        for (int i = 0; i < 8; i++) {
            uint8_t line = font[(int)(*str)][i];
            for (int j = 0; j < 8; j++) {
                if ((line >> j) & 1) {
                    drawPixel(x + j, y + i, color);
                } else {
                    drawPixel(x + j, y + i, bg_color);
                }
            }
        }
        str++;
        x += 8;
    }
}

void ST7796S::drawBitmap(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color, uint16_t bg_color) {
    uint16_t byte_width = (w + 7) / 8;
    for (uint16_t j = 0; j < h; j++) {
        for (uint16_t i = 0; i < w; i++) {
            if (bitmap[j * byte_width + i / 8] & (128 >> (i & 7))) {
                drawPixel(x + i, y + j, color);
            } else {
                drawPixel(x + i, y + j, bg_color);
            }
        }
    }
}
