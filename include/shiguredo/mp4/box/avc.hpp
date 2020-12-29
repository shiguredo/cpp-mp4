#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::bitio {

class Reader;
class Writer;

}  // namespace shiguredo::mp4::bitio

namespace shiguredo::mp4::box {

// https://martchus.no-ip.biz/gogs/Martchus/tagparser/src/1dc8fb4839eaaadc0ba0c5ebddf5c8efcccbbdf0/mediaformat.h#L199
enum AvcProfiles : std::uint8_t {
  AVCCavlc444IntraProfile = 0x2C,
  AVCBaselineProfile = 0x42,
  AVCMainProfile = 0x4D,
  AVCScalableBaselineProfile = 0x53,
  AVCScalableHighProfile = 0x56,
  AVCExtendedProfile = 0x58,
  AVCHighProfile = 0x64,
  AVCHigh10Profile = 0x6E,
  AVCHighMultiviewProfile = 0x76,
  AVCHigh422Profile = 0x7A,
  AVCStereoHighProfile = 0x80,
  AVCHighMultiviewDepthProfile = 0x8A,
  AVCHigh444Profile = 0x90,
  AVCHigh444PredictiveProfile = 0xF4
};

bool is_avc_high_profile(const std::uint8_t profile);

const std::int64_t AVCHighProfileMinimumSize = 4;

struct AVCParameterSetParameters {
  const std::vector<std::uint8_t> nal_unit;
};

class AVCParameterSet {
 public:
  AVCParameterSet() = default;
  explicit AVCParameterSet(const AVCParameterSetParameters&);

  std::string toString() const;
  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t getSize() const;
  std::uint64_t readData(bitio::Reader*);

 private:
  std::vector<std::uint8_t> m_nal_unit;
};

BoxType box_type_avcc();

struct AVCDecoderConfigurationParameters {
  const std::uint8_t configuration_version;
  const std::uint8_t profile;
  const std::uint8_t profile_compatibility;
  const std::uint8_t level;
  const std::uint8_t length_size_minus_one;
  const std::vector<AVCParameterSet> sequence_parameter_sets = {};
  const std::vector<AVCParameterSet> picture_parameter_sets = {};
  const bool high_profile_fields_enabled = false;
  const std::uint8_t chroma_format = 0;
  const std::uint8_t bit_depth_luma_minus8 = 0;
  const std::uint8_t bit_depth_chroma_minus8 = 0;
  const std::vector<AVCParameterSet> sequence_parameter_sets_ext = {};
};

class AVCDecoderConfiguration : public AnyTypeBox {
 public:
  AVCDecoderConfiguration();
  explicit AVCDecoderConfiguration(const AVCDecoderConfigurationParameters&);

  std::string toStringOnlyData() const override;
  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint8_t m_configuration_version;
  std::uint8_t m_profile;
  std::uint8_t m_profile_compatibility;
  std::uint8_t m_level;
  std::uint8_t m_reserved = 63;          // size=6
  std::uint8_t m_length_size_minus_one;  // size=2
  std::uint8_t m_reserved2 = 7;          // size=3
  std::vector<AVCParameterSet> m_sequence_parameter_sets;
  std::vector<AVCParameterSet> m_picture_parameter_sets;
  bool m_high_profile_fields_enabled;
  std::uint8_t m_reserved3 = 63;           // size=6
  std::uint8_t m_chroma_format;            // size=2
  std::uint8_t m_reserved4 = 31;           // size=5
  std::uint8_t m_bit_depth_luma_minus8;    // size=3
  std::uint8_t m_reserved5 = 31;           // size=5
  std::uint8_t m_bit_depth_chroma_minus8;  // size=3
  std::vector<AVCParameterSet> m_sequence_parameter_sets_ext;
};

}  // namespace shiguredo::mp4::box
