#include "shiguredo/mp4/endian/endian.hpp"

#include <array>
#include <cstdint>

namespace shiguredo::mp4::endian {

std::uint32_t be_to_uint32(const std::uint8_t d0, const std::uint8_t d1, const std::uint8_t d2, const std::uint8_t d3) {
  return (static_cast<std::uint32_t>(d0) << 24) + (static_cast<std::uint32_t>(d1) << 16) +
         (static_cast<std::uint32_t>(d2) << 8) + (static_cast<std::uint32_t>(d3));
}

std::uint32_t be_to_uint32(const std::array<std::uint8_t, 4>& a) {
  return be_to_uint32(a[0], a[1], a[2], a[3]);
}

std::uint64_t be_to_uint64(const std::uint8_t d0,
                           const std::uint8_t d1,
                           const std::uint8_t d2,
                           const std::uint8_t d3,
                           const std::uint8_t d4,
                           const std::uint8_t d5,
                           const std::uint8_t d6,
                           const std::uint8_t d7) {
  return (static_cast<std::uint64_t>(d0) << 56) + (static_cast<std::uint64_t>(d1) << 48) +
         (static_cast<std::uint64_t>(d2) << 40) + (static_cast<std::uint64_t>(d3) << 32) +
         (static_cast<std::uint64_t>(d4) << 24) + (static_cast<std::uint64_t>(d5) << 16) +
         (static_cast<std::uint64_t>(d6) << 8) + (static_cast<std::uint64_t>(d7));
}

std::uint64_t be_to_uint64(const std::array<std::uint8_t, 8>& a) {
  return be_to_uint64(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
}

std::array<std::uint8_t, 4> uint32_to_be(const std::uint32_t i) {
  return std::array<std::uint8_t, 4>{static_cast<std::uint8_t>((i >> 24) & 0xff),
                                     static_cast<std::uint8_t>((i >> 16) & 0xff),
                                     static_cast<std::uint8_t>((i >> 8) & 0xff), static_cast<std::uint8_t>((i)&0xff)};
}

std::array<std::uint8_t, 8> uint64_to_be(const std::uint64_t i) {
  return std::array<std::uint8_t, 8>{
      static_cast<std::uint8_t>((i >> 56) & 0xff), static_cast<std::uint8_t>((i >> 48) & 0xff),
      static_cast<std::uint8_t>((i >> 40) & 0xff), static_cast<std::uint8_t>((i >> 32) & 0xff),
      static_cast<std::uint8_t>((i >> 24) & 0xff), static_cast<std::uint8_t>((i >> 16) & 0xff),
      static_cast<std::uint8_t>((i >> 8) & 0xff),  static_cast<std::uint8_t>((i)&0xff)};
}

}  // namespace shiguredo::mp4::endian
