#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct VPCodecConfigurationParameters {
  const std::uint8_t version = 1;
  const std::uint32_t flags = 0x000000;
  const std::uint8_t profile = 0;
  const std::uint8_t level = 21;
  const std::uint8_t bit_depth = 8;
  const std::uint8_t chroma_sub_sampling = 1;
  const std::uint8_t video_full_range_flag = 0;
  const std::uint8_t colour_primaries = 2;
  const std::uint8_t transfer_characteristics = 2;
  const std::uint8_t matrix_coefficients = 2;
  const std::vector<std::uint8_t> codec_initialization_data = {};
};

BoxType box_type_vpcc();

class VPCodecConfiguration : public FullBox {
 public:
  VPCodecConfiguration();
  explicit VPCodecConfiguration(const VPCodecConfigurationParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t readData(std::istream&) override;
  std::uint64_t getDataSize() const override;

 private:
  std::uint8_t m_profile;
  std::uint8_t m_level;
  std::uint8_t m_bit_depth;
  std::uint8_t m_chroma_sub_sampling;
  std::uint8_t m_video_full_range_flag;
  std::uint8_t m_colour_primaries;
  std::uint8_t m_transfer_characteristics;
  std::uint8_t m_matrix_coefficients;
  std::vector<std::uint8_t> m_codec_initialization_data;
};

}  // namespace shiguredo::mp4::box
