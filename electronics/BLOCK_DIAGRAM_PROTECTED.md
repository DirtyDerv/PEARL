# PEARL Block Diagram (with Industrial Protection)

```
+-------------------+
|  0-24V AC/DC In   |
+--------+----------+
         |
   +-----v-----+
   |  Fuse     |
   +-----+-----+
         |
   +-----v-----+
   | TVS/MOV   |
   +-----+-----+
         |
   +-----v-----+
   | Reverse   |
   | Polarity  |
   | Protection|
   +-----+-----+
         |
   +-----v-----+
   | Bridge    |
   | Rectifier |
   +-----+-----+
         |
   +-----v-----+
   | Buck      |----+----> 5V Rail --+-- Fuse --+-- Ferrite --+
   | Converter |    |                |          |             |
   +-----+-----+    |                |          |             |
         |          |                |          |             |
   +-----v-----+    |                |          |             |
   | LDO 3.3V  |    |                |          |             |
   +-----+-----+    |                |          |             |
         |          |                |          |             |
   +-----v-----+    |                |          |             |
   | Battery   |----+----> Power Path (Diode-OR)
   | Backup    |         (auto switchover)
   +-----+-----+
         |
   +-----v-----+
   | Pico MCU  |<--- Shutdown Detect
   +-----+-----+
         |
   +-----v-----+
   | Screw     |
   | Terminals |
   +-----+-----+
         |
   +-----v-----+
   | ESD/Opto  |
   | Protection|
   +-----------+
```

- Fuses, TVS/MOV, and reverse polarity protection on all power inputs
- ESD and opto-isolation on all external I/O
- Ferrite beads on power rails
