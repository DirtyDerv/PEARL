# PEARL Main Board Schematic Walkthrough (with Industrial Protections)

This guide walks you through adding all recommended protection elements to your KiCad schematic.

---

## 1. Power Input Protection

### a. Input Terminal
- Symbol: `Connector:Conn_01x02` (for 0-24V AC/DC input)

### b. Fuse
- Symbol: `Device:FUSE` or `Device:PTC`
- Place immediately after input terminal
- Example: Littelfuse 1206L series (PTC), or standard glass fuse

### c. TVS Diode / MOV
- Symbol: `Device:D_TVS` or `Device:Varistor`
- Place across input (after fuse)
- Example: SMAJ24A (TVS), or S14K25 (MOV)

### d. Reverse Polarity Protection
- Symbol: `Device:D_Schottky` (diode) or MOSFET
- Place after fuse and TVS/MOV
- Example: SS14 (Schottky)

### e. Bridge Rectifier
- Symbol: `Device:Bridge`
- Allows AC or DC input
- Example: MB6S

---

## 2. Power Conversion & Filtering

### a. Buck Converter (24V to 5V)
- Symbol: `DC-DC Module` or build discrete with inductor, diode, IC
- Example: MP1584, LM2596

### b. LDO Regulator (5V to 3.3V)
- Symbol: `Device:U` (IC)
- Example: AMS1117-3.3

### c. Ferrite Beads
- Symbol: `Device:L_Ferrite_Bead`
- Place on 5V and 3.3V rails before sensitive circuits
- Example: BLM21PG221SN1D

---

## 3. Battery Backup & Power Path

### a. Battery
- Symbol: `Battery_Cell` or `Connector:Conn_01x02`
- Example: LiPo cell or supercapacitor

### b. Power Path (Diode-OR)
- Symbol: `Device:D_Schottky` (two diodes)
- Example: SS14

---

## 4. Shutdown Detect Circuit
- Symbol: `Device:Comparator` or `Device:Voltage_Detector`
- Output to Pico GPIO
- Example: TLV803S, MCP100

---

## 5. Pico MCU
- Symbol: `Module:Raspberry_Pi_Pico` (or 40-pin header)
- Place at center of schematic

---

## 6. I/O Protection (Screw Terminals)

### a. Screw Terminals
- Symbol: `Connector:Conn_01x02` (or x03/x04 as needed)
- Label each for function (LCD, encoder, button, etc.)

### b. ESD Protection
- Symbol: `Device:D_TVS` (low capacitance)
- Place at each I/O terminal
- Example: PESD5V0S1UL

### c. Opto-Isolators (for external/industrial I/O)
- Symbol: `OptoDevice:PC817` or similar
- Place between terminal and Pico input

---

## 7. Grounding & Layout
- Use a solid ground plane
- Keep high-voltage and low-voltage areas separated
- Route sensitive signals away from power traces

---

## 8. Example Net Names
- `VIN` (0-24V input)
- `V5P` (5V rail)
- `V3P3` (3.3V rail)
- `SHUTDOWN` (to Pico GPIO)
- `LCD_SCL`, `ENC_A`, etc. (I/O nets)

---

## 9. Reference
- See `BLOCK_DIAGRAM_PROTECTED.md` for system overview
- See `PROTECTION_GUIDE.md` for symbol and part suggestions

---
*Use this as a checklist as you build your schematic in KiCad. Ask for help on any section!*
