#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct MvhdParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint64_t creation_time;
  const std::uint64_t modification_time;
  const std::uint32_t timescale = 600;
  const std::uint64_t duration;
  const std::int32_t rate = 1 << 16;
  const std::int16_t volume = 256;
  const std::array<std::int32_t, 9> matrix = {1 << 16, 0, 0, 0, 1 << 16, 0, 0, 0, 1 << 30};
  const std::array<std::int32_t, 6> pre_defined = {0, 0, 0, 0, 0, 0};
  const std::uint32_t next_track_id;
};

BoxType box_type_mvhd();

class Mvhd : public FullBox {
 public:
  Mvhd();
  explicit Mvhd(const MvhdParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

  double getRate() const;

  void setNextTrackID(const std::uint32_t);

 private:
  std::uint64_t m_creation_time;
  std::uint64_t m_modification_time;
  std::uint32_t m_timescale;
  std::uint64_t m_duration;
  std::int32_t m_rate;
  std::int16_t m_volume;
  std::int16_t m_reserved = 0;
  std::uint32_t m_reserved2 = 0;
  std::uint32_t m_reserved3 = 0;
  std::array<std::int32_t, 9> m_matrix;
  std::array<std::int32_t, 6> m_pre_defined;
  std::uint32_t m_next_track_id;
};

}  // namespace shiguredo::mp4::box
