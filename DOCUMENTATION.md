# ESP32 Air Quality Sensor Documentation

## Project Overview

This project uses an ESP32 microcontroller with two I2C sensors to monitor air quality:
- **PMSA003I**: Particulate matter sensor (PM1.0, PM2.5, PM10)
- **ENS161**: Air quality sensor (eCO2, TVOC, AQI)

Both sensors communicate via I2C on separate buses to avoid address conflicts.

---

## Hardware Components

### Required Components

| Component | Description | Voltage | Interface |
|-----------|-------------|---------|-----------|
| ESP32 Development Board | Main microcontroller | 3.3V/5V | - |
| PMSA003I | Particulate matter sensor | 3.3V or 5V* | I2C (0x12) |
| ENS161 | Air quality sensor | 3.3V only | I2C (0x53) |
| Breadboard | For prototyping | - | - |
| Jumper wires | For connections | - | - |

*Check your specific PMSA003I module specifications

### ENS161 Pin Identification

Your ENS161 board has the following pins:
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

---

## Wiring Diagram

### Complete Wiring

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
VCC     ----> 3.3V (or 5V if your module requires it)
GND     ----> GND
```

### Visual Diagram

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

### Wiring Notes

1. **Power**: 
   - ENS161 requires 3.3V only (do NOT use 5V)
   - PMSA003I may accept 3.3V or 5V depending on your module
   - All GND pins must be connected to a common ground

2. **I2C Buses**:
   - Using two separate I2C buses prevents address conflicts
   - Each bus operates at 400kHz frequency
   - Bus A (GPIO21/22): ENS161 at address 0x53
   - Bus B (GPIO25/26): PMSA003I at address 0x12

3. **Pull-up Resistors**:
   - Most breakout boards have built-in 4.7kΩ pull-up resistors
   - If your boards don't have them, add external pull-ups from SDA to VCC and SCL to VCC

4. **Cable Length**:
   - Keep I2C wires as short as possible (ideally < 20cm)
   - Longer wires may require lower I2C frequency

---

## Software Setup

### File Structure

Your ESPHome configuration requires the following file structure:

```
/config/esphome/
├── esphome-web-0c1ee8.yaml       # Main configuration file
└── components/                    # Custom component directory
    └── ens161/                    # ENS161 sensor component
        ├── __init__.py            # Python init file
        ├── sensor.py              # ESPHome sensor platform
        ├── ens161.h               # C++ header file
        └── ens161.cpp             # C++ implementation
```

### Configuration Files

#### 1. Main Configuration (esphome-web-0c1ee8.yaml)

```yaml
esphome:
  name: esphome-web-0c1ee8
  friendly_name: Air Quality Sensor
  min_version: 2025.11.0
  name_add_mac_suffix: false

esp32:
  variant: esp32
  framework:
    type: esp-idf

# Load custom ENS161 component
external_components:
  - source:
      type: local
      path: components

# I2C Bus A - for ENS161
i2c:
  - id: bus_a
    sda: GPIO21
    scl: GPIO22
    scan: true
    frequency: 400kHz
  
  # I2C Bus B - for PMSA003I
  - id: bus_b
    sda: GPIO25
    scl: GPIO26
    scan: true
    frequency: 400kHz

sensor:
  # ENS161 Air Quality Sensor (on bus_a)
  - platform: ens161
    i2c_id: bus_a
    address: 0x53
    eco2:
      name: "eCO2"
      filters:
        - sliding_window_moving_average:
            window_size: 5
            send_every: 1
    tvoc:
      name: "TVOC"
      filters:
        - sliding_window_moving_average:
            window_size: 5
            send_every: 1
    aqi:
      name: "AQI"
    update_interval: 15s

  # PMSA003I Particulate Matter Sensor (on bus_b)
  - platform: pmsa003i
    i2c_id: bus_b
    address: 0x12
    pm_1_0:
      name: "PM1.0"
    pm_2_5:
      name: "PM2.5"
    pm_10_0:
      name: "PM10"
    pmc_0_3:
      name: "PM0.3 Count"
    pmc_0_5:
      name: "PM0.5 Count"
    pmc_1_0:
      name: "PM1.0 Count"
    pmc_2_5:
      name: "PM2.5 Count"
    pmc_5_0:
      name: "PM5.0 Count"
    pmc_10_0:
      name: "PM10 Count"
    update_interval: 60s

logger:

api:

ota:
  - platform: esphome

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
```

#### 2. Component Files

**components/ens161/__init__.py**
```python
import esphome.codegen as cg

CODEOWNERS = ["@yourusername"]
ens161_ns = cg.esphome_ns.namespace("ens161")
```

**components/ens161/sensor.py**
```python
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_CARBON_DIOXIDE,
    DEVICE_CLASS_VOLATILE_ORGANIC_COMPOUNDS_PARTS,
    STATE_CLASS_MEASUREMENT,
    UNIT_PARTS_PER_MILLION,
    UNIT_PARTS_PER_BILLION,
)

DEPENDENCIES = ["i2c"]

ens161_ns = cg.esphome_ns.namespace("ens161")
ENS161Component = ens161_ns.class_("ENS161Component", cg.PollingComponent, i2c.I2CDevice)

CONF_ECO2 = "eco2"
CONF_TVOC = "tvoc"
CONF_AQI = "aqi"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ENS161Component),
            cv.Optional(CONF_ECO2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_CARBON_DIOXIDE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TVOC): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_BILLION,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_VOLATILE_ORGANIC_COMPOUNDS_PARTS,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_AQI): sensor.sensor_schema(
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x52))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_ECO2 in config:
        sens = await sensor.new_sensor(config[CONF_ECO2])
        cg.add(var.set_eco2_sensor(sens))

    if CONF_TVOC in config:
        sens = await sensor.new_sensor(config[CONF_TVOC])
        cg.add(var.set_tvoc_sensor(sens))

    if CONF_AQI in config:
        sens = await sensor.new_sensor(config[CONF_AQI])
        cg.add(var.set_aqi_sensor(sens))
```

**components/ens161/ens161.h**
```cpp
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ens161 {

class ENS161Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_eco2_sensor(sensor::Sensor *eco2_sensor) { eco2_sensor_ = eco2_sensor; }
  void set_tvoc_sensor(sensor::Sensor *tvoc_sensor) { tvoc_sensor_ = tvoc_sensor; }
  void set_aqi_sensor(sensor::Sensor *aqi_sensor) { aqi_sensor_ = aqi_sensor; }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  sensor::Sensor *eco2_sensor_{nullptr};
  sensor::Sensor *tvoc_sensor_{nullptr};
  sensor::Sensor *aqi_sensor_{nullptr};
};

}  // namespace ens161
}  // namespace esphome
```

**components/ens161/ens161.cpp**
```cpp
#include "ens161.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace ens161 {

static const char *const TAG = "ens161";

static const uint8_t ENS16x_PART_ID = 0x00;
static const uint8_t ENS16x_OPMODE = 0x10;
static const uint8_t ENS16x_TEMP_IN = 0x13;
static const uint8_t ENS16x_RH_IN = 0x15;
static const uint8_t ENS16x_DATA_STATUS = 0x20;
static const uint8_t ENS16x_DATA_AQI = 0x21;
static const uint8_t ENS16x_DATA_TVOC = 0x22;
static const uint8_t ENS16x_DATA_ECO2 = 0x24;

static const uint8_t ENS16x_OPMODE_IDLE = 0x01;
static const uint8_t ENS16x_OPMODE_STANDARD = 0x02;

void ENS161Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ENS161...");
  
  // Add a small delay for sensor power-up
  delayMicroseconds(100000);  // 100ms
  
  uint8_t part_id[2];
  if (!this->read_bytes(ENS16x_PART_ID, part_id, 2)) {
    ESP_LOGE(TAG, "Failed to read Part ID! Check I2C wiring and address.");
    ESP_LOGE(TAG, "Make sure sensor is at address 0x52 or 0x53");
    this->mark_failed();
    return;
  }
  
  uint16_t id = (part_id[1] << 8) | part_id[0];
  ESP_LOGCONFIG(TAG, "Part ID: 0x%04X", id);
  
  // Verify it's an ENS160/161 (Part ID should be 0x0160 or 0x0161)
  if (id != 0x0160 && id != 0x0161) {
    ESP_LOGE(TAG, "Invalid Part ID! Expected 0x0160 or 0x0161, got 0x%04X", id);
    this->mark_failed();
    return;
  }
  
  // Set to idle mode first
  if (!this->write_byte(ENS16x_OPMODE, ENS16x_OPMODE_IDLE)) {
    ESP_LOGE(TAG, "Failed to set idle mode");
    this->mark_failed();
    return;
  }
  delayMicroseconds(20000);  // 20ms delay
  
  // Set temperature compensation (24 degrees C)
  uint16_t temp_comp = 24 * 64;
  this->write_byte(ENS16x_TEMP_IN, temp_comp & 0xFF);
  this->write_byte(ENS16x_TEMP_IN + 1, (temp_comp >> 8) & 0xFF);
  
  // Set humidity compensation (40 percent)
  uint16_t hum_comp = 40 * 512;
  this->write_byte(ENS16x_RH_IN, hum_comp & 0xFF);
  this->write_byte(ENS16x_RH_IN + 1, (hum_comp >> 8) & 0xFF);
  
  // Set to standard operating mode
  if (!this->write_byte(ENS16x_OPMODE, ENS16x_OPMODE_STANDARD)) {
    ESP_LOGE(TAG, "Failed to set standard mode");
    this->mark_failed();
    return;
  }
  delayMicroseconds(20000);  // 20ms delay
  
  ESP_LOGCONFIG(TAG, "ENS161 initialized successfully");
}

void ENS161Component::update() {
  uint8_t status;
  if (!this->read_byte(ENS16x_DATA_STATUS, &status)) {
    ESP_LOGW(TAG, "Failed to read status");
    this->status_set_warning();
    return;
  }
  
  if (!(status & 0x02)) {
    ESP_LOGD(TAG, "No new data available (status: 0x%02X)", status);
    return;
  }
  
  uint8_t aqi;
  if (this->read_byte(ENS16x_DATA_AQI, &aqi) && this->aqi_sensor_ != nullptr) {
    ESP_LOGD(TAG, "AQI: %d", aqi);
    this->aqi_sensor_->publish_state(aqi);
  }
  
  uint8_t tvoc_data[2];
  if (this->read_bytes(ENS16x_DATA_TVOC, tvoc_data, 2) && this->tvoc_sensor_ != nullptr) {
    uint16_t tvoc = (tvoc_data[1] << 8) | tvoc_data[0];
    ESP_LOGD(TAG, "TVOC: %d ppb", tvoc);
    this->tvoc_sensor_->publish_state(tvoc);
  }
  
  uint8_t eco2_data[2];
  if (this->read_bytes(ENS16x_DATA_ECO2, eco2_data, 2) && this->eco2_sensor_ != nullptr) {
    uint16_t eco2 = (eco2_data[1] << 8) | eco2_data[0];
    ESP_LOGD(TAG, "eCO2: %d ppm", eco2);
    this->eco2_sensor_->publish_state(eco2);
  }
  
  this->status_clear_warning();
}

void ENS161Component::dump_config() {
  ESP_LOGCONFIG(TAG, "ENS161:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "eCO2", this->eco2_sensor_);
  LOG_SENSOR("  ", "TVOC", this->tvoc_sensor_);
  LOG_SENSOR("  ", "AQI", this->aqi_sensor_);
}

}  // namespace ens161
}  // namespace esphome
```

---

## Sensor Metrics

### PMSA003I - Particulate Matter Sensor

**Concentration Measurements (μg/m³):**
- **PM1.0**: Particles ≤1.0 micrometers
- **PM2.5**: Particles ≤2.5 micrometers (most health-relevant)
- **PM10**: Particles ≤10 micrometers

**Particle Counts (per 0.1L of air):**
- **PM0.3 Count**: Particles >0.3μm
- **PM0.5 Count**: Particles >0.5μm
- **PM1.0 Count**: Particles >1.0μm
- **PM2.5 Count**: Particles >2.5μm
- **PM5.0 Count**: Particles >5.0μm
- **PM10 Count**: Particles >10μm

**Update Frequency**: Every 60 seconds

### ENS161 - Air Quality Sensor

**Measurements:**
- **eCO2** (Equivalent CO₂): 400-65000 ppm
  - Estimated CO₂ level based on VOC readings
  - Good: <1000 ppm, Moderate: 1000-2000 ppm, Poor: >2000 ppm
  
- **TVOC** (Total Volatile Organic Compounds): 0-65000 ppb
  - Measures various organic chemicals in the air
  - Good: <220 ppb, Moderate: 220-660 ppb, Poor: >660 ppb
  
- **AQI** (Air Quality Index): 1-5
  - 1: Excellent
  - 2: Good
  - 3: Moderate
  - 4: Poor
  - 5: Unhealthy

**Update Frequency**: Every 15 seconds with 5-point moving average filter

**Environmental Compensation:**
- Temperature: Set to 24°C (configurable in code)
- Humidity: Set to 40% RH (configurable in code)

---

## Installation Steps

### 1. Physical Assembly

1. Connect all wires according to the wiring diagram
2. Double-check all connections, especially power (3.3V vs 5V)
3. Ensure common ground connection
4. Keep I2C wires short and away from power cables

### 2. ESPHome Setup

1. Install ESPHome add-on in Home Assistant (if not already installed)
2. Navigate to ESPHome dashboard
3. Create the file structure in `/config/esphome/`
4. Copy all configuration files to appropriate locations
5. Update WiFi credentials in `secrets.yaml`:
   ```yaml
   wifi_ssid: "Your_WiFi_Name"
   wifi_password: "Your_WiFi_Password"
   ```

### 3. Upload Firmware

1. Connect ESP32 to computer via USB
2. In ESPHome dashboard, click "Install"
3. Select "Plug into this computer"
4. Wait for compilation and upload (first time takes 5-10 minutes)
5. Disconnect USB and power via external power supply

### 4. Verification

After upload, check the logs for:

```
[I][i2c.idf:111]: Results from bus scan:
[I][i2c.idf:117]: Found device at address 0x53  <- ENS161
[I][i2c.idf:117]: Found device at address 0x12  <- PMSA003I
[I][ens161:087]: ENS161 initialized successfully
```

---

## Troubleshooting

### Sensor Not Detected

**Problem**: I2C device not found
**Solutions**:
1. Check wiring connections
2. Verify power supply (3.3V for ENS161)
3. Measure voltage at sensor VIN pin
4. Try swapping SDA/SCL wires (in case of mislabeling)
5. Add external pull-up resistors (4.7kΩ)

### Wrong I2C Address

**Problem**: Sensor found at different address
**Solutions**:
1. Check I2C bus scan results in logs
2. Update `address:` in YAML to match detected address
3. Common addresses: ENS161 (0x52 or 0x53), PMSA003I (0x12)

### Compilation Errors

**Problem**: ESPHome won't compile
**Solutions**:
1. Verify all component files exist in correct locations
2. Check for syntax errors in YAML (use YAML validator)
3. Ensure proper indentation (2 spaces, no tabs)
4. Clear build cache: delete `.esphome/` folder

### No Sensor Data

**Problem**: Sensors detected but no readings
**Solutions**:
1. Wait 1-2 minutes for sensors to warm up
2. Check logs for "No new data available" messages
3. Verify sensors are powered (check voltage)
4. Increase `update_interval` if updates too frequent
5. For ENS161, wait up to 5 minutes for initial calibration

### Unstable Readings

**Problem**: Sensor values jump erratically
**Solutions**:
1. Check power supply stability (use capacitor if needed)
2. Shorten I2C wires
3. Add/verify pull-up resistors
4. Lower I2C frequency to 100kHz
5. Increase moving average filter window size

---

## Understanding the Data

### Air Quality Guidelines

**PM2.5 Levels (μg/m³):**
- 0-12: Good
- 12-35: Moderate
- 35-55: Unhealthy for sensitive groups
- 55-150: Unhealthy
- 150+: Very unhealthy

**eCO₂ Levels (ppm):**
- 400-1000: Excellent
- 1000-2000: Good (typical indoor)
- 2000-5000: Poor ventilation
- 5000+: Potential health concerns

**TVOC Levels (ppb):**
- 0-220: Good
- 220-660: Moderate
- 660-2200: Poor
- 2200+: Very poor

### Sensor Behavior

**PMSA003I:**
- Laser-based sensor, requires airflow
- May show elevated readings after disturbance
- Best for continuous monitoring
- Values stabilize after 30 seconds

**ENS161:**
- Metal oxide sensor with adaptive algorithms
- Requires 1-hour warm-up for best accuracy
- Self-calibrates over 24-48 hours
- Temperature/humidity compensation improves accuracy

---

## Home Assistant Integration

Once the device is online, it will automatically appear in Home Assistant:

### Entity Names
- `sensor.eco2` - Equivalent CO₂ (ppm)
- `sensor.tvoc` - Total VOC (ppb)
- `sensor.aqi` - Air Quality Index (1-5)
- `sensor.pm1_0` - PM1.0 (μg/m³)
- `sensor.pm2_5` - PM2.5 (μg/m³)
- `sensor.pm10` - PM10 (μg/m³)
- `sensor.pm0_3_count` - Particle count >0.3μm
- `sensor.pm0_5_count` - Particle count >0.5μm
- `sensor.pm1_0_count` - Particle count >1.0μm
- `sensor.pm2_5_count` - Particle count >2.5μm
- `sensor.pm5_0_count` - Particle count >5.0μm
- `sensor.pm10_count` - Particle count >10μm

### Dashboard Card Example

```yaml
type: entities
title: Air Quality Monitor
entities:
  - entity: sensor.aqi
    name: Air Quality Index
  - entity: sensor.pm2_5
    name: PM2.5
  - entity: sensor.eco2
    name: CO₂ (equivalent)
  - entity: sensor.tvoc
    name: VOCs
```

---

## Maintenance

### Regular Maintenance
- Clean PMSA003I optical chamber every 3-6 months
- Check wiring connections quarterly
- Monitor for firmware updates in ESPHome

### Calibration
- ENS161 self-calibrates automatically
- Place in clean air environment for 24 hours for best baseline
- PMSA003I is factory calibrated, no user calibration needed

### Expected Lifespan
- PMSA003I laser: ~3 years continuous operation
- ENS161 sensor: 5+ years typical
- ESP32: 10+ years

---

## Advanced Configuration

### Adding Temperature/Humidity Compensation

If you have a separate temperature/humidity sensor, you can improve ENS161 accuracy:

```yaml
sensor:
  - platform: dht
    pin: GPIO4
    temperature:
      name: "Temperature"
      id: ambient_temp
    humidity:
      name: "Humidity"
      id: ambient_humidity
```

Then modify `ens161.cpp` to use these values instead of the hardcoded 24°C/40% RH.

### Customizing Update Intervals

Adjust based on your needs:
- Battery powered: Increase to 5-15 minutes
- Real-time monitoring: Decrease to 10-30 seconds
- Balance: 60 seconds (current setting)

### Adding Alerts

Create automations in Home Assistant:

```yaml
automation:
  - alias: "High PM2.5 Alert"
    trigger:
      - platform: numeric_state
        entity_id: sensor.pm2_5
        above: 35
    action:
      - service: notify.mobile_app
        data:
          message: "Poor air quality detected! PM2.5: {{ states('sensor.pm2_5') }}"
```

---

## Technical Specifications

### I2C Configuration
- **Bus A**: GPIO21 (SDA), GPIO22 (SCL) @ 400kHz
- **Bus B**: GPIO25 (SDA), GPIO26 (SCL) @ 400kHz
- **Pull-up resistors**: Usually 4.7kΩ (on breakout boards)

### Power Consumption
- ESP32: ~80mA active, ~10mA deep sleep
- PMSA003I: ~100mA active (laser on)
- ENS161: ~2mA active
- **Total**: ~180mA @ 5V (900mW)

### Communication Protocol
- I2C at 400kHz (can be reduced to 100kHz if needed)
- 7-bit addressing
- No clock stretching required

---

## Resources

### Documentation Links
- [ESPHome Documentation](https://esphome.io/)
- [PMSA003I Datasheet](https://www.plantower.com/)
- [ENS160/161 Datasheet](https://www.sciosense.com/)
- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)

### Support
- ESPHome Discord: [https://discord.gg/KhAMKrd](https://discord.gg/KhAMKrd)
- Home Assistant Community: [https://community.home-assistant.io/](https://community.home-assistant.io/)

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2025-01-15 | Initial documentation |

---

## License

This documentation is provided as-is for personal use. Hardware and software components may have their own licenses.
