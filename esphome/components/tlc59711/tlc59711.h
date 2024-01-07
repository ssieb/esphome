#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/output/float_output.h"
#include <vector>

namespace esphome {
namespace tlc59711 {

class TLC59711 : public Component {
 public:
  class Channel;

  const uint8_t N_CHANNELS_PER_CHIP = 12;

  void set_data_pin(GPIOPin *data_pin) { data_pin_ = data_pin; }
  void set_clock_pin(GPIOPin *clock_pin) { clock_pin_ = clock_pin; }
  void set_num_chips(uint8_t num_chips) { num_chips_ = num_chips; }

  void setup() override;

  void dump_config() override;

  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  void loop() override;

  class Channel : public output::FloatOutput {
   public:
    void set_parent(TLC59711 *parent) { parent_ = parent; }
    void set_channel(uint8_t channel) { channel_ = channel; }

   protected:
    void write_state(float state) override {
      uint16_t amount = state * 0xffff;
      this->parent_->set_channel_value_(this->channel_, amount);
    }

    TLC59711 *parent_;
    uint8_t channel_;
  };

 protected:
  void set_channel_value_(uint16_t channel, uint16_t value) {
    if (channel >= this->num_chips_ * N_CHANNELS_PER_CHIP)
      return;
    if (this->pwm_amounts_[channel] != value) {
      this->update_ = true;
    }
    this->pwm_amounts_[channel] = value;
  }

  GPIOPin *data_pin_;
  GPIOPin *clock_pin_;
  uint8_t num_chips_;

  std::vector<uint16_t> pwm_amounts_;
  bool update_{true};
};

}  // namespace tlc59711
}  // namespace esphome
