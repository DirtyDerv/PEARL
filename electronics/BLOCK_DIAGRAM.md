# PEARL Electronics Block Diagram

```
+-------------------+
|  0-24V AC/DC In   |
+--------+----------+
         |
   +-----v-----+
   | Bridge    |
   | Rectifier |
   +-----+-----+
         |
   +-----v-----+
   | Buck      |----+----> 5V Rail
   | Converter |    |
   +-----+-----+    |
         |          |
   +-----v-----+    |
   | LDO 3.3V  |    |
   +-----+-----+    |
         |          |
   +-----v-----+    |
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
   +-----------+
```

- All I/O (LCD, encoders, buttons, etc.) connect to screw terminals.
- Shutdown detect circuit signals Pico to safely power down on loss of main power.
- Battery backup keeps Pico alive for safe shutdown.
