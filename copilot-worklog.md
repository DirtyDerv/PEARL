# Copilot Worklog: Engineer Menu Refactor and Enhancements

## Summary
This worklog documents the step-by-step refactor and enhancement of the PEARL project's engineer and user menus, including security, usability, and diagnostics improvements.

## Major Changes
- Complete redesign of User and Engineer menus
- Secure engineer menu entry (triple-click + password, changeable)
- Non-blocking confirmation dialogs for critical actions (Factory Reset, Save & Exit, Calibration)
- Parameter grouping in Set Params (Encoder, Display, System, Security)
- Diagnostics submenu with live feedback (encoder, errors, FIFO)
- Step-by-step calibration flow with review/undo
- Password management enhancements (change, show/hide, reset)
- Inactivity timeouts and auto-exit
- Help/info screens and descriptions
- Version/build info in About
- Menu customization and persistent storage feedback

## Implementation Phases
1. **Menu Structure Refactor**: Updated enums, state machine, and navigation logic for new menu flows.
2. **Security**: Password entry, change, and reset logic; secure engineer access.
3. **Confirmation Dialogs**: Added for Factory Reset, Save & Exit, Calibration.
4. **Parameter Grouping**: Set Params now groups settings with descriptions.
5. **Diagnostics**: Live encoder, error, and FIFO display.
6. **Calibration**: User-driven, editable position entry, review/undo before saving.
7. **Password Enhancements**: Show/hide toggle, reset to default, confirmation.
8. **Timeouts/Auto-Exit**: Menu auto-exit after inactivity.
9. **Help/Info**: Contextual help and About screens.
10. **Persistent Storage Feedback**: User feedback on save/flash operations.

## Next Steps
- Final user testing
- Documentation updates (instructions, user manual, troubleshooting)

## August 19, 2025: HW-040 Encoder False Click/Triple-Click Debug
- Issue: Turning the HW-040 encoder triggered false button (SW) interrupts, causing unwanted triple-clicks and menu actions.
- Debug: Added debug prints to both rotary and button interrupt handlers to trace click_count and event sources.
- Root Cause: Hardware crosstalk/noise on SW pin during rotation, not a software bug.
- Solution: Added software filter—button clicks are only counted if encoder is not rotating (CLK and DT both HIGH).
- Result: False button events during rotation are now ignored; menu logic is robust against hardware noise.
- Recommendation: For a fully robust solution, improve hardware (wiring, add stronger pull-up resistor to SW, or try another encoder).
