#include "pca9505.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pca9505 {

// include the auto-increment bit
const uint8_t INPUT_REG = 0x80;
const uint8_t OUTPUT_REG = 0x88;
const uint8_t INVERT_REG = 0x90;
const uint8_t CONFIG_REG = 0x98;

static const char *const TAG = "pca9505";

void PCA9505Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PCA9505...");
  // Test to see if device exists
  if (!this->read_inputs_()) {
    ESP_LOGE(TAG, "PCA9505 not detected at 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  // No polarity inversion
  this->write_register_(INVERT_REG, 0);
  // All inputs at initialization
  this->config_ = 0;
  // Invert mask as a 1 means an input
  this->write_register_(CONFIG_REG, ~this->config_);
  // All outputs low
  this->output_bits_ = 0;
  this->write_register_(OUTPUT_REG, this->output_bits_);
}

void PCA9505Component::loop() {
  // The read_inputs_() method will cache the input values from the chip.
  this->read_inputs_();
}

void PCA9505Component::dump_config() {
  ESP_LOGCONFIG(TAG, "PCA9505:");
  LOG_I2C_DEVICE(this)
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with PCA9505 failed!");
  }
}

bool PCA9505Component::digital_read(uint8_t pin) {
  // Note: We want to try and avoid doing any I2C bus read transactions here
  // to conserve I2C bus bandwidth. So what we do is check to see if we
  // have seen a read during the time esphome is running this loop. If we have,
  // we do an I2C bus transaction to get the latest value. If we haven't
  // we return a cached value which was read at the time loop() was called.
  uint64_t mask = 1ul << pin;
  if (this->was_previously_read_ & mask)
    this->read_inputs_();  // Force a read of a new value
  // Indicate we saw a read request for this pin in case a
  // read happens later in the same loop.
  this->was_previously_read_ |= mask;
  return this->input_bits_ & mask;
}

void PCA9505Component::digital_write(uint8_t pin, bool value) {
  if (value) {
    this->output_bits_ |= (1ul << pin);
  } else {
    this->output_bits_ &= ~(1ul << pin);
  }
  this->write_register_(OUTPUT_REG, this->output_bits_);
}

void PCA9505Component::pin_mode(uint8_t pin, gpio::Flags flags) {
  if (flags & gpio::FLAG_INPUT) {
    // Clear mode bit
    this->config_ &= ~(1ul << pin);
  } else if (flags & gpio::FLAG_OUTPUT) {
    // Set mode bit
    this->config_ |= 1ul << pin;
  }
  this->write_register_(CONFIG_REG, ~this->config_);
}

bool PCA9505Component::read_inputs_() {
  if (this->is_failed())
    return false;

  this->last_error_ = this->read_register(INPUT_REG, (uint8_t *) &this->input_bits_, 5, true);
  if (this->last_error_ != i2c::ERROR_OK) {
    this->status_set_warning("read register failed");
    return false;
  }
  this->status_clear_warning();
  // Clear all the previously read flags.
  this->was_previously_read_ = 0;
  return true;
}

bool PCA9505Component::write_register_(uint8_t reg, uint64_t value) {
  if (this->is_failed())
    return false;

  this->last_error_ = this->write_register(reg, (uint8_t *) &value, 5, true);
  if (this->last_error_ != i2c::ERROR_OK) {
    this->status_set_warning("write register failed");
    return false;
  }

  this->status_clear_warning();
  return true;
}

float PCA9505Component::get_setup_priority() const { return setup_priority::IO; }

// Run our loop() method very early in the loop, so that we cache read values before
// other components call our digital_read() method.
float PCA9505Component::get_loop_priority() const { return 9.0f; }  // Just after WIFI

void PCA9505GPIOPin::setup() { pin_mode(flags_); }

void PCA9505GPIOPin::pin_mode(gpio::Flags flags) { this->parent_->pin_mode(this->pin_, flags); }

bool PCA9505GPIOPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }

void PCA9505GPIOPin::digital_write(bool value) { this->parent_->digital_write(this->pin_, value != this->inverted_); }

std::string PCA9505GPIOPin::dump_summary() const { return str_sprintf("%u via PCA9505", pin_); }

}  // namespace pca9505
}  // namespace esphome
