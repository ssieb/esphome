#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ltc2483 {

class LTC2483 : public sensor::Sensor, public i2c::I2CDevice, public PollingComponent {
 public:
  void update() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_vref(float vref) { this->vref_ = vref; }

 protected:
  float vref_;
  bool updating_{false};
  uint32_t update_start_{0};
};

}  // namespace ltc2483
}  // namespace esphome
