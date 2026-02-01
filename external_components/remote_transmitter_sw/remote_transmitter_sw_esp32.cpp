#include "remote_transmitter_sw.h"

#ifdef USE_ESP32

namespace esphome::remote_transmitter_sw {
void RemoteTransmitterComponent::set_pin(gpio_num_t gpio_num) {
  esp_err_t error;
  if (this->initialized_ && this->pin_num != gpio_num) {
    if (gpio_num_t(this->pin_a_->get_pin() == gpio_num || gpio_num_t(this->pin_b_->get_pin()) == gpio_num)) {
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
}  // namespace esphome::remote_transmitter_sw

#endif
