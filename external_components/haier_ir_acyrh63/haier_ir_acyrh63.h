#pragma once

#include "esphome/components/climate_ir/climate_ir.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/select/select.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace haier_ir_acyrh63 {

// YR-H63 数据包大小为 14Byte, 即 112bit
const uint16_t kHaierAcYrh63StateLength = 14;
const uint16_t kHaierAcYrh63Bits = kHaierAcYrh63StateLength * 8;

uint8_t sum_bytes(const uint8_t *const start, const uint16_t length, const uint8_t init = 0);

union HaierAcYrh63Protocol {
  uint8_t remote_state[kHaierAcYrh63StateLength];
  struct {
    // Byte 0
    uint8_t Model : 8;
    // Byte 1
    uint8_t SwingV : 4;
    uint8_t Temp : 4;  // 16C~30C
    // Byte 2
    uint8_t : 5;
    uint8_t SwingH : 3;
    // Byte 3
    uint8_t : 1;
    uint8_t Health : 1;
    uint8_t Fresh : 2;
    uint8_t : 1;
    uint8_t TimerMode : 3;
    // Byte 4
    uint8_t : 6;
    uint8_t Power : 1;
    uint8_t : 1;
    // Byte 5
    uint8_t OffTimerHrs : 5;
    uint8_t Fan : 3;
    // Byte 6
    uint8_t OffTimerMins : 6;
    uint8_t Sterilize : 2;
    // Byte 7
    uint8_t OnTimerHrs : 5;
    uint8_t Mode : 3;
    // Byte 8
    uint8_t OnTimerMins : 6;
    uint8_t : 1;
    uint8_t Sleep : 1;
    // Byte 9
    uint8_t : 8;
    // Byte 10
    uint8_t : 8;
    // Byte 11
    uint8_t : 8;
    // Byte 12
    uint8_t Button : 5;
    uint8_t Lock : 1;
    uint8_t : 2;
    // Byte 13
    uint8_t Sum : 8;
  };
};

const uint8_t kHaierAcYrh63Model = 0xA6;

const uint8_t kHaierAcYrh63MinTemp = 16;
const uint8_t kHaierAcYrh63MaxTemp = 30;
const uint8_t kHaierAcYrh63DefTemp = 25;

const uint8_t kHaierAcYrh63SwingVHighest = 0b0010;
const uint8_t kHaierAcYrh63SwingVHigh = 0b0100;
const uint8_t kHaierAcYrh63SwingVMiddle = 0b0110;
const uint8_t kHaierAcYrh63SwingVLow = 0b1000;
// const uint8_t kHaierAcYrh63SwingVLowest = 0b1010;
const uint8_t kHaierAcYrh63SwingVAuto = 0b1100;

const uint8_t kHaierAcYrh63SwingHMiddle = 0b000;
const uint8_t kHaierAcYrh63SwingHLeftRight = 0b001;
const uint8_t kHaierAcYrh63SwingHLeftRightMax = 0b010;
const uint8_t kHaierAcYrh63SwingHLeftMax = 0b011;
const uint8_t kHaierAcYrh63SwingHLeft = 0b100;
const uint8_t kHaierAcYrh63SwingHRight = 0b101;
const uint8_t kHaierAcYrh63SwingHRightMax = 0b110;
const uint8_t kHaierAcYrh63SwingHAuto = 0b111;

const uint8_t kHaierAcYrh63FreshOff = 0b00;
const uint8_t kHaierAcYrh63FreshLow = 0b01;
const uint8_t kHaierAcYrh63FreshHigh = 0b10;

const uint8_t kHaierAcYrh63FanHigh = 0b001;
const uint8_t kHaierAcYrh63FanMed = 0b010;
const uint8_t kHaierAcYrh63FanLow = 0b011;
const uint8_t kHaierAcYrh63FanAuto = 0b101;

const uint8_t kHaierAcYrh63Auto = 0b000;
const uint8_t kHaierAcYrh63Cool = 0b001;
const uint8_t kHaierAcYrh63Dry = 0b010;
const uint8_t kHaierAcYrh63Heat = 0b100;
const uint8_t kHaierAcYrh63Fan = 0b110;

const uint8_t kHaierAcYrh63ButtonTempUp = 0b00000;
const uint8_t kHaierAcYrh63ButtonTempDown = 0b00001;
const uint8_t kHaierAcYrh63ButtonSwingV = 0b00010;
const uint8_t kHaierAcYrh63ButtonSwingH = 0b00011;
const uint8_t kHaierAcYrh63ButtonFan = 0b00100;
const uint8_t kHaierAcYrh63ButtonPower = 0b00101;
const uint8_t kHaierAcYrh63ButtonMode = 0b00110;
const uint8_t kHaierAcYrh63ButtonHealth = 0b00111;
const uint8_t kHaierAcYrh63ButtonSterilize = 0b01000;
const uint8_t kHaierAcYrh63ButtonSleep = 0b01011;
const uint8_t kHaierAcYrh63ButtonTimer = 0b10000;
const uint8_t kHaierAcYrh63ButtonFresh = 0b10011;
const uint8_t kHaierAcYrh63ButtonLock = 0b10100;
const uint8_t kHaierAcYrh63ButtonLight = 0b10101;

const uint8_t kHaierAcYrh63NoTimers = 0b000;
const uint8_t kHaierAcYrh63OffTimer = 0b001;
const uint8_t kHaierAcYrh63OnTimer = 0b010;
const uint8_t kHaierAcYrh63OnThenOffTimer = 0b100;
const uint8_t kHaierAcYrh63OffThenOnTimer = 0b101;

const uint16_t kHaierAcYrh63MaxTime = (23 * 60) + 59;

class HaierIrAcYrh63 : public climate_ir::ClimateIR {
 public:
  HaierIrAcYrh63()
      : climate_ir::ClimateIR(16.0f, 30.0f, 1.0f, true, true,
                              {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                               climate::CLIMATE_FAN_HIGH},
                              {}, {climate::CLIMATE_PRESET_NONE, climate::CLIMATE_PRESET_SLEEP}) {}

  bool get_health_switch();
  void set_health_switch();
  void set_health_switch_(switch_::Switch *sw);

  bool get_lock_switch();
  void set_lock_switch();
  void set_lock_switch_(switch_::Switch *sw);

  uint8_t get_fresh_select();
  void set_fresh_select();
  void set_fresh_select_(select::Select *s);

  uint8_t get_sterilize_select();
  void set_sterilize_select();
  void set_sterilize_select_(select::Select *s);

  uint8_t get_vertical_select();
  void set_vertical_select();
  void set_vertical_select_(select::Select *s);

  uint8_t get_horizontal_select();
  void set_horizontal_select();
  void set_horizontal_select_(select::Select *s);

  void set_light_button();
  void set_light_button_(button::Button *b);

  static bool valid_checksum(uint8_t state[], const uint16_t length = kHaierAcYrh63StateLength);

  uint8_t *get_raw(void);
  void set_raw(const uint8_t new_code[]);

  uint8_t get_temp(void) const;
  void set_temp(const uint8_t temp);

  uint8_t get_vertical(void) const;
  void set_vertical(const uint8_t vertical);

  uint8_t get_horizontal(void) const;
  void set_horizontal(const uint8_t horizontal);

  bool get_health(void) const;
  void set_health(const bool on);

  uint8_t get_fresh(void) const;
  void set_fresh(const uint8_t fresh);

  bool get_power(void) const;
  void set_power(const bool on);

  uint8_t get_fan(void) const;
  void set_fan(const uint8_t speed);

  uint8_t get_sterilize(void) const;
  void set_sterilize(const uint8_t sterilize);

  uint8_t get_mode(void) const;
  void set_mode(const uint8_t mode);

  bool get_sleep(void) const;
  void set_sleep(const bool on);

  uint8_t get_button(void) const;
  void set_button(const uint8_t button);

  bool get_lock(void) const;
  void set_lock(const bool on);

  uint16_t get_timer_mode(void) const;
  void set_timer_mode(const uint16_t mode);

  int16_t get_on_timer(void) const;
  void set_on_timer(const uint16_t mins);

  int16_t get_off_timer(void) const;
  void set_off_timer(const uint16_t mins);

  void transmit_state();

 protected:
  switch_::Switch *health_switch_{nullptr};
  switch_::Switch *lock_switch_{nullptr};
  select::Select *fresh_select_{nullptr};
  select::Select *sterilize_select_{nullptr};
  select::Select *vertical_select_{nullptr};
  select::Select *horizontal_select_{nullptr};
  button::Button *light_button_{nullptr};
  HaierAcYrh63Protocol _ = {
      .remote_state = {kHaierAcYrh63Model, 0x9C, 0xE0, 0, 0, 0, 0, 0, 0},
  };
  climate::ClimateMode last_mode = climate::CLIMATE_MODE_COOL;

  void checksum(void);

  climate::ClimateTraits traits() override;

  void transmit_(uint8_t remote_state[]);

  bool skip = false;
  bool on_receive(remote_base::RemoteReceiveData src) override;
  void publish_state_();
};

}  // namespace haier_ir_acyrh63
}  // namespace esphome
