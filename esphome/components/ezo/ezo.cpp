#include "ezo.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace ezo {

static const char *const TAG = "ezo.sensor";

static const char *const EZO_COMMAND_TYPE_STRINGS[] = {"EZO_READ",  "EZO_LED",         "EZO_DEVICE_INFORMATION",
                                                       "EZO_SLOPE", "EZO_CALIBRATION", "EZO_SLEEP",
                                                       "EZO_I2C",   "EZO_T",           "EZO_CUSTOM",
                                                       "EZO_INTERNAL"};

static const char *const EZO_CALIBRATION_TYPE_STRINGS[] = {"LOW", "MID", "HIGH"};

void EZOSensor::dump_common_() {
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "EZO device '%s', version %s", this->device_name_.c_str(), this->version_.c_str());
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with EZO circuit failed!");
  }
  LOG_UPDATE_INTERVAL(this);
}

void EZOSensor::setup() {
  this->send_internal_("i", [this](std::string payload) {
    auto start_location = payload.find(',', 3);
    this->device_name_ = payload.substr(3, start_location - 3);
    this->version_ = payload.substr(start_location + 1);
  });
}

void EZOSensor::update() {
  for (auto &cmd : this->commands_) {
    if (cmd->command_type == EzoCommandType::EZO_READ) {
      ESP_LOGW(TAG, "read already pending at update interval");
      return;
    }
  }
  this->get_state();
}

void EZOSensor::loop() {
  if (this->commands_.empty())
    return;

  EzoCommand *to_run = this->commands_.front().get();

  if (!to_run->command_sent) {
    const uint8_t *data = reinterpret_cast<const uint8_t *>(to_run->command.c_str());
    ESP_LOGV(TAG, "Sending command \"%s\"", data);

    this->write(data, to_run->command.length());

    if (to_run->command_type == EzoCommandType::EZO_SLEEP ||
        to_run->command_type == EzoCommandType::EZO_I2C) {  // Commands with no return data
      this->commands_.pop_front();
      if (to_run->command_type == EzoCommandType::EZO_I2C)
        this->address_ = this->new_address_;
      return;
    }

    this->start_time_ = millis();
    to_run->command_sent = true;
    return;
  }

  if (millis() - this->start_time_ < to_run->delay_ms)
    return;

  uint8_t buf[32];
  buf[0] = 0;

  if (!this->read_bytes_raw(buf, 32)) {
    ESP_LOGE(TAG, "read error");
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

  ESP_LOGV(TAG, "Received buffer \"%s\" for command type %s", &buf[1], EZO_COMMAND_TYPE_STRINGS[to_run->command_type]);
  this->commands_.pop_front();

  if (buf[0] != 1)
    return;

  std::string payload = reinterpret_cast<char *>(&buf[1]);

  if (to_run->callback != nullptr) {
    to_run->callback(payload);
    return;
  }

  if (payload.empty())
    return;

  switch (to_run->command_type) {
    case EzoCommandType::EZO_READ: {
      // some sensors return multiple comma-separated values, terminate string after first one
      int start_location = 0;
      if ((start_location = payload.find(',')) != std::string::npos) {
        payload.erase(start_location);
      }
      auto val = parse_number<float>(payload);
      if (!val.has_value()) {
        ESP_LOGW(TAG, "Can't convert '%s' to number!", payload.c_str());
      } else {
        //this->publish_state(*val);
      }
      break;
    }
    case EzoCommandType::EZO_LED: {
      this->led_callback_.call(payload.back() == '1');
      break;
    }
    case EzoCommandType::EZO_DEVICE_INFORMATION: {
      this->device_infomation_callback_.call(payload.substr(3));
      break;
    }
    case EzoCommandType::EZO_SLOPE: {
      int start_location = 0;
      if ((start_location = payload.find(',')) != std::string::npos) {
        this->slope_callback_.call(payload.substr(start_location + 1));
      }
      break;
    }
    case EzoCommandType::EZO_CALIBRATION: {
      int start_location = 0;
      if ((start_location = payload.find(',')) != std::string::npos) {
        this->calibration_callback_.call(payload.substr(start_location + 1));
      }
      break;
    }
    case EzoCommandType::EZO_T: {
      int start_location = 0;
      if ((start_location = payload.find(',')) != std::string::npos) {
        this->t_callback_.call(payload.substr(start_location + 1));
      }
      break;
    }
    case EzoCommandType::EZO_CUSTOM: {
      this->custom_callback_.call(payload);
      break;
    }
    default: {
      break;
    }
  }
}

void EZOSensor::add_command_(const std::string &command, EzoCommandType command_type, uint16_t delay_ms, std::function<void(std::string)> &&callback) {
  std::unique_ptr<EzoCommand> ezo_command(new EzoCommand);
  ezo_command->command = command;
  ezo_command->command_type = command_type;
  ezo_command->delay_ms = delay_ms;
  ezo_command->callback = std::move(callback);
  this->commands_.push_back(std::move(ezo_command));
};

void EZOSensor::set_calibration_point_(EzoCalibrationType type, float value) {
  std::string payload = str_sprintf("Cal,%s,%0.2f", EZO_CALIBRATION_TYPE_STRINGS[type], value);
  this->add_command_(payload, EzoCommandType::EZO_CALIBRATION, 900);
}

void EZOSensor::set_address(uint8_t address) {
  if (address > 0 && address < 128) {
    std::string payload = str_sprintf("I2C,%u", address);
    this->new_address_ = address;
    this->add_command_(payload, EzoCommandType::EZO_I2C);
  } else {
    ESP_LOGE(TAG, "Invalid I2C address");
  }
}

void EZOSensor::get_device_information() { this->add_command_("i", EzoCommandType::EZO_DEVICE_INFORMATION); }

void EZOSensor::set_sleep() { this->add_command_("Sleep", EzoCommandType::EZO_SLEEP); }

void EZOSensor::get_state() { this->add_command_("R", EzoCommandType::EZO_READ, 900); }

void EZOSensor::get_slope() { this->add_command_("Slope,?", EzoCommandType::EZO_SLOPE); }

void EZOSensor::get_t() { this->add_command_("T,?", EzoCommandType::EZO_T); }

void EZOSensor::set_t(float value) {
  std::string payload = str_sprintf("T,%0.2f", value);
  this->add_command_(payload, EzoCommandType::EZO_T);
}

void EZOSensor::set_tempcomp_value(float temp) { this->set_t(temp); }

void EZOSensor::get_calibration() { this->add_command_("Cal,?", EzoCommandType::EZO_CALIBRATION); }

void EZOSensor::set_calibration_point_low(float value) {
  this->set_calibration_point_(EzoCalibrationType::EZO_CAL_LOW, value);
}

void EZOSensor::set_calibration_point_mid(float value) {
  this->set_calibration_point_(EzoCalibrationType::EZO_CAL_MID, value);
}

void EZOSensor::set_calibration_point_high(float value) {
  this->set_calibration_point_(EzoCalibrationType::EZO_CAL_HIGH, value);
}

void EZOSensor::set_calibration_generic(float value) {
  std::string payload = str_sprintf("Cal,%0.2f", value);
  this->add_command_(payload, EzoCommandType::EZO_CALIBRATION, 900);
}

void EZOSensor::clear_calibration() { this->add_command_("Cal,clear", EzoCommandType::EZO_CALIBRATION); }

void EZOSensor::get_led_state() { this->add_command_("L,?", EzoCommandType::EZO_LED); }

void EZOSensor::set_led_state(bool on) {
  std::string to_send = "L,";
  to_send += on ? "1" : "0";
  this->add_command_(to_send, EzoCommandType::EZO_LED);
}

void EZOSensor::send_custom(const std::string &to_send) { this->add_command_(to_send, EzoCommandType::EZO_CUSTOM); }

void EZOSensor::send_internal_(const std::string &to_send, std::function<void(std::string)> &&callback) {
  this->add_command_(to_send, EzoCommandType::EZO_INTERNAL, 300, std::move(callback));
}

void EZOSensorSingle::dump_config() {
  LOG_SENSOR("", "EZO", this);
  this->dump_common_();
}

void EZOSensorSingle::handle_data_() {
  this->publish_state(this->data_[0]);
}

void EZOSensorMulti::handle_data_() {
  int n = std::min(this->data_.size(), this->sensors_.size());
  if (n < this->sensors_.size())
    ESP_LOGW(TAG, "received only %d/%d values", n, this->sensors_.size());
  for (int i = 0; i < n; i++)
    this->sensors_[i]->publish_state(this->data_[i]);
}

void EZOSensorDO::handle_data_() {
  if (this->data_.size() < 2) {
    ESP_LOGW(TAG, "received only %d/2 values, resetting data selection", this->data_.size());
    this->send_internal_("O,mg,1");
    this->send_internal_("O,%,1");
    return;
  }
  if (this->mg_sensor_ != nullptr)
    this->mg_sensor_->publish_state(this->data_[0]);
  if (this->percent_sensor_ != nullptr)
    this->percent_sensor_->publish_state(this->data_[1]);
}

void EZOSensorEC::handle_data_() {
  if (this->data_.size() < 4) {
    ESP_LOGW(TAG, "received only %d/4 values, resetting data selection", this->data_.size());
    this->send_internal_("O,EC,1");
    this->send_internal_("O,TDS,1");
    this->send_internal_("O,S,1");
    this->send_internal_("O,SG,1");
    return;
  }
  if (this->conductivity_sensor_ != nullptr)
    this->conductivity_sensor_->publish_state(this->data_[0]);
  if (this->tds_sensor_ != nullptr)
    this->tds_sensor_->publish_state(this->data_[1]);
  if (this->salinity_sensor_ != nullptr)
    this->salinity_sensor_->publish_state(this->data_[2]);
  if (this->specific_gravity_sensor_ != nullptr)
    this->specific_gravity_sensor_->publish_state(this->data_[3]);
}

void EZOSensorFLO::setup() {
}

void EZOSensorFLO::handle_data_() {
  if (this->data_.size() < 2) {
    ESP_LOGW(TAG, "received only %d/2 values, resetting data selection", this->data_.size());
    this->send_internal_("O,TV,1");
    this->send_internal_("O,FR,1");
    return;
  }
  if (this->total_volume_sensor_ != nullptr)
    this->total_volume_sensor_->publish_state(this->data_[0]);
  if (this->flow_rate_sensor_ != nullptr)
    this->flow_rate_sensor_->publish_state(this->data_[1]);
}

}  // namespace ezo
}  // namespace esphome
