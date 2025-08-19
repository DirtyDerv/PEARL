#include <stdio.h>
#include "pico/stdlib.h"
#include <cstring>

bool test_example_1() {
    // Replace with real test logic
    return 1 + 1 == 2;
}

bool test_example_2() {
    // Replace with real test logic
    return 2 * 2 == 5;
}

// Example test function signatures
typedef bool (*test_func_t)();

// --- Example: Test config manager factory defaults ---
#include "config_manager.h"
// extern ConfigManager g_config_manager;
// bool test_config_manager_defaults() {
//     g_config_manager.init();
//     // Example: check a default value (customize as needed)
//     PersistentConfig* cfg = g_config_manager.get_config();
//     return cfg && cfg->encoder_resolution == 600;
// }

// --- Example: Test menu logic (stub/mock) ---
bool test_menu_init() {
    // This is a stub. Real test would require hardware mocks.
    return true;
}

// --- Example: Test LCD logic (stub/mock) ---
bool test_lcd_init() {
    // This is a stub. Real test would require I2C hardware mocks.
    return true;
}

// --- Example: Test encoder logic (stub/mock) ---
bool test_encoder_init() {
    // This is a stub. Real test would require PIO hardware mocks.
    return true;
}

void run_all_tests() {
    struct { const char* name; test_func_t func; } tests[] = {
        {"Example 1", test_example_1},
        {"Example 2", test_example_2},
    // {"ConfigManager Defaults", test_config_manager_defaults},
        {"Menu Init", test_menu_init},
        {"LCD Init", test_lcd_init},
        {"Encoder Init", test_encoder_init},
        // Add more tests here
    };
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;
    for (int i = 0; i < num_tests; ++i) {
        bool result = tests[i].func();
        printf("%s: %s\n", tests[i].name, result ? "PASS" : "FAIL");
        if (result) passed++;
    }
    printf("%d/%d tests passed\n", passed, num_tests);
}

// Serial command interface for test runner

void print_prompt() {
    printf("\n> Enter command (ALL, EX1, EX2, CFG, MENU, LCD, ENC): ");
    fflush(stdout);
}

void process_serial_commands() {
    char cmd[32];
    uint32_t last_heartbeat = to_ms_since_boot(get_absolute_time());
    print_prompt();
    while (true) {
        int idx = 0;
        // Read a line from serial
        while (idx < (int)sizeof(cmd) - 1) {
            int c = getchar_timeout_us(10000); // 10ms timeout
            if (c == PICO_ERROR_TIMEOUT) {
                // Heartbeat every 5 seconds
                uint32_t now = to_ms_since_boot(get_absolute_time());
                if (now - last_heartbeat > 5000) {
                    printf("\n[Heartbeat] Test runner alive.\n");
                    print_prompt();
                    last_heartbeat = now;
                }
                continue;
            }
            if (c == '\r' || c == '\n') {
                if (idx == 0) continue; // Ignore empty lines
                break;
            }
            cmd[idx++] = (char)c;
        }
        cmd[idx] = '\0';
        if (idx == 0) {
            print_prompt();
            continue;
        }
        if (strcmp(cmd, "ALL") == 0) {
            printf("Running all tests...\n");
            run_all_tests();
        } else if (strcmp(cmd, "EX1") == 0) {
            printf("Running Example 1...\n");
            printf("Example 1: %s\n", test_example_1() ? "PASS" : "FAIL");
        } else if (strcmp(cmd, "EX2") == 0) {
            printf("Running Example 2...\n");
            printf("Example 2: %s\n", test_example_2() ? "PASS" : "FAIL");
    // } else if (strcmp(cmd, "CFG") == 0) {
    //     printf("Running ConfigManager Defaults...\n");
    //     printf("ConfigManager Defaults: %s\n", test_config_manager_defaults() ? "PASS" : "FAIL");
        } else if (strcmp(cmd, "MENU") == 0) {
            printf("Running Menu Init...\n");
            printf("Menu Init: %s\n", test_menu_init() ? "PASS" : "FAIL");
        } else if (strcmp(cmd, "LCD") == 0) {
            printf("Running LCD Init...\n");
            printf("LCD Init: %s\n", test_lcd_init() ? "PASS" : "FAIL");
        } else if (strcmp(cmd, "ENC") == 0) {
            printf("Running Encoder Init...\n");
            printf("Encoder Init: %s\n", test_encoder_init() ? "PASS" : "FAIL");
        } else {
            printf("Unknown command: %s\n", cmd);
        }
        print_prompt();
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000); // Wait for USB serial to be ready
    printf("PEARL Firmware Test Runner\n");
    printf("Type ALL, EX1, or EX2 and press Enter to run tests.\n");
    process_serial_commands();
}
