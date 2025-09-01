#include "graphical_display.h"
#include "config.h"
#include <cstdio>
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "gear_icon.h"

#define BG_COLOR 0x0000
#define FG_COLOR 0xFFFF

GraphicalDisplay::GraphicalDisplay(ST7796S* lcd, FT6336U* touch) {
    _lcd = lcd;
    _touch = touch;
}

void GraphicalDisplay::init() {
    _lcd->init();
    _touch->init();

    // Start the RTC
    rtc_init();
    datetime_t t = {
            .year  = 2025,
            .month = 9,
            .day   = 1,
            .dotw  = 1, // 0 is Sunday
            .hour  = 0,
            .min   = 0,
            .sec   = 0
    };
    rtc_set_datetime(&t);
}

void GraphicalDisplay::draw_main_ui() {
    _lcd->clear(BG_COLOR);

    // Top bar
    _lcd->drawRectangle(0, 0, 320, 20, 0x001F);
    _lcd->drawBitmap(290, 2, gear_icon, 16, 16, 0xFFFF, 0x001F);

    // Main display area
    _lcd->drawString(10, 100, "Position:", FG_COLOR, BG_COLOR);
    _lcd->drawString(10, 200, "-1234.5", FG_COLOR, BG_COLOR);

    // Bottom bar
    _lcd->drawRectangle(0, 460, 320, 20, 0x001F);
    _lcd->drawString(10, 462, "Speed: 123.4 mm/s", FG_COLOR, 0x001F);
    _lcd->drawString(200, 462, "Dir: ->", FG_COLOR, 0x001F);
}

void GraphicalDisplay::update_values(long count, double distance, double velocity, int rpm) {
    char buffer[50];

    // Time and date
    datetime_t t;
    rtc_get_datetime(&t);
    sprintf(buffer, "%02d:%02d:%02d %02d/%02d/%04d", t.hour, t.min, t.sec, t.day, t.month, t.year);
    _lcd->drawString(10, 2, buffer, FG_COLOR, 0x001F);

    // Position
    sprintf(buffer, "%8.1f", distance);
    _lcd->drawString(10, 200, buffer, FG_COLOR, BG_COLOR);

    // Speed and direction
    sprintf(buffer, "Speed: %.1f mm/s", velocity);
    _lcd->drawString(10, 462, buffer, FG_COLOR, 0x001F);
    if (velocity > 0) {
        _lcd->drawString(200, 462, "Dir: ->", FG_COLOR, 0x001F);
    } else if (velocity < 0) {
        _lcd->drawString(200, 462, "Dir: <-", FG_COLOR, 0x001F);
    } else {
        _lcd->drawString(200, 462, "Dir: --", FG_COLOR, 0x001F);
    }
}

void GraphicalDisplay::handle_touch() {
    uint16_t x, y;
    if (_touch->get_touch_data(x, y)) {
        printf("Touch at: %d, %d\n", x, y);
        if (x > 290 && x < 310 && y > 2 && y < 18) {
            printf("Gear icon touched!\n");
        }
    }
}
