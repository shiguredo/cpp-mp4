#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct MdhdParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint64_t creation_time;
  const std::uint64_t modification_time;
  const std::uint32_t timescale;
  const std::uint64_t duration;
  const bool pad = false;
  const std::array<std::uint8_t, 3> language = {'u', 'n', 'd'};
  const std::uint16_t pre_defined = 0;
};

BoxType box_type_mdhd();

class Mdhd : public FullBox {
 public:
  Mdhd();
  explicit Mdhd(const MdhdParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint64_t m_creation_time;
  std::uint64_t m_modification_time;
  std::uint32_t m_timescale;
  std::uint64_t m_duration;
  bool m_pad;
  std::array<std::uint8_t, 3> m_language;
  std::uint16_t m_pre_defined;
};

std::uint16_t encode_language(const std::array<std::uint8_t, 3>&);
std::array<std::uint8_t, 3> decode_language(const uint16_t enc);

}  // namespace shiguredo::mp4::box
