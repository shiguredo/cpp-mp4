#include "shiguredo/mp4/track/vide.hpp"

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"

namespace shiguredo::mp4::track {

VideTrack::VideTrack() {
  m_handler_type = HandlerType::vide;
}

void VideTrack::makeTkhdBoxInfo(BoxInfo* trak) {
  new BoxInfo({.parent = trak,
               .box = new box::Tkhd({
                   .flags = 0x000003,
                   .creation_time = m_time_from_epoch,
                   .modification_time = m_time_from_epoch,
                   .track_id = m_track_id,
                   .duration = getDurationInMvhdTimescale(),
                   .alternate_group = 0,
                   .volume = 0,
                   .width = m_width << 16,
                   .height = m_height << 16,
               })});
}

void VideTrack::makeVmhdBoxInfo(BoxInfo* minf) {
  new BoxInfo({.parent = minf, .box = new box::Vmhd({.flags = 0x000001})});
}

}  // namespace shiguredo::mp4::track
