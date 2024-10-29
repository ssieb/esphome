#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

namespace esphome {
namespace lp5562 {

class LP5562 : public Component, public i2c::I2CDevice {
 public:
  void dump_config() override;
  void setup() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
};

}  // namespace lp5562
}  // namespace esphome
