#include "shiguredo/mp4/track/vpx.hpp"

#include <memory>
#include <stdexcept>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

VPXTrack::VPXTrack(const VPXTrackParameters& params) : m_codec(params.codec) {
  m_timescale = params.timescale;
  m_duration = params.duration;
  m_media_time = params.media_time;
  m_mvhd_timescale = params.writer->getMvhdTimescale();
  m_time_from_epoch = params.writer->getTimeFromEpoch();
  m_track_id = params.track_id;
  if (m_track_id == 0) {
    throw std::runtime_error("VPXTrack::VPXTrack(): invalid track_id=0");
  }
  m_width = params.width;
  m_height = params.height;
  m_writer = params.writer;
}

void VPXTrack::makeStsdBoxInfo(BoxInfo* stbl) {
  // https://www.webmproject.org/vp9/mp4/
  auto stsd = new BoxInfo({.parent = stbl, .box = new box::Stsd({.entry_count = 1})});
  auto vp0x = new BoxInfo({.parent = stsd,
                           .box = new box::VisualSampleEntry({
                               .type = BoxType(m_codec == VPXCodec::VP8 ? "vp08" : "vp09"),
                               .data_reference_index = 1,
                               .width = static_cast<std::uint16_t>(m_width),
                               .height = static_cast<std::uint16_t>(m_height),
                           })});
  new BoxInfo(
      {.parent = vp0x, .box = new box::VPCodecConfiguration({.version = 1, .level = 30, .matrix_coefficents = 5})});
  new BoxInfo({.parent = vp0x, .box = new box::Fiel({.field_count = 1, .field_ordering = 0})});
  new BoxInfo({.parent = vp0x, .box = new box::PixelAspectRatio({.h_spacing = 1})});
  new BoxInfo({.parent = vp0x,
               .box = new box::Btrt({.decoding_buffer_size = 0, .max_bitrate = 500000, .avg_bitrate = 300000})});
}

void VPXTrack::appendTrakBoxInfo(BoxInfo* moov) {
  finalize();

  auto trak = makeTrakBoxInfo(moov);
  makeTkhdBoxInfo(trak);

  auto edts = makeEdtsBoxInfo(trak);
  makeElstBoxInfo(edts);

  auto mdia = makeMdiaBoxInfo(trak);
  makeMdhdBoxInfo(mdia);
  makeHdlrBoxInfo(mdia);

  auto minf = makeMinfBoxInfo(mdia);
  makeVmhdBoxInfo(minf);
  makeDinfBoxInfo(minf);

  auto stbl = makeStblBoxInfo(minf);
  makeStsdBoxInfo(stbl);
  makeSttsBoxInfo(stbl);
  makeStssBoxInfo(stbl);
  makeStscBoxInfo(stbl);
  makeStszBoxInfo(stbl);
  makeOffsetBoxInfo(stbl);
}

}  // namespace shiguredo::mp4::track
