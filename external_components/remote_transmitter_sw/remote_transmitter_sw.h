#pragma once

#include "esphome/components/remote_transmitter/remote_transmitter.h"

namespace esphome::remote_transmitter_sw {
class RemoteTransmitterComponent : public remote_transmitter::RemoteTransmitterComponent {
 public:
  void setup() { remote_transmitter::RemoteTransmitterComponent::setup(); };
  explicit RemoteTransmitterComponent(InternalGPIOPin *pin) : remote_transmitter::RemoteTransmitterComponent(pin) {}
  void dump_config() { remote_transmitter::RemoteTransmitterComponent::dump_config(); }
  float get_setup_priority() const { return remote_transmitter::RemoteTransmitterComponent::get_setup_priority(); }
#if defined(USE_ESP32) && SOC_RMT_SUPPORTED
  bool has_pin(int gpio) {
    gpio_num_t gpio_num = (gpio_num_t) gpio;
    return std::find(this->pin_nums.begin(), this->pin_nums.end(), gpio_num) != this->pin_nums.end();
  }
  void set_pin(int gpio) {
    gpio_num_t gpio_num = (gpio_num_t) gpio;
    esp_err_t error;
    if (this->initialized_ && this->pin_num != gpio_num) {
      if (this->has_pin(gpio_num)) {
        error = rmt_disable(this->channel_);
        if (error != ESP_OK) {
          this->error_code_ = error;
          this->error_string_ = "in rmt_disable";
          this->mark_failed();
          return;
        }
        error = rmt_tx_switch_gpio(this->channel_, gpio_num, this->inverted_);
        if (error != ESP_OK) {
          this->error_code_ = error;
          this->error_string_ = "in rmt_tx_switch_gpio";
          this->mark_failed();
          return;
        }
        error = rmt_enable(this->channel_);
        if (error != ESP_OK) {
          this->error_code_ = error;
          this->error_string_ = "in rmt_enable";
          this->mark_failed();
          return;
        }
        this->pin_num = gpio_num;
      } else {
        this->error_code_ = ESP_ERR_INVALID_ARG;
        this->error_string_ = "in set_pin";
        this->mark_failed();
      }
    }
  }
  void add_pin(int gpio) {
    gpio_num_t gpio_num = (gpio_num_t) gpio;
    if (!has_pin(gpio_num)) {
      this->pin_nums.push_back(gpio_num);
    }
  }
#endif
 protected:
#if defined(USE_ESP32) && SOC_RMT_SUPPORTED
  gpio_num_t pin_num = gpio_num_t(this->pin_->get_pin());
  std::vector<gpio_num_t> pin_nums;
#endif
  void send_internal(uint32_t send_times, uint32_t send_wait) {
    remote_transmitter::RemoteTransmitterComponent::send_internal(send_times, send_wait);
  }
};
}  // namespace esphome::remote_transmitter_sw
