#include <stdio.h>
#include "pico/stdlib.h"
#include "engineering_menu.h"
#include "hw040_encoder.h"
#include "lcd_i2c.h"

// Stub/mock for LCD and encoder (replace with real hardware if available)
LCD_I2C lcd(nullptr, 0x27); // Replace nullptr with real i2c_inst_t* if available
HW040Encoder encoder(0, 0, 0); // Replace with real pins if available
EngineeringMenu menu(&lcd, &encoder, nullptr, nullptr);

void print_menu_state() {
    printf("Menu State: %d\n", (int)menu.get_state());
    printf("Selected Item: %d\n", (int)menu.get_selected_item());
    // Add more details as needed
}

void process_serial_commands() {
    char cmd[32];
    printf("\n> Enter command (UP, DOWN, ENTER, SHOW): ");
    fflush(stdout);
    while (true) {
        int idx = 0;
        while (idx < (int)sizeof(cmd) - 1) {
            int c = getchar_timeout_us(10000);
            if (c == PICO_ERROR_TIMEOUT) continue;
            if (c == '\r' || c == '\n') break;
            cmd[idx++] = (char)c;
        }
        cmd[idx] = '\0';
        if (strcmp(cmd, "UP") == 0) {
            menu.encoder_up();
            print_menu_state();
        } else if (strcmp(cmd, "DOWN") == 0) {
            menu.encoder_down();
            print_menu_state();
        } else if (strcmp(cmd, "ENTER") == 0) {
            menu.encoder_enter();
            print_menu_state();
        } else if (strcmp(cmd, "SHOW") == 0) {
            print_menu_state();
        } else {
            printf("Unknown command: %s\n", cmd);
        }
        printf("\n> Enter command (UP, DOWN, ENTER, SHOW): ");
        fflush(stdout);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("PEARL Menu Test Firmware\n");
    menu.init();
    print_menu_state();
    process_serial_commands();
}
