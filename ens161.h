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
