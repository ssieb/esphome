#include "ezo.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace ezo {

static const char *const TAG = "ezo.sensor";

static const uint16_t EZO_STATE_WAIT = 1;
static const uint16_t EZO_STATE_SEND_TEMP = 2;
static const uint16_t EZO_STATE_WAIT_TEMP = 4;

void EZOSensor::dump_config() {
  LOG_SENSOR("", "EZO", this);
  LOG_I2C_DEVICE(this);
  if (this->is_failed())
    ESP_LOGE(TAG, "Communication with EZO circuit failed!");
  LOG_UPDATE_INTERVAL(this);
}

void EZOSensor::update() {
  if (!this->commands_.empty()) {
    ESP_LOGE(TAG, "update overrun, still waiting for previous response");  // Not sure if we care to log
    return;
  }

  this->get_state();
}

void EZOSensor::loop() {
  if (this->commands_.empty()) {
    return;
  }

  ezo_command *to_run = this->commands_.front();

  if (!to_run->command_sent) {
    auto data = reinterpret_cast<const uint8_t *>(&to_run->command.c_str()[0]);
    for (int i = 0; i < to_run->command.length(); i++) {
      ESP_LOGD(TAG, "Sending command index: %d char: \"%c\" hex: 0x%02X", i, data[i], data[i]);
    }

    this->write_bytes_raw(data, to_run->command.length());

    this->start_time_ = millis();
    to_run->command_sent = true;
    return;
  }

  if (millis() - this->start_time_ < to_run->delay_ms)
    return;

  uint8_t buf[20];

  buf[0] = 0;

  if (!this->read_bytes_raw(buf, 20)) {
    ESP_LOGE(TAG, "read error");
    delete this->commands_.front();
    this->commands_.pop_front();
    return;
  }

  switch (buf[0]) {
    case 1:
      break;
    case 2:
      ESP_LOGE(TAG, "device returned a syntax error");
      break;
    case 254:
      return;  // keep waiting
    case 255:
      ESP_LOGE(TAG, "device returned no data");
      break;
    default:
      ESP_LOGE(TAG, "device returned an unknown response: %d", buf[0]);
      break;
  }
  ezo_command *to_run = this->commands_.front();

  ESP_LOGD(TAG, "Received buffer \"%s\" for command type %s", buf, EzoCommandTypeStrings[to_run->command_type]);
  if (buf[0] == 1) {
    switch (to_run->command_type) {
      case EZO_COMMAND_TYPE::EZO_READ: {
        // some sensors return multiple comma-separated values, terminate string after first one
        for (size_t i = 1; i < sizeof(buf) - 1; i++) {
          if (buf[i] == ',') {
            buf[i] = '\0';
	    break;
	  }
        }
        float val = parse_number<float>((char *) &buf[1]).value_or(0);
        float val = atof((char *) &buf[1]);
        this->publish_state(val);
        break;
      }
      case EZO_COMMAND_TYPE::EZO_LED: {
        break;
      }
    }
  }

  delete this->commands_.front();
  this->commands_.pop_front();
}

// LED control
void EZOSensor::set_led_state(bool on) { this->add_command("L," + on ? "1" : "0", EZO_COMMAND_TYPE::EZO_LED); }

void EZOSensor::set_tempcomp_value(float temp) {}

}  // namespace ezo
}  // namespace esphome
