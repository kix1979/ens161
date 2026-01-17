# Changelog

All notable changes to this project will be documented in this file.

## [1.0.0] - 2025-01-15

### Added
- Initial release
- ENS161 air quality sensor support
  - eCO2 (Equivalent CO₂) measurement
  - TVOC (Total Volatile Organic Compounds) measurement
  - AQI (Air Quality Index) measurement
  - HCHO (Formaldehyde) measurement
  - Raw resistance values (HP0-HP3) for advanced diagnostics
- PMSA003I particulate matter sensor support
  - PM1.0, PM2.5, PM10 concentration measurements
  - Particle count measurements (>0.3, >0.5, >1.0, >2.5, >5.0, >10μm)
- Dual I2C bus configuration for reliable operation
- Moving average filters for stable readings
- Complete ESPHome integration
- Home Assistant automatic discovery
- Comprehensive documentation
- Wiring diagrams and examples
- Troubleshooting guide

### Configuration
- ESP32 with ESP-IDF framework
- I2C Bus A (GPIO21/22) for ENS161
- I2C Bus B (GPIO25/26) for PMSA003I
- 400kHz I2C frequency
- 15s update interval for ENS161
- 60s update interval for PMSA003I

## Future Enhancements
- [ ] Display support (ST7789 LCD)
- [ ] Temperature/humidity compensation from external sensor
- [ ] MQTT support
- [ ] Web dashboard
- [ ] Data logging to SD card
- [ ] Battery power optimization
