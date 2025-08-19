# PEARL Quick Start Guide

## 1. Connect Hardware
- Wire up the Raspberry Pi Pico, I2C LCD, HW-040 encoder, and main quadrature encoder as per the wiring diagram.
- Power on the system via USB.

## 2. Flash Firmware
- Download the latest `PEARL.uf2` file.
- Hold BOOTSEL on the Pico, connect USB, and copy the file to the Pico drive.

## 3. First Boot
- Splash screen appears, then main display.
- Triple-click the HW-040 encoder to access the engineer menu (password: 1234).

## 4. Essential Setup
- Set encoder resolution and thread pitch in the engineer menu.
- Run calibration (step-by-step guided process).
- Save and exit.

## 5. Test
- Rotate encoder, verify position changes.
- Confirm menu navigation and settings.

---
*For detailed instructions, see INSTALLATION.md and USER_MANUAL.md*

*Document version: v0.07, last updated: August 19, 2025*
