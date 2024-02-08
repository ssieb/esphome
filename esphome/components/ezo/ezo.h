#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include <deque>

namespace esphome {
namespace ezo {

enum EzoCommandType : uint8_t {
  EZO_READ = 0,
  EZO_LED,
  EZO_DEVICE_INFORMATION,
  EZO_SLOPE,
  EZO_CALIBRATION,
  EZO_SLEEP,
  EZO_I2C,
  EZO_T,
  EZO_CUSTOM,
  EZO_INTERNAL
};

enum EzoCalibrationType : uint8_t { EZO_CAL_LOW = 0, EZO_CAL_MID = 1, EZO_CAL_HIGH = 2 };

class EzoCommand {
 public:
  std::string command;
  uint16_t delay_ms = 0;
  bool command_sent = false;
  bool internal = false;
  std::function<void(std::string)> &&callback{nullptr};
  EzoCommandType command_type;
};

/// This class implements support for the EZO circuits in i2c mode
class EZOSensor : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void loop() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; };

  // I2C
  void set_address(uint8_t address);

  // Device Information
  void get_device_information();
  void add_device_infomation_callback(std::function<void(std::string)> &&callback) {
    this->device_infomation_callback_.add(std::move(callback));
  }

  // Sleep
  void set_sleep();

  // R
  void get_state();

  // Slope
  void get_slope();
  void add_slope_callback(std::function<void(std::string)> &&callback) {
    this->slope_callback_.add(std::move(callback));
  }

  // T
  void get_t();
  void set_t(float value);
  void set_tempcomp_value(float temp);  // For backwards compatibility
  void add_t_callback(std::function<void(std::string)> &&callback) { this->t_callback_.add(std::move(callback)); }

  // Calibration
  void get_calibration();
  void set_calibration_point_low(float value);
  void set_calibration_point_mid(float value);
  void set_calibration_point_high(float value);
  void set_calibration_generic(float value);
  void clear_calibration();
  void add_calibration_callback(std::function<void(std::string)> &&callback) {
    this->calibration_callback_.add(std::move(callback));
  }

  // LED
  void get_led_state();
  void set_led_state(bool on);
  void add_led_state_callback(std::function<void(bool)> &&callback) { this->led_callback_.add(std::move(callback)); }

  // Custom
  void send_custom(const std::string &to_send);
  void add_custom_callback(std::function<void(std::string)> &&callback) {
    this->custom_callback_.add(std::move(callback));
  }

 protected:
  void dump_common_();
  virtual void handle_data_() = 0;
  std::deque<std::unique_ptr<EzoCommand>> commands_;
  int new_address_;
  std::vector<float> data_;
  std::string device_name_;
  std::string version_;

  void add_command_(const std::string &command, EzoCommandType command_type, uint16_t delay_ms = 300, std::function<void(std::string)> &&callback = nullptr);
  void send_internal_(const std::string &to_send, std::function<void(std::string)> &&callback = nullptr);

  void set_calibration_point_(EzoCalibrationType type, float value);

  CallbackManager<void(std::string)> device_infomation_callback_{};
  CallbackManager<void(std::string)> calibration_callback_{};
  CallbackManager<void(std::string)> slope_callback_{};
  CallbackManager<void(std::string)> t_callback_{};
  CallbackManager<void(std::string)> custom_callback_{};
  CallbackManager<void(bool)> led_callback_{};

  uint32_t start_time_ = 0;
};

class EZOSensorSingle : public EZOSensor, public sensor::Sensor {
 public:
  void dump_config() override;
 protected:
  void handle_data_();
};

class EZOSensorMulti : public EZOSensor {
 public:
  void set_sensors(std::vector<sensor::Sensor *> sensors) { this->sensors_ = std::move(sensors); }
 protected:
  void handle_data_();
  std::vector<sensor::Sensor *> sensors_;
};

class EZOSensorDO : public EZOSensor {
 public:
  void set_mg_sensor(sensor::Sensor * sensor) { this->mg_sensor_ = sensor; }
  void set_percent_sensor(sensor::Sensor * sensor) { this->percent_sensor_ = sensor; }
 protected:
  void handle_data_();
  sensor::Sensor *mg_sensor_{nullptr};
  sensor::Sensor *percent_sensor_{nullptr};
};

class EZOSensorEC : public EZOSensor {
 public:
  void set_conductivity_sensor(sensor::Sensor * sensor) { this->conductivity_sensor_ = sensor; }
  void set_tds_sensor(sensor::Sensor * sensor) { this->tds_sensor_ = sensor; }
  void set_salinity_sensor(sensor::Sensor * sensor) { this->salinity_sensor_ = sensor; }
  void set_specific_gravity_sensor(sensor::Sensor * sensor) { this->specific_gravity_sensor_ = sensor; }
 protected:
  void handle_data_();
  sensor::Sensor *conductivity_sensor_{nullptr};
  sensor::Sensor *tds_sensor_{nullptr};
  sensor::Sensor *salinity_sensor_{nullptr};
  sensor::Sensor *specific_gravity_sensor_{nullptr};
};

class EZOSensorFLO : public EZOSensor {
 public:
  void setup() override;
  void set_flow_rate_sensor(sensor::Sensor * sensor, uint8_t period) {
    this->flow_rate_sensor_ = sensor;
    this->flow_rate_period_ = period;
  }
  void set_total_volume_sensor(sensor::Sensor * sensor) { this->total_volume_sensor_ = sensor; }
 protected:
  void handle_data_();
  sensor::Sensor *flow_rate_sensor_{nullptr};
  sensor::Sensor *total_volume_sensor_{nullptr};
  uint8_t flow_rate_period_;
};

}  // namespace ezo
}  // namespace esphome
