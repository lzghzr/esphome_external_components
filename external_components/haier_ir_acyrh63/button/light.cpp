#include "light.h"

namespace esphome {
namespace haier_ir_acyrh63 {

void LightButton::press_action() { parent_->set_light_button(); }

}  // namespace haier_ir_acyrh63
}  // namespace esphome
