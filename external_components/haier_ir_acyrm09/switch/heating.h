#pragma once

#include "esphome/components/switch/switch.h"
#include "../haier_ir_acyrm09.h"

namespace esphome {
namespace haier_ir_acyrm09 {

class HeatingSwitch : public switch_::Switch, public Parented<HaierIrAcYrm09> {
 public:
  HeatingSwitch() = default;

 protected:
  void write_state(bool on) override;
};

}  // namespace haier_ir_acyrm09
}  // namespace esphome
