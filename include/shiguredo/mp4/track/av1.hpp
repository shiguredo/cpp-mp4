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
  shiguredo::mp4::writer::Writer* writer;
};

class AV1Track : public VideTrack {
 public:
  explicit AV1Track(const AV1TrackParameters&);
  void appendTrakBoxInfo(BoxInfo*) override;
  void addData(const std::uint64_t, const std::vector<std::uint8_t>&, bool) override;
  void addData(const std::uint64_t, const std::uint8_t*, const std::size_t, bool) override;

 private:
  void makeStsdBoxInfo(BoxInfo*);
};

}  // namespace shiguredo::mp4::track
