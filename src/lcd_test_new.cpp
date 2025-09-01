#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "st7796s.h"
#include "ft6336u.h"
#include "config.h"

// Define the pins for the ST7796S display
#define LCD_CS_PIN 9
#define LCD_DC_PIN 25
#define LCD_RST_PIN 27
#define LCD_BL_PIN 18

#define SPI_PORT spi1
#define SPI_TX_PIN 11
#define SPI_RX_PIN 12
#define SPI_SCK_PIN 10

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // Initialize SPI for LCD
    spi_init(SPI_PORT, 5000000); // 5 MHz
    gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
    // MISO not used for display only
    // gpio_set_function(SPI_RX_PIN, GPIO_FUNC_SPI);

    ST7796S lcd(SPI_PORT, LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, LCD_BL_PIN);
    lcd.init();

    // Initialize I2C for touch
    i2c_init(i2c0, 100000); // 100 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    FT6336U touch(i2c0);
    touch.init();

    lcd.clear(0x0000); // Black

    lcd.drawRectangle(10, 10, 100, 100, 0xF800); // Red
    lcd.drawRectangle(120, 10, 100, 100, 0x07E0); // Green
    lcd.drawRectangle(230, 10, 80, 100, 0x001F); // Blue

    while (true) {
        uint16_t x, y;
        if (touch.get_touch_data(x, y)) {
            printf("Touch at: %d, %d\n", x, y);
            lcd.drawRectangle(x-2, y-2, 4, 4, 0xFFFF); // White
        }
        sleep_ms(20);
    }
}
