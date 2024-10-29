#include "lp5562.h"

#include "esphome/core/log.h"

namespace esphome {
namespace lp5562 {

static const char *const TAG = "lp5562";

void LP5562::dump_config() {
  ESP_LOGCONFIG(TAG, "LP5562:");
  LOG_I2C_DEVICE(this);
}

}  // namespace lp5562
}  // namespace esphome
