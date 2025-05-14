#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace pca9505 {

class PCA9505Component : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void loop() override;
  bool digital_read(uint8_t pin);
  void digital_write(uint8_t pin, bool value);
  void pin_mode(uint8_t pin, gpio::Flags flags);

  float get_setup_priority() const override;

  float get_loop_priority() const override;

  void dump_config() override;

 protected:
  bool read_inputs_();

  bool write_register_(uint8_t reg, uint64_t value);

  /// The pin config - 1 means OUTPUT, 0 means INPUT
  uint64_t config_{0x00};
  /// The output state - 1 means HIGH, 0 means LOW
  uint64_t output_bits_{0x00};
  /// The input pin states - 1 means HIGH, 0 means LOW
  uint64_t input_bits_{0x00};
  /// Flags to check if read previously during this loop
  uint64_t was_previously_read_ = {0x00};
  /// Storage for last I2C error seen
  esphome::i2c::ErrorCode last_error_;
};

class PCA9505GPIOPin : public GPIOPin {
 public:
  void setup() override;
  void pin_mode(gpio::Flags flags) override;
  bool digital_read() override;
  void digital_write(bool value) override;
  std::string dump_summary() const override;

  void set_parent(PCA9505Component *parent) { this->parent_ = parent; }
  void set_pin(uint8_t pin) { this->pin_ = pin; }
  void set_inverted(bool inverted) { this->inverted_ = inverted; }
  void set_flags(gpio::Flags flags) { this->flags_ = flags; }

  gpio::Flags get_flags() const override { return this->flags_; }

 protected:
  PCA9505Component *parent_;
  uint8_t pin_;
  bool inverted_;
  gpio::Flags flags_;
};

}  // namespace pca9505
}  // namespace esphome
