#include "sterilize.h"

namespace esphome {
namespace haier_ir_acyrh63 {

void SterilizeSelect::control(const std::string &value) {
  publish_state(value);
  parent_->transmit_state();
}

}  // namespace haier_ir_acyrh63
}  // namespace esphome
