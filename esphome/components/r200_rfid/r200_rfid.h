#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/uart/uart.h"

#include <vector>

namespace esphome {
namespace r200 {

class R200BinarySensor;
class R200Trigger;
class R200 : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; };


  void register_tag(R200BinarySensor *tag) { this->binary_sensors_.push_back(tag); }
  void register_ontag_trigger(R200Trigger *trig) { this->triggers_ontag_.push_back(trig); }
  void register_ontagremoved_trigger(R200Trigger *trig) { this->triggers_ontagremoved_.push_back(trig); }

 protected:
  std::vector<uint8_t> rx_data_;
  int state_{0};
  uint8_t type_;
  uint8_t command_;
  uint8_t csum_;
  uint16_t datalen_;
  uint32_t last_rx_time_{0};

  void handle_data_();
  void send_command_(uint8_t cmd, uint8_t *data, uint16_t len);
  void get_module_info_();
  void send_poll_();

  std::vector<R200BinarySensor *> binary_sensors_;
  std::vector<R200Trigger *> triggers_ontag_;
  std::vector<R200Trigger *> triggers_ontagremoved_;
  std::vector<uint8_t> current_uid_;
};

class R200BinarySensor : public binary_sensor::BinarySensor {
 public:
  void set_uid(const std::vector<uint8_t> &uid) { uid_ = uid; }

  bool process(std::vector<uint8_t> &data);

  void on_scan_end() {
    if (!this->found_) {
      this->publish_state(false);
    }
    this->found_ = false;
  }

 protected:
  std::vector<uint8_t> uid_;
  bool found_{false};
};

class R200Trigger : public Trigger<std::string> {
 public:
  void process(std::vector<uint8_t> &data);
};

}  // namespace r200
}  // namespace esphome
