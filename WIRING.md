# Wiring Diagram

## Text-based Diagram

```
                    ┌─────────────────┐
                    │     ESP32       │
    ENS161          │                 │
    ┌──────┐        │  GPIO21 (SDA)   │
    │ SDI  ├────────┤  GPIO22 (SCL)   │
    │ SDK  ├────────┤                 │
    │ VIN  ├────┐   │  GPIO25 ────────┼────────┤ SDA
    │ GND  ├────┼───┤  GPIO26 ────────┼────────┤ SCL      PMSA003I
    └──────┘    │   │                 │        │ VCC
    (leave SCK, │   │  3.3V   ────────┼────┬───┤ GND
     SDO, CS,   └───┤  GND    ────────┼────┘   └──────
     INT, 3VO       └─────────────────┘
     unconnected)
```

## Connection Table

| Sensor | Pin | ESP32 Pin | Description |
|--------|-----|-----------|-------------|
| ENS161 | VIN | 3.3V | Power (3.3V only!) |
| ENS161 | GND | GND | Ground |
| ENS161 | SDI | GPIO21 | I2C Data (Bus A) |
| ENS161 | SDK | GPIO22 | I2C Clock (Bus A) |
| PMSA003I | VCC | 3.3V/5V | Power (check module) |
| PMSA003I | GND | GND | Ground |
| PMSA003I | SDA | GPIO25 | I2C Data (Bus B) |
| PMSA003I | SCL | GPIO26 | I2C Clock (Bus B) |

## Notes

- **ENS161**: Use SDI/SDK pins for I2C (not SCK/SDO which are for SPI)
- **Power**: ENS161 requires 3.3V only. PMSA003I may accept 3.3V or 5V
- **Common Ground**: All GND pins must connect to ESP32 GND
- **Wire Length**: Keep I2C wires under 20cm for reliability
