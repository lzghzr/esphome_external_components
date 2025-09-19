#pragma once

#include "esphome/components/climate_ir/climate_ir.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace haier_ir_acyrm09 {

// YR-M09 数据包大小为 9Byte, 即 72bit
const uint16_t kHaierAcYrm09StateLength = 9;
const uint16_t kHaierAcYrm09Bits = kHaierAcYrm09StateLength * 8;

#define GETTIME(x) _.x##Hours * 60 + _.x##Mins
#define SETTIME(x, n) \
  do { \
    uint16_t mins = n; \
    if (n > kHaierAcYrm09MaxTime) \
      mins = kHaierAcYrm09MaxTime; \
    _.x##Hours = mins / 60; \
    _.x##Mins = mins % 60; \
  } while (0)

uint8_t sum_bytes(const uint8_t *const start, const uint16_t length, const uint8_t init = 0);

union HaierAcYrm09Protocol {
  uint8_t remote_state[kHaierAcYrm09StateLength];
  struct {
    // Byte 0
    uint8_t Prefix;
    // Byte 1
    uint8_t Command : 4;
    uint8_t Temp : 4;
    // Byte 2
    uint8_t CurrHours : 5;
    uint8_t SwingV : 1;
    uint8_t Swing : 2;
    // Byte 3
    uint8_t CurrMins : 6;
    uint8_t OffTimer : 1;
    uint8_t OnTimer : 1;
    // Byte 4
    uint8_t OffHours : 5;
    uint8_t Health : 1;
    uint8_t Heating : 2;
    // Byte 5
    uint8_t OffMins : 6;
    uint8_t Fan : 2;
    // Byte 6
    uint8_t OnHours : 5;
    uint8_t Mode : 3;
    // Byte 7
    uint8_t OnMins : 6;
    uint8_t Sleep : 1;
    uint8_t unknown : 1;
    // Byte 8
    uint8_t Sum;
  };
};

const uint8_t kHaierAcYrm09Prefix = 0b10100101;

const uint8_t kHaierAcYrm09MinTemp = 16;
const uint8_t kHaierAcYrm09DefTemp = 25;
const uint8_t kHaierAcYrm09MaxTemp = 30;

const uint8_t kHaierAcYrm09CmdOff = 0b0000;
const uint8_t kHaierAcYrm09CmdOn = 0b0001;
const uint8_t kHaierAcYrm09CmdMode = 0b0010;
const uint8_t kHaierAcYrm09CmdFan = 0b0011;
const uint8_t kHaierAcYrm09CmdSwingVOn = 0b0100;
const uint8_t kHaierAcYrm09CmdSwingVOff = 0b0101;
const uint8_t kHaierAcYrm09CmdTempUp = 0b0110;
const uint8_t kHaierAcYrm09CmdTempDown = 0b0111;
const uint8_t kHaierAcYrm09CmdSleep = 0b1000;
const uint8_t kHaierAcYrm09CmdTimerSet = 0b1001;
const uint8_t kHaierAcYrm09CmdTimerCancel = 0b1010;
const uint8_t kHaierAcYrm09CmdHeating = 0b1011;
const uint8_t kHaierAcYrm09CmdHealth = 0b1100;
const uint8_t kHaierAcYrm09CmdSwing = 0b1101;

const uint8_t kHaierAcYrm09Heating = 0b11;

const uint8_t kHaierAcYrm09SwingVOff = true;
const uint8_t kHaierAcYrm09SwingVOn = false;

const uint8_t kHaierAcYrm09SwingOff = 0b00;
const uint8_t kHaierAcYrm09SwingUp = 0b01;
const uint8_t kHaierAcYrm09SwingDown = 0b10;
const uint8_t kHaierAcYrm09SwingChg = 0b11;

const uint8_t kHaierAcYrm09Auto = 0;
const uint8_t kHaierAcYrm09Cool = 1;
const uint8_t kHaierAcYrm09Dry = 2;
const uint8_t kHaierAcYrm09Heat = 3;
const uint8_t kHaierAcYrm09Fan = 4;

const uint8_t kHaierAcYrm09FanAuto = 0;
const uint8_t kHaierAcYrm09FanLow = 1;
const uint8_t kHaierAcYrm09FanMed = 2;
const uint8_t kHaierAcYrm09FanHigh = 3;

const uint16_t kHaierAcYrm09MaxTime = (23 * 60) + 59;

class HaierIrAcYrm09 : public climate_ir::ClimateIR {
 public:
  HaierIrAcYrm09()
      : climate_ir::ClimateIR(16.0f, 30.0f, 1.0f, true, true,
                              {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                               climate::CLIMATE_FAN_HIGH},
                              {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL},
                              {climate::CLIMATE_PRESET_NONE, climate::CLIMATE_PRESET_SLEEP}) {}

  bool get_heating_switch();
  void set_heating_switch();
  bool get_health_switch();
  void set_health_switch();
  void set_heating_switch_(switch_::Switch *sw);
  void set_health_switch_(switch_::Switch *sw);

  static bool valid_checksum(uint8_t state[], const uint16_t length = kHaierAcYrm09StateLength);

  uint8_t *get_raw(void);
  void set_raw(const uint8_t new_code[]);

  uint8_t get_command(void) const;
  void set_command(const uint8_t command);

  uint8_t get_temp(void) const;
  void set_temp(const uint8_t temp);

  bool get_swing_v(void) const;
  void set_swing_v(const bool on);

  bool get_health(void) const;
  void set_health(const bool on);

  bool get_heating(void) const;
  void set_heating(const bool on);

  uint8_t get_fan(void) const;
  void set_fan(const uint8_t speed);

  bool get_sleep(void) const;
  void set_sleep(const bool on);

  uint8_t get_mode(void) const;
  void set_mode(const uint8_t mode);

  uint16_t get_curr_time(void) const;
  void set_curr_time(const uint16_t mins);

  int16_t get_on_timer(void) const;
  void set_on_timer(const uint16_t mins);

  int16_t get_off_timer(void) const;
  void set_off_timer(const uint16_t mins);

  void cancel_timers(void);

  void transmit_state();

 protected:
  switch_::Switch *heating_switch_{nullptr};
  switch_::Switch *health_switch_{nullptr};
  HaierAcYrm09Protocol _ = {
      .remote_state = {kHaierAcYrm09Prefix, 0, 0, 0, 0, 0, 0, 0, 0},
  };
  climate::ClimateMode last_mode = climate::CLIMATE_MODE_COOL;

  void checksum(void);

  climate::ClimateTraits traits() override;

  void transmit_(uint8_t remote_state[]);

  bool skip = false;
  bool on_receive(remote_base::RemoteReceiveData src) override;
  void publish_state_();
};

}  // namespace haier_ir_acyrm09
}  // namespace esphome
