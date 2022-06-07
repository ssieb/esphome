#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace cdl210 {

class CDL210Component : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void update() override;
  void loop() override;
  void dump_config() override;

  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_co2_sensor(sensor::Sensor *co2_sensor) { co2_sensor_ = co2_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }

 protected:
  bool cdl210_write_command_(const uint8_t *command, uint8_t *response);
  std::vector<uint8_t> rx_message_;
  std::string data;
  void handle_char_(uint8_t c, std::string *s);

  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *co2_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
};

}  // namespace cdl210
}  // namespace esphome
