#include "heating.h"

namespace esphome {
namespace haier_ir_acyrm09 {

void HeatingSwitch::write_state(bool on) {
  publish_state(on);
  parent_->transmit_state();
}

}  // namespace haier_ir_acyrm09
}  // namespace esphome
