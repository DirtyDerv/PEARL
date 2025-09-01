#ifndef FT6336U_H
#define FT6336U_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define FT6336U_ADDR 0x38

class FT6336U {
public:
    FT6336U(i2c_inst_t* i2c);
    bool get_touch_data(uint16_t& x, uint16_t& y);
    void init();

private:
    i2c_inst_t* _i2c;
};

#endif // FT6336U_H
