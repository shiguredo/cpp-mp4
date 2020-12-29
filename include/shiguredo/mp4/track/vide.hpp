#pragma once

#include <cstdint>

#include "shiguredo/mp4/track/track.hpp"

namespace shiguredo::mp4 {

class BoxInfo;

}

namespace shiguredo::mp4::track {

class VideTrack : public Track {
 protected:
  VideTrack();
  void makeTkhdBoxInfo(BoxInfo*);
  void makeVmhdBoxInfo(BoxInfo*);
  std::uint32_t m_width;
  std::uint32_t m_height;
};

}  // namespace shiguredo::mp4::track
