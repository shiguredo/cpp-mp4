#include "shiguredo/mp4/track/av1.hpp"

#include <memory>
#include <stdexcept>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

AV1Track::AV1Track(const AV1TrackParameters& params) {
  m_timescale = params.timescale;
  m_duration = params.duration;
  m_media_time = params.media_time;
  m_mvhd_timescale = params.writer->getMvhdTimescale();
  m_time_from_epoch = params.writer->getTimeFromEpoch();
  m_track_id = params.track_id;
  if (m_track_id == 0) {
    throw std::runtime_error("AV1Track::AV1Track(): invalid track_id=0");
  }
  m_width = params.width;
  m_height = params.height;
  m_writer = params.writer;
}

void AV1Track::makeStsdBoxInfo(BoxInfo* stbl) {
  // https://www.webmproject.org/vp9/mp4/
  auto stsd = new BoxInfo({.parent = stbl, .box = new box::Stsd({.entry_count = 1})});
  auto av01 = new BoxInfo({.parent = stsd,
                           .box = new box::VisualSampleEntry({
                               .type = BoxType("av01"),
                               .data_reference_index = 1,
                               .width = static_cast<std::uint16_t>(m_width),
                               .height = static_cast<std::uint16_t>(m_height),
                           })});
  new BoxInfo({.parent = av01,
               .box = new box::AV1CodecConfiguration(
                   {.seq_profile = 0,
                    .seq_level_idx_0 = 0,
                    .seq_tier_0 = 0,
                    .chroma_subsampling_x = 0,
                    .chroma_subsampling_y = 0,
                    .chroma_sample_position = 0,
                    .config_OBUs = {0xa, 0xb, 0x0, 0x0, 0x0, 0x4, 0x47, 0x7e, 0x1a, 0xff, 0xfc, 0xc0, 0x20}})});
}

void AV1Track::appendTrakBoxInfo(BoxInfo* moov) {
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
