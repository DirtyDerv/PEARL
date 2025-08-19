# Industrial-Grade Protection for PEARL Electronics

## Recommended Protections

1. **Fuses**
   - PTC resettable or standard fuses on all power rails (24V input, 5V, 3.3V)
2. **Surge Protection**
   - TVS diode or MOV on main power input
3. **Reverse Polarity Protection**
   - Schottky diode or MOSFET on main input
4. **ESD Protection**
   - ESD TVS diodes on all external I/O (screw terminals)
5. **EMI Filtering**
   - Ferrite beads or common-mode chokes on power and signal lines
6. **Opto-Isolation**
   - For any I/O connected to external/industrial equipment
7. **Grounding**
   - Solid ground plane, separation of high/low voltage

## Schematic Placement
- Place fuses immediately after each power input terminal
- TVS/MOV and reverse polarity protection at the main input
- ESD diodes at each screw terminal I/O
- Ferrite beads on power rails before entering sensitive circuits
- Opto-isolators between Pico and any external high-voltage logic

## Example Symbols in KiCad
- Fuse: `Device:FUSE` or `Device:PTC`
- TVS: `Device:D_TVS`
- MOV: `Device:Varistor`
- Schottky: `Device:D_Schottky`
- ESD: `Device:D_TVS` (low capacitance)
- Ferrite: `Device:L_Ferrite_Bead`
- Opto: `OptoDevice:PC817` or similar

---
*Review this guide as you build your schematic for robust protection.*
