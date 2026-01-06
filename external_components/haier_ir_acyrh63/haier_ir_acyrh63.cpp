#include "haier_ir_acyrh63.h"

namespace esphome {
namespace haier_ir_acyrh63 {

static const char *const TAG = "climate.haier_acyrh63";

const uint32_t HEADER_LOW_US = 3100;
const uint32_t HEADER_HIGH_US = 4450;
const uint32_t BIT_MARK_US = 530;
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
bool HaierIrAcYrh63::get_health_switch() {
  if (health_switch_ != nullptr) {
    return health_switch_->state;
  }
  return false;
}
void HaierIrAcYrh63::set_health_switch() {
  if (health_switch_ != nullptr) {
    health_switch_->publish_state(get_health());
  }
}
void HaierIrAcYrh63::set_health_switch_(switch_::Switch *sw) {
  health_switch_ = sw;
  set_health_switch();
}
// 锁定开关
bool HaierIrAcYrh63::get_lock_switch() {
  if (lock_switch_ != nullptr) {
    return lock_switch_->state;
  }
  return false;
}
void HaierIrAcYrh63::set_lock_switch() {
  if (lock_switch_ != nullptr) {
    lock_switch_->publish_state(get_lock());
  }
}
void HaierIrAcYrh63::set_lock_switch_(switch_::Switch *sw) {
  lock_switch_ = sw;
  set_lock_switch();
}
// 新风选择
uint8_t HaierIrAcYrh63::get_fresh_select() {
  if (fresh_select_ != nullptr) {
    return fresh_select_->active_index().value();
  }
  return 0;
}
void HaierIrAcYrh63::set_fresh_select() {
  if (fresh_select_ != nullptr) {
    std::string value = fresh_select_->at(get_fresh()).value();
    fresh_select_->publish_state(value);
  }
}
void HaierIrAcYrh63::set_fresh_select_(select::Select *s) {
  fresh_select_ = s;
  set_fresh_select();
}
// 除菌选择
uint8_t HaierIrAcYrh63::get_sterilize_select() {
  if (sterilize_select_ != nullptr) {
    return sterilize_select_->active_index().value();
  }
  return 0;
}
void HaierIrAcYrh63::set_sterilize_select() {
  if (sterilize_select_ != nullptr) {
    std::string value = sterilize_select_->at(get_sterilize()).value();
    sterilize_select_->publish_state(value);
  }
}
void HaierIrAcYrh63::set_sterilize_select_(select::Select *s) {
  sterilize_select_ = s;
  set_sterilize_select();
}
// 点亮
void HaierIrAcYrh63::set_light_button() {
  set_button(kHaierAcYrh63ButtonLight);
  transmit_(_.remote_state);
}
void HaierIrAcYrh63::set_light_button_(button::Button *b) { light_button_ = b; }
// 垂直选择
uint8_t HaierIrAcYrh63::get_vertical_select() {
  if (vertical_select_ != nullptr) {
    switch (vertical_select_->active_index().value()) {
      case 0:
        return kHaierAcYrh63SwingVHighest;
      case 1:
        return kHaierAcYrh63SwingVHigh;
      case 2:
        return kHaierAcYrh63SwingVMiddle;
      case 3:
        return kHaierAcYrh63SwingVLow;
      case 4:
        return kHaierAcYrh63SwingVAuto;
      default:
        break;
    }
  }
  return kHaierAcYrh63SwingVAuto;
}
void HaierIrAcYrh63::set_vertical_select() {
  if (vertical_select_ != nullptr) {
    std::string value;
    switch (get_vertical()) {
      case kHaierAcYrh63SwingVHighest:
        value = vertical_select_->at(0).value();
        break;
      case kHaierAcYrh63SwingVHigh:
        value = vertical_select_->at(1).value();
        break;
      case kHaierAcYrh63SwingVMiddle:
        value = vertical_select_->at(2).value();
        break;
      case kHaierAcYrh63SwingVLow:
        value = vertical_select_->at(3).value();
        break;
      case kHaierAcYrh63SwingVAuto:
        value = vertical_select_->at(4).value();
        break;
      default:
        value = vertical_select_->at(4).value();
        break;
    }
    vertical_select_->publish_state(value);
  }
}
void HaierIrAcYrh63::set_vertical_select_(select::Select *s) {
  vertical_select_ = s;
  set_vertical_select();
}
// 水平选择
uint8_t HaierIrAcYrh63::get_horizontal_select() {
  if (horizontal_select_ != nullptr) {
    return horizontal_select_->active_index().value();
  }
  return kHaierAcYrh63SwingHAuto;
}
void HaierIrAcYrh63::set_horizontal_select() {
  if (horizontal_select_ != nullptr) {
    std::string value = horizontal_select_->at(get_horizontal()).value();
    horizontal_select_->publish_state(value);
  }
}
void HaierIrAcYrh63::set_horizontal_select_(select::Select *s) {
  horizontal_select_ = s;
  set_horizontal_select();
}
// 计算 remote_state 校验码
void HaierIrAcYrh63::checksum(void) {
  _.Sum = sum_bytes(_.remote_state, kHaierAcYrh63StateLength - 1);
  base_rtc_.save(&_);
}
// 检查 state 校验码
bool HaierIrAcYrh63::valid_checksum(uint8_t state[], const uint16_t length) {
  if (length < 2) {
    return false;
  }  // 1 byte of data can't have a checksum.
  return (sum_bytes(state, length - 1) == state[length - 1]);
}
// 获取和设置 remote_state
uint8_t *HaierIrAcYrh63::get_raw(void) {
  checksum();
  return _.remote_state;
}
void HaierIrAcYrh63::set_raw(const uint8_t new_code[]) {
  std::memcpy(_.remote_state, new_code, kHaierAcYrh63StateLength);
  checksum();
}
// 温度
uint8_t HaierIrAcYrh63::get_temp(void) const { return _.Temp + kHaierAcYrh63MinTemp; }
void HaierIrAcYrh63::set_temp(const uint8_t temp) {
  uint8_t old_temp = get_temp();
  if (temp == old_temp) {
    return;
  }

  if (temp > old_temp) {
    set_button(kHaierAcYrh63ButtonTempUp);
  } else {
    set_button(kHaierAcYrh63ButtonTempDown);
  }
  _.Temp = temp - kHaierAcYrh63MinTemp;
}
// 垂直扫风, 注意和 swing(健康风) 区分
uint8_t HaierIrAcYrh63::get_vertical(void) const { return _.SwingV; }
void HaierIrAcYrh63::set_vertical(const uint8_t vertical) {
  if (get_vertical() == vertical) {
    return;
  }

  set_button(kHaierAcYrh63ButtonSwingV);
  _.SwingV = vertical;
}
// 水平扫风, 注意和 swing(健康风) 区分
uint8_t HaierIrAcYrh63::get_horizontal(void) const { return _.SwingH; }
void HaierIrAcYrh63::set_horizontal(const uint8_t horizontal) {
  if (get_horizontal() == horizontal) {
    return;
  }

  set_button(kHaierAcYrh63ButtonSwingH);
  _.SwingH = horizontal;
}
// 健康
bool HaierIrAcYrh63::get_health(void) const { return _.Health; }
void HaierIrAcYrh63::set_health(const bool on) {
  if (get_health() == on) {
    return;
  }

  set_button(kHaierAcYrh63ButtonHealth);
  _.Health = on;
}
// 新风
uint8_t HaierIrAcYrh63::get_fresh(void) const { return _.Fresh; }
void HaierIrAcYrh63::set_fresh(const uint8_t fresh) {
  if (get_fresh() == fresh) {
    return;
  }

  set_button(kHaierAcYrh63ButtonFresh);
  _.Fresh = fresh;
}
// 电源
bool HaierIrAcYrh63::get_power(void) const { return _.Power; }
void HaierIrAcYrh63::set_power(const bool on) {
  if (get_power() == on) {
    return;
  }

  set_button(kHaierAcYrh63ButtonPower);
  _.Power = on;
}
// 风速, 仅吹风模式时不支持自动
uint8_t HaierIrAcYrh63::get_fan(void) const { return _.Fan; }
void HaierIrAcYrh63::set_fan(const uint8_t speed) {
  if (climate::CLIMATE_MODE_FAN_ONLY == mode && kHaierAcYrh63FanAuto == speed) {
    fan_mode = climate::CLIMATE_FAN_LOW;
    _.Fan = kHaierAcYrh63FanLow;
    return;
  }
  if (get_fan() == speed) {
    return;
  }

  set_button(kHaierAcYrh63ButtonFan);
  _.Fan = speed;
}
// 除菌
uint8_t HaierIrAcYrh63::get_sterilize(void) const { return _.Sterilize; }
void HaierIrAcYrh63::set_sterilize(const uint8_t sterilize) {
  if (get_sterilize() == sterilize) {
    return;
  }

  set_button(kHaierAcYrh63ButtonSterilize);
  _.Sterilize = sterilize;
}
// 工作模式
uint8_t HaierIrAcYrh63::get_mode(void) const { return _.Mode; }
void HaierIrAcYrh63::set_mode(const uint8_t mode) {
  if (get_mode() == mode) {
    return;
  }

  set_button(kHaierAcYrh63ButtonMode);
  _.Mode = mode;
}
// 睡眠, 不支持仅吹风模式 (基本没用, 还占着预设)
bool HaierIrAcYrh63::get_sleep(void) const { return _.Sleep; }
void HaierIrAcYrh63::set_sleep(const bool on) {
  if (climate::CLIMATE_MODE_FAN_ONLY != mode) {
    if (get_sleep() == on) {
      return;
    }

    set_button(kHaierAcYrh63ButtonSleep);
    _.Sleep = on;
  } else {
    _.Sleep = false;
    preset = climate::CLIMATE_PRESET_NONE;
  }
}
// 遥控器按键
uint8_t HaierIrAcYrh63::get_button(void) const { return _.Button; }
void HaierIrAcYrh63::set_button(const uint8_t button) { _.Button = button; }
// 锁定
bool HaierIrAcYrh63::get_lock(void) const { return _.Lock; }
void HaierIrAcYrh63::set_lock(const bool on) {
  if (get_lock() == on) {
    return;
  }

  set_button(kHaierAcYrh63ButtonLock);
  _.Lock = on;
}
// 时间设置, 定时开关机相关, 基本没用, 只是 IRremoteESP8266 有就弄过来了
uint16_t HaierIrAcYrh63::get_timer_mode(void) const { return _.TimerMode; }
void HaierIrAcYrh63::set_timer_mode(const uint16_t mode) {
  _.TimerMode = (mode > kHaierAcYrh63OffThenOnTimer) ? kHaierAcYrh63NoTimers : mode;
  switch (_.TimerMode) {
    case kHaierAcYrh63NoTimers:
      set_on_timer(0);   // Disable the On timer.
      set_off_timer(0);  // Disable the Off timer.
      break;
    case kHaierAcYrh63OffTimer:
      set_on_timer(0);  // Disable the On timer.
      break;
    case kHaierAcYrh63OnTimer:
      set_off_timer(0);  // Disable the Off timer.
      break;
    default:
      break;
  }
}
int16_t HaierIrAcYrh63::get_on_timer(void) const { return _.OnTimerHrs * 60 + _.OnTimerMins; }
void HaierIrAcYrh63::set_on_timer(const uint16_t mins) {
  const uint16_t nr_mins = std::min(kHaierAcYrh63MaxTime, mins);
  _.OnTimerHrs = nr_mins / 60;
  _.OnTimerMins = nr_mins % 60;

  const bool enabled = (nr_mins > 0);
  uint8_t mode = get_timer_mode();
  switch (mode) {
    case kHaierAcYrh63OffTimer:
      mode = enabled ? kHaierAcYrh63OffThenOnTimer : mode;
      break;
    case kHaierAcYrh63OnThenOffTimer:
    case kHaierAcYrh63OffThenOnTimer:
      mode = enabled ? kHaierAcYrh63OffThenOnTimer : kHaierAcYrh63OffTimer;
      break;
    default:
      // Enable/Disable the On timer for the simple case.
      mode = enabled << 1;
      break;
  }
  _.TimerMode = mode;
}
int16_t HaierIrAcYrh63::get_off_timer(void) const { return _.OffTimerHrs * 60 + _.OffTimerMins; }
void HaierIrAcYrh63::set_off_timer(const uint16_t mins) {
  const uint16_t nr_mins = std::min(kHaierAcYrh63MaxTime, mins);
  _.OffTimerHrs = nr_mins / 60;
  _.OffTimerMins = nr_mins % 60;

  const bool enabled = (nr_mins > 0);
  uint8_t mode = get_timer_mode();
  switch (mode) {
    case kHaierAcYrh63OnTimer:
      mode = enabled ? kHaierAcYrh63OnThenOffTimer : mode;
      break;
    case kHaierAcYrh63OnThenOffTimer:
    case kHaierAcYrh63OffThenOnTimer:
      mode = enabled ? kHaierAcYrh63OnThenOffTimer : kHaierAcYrh63OnTimer;
      break;
    default:
      // Enable/Disable the Off timer for the simple case.
      mode = enabled;
      break;
  }
  _.TimerMode = mode;
}
void HaierIrAcYrh63::setup() {
  climate_ir::ClimateIR::setup();
  constexpr uint32_t restore_settings_version = 0x695D01E1;
  base_rtc_ =
      global_preferences->make_preference<HaierAcYrh63Protocol>(get_preference_hash() ^ restore_settings_version);
  if (!base_rtc_.load(&_)) {
    _ = {kHaierAcYrh63Model, 0x9C, 0xE0, 0, 0, 0, 0, 0, 0};
  }
  publish_state_();
}
// 将 "制冷/制热" 替换为 "自动", 也可以不动
climate::ClimateTraits HaierIrAcYrh63::traits() {
  auto traits = climate_ir::ClimateIR::traits();
  traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_AUTO, climate::CLIMATE_MODE_COOL,
                              climate::CLIMATE_MODE_DRY, climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_FAN_ONLY});
  return traits;
}
// 转换数据
void HaierIrAcYrh63::transmit_state() {
  uint8_t power = get_power();
  if (climate::CLIMATE_MODE_OFF == mode) {
    if (!power) {
      return;
    }
    set_button(kHaierAcYrh63ButtonPower);
    set_power(false);
  } else {
    set_health(get_health_switch());
    set_health_switch();

    set_lock(get_lock_switch());
    set_lock_switch();

    set_fresh(get_fresh_select());
    set_fresh_select();

    set_sterilize(get_sterilize_select());
    set_sterilize_select();

    set_vertical(get_vertical_select());
    set_vertical_select();

    set_horizontal(get_horizontal_select());
    set_horizontal_select();

    set_sleep(climate::CLIMATE_PRESET_SLEEP == preset);

    set_temp((uint8_t) target_temperature);

    switch (fan_mode.value()) {
      case climate::CLIMATE_FAN_AUTO:
        set_fan(kHaierAcYrh63FanAuto);
        break;
      case climate::CLIMATE_FAN_LOW:
        set_fan(kHaierAcYrh63FanLow);
        break;
      case climate::CLIMATE_FAN_MEDIUM:
        set_fan(kHaierAcYrh63FanMed);
        break;
      case climate::CLIMATE_FAN_HIGH:
        set_fan(kHaierAcYrh63FanHigh);
        break;
      default:
        break;
    }

    if (power || climate::CLIMATE_MODE_HEAT != mode) {
      switch (mode) {
        case climate::CLIMATE_MODE_AUTO:
          set_mode(kHaierAcYrh63Auto);
          break;
        case climate::CLIMATE_MODE_COOL:
          set_mode(kHaierAcYrh63Cool);
          break;
        case climate::CLIMATE_MODE_DRY:
          set_mode(kHaierAcYrh63Dry);
          break;
        case climate::CLIMATE_MODE_HEAT:
          set_mode(kHaierAcYrh63Heat);
          break;
        case climate::CLIMATE_MODE_FAN_ONLY:
          set_mode(kHaierAcYrh63Fan);
          break;
        default:
          break;
      }
    }

    if (!power) {
      set_button(kHaierAcYrh63ButtonPower);
      set_power(true);
    }
  }
  if (!valid_checksum(_.remote_state)) {
    transmit_(_.remote_state);
  }
}
// 发送数据
void HaierIrAcYrh63::transmit_(uint8_t remote_state[]) {
  checksum();

  auto transmit = transmitter_->transmit();
  auto *dst = transmit.get_data();

  dst->set_carrier_frequency(38000);
  dst->reserve(5 + (kHaierAcYrh63StateLength * 2));
  dst->mark(HEADER_LOW_US);
  dst->space(HEADER_LOW_US);
  dst->mark(HEADER_LOW_US);
  dst->space(HEADER_HIGH_US);
  dst->mark(BIT_MARK_US);
  for (size_t i = 0; i < kHaierAcYrh63StateLength; ++i) {
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
bool HaierIrAcYrh63::on_receive(remote_base::RemoteReceiveData src) {
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
  if (size != kHaierAcYrh63Bits * 2) {
    return false;
  }

  uint8_t recv_message[kHaierAcYrh63StateLength] = {0};

  for (int pos = 0; pos < kHaierAcYrh63StateLength; pos++) {
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
  char buffer[kHaierAcYrh63StateLength * 2 + 1];
  for (int i = 0; i < kHaierAcYrh63StateLength; ++i) {
    sprintf(&buffer[i * 2], "%02X", recv_message[i]);
  }
  buffer[kHaierAcYrh63StateLength * 2] = '\0';
  ESP_LOGD(TAG, "Full message: %s", buffer);
#endif  // ESP_LOGD

  if (kHaierAcYrh63Model == recv_message[0] && valid_checksum(recv_message, kHaierAcYrh63StateLength)) {
    set_raw(recv_message);
    publish_state_();
    return true;
  } else {
    return false;
  }
}
// 更新状态
void HaierIrAcYrh63::publish_state_() {
  if (!get_power()) {
    mode = climate::CLIMATE_MODE_OFF;
  } else {
    set_health_switch();

    set_lock_switch();

    set_fresh_select();

    set_sterilize_select();

    set_vertical_select();

    set_horizontal_select();

    if (get_sleep()) {
      preset = climate::CLIMATE_PRESET_SLEEP;
    } else {
      preset = climate::CLIMATE_PRESET_NONE;
    }

    target_temperature = get_temp();

    switch (get_fan()) {
      case kHaierAcYrh63FanLow:
        fan_mode = climate::CLIMATE_FAN_LOW;
        break;
      case kHaierAcYrh63FanMed:
        fan_mode = climate::CLIMATE_FAN_MEDIUM;
        break;
      case kHaierAcYrh63FanHigh:
        fan_mode = climate::CLIMATE_FAN_HIGH;
        break;
      case kHaierAcYrh63FanAuto:
        fan_mode = climate::CLIMATE_FAN_AUTO;
        break;
      default:
        break;
    }

    switch (get_mode()) {
      case kHaierAcYrh63Auto:
        mode = climate::CLIMATE_MODE_AUTO;
        break;
      case kHaierAcYrh63Cool:
        mode = climate::CLIMATE_MODE_COOL;
        break;
      case kHaierAcYrh63Dry:
        mode = climate::CLIMATE_MODE_DRY;
        break;
      case kHaierAcYrh63Heat:
        mode = climate::CLIMATE_MODE_HEAT;
        break;
      case kHaierAcYrh63Fan:
        mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;
      default:
        break;
    }
  }

  publish_state();
}

}  // namespace haier_ir_acyrh63
}  // namespace esphome
