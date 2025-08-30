#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd_i2c.h"
#include "config.h"
#include "big_font.h"

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

    BigFont big_font(&lcd);
    big_font.init_matrix_font();

    big_font.display_big_char('1', 0, 0);
    big_font.display_big_char('2', 4, 0);
    big_font.display_big_char('.', 8, 0);
    big_font.display_big_char('3', 10, 0);


    while (true) {
        sleep_ms(1000);
    }
}
