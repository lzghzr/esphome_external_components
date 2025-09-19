#pragma once

#include "esphome/components/switch/switch.h"
#include "../haier_ir_acyrh63.h"

namespace esphome {
namespace haier_ir_acyrh63 {

class HealthSwitch : public switch_::Switch, public Parented<HaierIrAcYrh63> {
 public:
  HealthSwitch() = default;

 protected:
  void write_state(bool on) override;
};

}  // namespace haier_ir_acyrh63
}  // namespace esphome
