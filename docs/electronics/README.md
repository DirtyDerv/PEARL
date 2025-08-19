# PEARL Electronics Overview

## Schematic & PCB Plan

- **Controller:** Raspberry Pi Pico (on-board)
- **I/O:** All inputs/outputs routed to labeled screw terminals
- **Power Supply:**
  - Input: 0–24V AC or DC
  - Bridge rectifier for AC/DC compatibility
  - Buck converter for 5V output
  - LDO or regulator for 3.3V output
- **Battery Backup:**
  - Small LiPo or supercap for safe shutdown
  - Automatic switchover on power loss
- **Shutdown Circuit:**
  - Detects main power loss
  - Signals Pico to begin safe shutdown

## Next Steps
- [ ] Draw schematic (KiCad format)
- [ ] Design PCB layout
- [ ] Generate BOM
- [ ] Add wiring diagrams and test points

---
*Edit this file to track electronics progress and notes.*
