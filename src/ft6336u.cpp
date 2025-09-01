#include "ft6336u.h"
#include <cstring>

#define FT6336U_TOUCH_NUM_REG 0x02
#define FT6336U_TOUCH_XH_REG 0x03
#define FT6336U_TOUCH_XL_REG 0x04
#define FT6336U_TOUCH_YH_REG 0x05
#define FT6336U_TOUCH_YL_REG 0x06


FT6336U::FT6336U(i2c_inst_t* i2c) {
    _i2c = i2c;
}

void FT6336U::init() {
    // Nothing to do here for now
}

bool FT6336U::get_touch_data(uint16_t& x, uint16_t& y) {
    uint8_t touch_num = 0;
    uint8_t buffer[6];

    uint8_t reg = FT6336U_TOUCH_NUM_REG;
    i2c_write_blocking(_i2c, FT6336U_ADDR, &reg, 1, true);
    i2c_read_blocking(_i2c, FT6336U_ADDR, &touch_num, 1, false);

    if (touch_num != 1) {
        return false;
    }

    reg = FT6336U_TOUCH_XH_REG;
    i2c_write_blocking(_i2c, FT6336U_ADDR, &reg, 1, true);
    i2c_read_blocking(_i2c, FT6336U_ADDR, buffer, 6, false);

    x = (uint16_t)(buffer[0] & 0x0F) << 8 | (uint16_t)buffer[1];
    y = (uint16_t)(buffer[2] & 0x0F) << 8 | (uint16_t)buffer[3];

    return true;
}
