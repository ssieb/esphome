#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/binary_output.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/light/light_effect.h"

namespace esphome {
namespace jandy_watercolors {

class JandyWatercolorsLightOutput;

class JandyWatercolorsEffect : public light::LightEffect {
 public:
  explicit JandyWatercolorsEffect(const std::string &name, int effect_position, JandyWatercolorsLightOutput *parent) : LightEffect(name) {
    this->effect_position_ = effect_position;
    this->parent_ = parent;
  }

  void apply() override;

 protected:
  int effect_position_ = 0;
  JandyWatercolorsLightOutput *parent_;
};

class JandyWatercolorsLightOutput : public light::LightOutput, public Component {
 public:
  void loop() override;

  void setup_state(light::LightState *state) override;

  void set_output(output::BinaryOutput *output) { output_ = output; }

  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::ON_OFF});
    return traits;
  }

  void write_state(light::LightState *state) override;

 protected:
  friend JandyWatercolorsEffect;

  void set_effect_(int effect_num);

  output::BinaryOutput *output_;
  int current_effect_{0};
  int target_effect_{0};
  bool current_state_{false};
  bool state_{false};
  uint32_t last_turned_off_ = 0;
  uint32_t last_change_;
  uint32_t delay_;
};

} // namespace jandy_watercolors
} // namespace esphome
