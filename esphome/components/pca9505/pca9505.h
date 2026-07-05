#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/gpio_expander/cached_gpio.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome::pca9505 {

class PCA9505Component : public Component,
                         public i2c::I2CDevice,
                         public gpio_expander::CachedGpioExpander<uint64_t, 64> {
 public:
  PCA9505Component() = default;
  void set_interrupt_pin(InternalGPIOPin *pin) { this->interrupt_pin_ = pin; }

  void setup() override;
  void loop() override;
  void pin_mode(uint8_t pin, gpio::Flags flags);

  float get_setup_priority() const override;

  void dump_config() override;

 protected:
  static void IRAM_ATTR gpio_intr(PCA9505Component *arg);

  bool digital_read_hw(uint8_t pin) override;
  bool digital_read_cache(uint8_t pin) override;
  void digital_write_hw(uint8_t pin, bool value) override;

  bool read_inputs_();

  bool write_register_(uint8_t reg, uint64_t value);

  /// The pin config - 1 means OUTPUT, 0 means INPUT
  uint64_t config_{0x00};
  /// The output state - 1 means HIGH, 0 means LOW
  uint64_t output_bits_{0x00};
  /// The input pin states - 1 means HIGH, 0 means LOW
  uint64_t input_bits_{0x00};
  InternalGPIOPin *interrupt_pin_{nullptr};
};

class PCA9505GPIOPin : public GPIOPin {
 public:
  void setup() override;
  void pin_mode(gpio::Flags flags) override;
  bool digital_read() override;
  void digital_write(bool value) override;
  size_t dump_summary(char *buffer, size_t len) const override;

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

}  // namespace esphome::pca9505
