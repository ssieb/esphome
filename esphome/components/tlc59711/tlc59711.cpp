#include "tlc59711.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tlc59711 {

static const char *const TAG = "tlc59711";

void TLC59711::setup() {
  this->data_pin_->setup();
  this->data_pin_->digital_write(false);
  this->clock_pin_->setup();
  this->clock_pin_->digital_write(false);

  this->pwm_amounts_.resize(this->num_chips_ * N_CHANNELS_PER_CHIP, 0);
}
void TLC59711::dump_config() {
  ESP_LOGCONFIG(TAG, "TLC59711:");
  LOG_PIN("  Data Pin: ", this->data_pin_);
  LOG_PIN("  Clock Pin: ", this->clock_pin_);
  ESP_LOGCONFIG(TAG, "  Number of chips: %u", this->num_chips_);
}

void TLC59711::loop() {
  if (!this->update_)
    return;

  for (int chip = 0; chip < this->num_chips_; chip++) {
    // send the write command and settings and group brightness
    uint32_t data = (0x25 << 23) | (0b10010 << 18) | 0x3ffff;
    for (uint8_t bit = 0; bit < 28; bit++) {
      this->clock_pin_->digital_write(false);
      this->data_pin_->digital_write(data & 0x8000000);
      data <<= 1;
      this->clock_pin_->digital_write(true);
      this->clock_pin_->digital_write(true);  // TWH0>12ns, so we should be fine using this as delay
    }
    // push the data out, MSB first, 12 bit word per channel, 24 channels per chip
    for (int ch = N_CHANNELS_PER_CHIP - 1; ch >= 0; ch--) {
      uint16_t word = pwm_amounts_[ch];
      for (uint8_t bit = 0; bit < 16; bit++) {
        this->clock_pin_->digital_write(false);
        this->data_pin_->digital_write(word & 0x8000);
        word <<= 1;

        this->clock_pin_->digital_write(true);
        this->clock_pin_->digital_write(true);  // TWH0>12ns, so we should be fine using this as delay
      }
    }
  }

  this->clock_pin_->digital_write(false);

  this->update_ = false;
}

}  // namespace tlc59711
}  // namespace esphome
