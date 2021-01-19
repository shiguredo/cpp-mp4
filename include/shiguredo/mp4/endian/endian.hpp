#pragma once

#include <array>
#include <cstdint>

namespace shiguredo::mp4::endian {

std::uint32_t be_to_uint32(const std::uint8_t d0, const std::uint8_t d1, const std::uint8_t d2, const std::uint8_t d3);
std::uint32_t be_to_uint32(const std::array<std::uint8_t, 4>& a);

std::uint64_t be_to_uint64(const std::uint8_t d0,
                           const std::uint8_t d1,
                           const std::uint8_t d2,
                           const std::uint8_t d3,
                           const std::uint8_t d4,
                           const std::uint8_t d5,
                           const std::uint8_t d6,
                           const std::uint8_t d7);
std::uint64_t be_to_uint64(const std::array<std::uint8_t, 8>& a);

std::array<std::uint8_t, 4> uint32_to_be(const std::uint32_t i);
std::array<std::uint8_t, 8> uint64_to_be(const std::uint64_t i);

}  // namespace shiguredo::mp4::endian
