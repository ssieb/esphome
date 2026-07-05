#include "pca9505.h"
#include "esphome/core/log.h"

namespace esphome::pca9505 {

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

  if (this->interrupt_pin_ != nullptr) {
    this->interrupt_pin_->setup();
    this->interrupt_pin_->attach_interrupt(&PCA9505Component::gpio_intr, this, gpio::INTERRUPT_FALLING_EDGE);
    // Don't invalidate cache on read — only invalidate when interrupt fires
    this->set_invalidate_on_read_(false);
  }
  // Disable loop until an input pin is configured via pin_mode()
  // For interrupt-driven mode, loop is re-enabled by the ISR
  // For polling mode, loop is re-enabled when pin_mode() registers an input pin
  this->disable_loop();
}

void IRAM_ATTR PCA9505Component::gpio_intr(PCA9505Component *arg) { arg->enable_loop_soon_any_context(); }

void PCA9505Component::loop() {
  // Invalidate the cache so the next digital_read() triggers a fresh I2C read
  this->reset_pin_cache_();
  // Only disable the loop once INT has actually gone HIGH. Input transitions that straddle the
  // I2C read leave INT asserted without re-firing a falling edge, which would strand us with
  // stale state forever; keep looping until the line is released so we self-heal.
  if ((this->interrupt_pin_ != nullptr) && this->interrupt_pin_->digital_read())
    this->disable_loop();
}

void PCA9505Component::dump_config() {
  ESP_LOGCONFIG(TAG, "PCA9505:");
  LOG_PIN("  Interrupt Pin: ", this->interrupt_pin_);
  LOG_I2C_DEVICE(this)
  if (this->is_failed())
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
}

bool PCA9505Component::digital_read_hw(uint8_t pin) {
  // Read all pins from hardware
  return this->read_inputs_();  // Return true if I2C read succeeded, false on error
}

bool PCA9505Component::digital_read_cache(uint8_t pin) { return this->input_bits_ & (1 << pin); }

void PCA9505Component::digital_write_hw(uint8_t pin, bool value) {
  if (value) {
    this->output_bits_ |= (1ull << pin);
  } else {
    this->output_bits_ &= ~(1ull << pin);
  }
  this->write_register_(OUTPUT_REG, this->output_bits_);
}

void PCA9505Component::pin_mode(uint8_t pin, gpio::Flags flags) {
  if (flags & gpio::FLAG_INPUT) {
    // Clear mode bit
    this->config_ &= ~(1ull << pin);
    // Enable polling loop for input pins if no interrupt pin
    if (this->interrupt_pin_ == nullptr)
      this->enable_loop();
  } else if (flags & gpio::FLAG_OUTPUT) {
    // Set mode bit
    this->config_ |= 1ull << pin;
  }
  this->write_register_(CONFIG_REG, ~this->config_);
}

bool PCA9505Component::read_inputs_() {
  if (this->is_failed())
    return false;

  i2c::ErrorCode err = this->read_register(INPUT_REG, (uint8_t *) &this->input_bits_, 5);
  if (err != i2c::ERROR_OK) {
    this->status_set_warning("read register failed");
    return false;
  }
  this->status_clear_warning();
  return true;
}

bool PCA9505Component::write_register_(uint8_t reg, uint64_t value) {
  if (this->is_failed())
    return false;

  i2c::ErrorCode err = this->write_register(reg, (uint8_t *) &value, 5);
  if (err != i2c::ERROR_OK) {
    this->status_set_warning("write register failed");
    return false;
  }

  this->status_clear_warning();
  return true;
}

float PCA9505Component::get_setup_priority() const { return setup_priority::IO; }

void PCA9505GPIOPin::setup() { pin_mode(flags_); }

void PCA9505GPIOPin::pin_mode(gpio::Flags flags) { this->parent_->pin_mode(this->pin_, flags); }

bool PCA9505GPIOPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }

void PCA9505GPIOPin::digital_write(bool value) { this->parent_->digital_write(this->pin_, value != this->inverted_); }

size_t PCA9505GPIOPin::dump_summary(char *buffer, size_t len) const {
  return buf_append_printf(buffer, len, 0, "%u via PCA9505", this->pin_);
}

}  // namespace esphome::pca9505
