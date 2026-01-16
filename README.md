# ens161
Custom Library for ENS161 air quality monitor for ESPHome and ESP32 boards

### ENS161 Pin Identification
The ENS161 board has the following pins:
- **VIN**: Power input (3.3V)
- **3VO**: 3.3V output (leave unconnected)
- **GND**: Ground
- **SDI**: I2C Data (SDA) - use this for I2C
- **SDK**: I2C Clock (SCL) - use this for I2C
- **SCK**: SPI Clock (leave unconnected in I2C mode)
- **SDO**: SPI Data Out (leave unconnected in I2C mode)
- **CS**: Chip Select (leave unconnected in I2C mode)
- **INT**: Interrupt (optional, leave unconnected)
- **STEMMA QT**: 4-pin I2C connector (alternative to pin headers)

**Important**: The ENS161 supports both SPI and I2C. When using the SDI/SDK pins, it automatically operates in I2C mode.
For my code I use the following pins

ENS161        ESP32
------        -----
VIN     ----> 3.3V (DO NOT USE 5V)
GND     ----> GND
SDI     ----> GPIO21  (I2C Bus A - SDA)
SDK     ----> GPIO22  (I2C Bus A - SCL)

File Structure
ESPHome configuration requires the following file structure:
/config/esphome/
├── esphome-[YOUR_YAML_FILE].yaml  # Main configuration file
└── components/                    # Custom component directory
    └── ens161/                    # ENS161 sensor component
        ├── __init__.py            # Python init file
        ├── sensor.py              # ESPHome sensor platform
        ├── ens161.h               # C++ header file
        └── ens161.cpp             # C++ implementation
