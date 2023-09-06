#pragma once

#include <cstdint>
#include <vector>

#include "shiguredo/mp4/track/vide.hpp"

namespace shiguredo::mp4 {

class BoxInfo;

namespace writer {

class Writer;

}

}  // namespace shiguredo::mp4

namespace shiguredo::mp4::track {

struct AV1TrackParameters {
  const std::uint32_t timescale;
  const std::int64_t media_time = 0;
  const float duration;
  const std::uint32_t track_id = 0;
  const std::uint32_t width;
  const std::uint32_t height;
  const std::uint8_t seq_profile = 0;
  const std::uint8_t seq_level_idx_0 = 0;
  const std::uint8_t seq_tier_0 = 0;
  const std::uint8_t chroma_subsampling_x = 1;
  const std::uint8_t chroma_subsampling_y = 1;
  const std::uint8_t chroma_sample_position = 0;
  const std::vector<std::uint8_t>& config_OBUs = {0xa,  0xb,  0x0,  0x0,  0x0,  0x4, 0x47,
                                                  0x7e, 0x1a, 0xff, 0xfc, 0xc0, 0x20};
  shiguredo::mp4::writer::Writer* writer;
};

class AV1Track : public VideTrack {
 public:
  explicit AV1Track(const AV1TrackParameters&);
  void appendTrakBoxInfo(BoxInfo*) override;
  void addData(const std::uint64_t, const std::vector<std::uint8_t>&, bool) override;
  void addData(const std::uint64_t, const std::uint8_t*, const std::size_t, bool) override;
  void setConfigOBUs(const std::vector<std::uint8_t>&);

 private:
  std::uint8_t m_seq_profile;
  std::uint8_t m_seq_level_idx_0;
  std::uint8_t m_seq_tier_0;
  std::uint8_t m_chroma_subsampling_x;
  std::uint8_t m_chroma_subsampling_y;
  std::uint8_t m_chroma_sample_position;
  std::vector<std::uint8_t> m_config_OBUs;
  void makeStsdBoxInfo(BoxInfo*);
};

}  // namespace shiguredo::mp4::track
