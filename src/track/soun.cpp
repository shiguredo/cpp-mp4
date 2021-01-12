#include "shiguredo/mp4/track/soun.hpp"

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"

namespace shiguredo::mp4::track {

SounTrack::SounTrack() {
  m_handler_type = HandlerType::soun;
}

void SounTrack::makeTkhdBoxInfo(BoxInfo* trak) {
  new BoxInfo({.parent = trak,
               .box = new box::Tkhd({.flags = 0x000003,
                                     .creation_time = m_time_from_epoch,
                                     .modification_time = m_time_from_epoch,
                                     .track_id = m_track_id,
                                     .duration = getDurationInMvhdTimescale()})});
}

void SounTrack::makeSmhdBoxInfo(BoxInfo* minf) {
  new BoxInfo({.parent = minf, .box = new box::Smhd({.balance = 0})});
}

}  // namespace shiguredo::mp4::track
