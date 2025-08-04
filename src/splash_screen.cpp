#include "splash_screen.h"
#include "version.h"
#include <stdio.h>
#include <string.h>

const char* SplashScreen::SPINNER_CHARS = "|/-\\";

SplashScreen::SplashScreen(LCD_I2C* display) 
    : lcd(display), animation_frame(0), last_animation_time(0) {
}

void SplashScreen::show_startup() {
    lcd->clear();
    
    // PEARL logo/title
    lcd->set_cursor(0, 0);
    lcd->print("  ████████████  ");
    lcd->set_cursor(0, 1);
    lcd->print("  ██  PEARL ██  ");
    lcd->set_cursor(0, 2);
    lcd->print("  ████████████  ");
    lcd->set_cursor(0, 3);
    lcd->print("   Starting...   ");
    
    sleep_ms(1500); // Show logo for 1.5 seconds
}

void SplashScreen::show_version_info() {
    lcd->clear();
    
    // Product name and version
    center_text("PEARL ENCODER", 0);
    
    char version_str[20];
    sprintf(version_str, "Version %s", PROJECT_VERSION_STRING);
    center_text(version_str, 1);
    
    char build_str[20];
    sprintf(build_str, "Build %s", BUILD_DATE);
    center_text(build_str, 2);
    
    center_text("Initializing...", 3);
    
    sleep_ms(2000); // Show version info for 2 seconds
}

void SplashScreen::show_progress(const char* step, uint8_t percentage) {
    lcd->clear();
    
    // Title
    center_text("PEARL STARTING", 0);
    
    // Current step
    lcd->set_cursor(0, 1);
    char step_text[21];
    sprintf(step_text, "%-16s", step);
    lcd->print(step_text);
    
    // Add spinner animation
    update_loading_animation();
    lcd->set_cursor(16, 1);
    lcd->print(&SPINNER_CHARS[animation_frame]);
    
    // Progress bar
    draw_progress_bar(percentage, 2);
    
    // Percentage
    lcd->set_cursor(0, 3);
    char percent_text[20];
    sprintf(percent_text, "Progress: %d%%", percentage);
    center_text(percent_text, 3);
    
    sleep_ms(300); // Brief pause to show progress
}

void SplashScreen::show_hardware_status(const char* component, bool status) {
    lcd->set_cursor(0, 1);
    char status_text[21];
    sprintf(status_text, "%-12s %s", component, status ? "OK" : "FAIL");
    lcd->print(status_text);
    
    if (!status) {
        // Flash error indicator
        for (int i = 0; i < 3; i++) {
            lcd->set_cursor(19, 1);
            lcd->print("!");
            sleep_ms(200);
            lcd->set_cursor(19, 1);
            lcd->print(" ");
            sleep_ms(200);
        }
    }
    
    sleep_ms(500);
}

void SplashScreen::show_ready() {
    lcd->clear();
    
    // Success message
    center_text("PEARL READY", 0);
    center_text("===============", 1);
    center_text("System Online", 2);
    center_text("Press to begin", 3);
    
    // Brief flash to indicate ready state
    for (int i = 0; i < 2; i++) {
        sleep_ms(300);
        lcd->clear();
        sleep_ms(200);
        center_text("PEARL READY", 0);
        center_text("===============", 1);
        center_text("System Online", 2);
        center_text("Press to begin", 3);
    }
    
    sleep_ms(1000);
}

void SplashScreen::update_loading_animation() {
    uint32_t current_time = time_us_32() / 1000; // Convert to milliseconds
    
    if (current_time - last_animation_time >= ANIMATION_INTERVAL_MS) {
        animation_frame = (animation_frame + 1) % 4;
        last_animation_time = current_time;
    }
}

void SplashScreen::center_text(const char* text, uint8_t row) {
    uint8_t text_len = strlen(text);
    uint8_t start_col = 0;
    
    // Center text on 20-character display (adjust if your display is different)
    if (text_len < 20) {
        start_col = (20 - text_len) / 2;
    }
    
    lcd->set_cursor(start_col, row);
    lcd->print(text);
}

void SplashScreen::draw_progress_bar(uint8_t percentage, uint8_t row) {
    lcd->set_cursor(0, row);
    lcd->print("[");
    
    // Progress bar is 18 characters wide (20 total - 2 for brackets)
    uint8_t filled_chars = (percentage * 18) / 100;
    
    for (uint8_t i = 0; i < 18; i++) {
        if (i < filled_chars) {
            lcd->print("=");
        } else if (i == filled_chars && percentage < 100) {
            lcd->print(">");
        } else {
            lcd->print(" ");
        }
    }
    
    lcd->print("]");
}
