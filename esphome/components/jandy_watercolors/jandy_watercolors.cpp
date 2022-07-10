#include "esphome/core/component.h"
#include "esphome/components/output/binary_output.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/light/light_effect.h"
#include "jandy_watercolors.h"

namespace esphome
{
  namespace jandy_watercolors
  {

      void JandyWatercolorsEffect::apply() 
      {
        const uint32_t now = millis();

        int time_to_wait = this->on_ ? this->light_on_duration_ : this->light_off_duration_;

        // If we should not update yet, return;
        if (now - this->last_color_change_ < time_to_wait)
        {
          return;
        }

        auto output = this->state_->get_output();
        esphome::light::LightOutput *bleh = output;

        auto www = static_cast<JandyWatercolorsLightOutput&>(output);

        // // If we are already at the right effect, return;
        // if (output->get_current_effect_position() == this->effect_position_)
        // {
        //   output->set_is_applying_effect(false);
        //   return;
        // }

        // output->set_is_applying_effect(true);

        auto call = this->state_->turn_on();
        float out = this->on_ ? 0.0 : 1.0;
        call.set_brightness_if_supported(out);
        this->on_ = !this->on_;
        // don't tell HA every change
        call.set_publish(false);
        call.set_save(false);
        call.perform();

        this->last_color_change_ = now;
      }
  } // namespace jandy_watercolors
} // namespace esphome