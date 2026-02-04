#pragma once

#include "esphome/components/remote_transmitter/remote_transmitter.h"

namespace esphome::remote_transmitter_sw {
class RemoteTransmitterComponent : public remote_transmitter::RemoteTransmitterComponent {
 public:
  void setup() { remote_transmitter::RemoteTransmitterComponent::setup(); };
  explicit RemoteTransmitterComponent(InternalGPIOPin *pin) : remote_transmitter::RemoteTransmitterComponent(pin) {}
  void dump_config() { remote_transmitter::RemoteTransmitterComponent::dump_config(); }
  float get_setup_priority() const { return remote_transmitter::RemoteTransmitterComponent::get_setup_priority(); }
#ifdef USE_ESP32
  void set_pin(gpio_num_t gpio_num);
  void set_pin_a(InternalGPIOPin *pin_a) { pin_a_ = pin_a; }
  void set_pin_b(InternalGPIOPin *pin_b) { pin_b_ = pin_b; }
#endif
 protected:
#ifdef USE_ESP32
  gpio_num_t pin_num = gpio_num_t(this->pin_->get_pin());
  InternalGPIOPin *pin_a_;
  InternalGPIOPin *pin_b_;
#endif
  void send_internal(uint32_t send_times, uint32_t send_wait) {
    remote_transmitter::RemoteTransmitterComponent::send_internal(send_times, send_wait);
  }
};
}  // namespace esphome::remote_transmitter_sw
