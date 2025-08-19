#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd_i2c.h"
#include "config.h"

int main() {
    stdio_init_all();
    sleep_ms(2000); // Give USB time for serial

    // Initialize I2C for LCD
    i2c_init(i2c0, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    LCD_I2C lcd(i2c0, LCD_ADDR);
    lcd.init();
    lcd.backlight_on();
    lcd.set_cursor(0, 0);
    lcd.print("LCD TEST OK");
    lcd.set_cursor(0, 1);
    lcd.print("ADDR: 0x27");
    while (true) {
        sleep_ms(1000);
    }
}
