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
  void set_hcho_sensor(sensor::Sensor *hcho_sensor) { hcho_sensor_ = hcho_sensor; }
  void set_hp0_sensor(sensor::Sensor *hp0_sensor) { hp0_sensor_ = hp0_sensor; }
  void set_hp1_sensor(sensor::Sensor *hp1_sensor) { hp1_sensor_ = hp1_sensor; }
  void set_hp2_sensor(sensor::Sensor *hp2_sensor) { hp2_sensor_ = hp2_sensor; }
  void set_hp3_sensor(sensor::Sensor *hp3_sensor) { hp3_sensor_ = hp3_sensor; }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  sensor::Sensor *eco2_sensor_{nullptr};
  sensor::Sensor *tvoc_sensor_{nullptr};
  sensor::Sensor *aqi_sensor_{nullptr};
  sensor::Sensor *hcho_sensor_{nullptr};
  sensor::Sensor *hp0_sensor_{nullptr};
  sensor::Sensor *hp1_sensor_{nullptr};
  sensor::Sensor *hp2_sensor_{nullptr};
  sensor::Sensor *hp3_sensor_{nullptr};
};

}  // namespace ens161
}  // namespace esphome
