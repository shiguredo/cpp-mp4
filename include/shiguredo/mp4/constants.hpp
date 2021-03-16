#pragma once

#include <cstdint>

namespace shiguredo::mp4 {

class Constants {
 public:
  static const std::uint64_t SMALL_HEADER_SIZE = 8;
  static const std::uint64_t LARGE_HEADER_SIZE = 16;
  static const std::uint64_t VERSION_YEAR = 2021;
  static const std::uint64_t VERSION_NUMBER_OF_RELEASES = 2;
};

}  // namespace shiguredo::mp4
