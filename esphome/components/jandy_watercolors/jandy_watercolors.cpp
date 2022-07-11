#include "esphome/core/component.h"
#include "esphome/components/output/binary_output.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/light/light_effect.h"
#include "jandy_watercolors.h"

static const int NUM_EFFECTS = 14;
static const std::string effect_names[NUM_EFFECTS] = {
  "Alpine White",
  "Sky Blue",
  "Cobalt Blue",
  "Caribbean Blue",
  "Spring Green",
  "Emerald Green",
  "Emerald Rose",
  "Magenta",
  "Violet",
  "Slow Color Splash",
  "Fast Color Splash",
  "America the Beautiful",
  "Fat Tuesday",
  "Disco Tech"
};


namespace esphome {
namespace jandy_watercolors {

void JandyWatercolorsEffect::apply() {
  this->parent_->set_effect_(this->effect_position_);
}

void JandyWatercolorsLightOutput::setup_state(light::LightState *state) {
  std::vector<JandyWatercolorsEffect *> effects(NUM_EFFECTS);
  for (int i = 0; i < NUM_EFFECTS; i++)
    effects.push_back(new JandyWatercolorsEffect(effect_names[i], i));
  state->add_effects(&effects);
}

void JandyWatercolorsLightOutput::loop() {
  if (!this->state_)
    return;
  const uint32_t now = millis();
  const uint32_t elapsed = now - this->last_change_;
  if (elapsed < this->delay_)
    return;
  if (!this->current_state_) {
    this->output_->turn_on();
    this->current_state_ = true;
    this->last_change_ = now;
    this->delay_ = 1000;
    return;
  }
  if (this->current_effect_ == this->target_effect_)
    return;
  if (this->target_effect_ < this->current_effect_) {
    this->current_effect_ = 0;
    this->delay_ = 4500;
  } else {
    this->current_effect_++;
    this->delay_ = 1000;
  }
  this->output_->turn_off();
  this->current_state_ = false;
  this->last_change_ = now;
}

void JandyWatercolorsLightOutput::write_state(light::LightState *state) {
  bool should_be_on;
  state->current_values_as_binary(&should_be_on);
  this->state_ = should_be_on;

  const uint32_t now = millis();

  if (should_be_on) {
    this->output_->turn_on();

    auto last_off_duration = now - this->last_turned_off_;

    if (last_off_duration < 3000) {
      // The current effect will have been incremented
      this->current_effect_ = (this->current_effect_ + 1) % NUM_EFFECTS;
    } else if (last_off_duration < 5000) {
      // Light was turned on before 5 seconds have elapsed after off, so
      // the light is reset to effect 0;
      this->current_effect_ = 0;
    }
    this->start_effect_(this->current_effect_ + 1);
    this->publish_state();
    this->current_state_ = true;
    this->last_change_ = now;
  } else {
    this->last_turned_off_ = now;
    this->output_->turn_off();
  }
}

} // namespace jandy_watercolors
} // namespace esphome
