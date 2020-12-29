#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct TkhdParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint64_t creation_time;
  const std::uint64_t modification_time;
  const std::uint32_t track_id;
  const std::uint64_t duration;
  const std::int16_t layer = 0;
  const std::int16_t alternate_group = 0;
  const std::int16_t volume = 256;
  const std::array<std::int32_t, 9> matrix = {1 << 16, 0, 0, 0, 1 << 16, 0, 0, 0, 1 << 30};
  const std::uint32_t width = 0;
  const std::uint32_t height = 0;
};

BoxType box_type_tkhd();

class Tkhd : public FullBox {
 public:
  Tkhd();
  explicit Tkhd(const TkhdParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

  double getWidth() const;
  double getHeight() const;

 private:
  std::uint64_t m_creation_time;
  std::uint64_t m_modification_time;
  std::uint32_t m_track_id;
  std::uint32_t m_reserved = 0;
  std::uint64_t m_duration;
  std::uint32_t m_reserved2 = 0;
  std::uint32_t m_reserved3 = 0;
  std::int16_t m_layer;
  std::int16_t m_alternate_group;
  std::int16_t m_volume;
  std::uint16_t m_reserved4 = 0;
  std::array<std::int32_t, 9> m_matrix;
  std::uint32_t m_width;
  std::uint32_t m_height;
};

}  // namespace shiguredo::mp4::box
