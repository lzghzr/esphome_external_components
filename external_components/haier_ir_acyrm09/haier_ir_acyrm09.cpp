#include "haier_ir_acyrm09.h"

namespace esphome {
namespace haier_ir_acyrm09 {

static const char *const TAG = "climate.haier_acyrm09";

const uint32_t HEADER_LOW_US = 3000;
const uint32_t HEADER_HIGH_US = 4300;
const uint32_t BIT_MARK_US = 520;
const uint32_t BIT_ONE_SPACE_US = 1650;
// BIT_ZERO_SPACE_US 范围 580 ~ 650, 建议 630
// 越小发送被识别率越高, 但接收识别率越低
// 越大接收识别率越高, 但发送被识别率越低
// 也可以 TX 和 RX 分开设置
const uint32_t BIT_ZERO_SPACE_US_TX = 580;
const uint32_t BIT_ZERO_SPACE_US_RX = 650;

// 数据包最后一位为校验码, 为前面所有字节的和
uint8_t sum_bytes(const uint8_t *const start, const uint16_t length, const uint8_t init) {
  uint8_t checksum = init;
  const uint8_t *ptr;
  for (ptr = start; ptr - start < length; ptr++) {
    checksum += *ptr;
  }
  return checksum;
}

// 健康开关
bool HaierIrAcYrm09::get_health_switch() {
  if (health_switch_ != nullptr) {
    return health_switch_->state;
  }
  return false;
}
void HaierIrAcYrm09::set_health_switch() {
  if (health_switch_ != nullptr) {
    bool on = get_health();
    health_switch_->state = on;
    health_switch_->publish_state(on);
  }
}
void HaierIrAcYrm09::set_health_switch_(switch_::Switch *sw) {
  health_switch_ = sw;
  set_health_switch();
}
// 电加热开关
bool HaierIrAcYrm09::get_heating_switch() {
  if (heating_switch_ != nullptr) {
    return heating_switch_->state;
  }
  return false;
}
void HaierIrAcYrm09::set_heating_switch() {
  if (heating_switch_ != nullptr) {
    bool on = get_heating();
    heating_switch_->state = on;
    heating_switch_->publish_state(on);
  }
}
void HaierIrAcYrm09::set_heating_switch_(switch_::Switch *sw) {
  heating_switch_ = sw;
  set_heating_switch();
}
// 计算 remote_state 校验码
void HaierIrAcYrm09::checksum(void) {
  _.Sum = sum_bytes(_.remote_state, kHaierAcYrm09StateLength - 1);
  base_rtc_.save(&_);
}
// 检查 state 校验码
bool HaierIrAcYrm09::valid_checksum(uint8_t state[], const uint16_t length) {
  if (length < 2) {
    return false;
  }  // 1 byte of data can't have a checksum.
  return (sum_bytes(state, length - 1) == state[length - 1]);
}
// 获取和设置 remote_state
uint8_t *HaierIrAcYrm09::get_raw(void) {
  checksum();
  return _.remote_state;
}
void HaierIrAcYrm09::set_raw(const uint8_t new_code[]) {
  std::memcpy(_.remote_state, new_code, kHaierAcYrm09StateLength);
  checksum();
}
// 遥控器按键
uint8_t HaierIrAcYrm09::get_command(void) const { return _.Command; }
void HaierIrAcYrm09::set_command(const uint8_t command) { _.Command = command; }
// 温度
uint8_t HaierIrAcYrm09::get_temp(void) const { return _.Temp + kHaierAcYrm09MinTemp; }
void HaierIrAcYrm09::set_temp(const uint8_t temp) {
  uint8_t old_temp = get_temp();
  if (temp == old_temp) {
    return;
  }

  if (temp > old_temp) {
    set_command(kHaierAcYrm09CmdTempUp);
  } else {
    set_command(kHaierAcYrm09CmdTempDown);
  }
  _.Temp = temp - kHaierAcYrm09MinTemp;
}
// 垂直扫风, 注意和 swing(健康风) 区分
bool HaierIrAcYrm09::get_swing_v(void) const { return !_.SwingV; }
void HaierIrAcYrm09::set_swing_v(const bool on) {
  if (get_swing_v() == on) {
    return;
  }

  if (on) {
    set_command(kHaierAcYrm09CmdSwingVOn);
  } else {
    set_command(kHaierAcYrm09CmdSwingVOff);
  }
  _.SwingV = !on;
}
// 健康, YR-M09 未包含此功能
bool HaierIrAcYrm09::get_health(void) const { return _.Health; }
void HaierIrAcYrm09::set_health(const bool on) {
  if (get_health() == on) {
    return;
  }

  set_command(kHaierAcYrm09CmdHealth);
  _.Health = on;
}
// 电加热, 仅制热模式下生效
bool HaierIrAcYrm09::get_heating(void) const {
  if (kHaierAcYrm09Heating == _.Heating) {
    return true;
  }
  return false;
}
void HaierIrAcYrm09::set_heating(const bool on) {
  if (climate::CLIMATE_MODE_HEAT == mode) {
    if (get_heating() == on) {
      return;
    }

    set_command(kHaierAcYrm09CmdHeating);
    if (on) {
      _.Heating = kHaierAcYrm09Heating;
    } else {
      _.Heating = !kHaierAcYrm09Heating;
    }
  } else {
    _.Heating = !kHaierAcYrm09Heating;
  }
}
// 风速, 仅吹风模式时不支持自动
uint8_t HaierIrAcYrm09::get_fan(void) const {
  switch (_.Fan) {
    case 1:
      return kHaierAcYrm09FanHigh;
    case 2:
      return kHaierAcYrm09FanMed;
    case 3:
      return kHaierAcYrm09FanLow;
    default:
      return kHaierAcYrm09FanAuto;
  }
}
void HaierIrAcYrm09::set_fan(const uint8_t speed) {
  if (climate::CLIMATE_MODE_FAN_ONLY == mode && kHaierAcYrm09FanAuto == speed) {
    fan_mode = climate::CLIMATE_FAN_LOW;
    _.Fan = kHaierAcYrm09FanLow;
    return;
  }
  if (get_fan() == speed) {
    return;
  }

  uint8_t new_speed = kHaierAcYrm09FanAuto;
  switch (speed) {
    case kHaierAcYrm09FanLow:
      new_speed = 3;
      break;
    case kHaierAcYrm09FanMed:
      new_speed = 2;
      break;
    case kHaierAcYrm09FanHigh:
      new_speed = 1;
      break;
    default:
      break;
  }
  set_command(kHaierAcYrm09CmdFan);
  _.Fan = new_speed;
}
// 睡眠, 不支持仅吹风模式 (基本没用, 还占着预设, 可以替换为电加热)
bool HaierIrAcYrm09::get_sleep(void) const { return _.Sleep; }
void HaierIrAcYrm09::set_sleep(const bool on) {
  if (climate::CLIMATE_MODE_FAN_ONLY != mode) {
    if (get_sleep() == on) {
      return;
    }

    set_command(kHaierAcYrm09CmdSleep);
    _.Sleep = on;
  } else {
    _.Sleep = false;
    preset = climate::CLIMATE_PRESET_NONE;
  }
}
// 工作模式
uint8_t HaierIrAcYrm09::get_mode(void) const { return _.Mode; }
void HaierIrAcYrm09::set_mode(const uint8_t mode) {
  if (get_mode() == mode) {
    return;
  }

  set_command(kHaierAcYrm09CmdMode);
  _.Mode = mode;
}
void HaierIrAcYrm09::setup() {
  climate_ir::ClimateIR::setup();
  constexpr uint32_t restore_settings_version = 0x695BA610;
  base_rtc_ =
      global_preferences->make_preference<HaierAcYrm09Protocol>(get_preference_hash() ^ restore_settings_version);
  if (!base_rtc_.load(&_)) {
    _ = {kHaierAcYrm09Prefix, 0, 0, 0, 0, 0, 0, 0, 0};
  }
  publish_state_();
}
// 将 "制冷/制热" 替换为 "自动", 也可以不动
climate::ClimateTraits HaierIrAcYrm09::traits() {
  auto traits = climate_ir::ClimateIR::traits();
  traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_AUTO, climate::CLIMATE_MODE_COOL,
                              climate::CLIMATE_MODE_DRY, climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_FAN_ONLY});
  traits.add_feature_flags(climate::CLIMATE_SUPPORTS_ACTION);
  return traits;
}
// 时间设置, 定时开关机相关, 基本没用, 只是 IRremoteESP8266 有就弄过来了
uint16_t HaierIrAcYrm09::get_curr_time(void) const { return GETTIME(Curr); }
void HaierIrAcYrm09::set_curr_time(const uint16_t nr_mins) { SETTIME(Curr, nr_mins); }

int16_t HaierIrAcYrm09::get_on_timer(void) const {
  if (_.OnTimer) {
    return GETTIME(On);
  } else {
    return -1;
  }
}
void HaierIrAcYrm09::set_on_timer(const uint16_t nr_mins) {
  set_command(kHaierAcYrm09CmdTimerSet);
  _.OnTimer = 1;

  SETTIME(On, nr_mins);
}
int16_t HaierIrAcYrm09::get_off_timer(void) const {
  if (_.OffTimer) {
    return GETTIME(Off);
  } else {
    return -1;
  }
}
void HaierIrAcYrm09::set_off_timer(const uint16_t nr_mins) {
  set_command(kHaierAcYrm09CmdTimerSet);
  _.OffTimer = 1;

  SETTIME(Off, nr_mins);
}
void HaierIrAcYrm09::cancel_timers(void) {
  set_command(kHaierAcYrm09CmdTimerCancel);
  _.OffTimer = 0;
  _.OnTimer = 0;
}
// 转换数据
void HaierIrAcYrm09::transmit_state() {
  uint8_t command = get_command();
  if (climate::CLIMATE_MODE_OFF == mode) {
    if (kHaierAcYrm09CmdOff == command) {
      return;
    }
    set_command(kHaierAcYrm09CmdOff);
  } else {
    set_health(get_health_switch());
    set_health_switch();

    set_heating(get_heating_switch());
    set_heating_switch();

    set_swing_v(climate::CLIMATE_SWING_VERTICAL == swing_mode);

    set_sleep(climate::CLIMATE_PRESET_SLEEP == preset);

    set_temp((uint8_t) target_temperature);

    switch (fan_mode.value()) {
      case climate::CLIMATE_FAN_AUTO:
        set_fan(kHaierAcYrm09FanAuto);
        break;
      case climate::CLIMATE_FAN_LOW:
        set_fan(kHaierAcYrm09FanLow);
        break;
      case climate::CLIMATE_FAN_MEDIUM:
        set_fan(kHaierAcYrm09FanMed);
        break;
      case climate::CLIMATE_FAN_HIGH:
        set_fan(kHaierAcYrm09FanHigh);
        break;
      default:
        break;
    }

    if (kHaierAcYrm09CmdOn == command || climate::CLIMATE_MODE_HEAT != mode) {
      switch (mode) {
        case climate::CLIMATE_MODE_AUTO:
          set_mode(kHaierAcYrm09Auto);
          break;
        case climate::CLIMATE_MODE_COOL:
          set_mode(kHaierAcYrm09Cool);
          break;
        case climate::CLIMATE_MODE_DRY:
          set_mode(kHaierAcYrm09Dry);
          break;
        case climate::CLIMATE_MODE_HEAT:
          set_mode(kHaierAcYrm09Heat);
          break;
        case climate::CLIMATE_MODE_FAN_ONLY:
          set_mode(kHaierAcYrm09Fan);
          break;
        default:
          break;
      }
    }

    if (kHaierAcYrm09CmdOff == command) {
      set_command(kHaierAcYrm09CmdOn);
    }
  }
  if (!valid_checksum(_.remote_state)) {
    transmit_(_.remote_state);
  }
}
// 发送数据
void HaierIrAcYrm09::transmit_(uint8_t remote_state[]) {
  checksum();

  auto transmit = transmitter_->transmit();
  auto *dst = transmit.get_data();

  dst->set_carrier_frequency(38000);
  dst->reserve(5 + (kHaierAcYrm09StateLength * 2));
  dst->mark(HEADER_LOW_US);
  dst->space(HEADER_LOW_US);
  dst->mark(HEADER_LOW_US);
  dst->space(HEADER_HIGH_US);
  dst->mark(BIT_MARK_US);
  for (size_t i = 0; i < kHaierAcYrm09StateLength; ++i) {
    for (uint8_t mask = 1 << 7; mask != 0; mask >>= 1) {
      if (remote_state[i] & mask) {
        dst->space(BIT_ONE_SPACE_US);
      } else {
        dst->space(BIT_ZERO_SPACE_US_TX);
      }
      dst->mark(BIT_MARK_US);
    }
  }
  skip = true;
  transmit.perform();
}
// 接收数据
bool HaierIrAcYrm09::on_receive(remote_base::RemoteReceiveData src) {
  // 跳过自己发送的数据, 不知道有没有更好的方法
  if (skip) {
    skip = false;
    return true;
  }

  if (!src.expect_item(HEADER_LOW_US, HEADER_LOW_US) || !src.expect_item(HEADER_LOW_US, HEADER_HIGH_US)) {
    return false;
  }
  if (!src.expect_mark(BIT_MARK_US)) {
    return false;
  }

  size_t size = src.size() - src.get_index() - 1;
  if (size != kHaierAcYrm09Bits * 2) {
    return false;
  }

  uint8_t recv_message[kHaierAcYrm09StateLength] = {0};

  for (int pos = 0; pos < kHaierAcYrm09StateLength; pos++) {
    uint8_t byte = 0;
    for (uint8_t mask = 0x80; mask != 0; mask >>= 1) {
      if (src.expect_space(BIT_ONE_SPACE_US)) {
        byte |= mask;
      } else if (!src.expect_space(BIT_ZERO_SPACE_US_RX)) {
        return false;
      }
      if (!src.expect_mark(BIT_MARK_US)) {
        return false;
      }
    }
    recv_message[pos] = byte;
  }

#ifdef ESP_LOGD
  char buffer[kHaierAcYrm09StateLength * 2 + 1];
  for (int i = 0; i < kHaierAcYrm09StateLength; ++i) {
    sprintf(&buffer[i * 2], "%02X", recv_message[i]);
  }
  buffer[kHaierAcYrm09StateLength * 2] = '\0';
  ESP_LOGD(TAG, "Full message: %s", buffer);
#endif  // ESP_LOGD

  if (kHaierAcYrm09Prefix == recv_message[0] && valid_checksum(recv_message, kHaierAcYrm09StateLength)) {
    set_raw(recv_message);
    publish_state_();
    return true;
  } else {
    return false;
  }
}
// 更新状态
void HaierIrAcYrm09::publish_state_() {
  if (get_command() == kHaierAcYrm09CmdOff) {
    mode = climate::CLIMATE_MODE_OFF;
  } else {
    set_health_switch();

    set_heating_switch();

    if (get_swing_v()) {
      swing_mode = climate::CLIMATE_SWING_VERTICAL;
    } else {
      swing_mode = climate::CLIMATE_SWING_OFF;
    }

    if (get_sleep()) {
      preset = climate::CLIMATE_PRESET_SLEEP;
    } else {
      preset = climate::CLIMATE_PRESET_NONE;
    }

    target_temperature = get_temp();

    switch (get_fan()) {
      case kHaierAcYrm09FanAuto:
        fan_mode = climate::CLIMATE_FAN_AUTO;
        break;
      case kHaierAcYrm09FanLow:
        fan_mode = climate::CLIMATE_FAN_LOW;
        break;
      case kHaierAcYrm09FanMed:
        fan_mode = climate::CLIMATE_FAN_MEDIUM;
        break;
      case kHaierAcYrm09FanHigh:
        fan_mode = climate::CLIMATE_FAN_HIGH;
        break;
      default:
        break;
    }

    switch (get_mode()) {
      case kHaierAcYrm09Auto:
        mode = climate::CLIMATE_MODE_AUTO;
        break;
      case kHaierAcYrm09Cool:
        mode = climate::CLIMATE_MODE_COOL;
        break;
      case kHaierAcYrm09Dry:
        mode = climate::CLIMATE_MODE_DRY;
        break;
      case kHaierAcYrm09Heat:
        mode = climate::CLIMATE_MODE_HEAT;
        break;
      case kHaierAcYrm09Fan:
        mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;
      default:
        break;
    }
  }

  publish_state();
}

}  // namespace haier_ir_acyrm09
}  // namespace esphome
