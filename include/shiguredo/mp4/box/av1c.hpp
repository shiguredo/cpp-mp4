#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct AV1CodecConfigurationParameters {
  const std::uint8_t marker = 1;
  const std::uint8_t version = 1;
  const std::uint8_t seq_profile;
  const std::uint8_t seq_level_idx_0;
  const std::uint8_t seq_tier_0;
  const bool high_bitdepth = false;
  const bool twelve_bit = false;
  const bool monochrome = false;
  const std::uint8_t chroma_subsampling_x;
  const std::uint8_t chroma_subsampling_y;
  const std::uint8_t chroma_sample_position;
  const bool initial_presentation_delay_present = false;
  const std::uint8_t initial_presentation_delay_minus_one = 0;
  const std::vector<std::uint8_t> config_OBUs = {};
};

BoxType box_type_av1c();

class AV1CodecConfiguration : public Box {
 public:
  AV1CodecConfiguration();
  explicit AV1CodecConfiguration(const AV1CodecConfigurationParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t readData(std::istream&) override;
  std::uint64_t getDataSize() const override;

 private:
  std::uint8_t m_marker = 1;
  std::uint8_t m_version = 1;
  std::uint8_t m_seq_profile;
  std::uint8_t m_seq_level_idx_0;
  std::uint8_t m_seq_tier_0;
  bool m_high_bitdepth;
  bool m_twelve_bit;
  bool m_monochrome;
  std::uint8_t m_chroma_subsampling_x;
  std::uint8_t m_chroma_subsampling_y;
  std::uint8_t m_chroma_sample_position;
  std::uint8_t m_reserved = 0;
  bool m_initial_presentation_delay_present;
  std::uint8_t m_initial_presentation_delay_minus_one;
  std::vector<std::uint8_t> m_config_OBUs;
};

}  // namespace shiguredo::mp4::box
