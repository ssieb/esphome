#pragma once
#include "esphome/core/automation.h"
#include "ezo.h"

namespace esphome {
namespace ezo {

class LedTrigger : public Trigger<bool> {
 public:
  explicit LedTrigger(EZOSensor *ezo) {
    ezo->add_led_state_callback([this](bool value) { this->trigger(value); });
  }
};

class TTrigger : public Trigger<std::string> {
 public:
  explicit TTrigger(EZOSensor *ezo) {
    ezo->add_calibration_callback([this](std::string value) { this->trigger(value); });
  }
};

class CalibrationTrigger : public Trigger<std::string> {
 public:
  explicit CalibrationTrigger(EZOSensor *ezo) {
    ezo->add_calibration_callback([this](std::string value) { this->trigger(value); });
  }
};

class SlopeTrigger : public Trigger<std::string> {
 public:
  explicit SlopeTrigger(EZOSensor *ezo) {
    ezo->add_slope_callback([this](std::string value) { this->trigger(value); });
  }
};

class DeviceInformationTrigger : public Trigger<std::string> {
 public:
  explicit DeviceInformationTrigger(EZOSensor *ezo) {
    ezo->add_device_infomation_callback([this](std::string value) { this->trigger(value); });
  }
};

}  // namespace ezo
}  // namespace esphome
