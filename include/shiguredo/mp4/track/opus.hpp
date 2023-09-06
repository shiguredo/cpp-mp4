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

struct OpusTrackParameters {
  const std::uint64_t pre_skip;
  const float duration;
  const std::int64_t media_time = 0;
  const std::int16_t roll_distance = -4;
  const std::uint32_t track_id = 0;
  shiguredo::mp4::writer::Writer* writer;
};

class OpusTrack : public SounTrack {
 public:
  explicit OpusTrack(const OpusTrackParameters&);
  void appendTrakBoxInfo(BoxInfo*) override;
  void addData(const std::uint64_t, const std::vector<std::uint8_t>&, bool) override;
  void addData(const std::uint64_t, const std::uint8_t*, const std::size_t, bool) override;

 private:
  const std::uint64_t m_pre_skip;
  const std::int16_t m_roll_distance;

  void makeStsdBoxInfo(BoxInfo*);
  void makeSgpdBoxInfo(BoxInfo*);
  void makeSbgpBoxInfo(BoxInfo*);
};

}  // namespace shiguredo::mp4::track
