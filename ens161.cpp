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
  
  uint8_t part_id[2];
  if (!this->read_bytes(ENS16x_PART_ID, part_id, 2)) {
    ESP_LOGE(TAG, "Failed to read Part ID!");
    this->mark_failed();
    return;
  }
  
  uint16_t id = (part_id[1] << 8) | part_id[0];
  ESP_LOGCONFIG(TAG, "Part ID: 0x%04X", id);
  
  // Set to idle mode first
  this->write_byte(ENS16x_OPMODE, ENS16x_OPMODE_IDLE);
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
  this->write_byte(ENS16x_OPMODE, ENS16x_OPMODE_STANDARD);
  delayMicroseconds(20000);  // 20ms delay
  
  ESP_LOGCONFIG(TAG, "ENS161 initialized successfully");
}

void ENS161Component::update() {
  uint8_t status;
  if (!this->read_byte(ENS16x_DATA_STATUS, &status)) {
    ESP_LOGW(TAG, "Failed to read status");
    return;
  }
  
  if (!(status & 0x02)) {
    ESP_LOGD(TAG, "No new data available");
    return;
  }
  
  uint8_t aqi;
  if (this->read_byte(ENS16x_DATA_AQI, &aqi) && this->aqi_sensor_ != nullptr) {
    this->aqi_sensor_->publish_state(aqi);
  }
  
  uint8_t tvoc_data[2];
  if (this->read_bytes(ENS16x_DATA_TVOC, tvoc_data, 2) && this->tvoc_sensor_ != nullptr) {
    uint16_t tvoc = (tvoc_data[1] << 8) | tvoc_data[0];
    this->tvoc_sensor_->publish_state(tvoc);
  }
  
  uint8_t eco2_data[2];
  if (this->read_bytes(ENS16x_DATA_ECO2, eco2_data, 2) && this->eco2_sensor_ != nullptr) {
    uint16_t eco2 = (eco2_data[1] << 8) | eco2_data[0];
    this->eco2_sensor_->publish_state(eco2);
  }
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
