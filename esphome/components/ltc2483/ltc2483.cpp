#include "ltc2483.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ltc2483 {

static const char *const TAG = "ltc2483";

void LTC2483::dump_config() {
  LOG_SENSOR("", "LTC2483 Sensor", this);
  ESP_LOGCONFIG(TAG, "  VRef: %f", this->vref_);
  LOG_UPDATE_INTERVAL(this);
}

float LTC2483::get_setup_priority() const { return setup_priority::DATA; }

void LTC2483::update() {
  uint8_t data[3];
  this->read(data, 3);  // discard old data to start new conversion
  this->updating_ = true;
  this->update_start_ = millis();
}

void LTC2483::loop() {
  if (!this->updating_)
    return;
  if (millis() - this->update_start_ < 200)
    return;
  uint8_t data[3];
  i2c::ErrorCode err = this->read(data, 3);
  if ((err == i2c::ERROR_NOT_ACKNOWLEDGED) || (err == i2c::ERROR_TIMEOUT))
    return;  // not ready
  this->updating_ = false;
  if (err != i2c::NO_ERROR) {
    ESP_LOGE(TAG, "Error reading data: %d", err);
    return;
  }
  ESP_LOGV(TAG, "raw data: %02x%02x%02x", data[0], data[1], data[2]);
  int32_t raw = (data[0] << 16) | (data[1] << 8) | data[2];
  if (((raw ^ (raw << 1)) & 0x800000) == 0) {
    if (raw & 0x800000) {
      ESP_LOGW(TAG, "voltage too high");
    } else {
      ESP_LOGW(TAG, "voltage too low");
    }
    this->publish_state(NAN);
    return;
  }
  raw ^= 0x8000000;
  if (raw & 0x800000)
    raw |= 0xff000000;  // sign-extend the value
  float value = float(raw) / 0x400000 * this->vref_;
  ESP_LOGV(TAG, "Got voltage=%.4fV", value);
  this->publish_state(value);
}

}  // namespace ltc2483
}  // namespace esphome
