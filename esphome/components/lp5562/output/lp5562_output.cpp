#include "lp5562_output.h"

#include "esphome/core/log.h"

namespace esphome {
namespace lp5562 {

static const char *const TAG = "lp5562.output";

static const uint8_t PWM_REGISTERS[] = {4, 3, 2, 0xe};

void LP5562Output::dump_config() {
  ESP_LOGCONFIG(TAG, "LP5562 Output:");
  ESP_LOGCONFIG(TAG, "  Channel: %u", this->channel_);
}

void LP5562Output::write_state(float state) {
  uint8_t value = state * 255;
  i2c::ErrorCode err = this->parent_->write_register(PWM_REGISTERS[this->channel_], &value, 1);
  if (err != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Error writing to LP5562, code %d", err);
  }
}

}  // namespace lp5562
}  // namespace esphome
