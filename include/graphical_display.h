#ifndef GRAPHICAL_DISPLAY_H
#define GRAPHICAL_DISPLAY_H

#include "st7796s.h"
#include "ft6336u.h"

class GraphicalDisplay {
public:
    GraphicalDisplay(ST7796S* lcd, FT6336U* touch);
    void init();
    void draw_main_ui();
    void update_values(long count, double distance, double velocity, int rpm);
    void handle_touch();

private:
    ST7796S* _lcd;
    FT6336U* _touch;
};

#endif // GRAPHICAL_DISPLAY_H
