#pragma once

#include "esphome/core/helpers.h"
#include "esphome/components/remote_base/raw_protocol.h"
#include "esphome/components/remote_base/nec_protocol.h"
#include "esphome/components/remote_base/rc_switch_protocol.h"

// 格式化 vector<uint8_t> 为 "[0xXX, 0xYY, ...]"
std::string format_vector_with_prefix(const std::vector<uint8_t> &data) {
  std::string result;
  result.reserve(data.size() * 5 + 2);
  result = "[";
  for (size_t i = 0; i < data.size(); ++i) {
    if (i > 0) result += ", ";
    result += "0x";
    result += format_hex(data[i]);
  }
  result += "]";
  return result;
}

// 格式化 NEC 数据
std::string format_nec_data(const remote_base::NECData &data) {
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "{type: nec, address: 0x%04X, command: 0x%04X}",
           data.address, data.command);
  return std::string(buffer);
}

// 格式化 RCSwitch 数据
std::string format_rc_switch_data(const remote_base::RCSwitchData &data) {
  std::string code_bin = format_bin(data.code);
  size_t pos = code_bin.find('1');
  if (pos == std::string::npos) {
    code_bin = std::string(24, '0');
  } else {
    code_bin.erase(0, pos);
    if (code_bin.length() >= 24) {
      code_bin.resize(24);
    } else {
      code_bin.insert(0, 24 - code_bin.length(), '0');
    }
  }
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "{type: rc_switch, code: %s, protocol: %d}",
           code_bin.c_str(), data.protocol);
  return std::string(buffer);
}

// 将 long int 转为 uint8_t
std::vector<uint8_t> action_int_to_uint(const std::vector<long int> &data) {
  std::vector<uint8_t> result;
  result.reserve(data.size());
  for (long int value : data) {
    result.push_back(static_cast<uint8_t>(value));
  }
  return result;
}

// 原始数据分块打印
bool on_raw_dump(const char *TAG, std::vector<long int> src) {
  char buffer[256];
  size_t pos = buf_append_printf(buffer, sizeof(buffer), 0, "Received Raw: ");

  for (int32_t i = 0; i < src.size() - 1; ++i) {
    const int32_t value = src[i];
    size_t prev_pos = pos;

    if (i + 1 < src.size() - 1) {
      pos = buf_append_printf(buffer, sizeof(buffer), pos, "%" PRId32 ", ", value);
    } else {
      pos = buf_append_printf(buffer, sizeof(buffer), pos, "%" PRId32, value);
    }

    if (pos >= sizeof(buffer) - 1) {
      // buffer full, flush and continue
      buffer[prev_pos] = '\0';
      ESP_LOGI(TAG, "%s", buffer);
      if (i + 1 < src.size() - 1) {
        pos = buf_append_printf(buffer, sizeof(buffer), 0, "  %" PRId32 ", ", value);
      } else {
        pos = buf_append_printf(buffer, sizeof(buffer), 0, "  %" PRId32, value);
      }
    }
  }
  if (pos != 0) {
    ESP_LOGI(TAG, "%s", buffer);
  }
  return true;
}
