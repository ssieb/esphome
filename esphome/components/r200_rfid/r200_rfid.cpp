#include "r200_rfid.h"
#include "esphome/core/log.h"

namespace esphome {
namespace r200 {

static const char *const TAG = "r200";

std::string format_buffer(uint8_t *b, uint8_t len) {
  char buf[32];
  int offset = 0;
  for (uint8_t i = 0; i < len; i++) {
    const char *format = "%02X";
    if (i + 1 < len)
      format = "%02X-";
    offset += sprintf(buf + offset, format, b[i]);
  }
  return std::string(buf);
}

std::string format_uid(std::vector<uint8_t> &uid) {
  char buf[32];
  int offset = 0;
  for (size_t i = 0; i < uid.size(); i++) {
    const char *format = "%02X";
    if (i + 1 < uid.size())
      format = "%02X-";
    offset += sprintf(buf + offset, format, uid[i]);
  }
  return std::string(buf);
}

void R200::setup() {
  this->get_module_info_();
}

void R200::dump_config() {
  ESP_LOGCONFIG(TAG, "R200:");
  LOG_UPDATE_INTERVAL(this);

  for (auto *child : this->binary_sensors_) {
    LOG_BINARY_SENSOR("  ", "Tag", child);
  }
}

void R200::update() {
  this->send_poll_();
}

void R200::loop() {
  uint32_t now = millis();
  if (!this->available()) {
    if ((this->state_ == 0) || (this->last_rx_time_ - now < 100))
      return;
    this->rx_data_.clear();
    this->state_ = 0;
    return;
  }

  this->last_rx_time_ = now;

  while (this->available()) {
    uint8_t c;
    this->read_byte(&c);
    switch (this->state_) {
     case 0:
      if (c == 0xaa)
        this->state_ = 1;
      break;
     case 1:
      if ((c == 0) || (c > 2)) {
        ESP_LOGE(TAG, "invalid message type %d", c);
        this->state_ = 0;
      } else {
        this->type_ = c;
        this->state_ = 2;
      }
      break;
     case 2:
      this->command_ = c;
      this->state_ = 3;
      break;
     case 3:
      this->datalen_ = c << 8;
      this->state_ = 4;
      break;
     case 4:
      this->datalen_ += c;
      if (this->datalen_ > 1000) {
        ESP_LOGE(TAG, "invalid data length: %d", this->datalen_);
        this->state_ = 0;
      } else {
        this->state_ = 5;
      }
      break;
     case 5:
      this->rx_data_.push_back(c);
      if (this->rx_data_.size() >= this->datalen_)
        this->state_ = 6;
      break;
     case 6:
      this->csum_ = c;
      this->state_ = 7;
      break;
     case 7:
      if (c != 0xdd)
        ESP_LOGE(TAG, "missing end value");
      this->handle_data_();
      this->state_ = 0;
      this->rx_data_.clear();
      break;
    }
  }
}

void R200::handle_data_() {
  uint8_t csum = this->type_ + this->command_ + (this->datalen_ >> 8) + (this->datalen_ & 0xff);
  for (uint8_t c : this->rx_data_)
    csum += c;
  if (csum != this->csum_) {
    ESP_LOGE(TAG, "invalid checksum: %02x != %02x", csum, this->csum_);
    return;
  }
  switch (this->command_) {
   case 0xff:
    switch (this->rx_data_[0]) {
     case 0x17:
      ESP_LOGE(TAG, "command error");
      break;
     case 0x09:
      ESP_LOGE(TAG, "read fail");
      break;
     case 0x15:
      ESP_LOGD(TAG, "no card detected");
      break;
     default:
      ESP_LOGE(TAG, "error: %02x", this->rx_data_[0]);
      break;
    }
    break;
   case 0x03:
    this->rx_data_.push_back(0);
    ESP_LOGD(TAG, "module info: %02x '%s'", this->rx_data_[0], &this->rx_data_[1]);
    break;
   case 0x22:
    {
    uint8_t *data = this->rx_data_.data();
    ESP_LOGD(TAG, "card detected: rssi %02x, PC %02x%02x, EPC %s", data[0], data[1], data[2], format_hex_pretty(&data[3], 12).c_str());
    break;
    }
   default:
    ESP_LOGD(TAG, "unhandled message: %02x", this->command_);
    break;
  }
}

void R200::send_command_(uint8_t cmd, uint8_t *data, uint16_t len) {
  this->write_byte(0xaa);
  this->write_byte(0);
  this->write_byte(cmd);
  this->write_byte(len >> 8);
  this->write_byte(len & 0xff);
  if (len)
    this->write_array(data, len);
  uint8_t csum = cmd + (len >> 8) + (len & 0xff);
  for (int i = 0; i < len; i++)
    csum += data[i];
  this->write_byte(csum);
  this->write_byte(0xdd);
}

void R200::get_module_info_() {
  uint8_t c = 0;
  this->send_command_(3, &c, 1);
}

void R200::send_poll_() {
  this->send_command_(0x22, nullptr, 0);
}

bool R200BinarySensor::process(std::vector<uint8_t> &data) {
}

void R200Trigger::process(std::vector<uint8_t> &data) { this->trigger(format_uid(data)); }

}  // namespace r200
}  // namespace esphome
