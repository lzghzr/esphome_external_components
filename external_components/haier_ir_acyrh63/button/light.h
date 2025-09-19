#pragma once

#include "esphome/components/button/button.h"
#include "../haier_ir_acyrh63.h"

namespace esphome {
namespace haier_ir_acyrh63 {

class LightButton : public button::Button, public Parented<HaierIrAcYrh63> {
 public:
  LightButton() = default;

 protected:
  void press_action() override;
};

}  // namespace haier_ir_acyrh63
}  // namespace esphome
