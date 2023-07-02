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
  m_seq_profile = params.seq_profile;
  m_seq_level_idx_0 = params.seq_level_idx_0;
  m_seq_tier_0 = params.seq_tier_0;
  m_chroma_subsampling_x = params.chroma_subsampling_x;
  m_chroma_subsampling_y = params.chroma_subsampling_y;
  m_chroma_sample_position = params.chroma_sample_position;
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
               .box = new box::AV1CodecConfiguration({.seq_profile = m_seq_profile,
                                                      .seq_level_idx_0 = m_seq_level_idx_0,
                                                      .seq_tier_0 = m_seq_tier_0,
                                                      .chroma_subsampling_x = m_chroma_subsampling_x,
                                                      .chroma_subsampling_y = m_chroma_subsampling_y,
                                                      .chroma_sample_position = m_chroma_sample_position,
                                                      .config_OBUs = m_config_OBUs})});
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

void AV1Track::addData(const std::uint64_t timestamp, const std::vector<std::uint8_t>& data, bool is_key) {
  addMdatData(timestamp, data, is_key);
}

void AV1Track::addData(const std::uint64_t timestamp,
                       const std::uint8_t* data,
                       const std::size_t data_size,
                       bool is_key) {
  addMdatData(timestamp, data, data_size, is_key);
}

}  // namespace shiguredo::mp4::track
