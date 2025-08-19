# Copilot Action Log

This file records all significant actions, commands, and tool invocations performed by GitHub Copilot in this repository. Use this log to track changes, troubleshooting steps, and project history.

---
## August 19, 2025: HW-040 Encoder False Click/Triple-Click Debug
- Added debug prints to rotary and button interrupt handlers in hw040_encoder.cpp.
- Confirmed SW (button) interrupt was triggered by encoder rotation (hardware crosstalk/noise).
- Implemented software filter: button clicks only counted if CLK and DT are HIGH (not rotating).
- Verified via debug output that false button events are now ignored during rotation.
- Recommended hardware improvements for full robustness.
