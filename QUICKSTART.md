# Quick Start Guide

Get your ESP32 Air Quality Monitor up and running in 10 minutes!

## Prerequisites

- ESP32 development board
- ENS161 air quality sensor
- PMSA003I particulate matter sensor
- Jumper wires and breadboard
- USB cable for ESP32
- Home Assistant with ESPHome add-on installed

## Step 1: Hardware Setup (5 minutes)

### Wiring

Connect sensors to ESP32 according to this table:

| Sensor | Pin | → | ESP32 |
|--------|-----|---|-------|
| ENS161 | VIN | → | 3.3V |
| ENS161 | GND | → | GND |
| ENS161 | SDI | → | GPIO21 |
| ENS161 | SDK | → | GPIO22 |
| PMSA003I | VCC | → | 3.3V or 5V* |
| PMSA003I | GND | → | GND |
| PMSA003I | SDA | → | GPIO25 |
| PMSA003I | SCL | → | GPIO26 |

*Check your PMSA003I module specifications

⚠️ **Important**: ENS161 requires 3.3V only - do NOT use 5V!

## Step 2: Software Setup (5 minutes)

### 1. Clone Repository

```bash
git clone https://github.com/kix1979/ens161.git
cd ens161
```

### 2. Copy Files to ESPHome

Copy files to your Home Assistant ESPHome directory:

```bash
# If using Home Assistant OS with Samba share:
# Copy esphome-web-0c1ee8.yaml to /config/esphome/
# Copy components/ folder to /config/esphome/components/
```

Or via SSH:
```bash
scp -r components root@homeassistant.local:/config/esphome/
scp esphome-web-0c1ee8.yaml root@homeassistant.local:/config/esphome/
```

### 3. Configure WiFi

Create `secrets.yaml` in `/config/esphome/`:

```yaml
wifi_ssid: "YourWiFiName"
wifi_password: "YourWiFiPassword"
```

### 4. Upload Firmware

1. Open Home Assistant → Settings → Add-ons → ESPHome
2. Click "Open Web UI"
3. You should see "esphome-web-0c1ee8"
4. Click the three dots → Install
5. Select "Plug into this computer"
6. Wait 5-10 minutes for first compilation

## Step 3: Verification

### Check Logs

After upload, click "Logs" to verify:

```
[I][i2c.idf:117]: Found device at address 0x53  ← ENS161
[I][i2c.idf:117]: Found device at address 0x12  ← PMSA003I
[I][ens161:XXX]: ENS161 initialized successfully
```

### Home Assistant Entities

Navigate to Settings → Devices & Services → ESPHome

You should see 13 new entities:
- sensor.eco2
- sensor.tvoc
- sensor.aqi
- sensor.formaldehyde
- sensor.pm1_0
- sensor.pm2_5
- sensor.pm10
- Plus 6 particle count sensors

## Troubleshooting

### Sensor Not Found

**Check I2C scan in logs:**
```
[I][i2c.idf:111]: Results from bus scan:
[I][i2c.idf:117]: Found device at address 0x??
```

If addresses don't match:
- 0x53 (ENS161) or 0x12 (PMSA003I)
- Update `address:` in YAML configuration

### No Data

- Wait 1-2 minutes for sensor warm-up
- ENS161 needs up to 1 hour for full calibration
- Check power connections (measure voltage at sensor pins)

### Compilation Errors

- Ensure all files are in correct locations
- Check YAML syntax (use validator)
- Delete `.esphome/` folder to clear cache

## Next Steps

### Create Dashboard

Add a card in Home Assistant:

```yaml
type: entities
title: Air Quality
entities:
  - entity: sensor.aqi
  - entity: sensor.pm2_5
  - entity: sensor.eco2
  - entity: sensor.tvoc
  - entity: sensor.formaldehyde
```

### Set Up Automations

Example: Alert on poor air quality

```yaml
automation:
  - alias: "Poor Air Quality Alert"
    trigger:
      - platform: numeric_state
        entity_id: sensor.pm2_5
        above: 35
    action:
      - service: notify.mobile_app
        data:
          message: "Poor air quality! PM2.5: {{ states('sensor.pm2_5') }}"
```

## Support

- **Issues**: https://github.com/kix1979/ens161/issues
- **Documentation**: See [DOCUMENTATION.md](DOCUMENTATION.md)
- **Wiring**: See [WIRING.md](WIRING.md)

## What's Next?

- ✅ Sensors working
- ⬜ Add display (ST7789 LCD)
- ⬜ Set up automations
- ⬜ Create custom dashboard
- ⬜ Add data logging

Happy monitoring! 🌿
