#include "template_fan.h"
#include "esphome/core/log.h"

namespace esphome::template_ {

static const char *const TAG = "template.fan";

void TemplateFan::setup() {
  // Construct traits before restore so preset modes can be looked up by index
  this->traits_ =
      fan::FanTraits(this->has_oscillating_, this->speed_count_ > 0, this->has_direction_, this->speed_count_);

  auto restore = this->restore_state_();
  if (restore.has_value()) {
    restore->apply(*this);
  }
}

void TemplateFan::dump_config() { LOG_FAN("", "Template Fan", this); }

void TemplateFan::control(const fan::FanCall &call) {
  auto call_state = call.get_state();
  if (call_state.has_value()) {
    bool state = *call_state;
    if (this->optimistic_)
      this->state = state;
    if (state) {
      this->turn_on_trigger_->trigger();
    } else {
      this->turn_off_trigger_->trigger();
    }
  }
  auto call_speed = call.get_speed();
  if (call_speed.has_value() && (this->speed_count_ > 0)) {
    int speed = *call_speed;
    if (this->optimistic_)
      this->speed = speed;
    this->speed_trigger_->trigger(speed);
  }
  auto call_oscillating = call.get_oscillating();
  if (call_oscillating.has_value() && this->has_oscillating_) {
    bool oscillating = *call_oscillating;
    if (this->optimistic_)
      this->oscillating = oscillating;
  }
  auto call_direction = call.get_direction();
  if (call_direction.has_value() && this->has_direction_) {
    fan::FanDirection direction = *call_direction;
    if (this->optimistic_)
      this->direction = direction;
  }
  if (this->optimistic_) {
    this->apply_preset_mode_(call);
    this->publish_state();
  }
}

}  // namespace esphome::template_
