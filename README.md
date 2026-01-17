# ESP32 Air Quality Monitor with ENS161 and PMSA003I

A comprehensive air quality monitoring system using ESP32, ENS161 (air quality sensor), and PMSA003I (particulate matter sensor) integrated with Home Assistant via ESPHome.

## Features

- **ENS161 Air Quality Sensor**
  - eCO2 (Equivalent CO₂): 400-65000 ppm
  - TVOC (Total Volatile Organic Compounds): 0-65000 ppb
  - AQI (Air Quality Index): 1-5 scale
  - HCHO (Formaldehyde): 0-1000 ppb
  
- **PMSA003I Particulate Matter Sensor**
  - PM1.0, PM2.5, PM10 concentrations (μg/m³)
  - Particle counts (>0.3, >0.5, >1.0, >2.5, >5.0, >10μm)

- **Dual I2C Buses** for reliable communication
- **Home Assistant Integration** via ESPHome
- **Moving Average Filters** for stable readings

## Hardware Requirements

| Component | Description | Interface |
|-----------|-------------|-----------|
| ESP32 Development Board | Main microcontroller | - |
| ENS161 | Air quality sensor | I2C (0x53) |
| PMSA003I | Particulate matter sensor | I2C (0x12) |
| Breadboard & Jumper wires | For prototyping | - |

## Wiring Diagram

```
ENS161        ESP32
------        -----
VIN     ----> 3.3V
GND     ----> GND
SDI     ----> GPIO21  (I2C Bus A - SDA)
SDK     ----> GPIO22  (I2C Bus A - SCL)

PMSA003I      ESP32
--------      -----
SDA     ----> GPIO25  (I2C Bus B - SDA)
SCL     ----> GPIO26  (I2C Bus B - SCL)
VCC     ----> 3.3V (or 5V depending on module)
GND     ----> GND
```

**Important Notes:**
- ENS161 requires 3.3V only (do NOT use 5V)
- All GND pins must be connected to common ground
- ENS161 uses SDI/SDK pins for I2C (not the SPI pins)

## Installation

### 1. File Structure

Create the following structure in your ESPHome configuration directory (`/config/esphome/`):

```
/config/esphome/
├── esphome-web-0c1ee8.yaml       # Main configuration
└── components/                    # Custom components
    └── ens161/                    # ENS161 sensor driver
        ├── __init__.py
        ├── sensor.py
        ├── ens161.h
        └── ens161.cpp
```

### 2. Copy Files

Copy all files from this repository:
- `esphome-web-0c1ee8.yaml` → Your ESPHome config directory
- `components/ens161/*` → Create `components/ens161/` directory and copy all files

### 3. Configure WiFi

Create or update `secrets.yaml` in your ESPHome directory:

```yaml
wifi_ssid: "Your_WiFi_Name"
wifi_password: "Your_WiFi_Password"
```

### 4. Upload to ESP32

1. Connect ESP32 via USB
2. In ESPHome dashboard, click "Install"
3. Select "Plug into this computer"
4. Wait for compilation and upload (5-10 minutes first time)

## Configuration

The main configuration file is `esphome-web-0c1ee8.yaml`. Key sections:

### I2C Buses
```yaml
i2c:
  - id: bus_a          # ENS161
    sda: GPIO21
    scl: GPIO22
  - id: bus_b          # PMSA003I
    sda: GPIO25
    scl: GPIO26
```

### Sensors
All sensors are configured with appropriate filters and update intervals:
- ENS161: 15 second updates with 5-point moving average
- PMSA003I: 60 second updates

## Home Assistant Integration

Once running, the device automatically appears in Home Assistant with these entities:

**Air Quality (ENS161):**
- `sensor.eco2` - Equivalent CO₂ (ppm)
- `sensor.tvoc` - Total VOC (ppb)
- `sensor.aqi` - Air Quality Index (1-5)
- `sensor.formaldehyde` - HCHO (ppb)

**Particulates (PMSA003I):**
- `sensor.pm1_0` - PM1.0 (μg/m³)
- `sensor.pm2_5` - PM2.5 (μg/m³)
- `sensor.pm10` - PM10 (μg/m³)
- Plus 6 particle count sensors

## Air Quality Guidelines

### PM2.5 Levels (μg/m³)
- **0-12**: Good
- **12-35**: Moderate
- **35-55**: Unhealthy for sensitive groups
- **55-150**: Unhealthy
- **150+**: Very unhealthy

### eCO₂ Levels (ppm)
- **400-1000**: Excellent
- **1000-2000**: Good (typical indoor)
- **2000-5000**: Poor ventilation
- **5000+**: Health concerns

### TVOC Levels (ppb)
- **0-220**: Good
- **220-660**: Moderate
- **660-2200**: Poor
- **2200+**: Very poor

### Formaldehyde (ppb)
- **<10**: Good
- **10-50**: Moderate
- **>50**: Poor

## Troubleshooting

### Sensor Not Detected
1. Check wiring connections
2. Verify power supply (3.3V for ENS161)
3. Run I2C scan in logs - should show 0x53 (ENS161) and 0x12 (PMSA003I)
4. Try swapping SDA/SCL if mislabeled

### No Sensor Data
1. Wait 1-2 minutes for sensor warm-up
2. ENS161 requires up to 1 hour for initial calibration
3. Check logs for error messages
4. Verify sensor power

### Unstable Readings
1. Check power supply stability
2. Shorten I2C wires (<20cm ideal)
3. Lower I2C frequency to 100kHz
4. Increase moving average window size

## Technical Specifications

- **I2C Frequency**: 400kHz
- **Update Rates**: 15s (ENS161), 60s (PMSA003I)
- **Power Consumption**: ~180mA @ 5V
- **Operating Voltage**: 3.3V (ENS161), 3.3V/5V (PMSA003I)

## Documentation

See [DOCUMENTATION.md](DOCUMENTATION.md) for complete technical documentation including:
- Detailed wiring diagrams
- Complete code explanations
- Advanced configuration options
- Maintenance guidelines
- Troubleshooting guide

## License

This project is provided as-is for personal use. Hardware components and libraries may have their own licenses.

## Credits

- ESPHome: https://esphome.io/
- ScioSense ENS160/161: https://www.sciosense.com/
- Plantower PMSA003I: https://www.plantower.com/

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Support

For issues and questions:
- Open an issue on GitHub
- ESPHome Discord: https://discord.gg/KhAMKrd
- Home Assistant Community: https://community.home-assistant.io/

---

**Repository**: https://github.com/kix1979/ens161
**Version**: 1.0
**Last Updated**: January 2025
