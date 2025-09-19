#pragma once

#include "esphome/components/select/select.h"
#include "../haier_ir_acyrh63.h"

namespace esphome {
namespace haier_ir_acyrh63 {

class FreshSelect : public select::Select, public Parented<HaierIrAcYrh63> {
 protected:
  void control(const std::string &value) override;
};

}  // namespace haier_ir_acyrh63
}  // namespace esphome
