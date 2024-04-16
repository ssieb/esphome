#pragma once

#include "esphome/core/automation.h"
#include "daly_bms.h"

#include <vector>

namespace esphome {
namespace daly_bms {

class DalyOnFailureStatus : public Trigger<const std::vector<uint8_t> &> {
 public:
  explicit DalyOnFailureStatus(DalyBmsComponent *daly) {
    daly->add_failure_callback([this](const std::vector<uint8_t> &data) { this->trigger(data); });
  }
};

}  // namespace daly_bms
}  // namespace esphome

