#pragma once

#include <cstdint>
#include <vector>

#include "shiguredo/mp4/track/soun.hpp"

namespace shiguredo::mp4 {

class BoxInfo;

namespace writer {

class Writer;

}

}  // namespace shiguredo::mp4

namespace shiguredo::mp4::track {

struct AACTrackParameters {
  const std::uint32_t timescale;
  const float duration;
  const std::int64_t media_time = 0;
  const std::uint32_t track_id = 0;
  const std::uint32_t buffer_size_db = 768;
  const std::uint32_t max_bitrate;
  const std::uint32_t avg_bitrate;
  shiguredo::mp4::writer::Writer* writer;
};

class AACTrack : public SounTrack {
 public:
  explicit AACTrack(const AACTrackParameters&);
  void appendTrakBoxInfo(BoxInfo*) override;
  void addData(const std::uint64_t, const std::vector<std::uint8_t>&, bool) override;
  void addData(const std::uint64_t, const std::uint8_t*, const std::size_t, bool) override;

 private:
  const std::uint32_t m_buffer_size_db;
  const std::uint32_t m_max_bitrate;
  const std::uint32_t m_avg_bitrate;
  const std::int16_t m_roll_distance = -1;
  void makeStsdBoxInfo(BoxInfo*);
  void makeSgpdBoxInfo(BoxInfo*);
  void makeSbgpBoxInfo(BoxInfo*);
};

}  // namespace shiguredo::mp4::track
