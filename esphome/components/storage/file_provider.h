#pragma once

namespace esphome {
namespace storage {

class File {
 public:
  virtual int32_t get_size() = 0;
  virtual int32_t seek(int32_t pos) = 0;
  virtual int32_t read(uint8_t *data, int32_t length) = 0;
  virtual int32_t write(uint8_t *data, int32_t length) = 0;
}

/// interface for components that provide files
class FileProvider {
 public:
  virtual File *open_file(const std::string &path) = 0;
  virtual int close_file(File *) = 0;
};

}  // namespace storage
}  // namespace esphome
