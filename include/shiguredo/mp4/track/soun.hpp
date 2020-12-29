#pragma once

#include "shiguredo/mp4/track/track.hpp"

namespace shiguredo::mp4 {

class BoxInfo;

}

namespace shiguredo::mp4::track {

class SounTrack : public Track {
 protected:
  SounTrack();
  void makeTkhdBoxInfo(BoxInfo*);
  void makeSmhdBoxInfo(BoxInfo*);
};

}  // namespace shiguredo::mp4::track
