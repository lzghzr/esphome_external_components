#pragma once

#include "esphome/core/helpers.h"
#include "esphome/components/remote_base/raw_protocol.h"
#include "esphome/components/remote_base/nec_protocol.h"
#include "esphome/components/remote_base/rc_switch_protocol.h"

std::string format_vector_with_prefix(const std::vector<uint8_t> &data) {
  std::string result;
  result = "[";
  for (size_t i = 0; i < data.size(); i++) {
    if (i > 0) {
      result += ", ";
    }
    result += "0x";
    result += format_hex(data[i]);
  }
  result += "]";
  return result;
}
std::string format_nec_data(const remote_base::NECData &data) {
  std::string result;
  result = "{type: nec, address: 0x";
  result += format_hex(data.address);
  result += ", command: 0x";
  result += format_hex(data.command);
  result += "}";
  return result;
}
std::string format_rc_switch_data(const remote_base::RCSwitchData &data) {
  std::string result;
  std::string code = format_bin(data.code);
  int index = code.find('1');
  result = "{type: rc_switch, code: ";
  result += code.substr(index);
  result += ", protocol: ";
  result += to_string(data.protocol);
  result += "}";
  return result;
}
std::vector<uint8_t> action_int_to_uint(const std::vector<long int> &data) {
  std::vector<uint8_t> result;
  result.reserve(data.size());
  for (int value : data) {
    result.push_back(static_cast<uint8_t>(value));
  }
  return result;
}
bool on_raw_dump(const char *TAG, std::vector<long int> src) {
  char buffer[256];
  uint32_t buffer_offset = 0;
  buffer_offset += sprintf(buffer, "Received Raw: ");

  for (int32_t i = 0; i < src.size() - 1; i++) {
    const int32_t value = src[i];
    const uint32_t remaining_length = sizeof(buffer) - buffer_offset;
    int written;

    if (i + 1 < src.size() - 1) {
      written = snprintf(buffer + buffer_offset, remaining_length, "%" PRId32 ", ", value);
    } else {
      written = snprintf(buffer + buffer_offset, remaining_length, "%" PRId32, value);
    }

    if (written < 0 || written >= int(remaining_length)) {
      buffer[buffer_offset] = '\0';
      ESP_LOGI(TAG, "%s", buffer);
      buffer_offset = 0;
      written = sprintf(buffer, "  ");
      if (i + 1 < src.size() - 1) {
        written += sprintf(buffer + written, "%" PRId32 ", ", value);
      } else {
        written += sprintf(buffer + written, "%" PRId32, value);
      }
    }

    buffer_offset += written;
  }
  if (buffer_offset != 0) {
    ESP_LOGI(TAG, "%s", buffer);
  }
  return true;
}
