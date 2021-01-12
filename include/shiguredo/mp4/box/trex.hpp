#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct TrexParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint32_t track_id;
  const std::uint32_t default_sample_description_index;
  const std::uint32_t default_sample_duration;
  const std::uint32_t default_sample_size;
  const std::uint32_t default_sample_flags;
};

BoxType box_type_trex();

class Trex : public FullBox {
 public:
  Trex();
  explicit Trex(const TrexParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint32_t m_track_id;
  std::uint32_t m_default_sample_description_index;
  std::uint32_t m_default_sample_duration;
  std::uint32_t m_default_sample_size;
  std::uint32_t m_default_sample_flags;
};

}  // namespace shiguredo::mp4::box
