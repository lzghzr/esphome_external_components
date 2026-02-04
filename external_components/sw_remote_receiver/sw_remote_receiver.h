#pragma once

#include "esphome/components/remote_base/remote_base.h"
#include "esphome/core/component.h"

#include <cinttypes>

namespace esphome::sw_remote_receiver {

struct RemoteReceiverComponentStore {
  static void gpio_intr(RemoteReceiverComponentStore *arg);

  /// Stores pulse durations in microseconds as signed integers
  ///  * Positive values indicate high pulses (marks)
  ///  * Negative values indicate low pulses (spaces)
  volatile int32_t *buffer{nullptr};
  /// The position last written to
  volatile uint32_t buffer_write{0};
  /// The start position of the last sequence
  volatile uint32_t buffer_start{0};
  /// The position last read from
  uint32_t buffer_read{0};
  volatile uint32_t commit_micros{0};
  volatile uint32_t prev_micros{0};
  uint32_t buffer_size{1000};
  uint32_t filter_us{10};
  uint32_t idle_us{10000};
  ISRInternalGPIOPin pin;
  volatile bool commit_level{false};
  volatile bool prev_level{false};
  volatile bool overflow{false};
};

class RemoteReceiverComponent : public remote_base::RemoteReceiverBase, public Component {
 public:
  RemoteReceiverComponent(InternalGPIOPin *pin) : RemoteReceiverBase(pin) {}
  void setup() override;
  void dump_config() override;
  void loop() override;

  void set_buffer_size(uint32_t buffer_size) { this->buffer_size_ = buffer_size; }
  void set_filter_us(uint32_t filter_us) { this->filter_us_ = filter_us; }
  void set_idle_us(uint32_t idle_us) { this->idle_us_ = idle_us; }

 protected:
  RemoteReceiverComponentStore store_;
  HighFrequencyLoopRequester high_freq_;

  uint32_t buffer_size_{};
  uint32_t filter_us_{10};
  uint32_t idle_us_{10000};
};

}  // namespace esphome::sw_remote_receiver
