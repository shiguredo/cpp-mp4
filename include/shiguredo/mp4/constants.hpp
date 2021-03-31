#pragma once

#include <cstdint>

namespace shiguredo::mp4 {

class Constants {
 public:
  static const std::uint64_t SMALL_HEADER_SIZE = 8;
  static const std::uint64_t LARGE_HEADER_SIZE = 16;
};

}  // namespace shiguredo::mp4
