#pragma once

#include <array>
#include <cstdint>

namespace shiguredo::mp4::endian {

std::uint32_t be_to_uint32(std::uint8_t d0, std::uint8_t d1, std::uint8_t d2, std::uint8_t d3);
std::uint32_t be_to_uint32(std::array<std::uint8_t, 4> a);

std::uint64_t be_to_uint64(std::uint8_t d0,
                           std::uint8_t d1,
                           std::uint8_t d2,
                           std::uint8_t d3,
                           std::uint8_t d4,
                           std::uint8_t d5,
                           std::uint8_t d6,
                           std::uint8_t d7);
std::uint64_t be_to_uint64(std::array<std::uint8_t, 8> a);

std::array<std::uint8_t, 4> uint32_to_be(std::uint32_t i);
std::array<std::uint8_t, 8> uint64_to_be(std::uint64_t i);

}  // namespace shiguredo::mp4::endian
