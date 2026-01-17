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
static const uint8_t ENS16x_DATA_HCHO = 0x26;
static const uint8_t ENS16x_DATA_HP0 = 0x28;
static const uint8_t ENS16x_DATA_HP1 = 0x2A;
static const uint8_t ENS16x_DATA_HP2 = 0x2C;
static const uint8_t ENS16x_DATA_HP3 = 0x2E;

static const uint8_t ENS16x_OPMODE_IDLE = 0x01;
static const uint8_t ENS16x_OPMODE_STANDARD = 0x02;

void ENS161Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ENS161...");
  
  delayMicroseconds(100000);
  
  uint8_t part_id[2];
  if (!this->read_bytes(ENS16x_PART_ID, part_id, 2)) {
    ESP_LOGE(TAG, "Failed to read Part ID! Check I2C wiring and address.");
    ESP_LOGE(TAG, "Make sure sensor is at address 0x52 or 0x53");
    this->mark_failed();
    return;
  }
  
  uint16_t id = (part_id[1] << 8) | part_id[0];
  ESP_LOGCONFIG(TAG, "Part ID: 0x%04X", id);
  
  if (id != 0x0160 && id != 0x0161) {
    ESP_LOGE(TAG, "Invalid Part ID! Expected 0x0160 or 0x0161, got 0x%04X", id);
    this->mark_failed();
    return;
  }
  
  if (!this->write_byte(ENS16x_OPMODE, ENS16x_OPMODE_IDLE)) {
    ESP_LOGE(TAG, "Failed to set idle mode");
    this->mark_failed();
    return;
  }
  delayMicroseconds(20000);
  
  uint16_t temp_comp = 24 * 64;
  this->write_byte(ENS16x_TEMP_IN, temp_comp & 0xFF);
  this->write_byte(ENS16x_TEMP_IN + 1, (temp_comp >> 8) & 0xFF);
  
  uint16_t hum_comp = 40 * 512;
  this->write_byte(ENS16x_RH_IN, hum_comp & 0xFF);
  this->write_byte(ENS16x_RH_IN + 1, (hum_comp >> 8) & 0xFF);
  
  if (!this->write_byte(ENS16x_OPMODE, ENS16x_OPMODE_STANDARD)) {
    ESP_LOGE(TAG, "Failed to set standard mode");
    this->mark_failed();
    return;
  }
  delayMicroseconds(20000);
  
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
  
  uint8_t hcho_data[2];
  if (this->read_bytes(ENS16x_DATA_HCHO, hcho_data, 2) && this->hcho_sensor_ != nullptr) {
    uint16_t hcho = (hcho_data[1] << 8) | hcho_data[0];
    float hcho_ppb = hcho / 10.0;
    ESP_LOGD(TAG, "HCHO: %.2f ppb", hcho_ppb);
    this->hcho_sensor_->publish_state(hcho_ppb);
  }
  
  uint8_t hp0_data[2];
  if (this->read_bytes(ENS16x_DATA_HP0, hp0_data, 2) && this->hp0_sensor_ != nullptr) {
    uint16_t hp0 = (hp0_data[1] << 8) | hp0_data[0];
    this->hp0_sensor_->publish_state(hp0);
  }
  
  uint8_t hp1_data[2];
  if (this->read_bytes(ENS16x_DATA_HP1, hp1_data, 2) && this->hp1_sensor_ != nullptr) {
    uint16_t hp1 = (hp1_data[1] << 8) | hp1_data[0];
    this->hp1_sensor_->publish_state(hp1);
  }
  
  uint8_t hp2_data[2];
  if (this->read_bytes(ENS16x_DATA_HP2, hp2_data, 2) && this->hp2_sensor_ != nullptr) {
    uint16_t hp2 = (hp2_data[1] << 8) | hp2_data[0];
    this->hp2_sensor_->publish_state(hp2);
  }
  
  uint8_t hp3_data[2];
  if (this->read_bytes(ENS16x_DATA_HP3, hp3_data, 2) && this->hp3_sensor_ != nullptr) {
    uint16_t hp3 = (hp3_data[1] << 8) | hp3_data[0];
    this->hp3_sensor_->publish_state(hp3);
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
  LOG_SENSOR("  ", "HCHO", this->hcho_sensor_);
  LOG_SENSOR("  ", "HP0 Resistance", this->hp0_sensor_);
  LOG_SENSOR("  ", "HP1 Resistance", this->hp1_sensor_);
  LOG_SENSOR("  ", "HP2 Resistance", this->hp2_sensor_);
  LOG_SENSOR("  ", "HP3 Resistance", this->hp3_sensor_);
}

}  // namespace ens161
}  // namespace esphome
