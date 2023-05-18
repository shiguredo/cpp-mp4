#pragma once

#include <cstdint>
#include <vector>

#include "shiguredo/mp4/box/avc.hpp"
#include "shiguredo/mp4/track/vide.hpp"

namespace shiguredo::mp4 {

class BoxInfo;

namespace writer {

class Writer;

}

}  // namespace shiguredo::mp4

namespace shiguredo::mp4::track {

struct H264TrackParameters {
  const std::uint32_t timescale;
  const std::int64_t media_time = 0;
  const float duration;
  const std::uint32_t track_id = 0;
  const std::uint32_t width;
  const std::uint32_t height;
  const std::uint8_t configuration_version = 0x1;
  const std::uint8_t profile = box::AvcProfiles::AVCBaselineProfile;
  const std::uint8_t profile_compatibility = 0xc0;
  const std::uint8_t level = 31;
  shiguredo::mp4::writer::Writer* writer;
};

class H264Track : public VideTrack {
 public:
  explicit H264Track(const H264TrackParameters&);
  void appendTrakBoxInfo(BoxInfo*) override;
  void addData(const std::uint64_t, const std::vector<std::uint8_t>&, bool) override;
  void addData(const std::uint64_t, const std::uint8_t*, const std::size_t, bool) override;

 private:
  void makeStsdBoxInfo(BoxInfo*);
  void appendSequenceParameterSets(const shiguredo::mp4::box::AVCParameterSet&);
  void appendPictureParameterSets(const shiguredo::mp4::box::AVCParameterSet&);

  std::uint8_t m_configuration_version;
  std::uint8_t m_profile;
  std::uint8_t m_profile_compatibility;
  std::uint8_t m_level;
  std::vector<shiguredo::mp4::box::AVCParameterSet> m_sequence_parameter_sets = {};
  std::vector<shiguredo::mp4::box::AVCParameterSet> m_picture_parameter_sets = {};
};

struct NalUnit {
  std::size_t start_code_size;
  std::size_t start;
  std::size_t end;
  std::uint8_t header;
};

bool operator==(NalUnit const& left, NalUnit const& right);

std::ostream& operator<<(std::ostream& os, const NalUnit& nu);

NalUnit find_next_nal_unit(const std::uint8_t*, const std::size_t, const std::size_t);

}  // namespace shiguredo::mp4::track
