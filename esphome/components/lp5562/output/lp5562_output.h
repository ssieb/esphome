#pragma once

#include "esphome/components/output/float_output.h"
#include "esphome/core/component.h"

#include "esphome/components/lp5562/lp5562.h"

namespace esphome {
namespace lp5562 {

class LP5562Output : public Component, public output::FloatOutput, public Parented<LP5562> {
 public:
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA - 1; }

  void set_channel(uint8_t channel) { this->channel_ = channel; }

  void write_state(float state) override;

 protected:
  uint8_t channel_;
};

}  // namespace lp5562
}  // namespace esphome
