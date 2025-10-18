#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/fan/fan.h"

namespace esphome::template_ {

class TemplateFan final : public Component, public fan::Fan {
 public:
  TemplateFan() {}
  void setup() override;
  void dump_config() override;
  void set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }
  void set_has_direction(bool has_direction) { this->has_direction_ = has_direction; }
  void set_has_oscillating(bool has_oscillating) { this->has_oscillating_ = has_oscillating; }
  void set_speed_count(int count) { this->speed_count_ = count; }
  void set_preset_modes(std::initializer_list<const char *> presets) { this->set_supported_preset_modes(presets); }
  fan::FanTraits get_traits() override {
    this->wire_preset_modes_(this->traits_);
    return this->traits_;
  }
  Trigger<> *get_turn_on_trigger() const { return this->turn_on_trigger_; };
  Trigger<> *get_turn_off_trigger() const { return this->turn_off_trigger_; };
  Trigger<int> *get_speed_trigger() const { return this->speed_trigger_; };
  Trigger<fan::FanDirection> *get_direction_trigger() const { return this->direction_trigger_; };

 protected:
  void control(const fan::FanCall &call) override;

  bool optimistic_{true};
  bool has_oscillating_{false};
  bool has_direction_{false};
  int speed_count_{0};
  Trigger<> *turn_on_trigger_ = new Trigger<>();
  Trigger<> *turn_off_trigger_ = new Trigger<>();
  Trigger<int> *speed_trigger_ = new Trigger<int>();
  Trigger<fan::FanDirection> *direction_trigger_ = new Trigger<fan::FanDirection>();
  fan::FanTraits traits_;
};

}  // namespace esphome::template_
