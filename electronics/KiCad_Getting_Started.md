# Getting Started with KiCad for PEARL

## 1. Install KiCad
- Download from https://kicad.org/download/
- Install the latest stable version for your OS

## 2. Open the Project
- Open KiCad, click "Open Project", and select the `PEARL_Schematic.kicad_sch` file in `electronics/schematic/`

## 3. Schematic Editing Basics
- **Add Components:** Press `A` and search for parts (e.g., "Raspberry Pi Pico", "Screw Terminal", "Bridge Rectifier", "Buck Converter", "LDO", "Battery", etc.)
- **Wire Components:** Press `W` to draw wires between pins
- **Label Nets:** Press `L` to add net labels (e.g., `5V`, `3.3V`, `GND`, `SHUTDOWN`)
- **Move/Rotate:** Select and press `M` (move), `R` (rotate)
- **Annotate:** Click the "Annotate Schematic" button to auto-number parts
- **Electrical Rules Check:** Click the "ERC" button to check for errors

## 4. Key Sections for PEARL
- **Pico:** Place the Pico symbol (or use 40-pin header if not available)
- **Power Supply:**
  - Bridge rectifier (for AC/DC input)
  - Buck converter (0-24V to 5V)
  - LDO (5V to 3.3V)
  - Decoupling capacitors
- **Battery Backup:**
  - Diode-OR or power path controller
  - LiPo or supercap
  - Charger IC (optional)
- **Shutdown Circuit:**
  - Comparator or voltage detector
  - Connect output to Pico GPIO
- **Screw Terminals:**
  - For each I/O (label clearly: LCD, encoders, buttons, power, etc.)

## 5. Save and Version
- Save often! Use Git to track changes if possible.

## 6. Next Steps
- Draw the schematic, then use KiCad’s PCB editor to lay out the board.
- Export BOM and Gerbers for manufacturing.

---
*Ask for help on any step!*
