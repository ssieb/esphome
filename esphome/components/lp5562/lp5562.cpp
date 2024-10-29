#include "lp5562.h"

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace lp5562 {

static const char *const TAG = "lp5562";
static const uint8_t REG_ENABLE = 0;
static const uint8_t REG_CONFIG = 8;
static const uint8_t REG_LED_MAP = 0x70;

void LP5562::dump_config() {
  ESP_LOGCONFIG(TAG, "LP5562:");
  LOG_I2C_DEVICE(this);
}

void LP5562::setup() {
  uint8_t c = 0xc0;
  this->write_register(REG_ENABLE, &c, 1);
  delay(1);
  c = 0x43;
  this->write_register(REG_CONFIG, &c, 1);
  c = 0;
  this->write_register(REG_LED_MAP, &c, 1);
}

}  // namespace lp5562
}  // namespace esphome
