#pragma once

namespace esphome {
namespace storage {

/// interface for components that provide raw data storage
class RawStorage {
 public:
  virtual int32_t get_size() = 0;
  virtual int32_t read(uint8_t *data, int32_t pos, int32_t length) = 0;
  virtual int32_t write(uint8_t *data, int32_t pos, int32_t length) = 0;
}

}  // namespace storage
}  // namespace esphome
