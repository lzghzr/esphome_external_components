#include "lock.h"

namespace esphome {
namespace haier_ir_acyrh63 {

void LockSwitch::write_state(bool on) {
  publish_state(on);
  parent_->transmit_state();
}

}  // namespace haier_ir_acyrh63
}  // namespace esphome
