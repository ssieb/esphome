#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/binary_output.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/light/light_effect.h"

namespace esphome
{
  namespace jandy_watercolors
  {

    class JandyWatercolorsEffect : public light::LightEffect
    {
    public:
      explicit JandyWatercolorsEffect(const std::string &name, int effect_position, uint32_t light_on_duration, uint32_t light_off_duration) : LightEffect(name)
      {
        this->effect_position_ = effect_position;
        this->light_on_duration_ = light_on_duration;
        this->light_off_duration_ = light_off_duration;
      }

      void apply() override;

    protected:
      bool on_ = false;
      int effect_position_ = 0;
      uint32_t light_on_duration_ = 0;
      uint32_t light_off_duration_ = 0;

      uint32_t last_color_change_{0};
    };

    class JandyWatercolorsLightOutput : public light::LightOutput
    {
    public:
      void setup_state(light::LightState *state)
      {
        for (int i = 0; i < sizeof(this->current_effect_); i++)
        {
          state->add_effects({new JandyWatercolorsEffect(this->effect_names[i], i, 1000, 1100)});
        }

        state->add_effects({new JandyWatercolorsEffect("Reset", -1, 1000, 5000)});
      }

      void set_output(output::BinaryOutput *output) { output_ = output; }

      light::LightTraits get_traits() override
      {
        auto traits = light::LightTraits();
        traits.set_supported_color_modes({light::ColorMode::ON_OFF});
        return traits;
      }

      void write_state(light::LightState *state) override
      {
        bool should_be_on;
        state->current_values_as_binary(&should_be_on);

        const uint32_t now = millis();

        if (should_be_on)
        {
          this->output_->turn_on();

          auto last_off_duration = now - this->last_turned_off_;

          if (last_off_duration < 3000)
          {
            this->current_effect_++;

            // Jandy Lights support a set number of effects. Since our current effect position starts
            // at zero, anything above the length of the effect array is out of bounds and the light would have wrapped around
            if (this->current_effect_ >= sizeof(this->effect_names))
            {
              this->current_effect_ = 0;
            }

            // If not applying an effect, set the effect for this color
            if (!this->is_applying_effect())
            {
              auto call = state->turn_on();
              call.set_effect(this->effect_names[this->current_effect_]);
              call.perform();
            }
          }
          else if (last_off_duration && now - this->last_turned_off_ < 5000)
          {
            // Light was turned on before 5 seconds have elapsed after off, so
            // the light is reset to effect 0;
            this->current_effect_ = 0;

            // Set the effect for this color
            auto call = state->turn_on();
            call.set_effect(this->effect_names[0]);
            call.perform();
          }
        }
        else
        {
          this->last_turned_off_ = now;
          this->output_->turn_off();
        }
      }

      int get_current_effect_position()
      {
        return this->current_effect_;
      }

      void set_is_applying_effect(bool is_applying_effect)
      {
        this->is_applying_effect_ = is_applying_effect;
      }

      bool is_applying_effect()
      {
        return this->is_applying_effect_;
      }

    protected:
      output::BinaryOutput *output_;
      int current_effect_;
      bool is_applying_effect_;
      uint32_t last_turned_off_ = 0;
      std::string effect_names[14] = {"Alpine White",
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
                                      "Disco Tech"};
    };
  } // namespace jandy_watercolors
} // namespace esphome