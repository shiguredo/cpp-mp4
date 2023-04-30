#include "shiguredo/mp4/track/opus.hpp"

#include <iterator>
#include <stdexcept>
#include <vector>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

OpusTrack::OpusTrack(const OpusTrackParameters& params)
    : m_pre_skip(params.pre_skip), m_roll_distance(params.roll_distance) {
  m_timescale = 48000;
  m_duration = params.duration;
  m_media_time = params.media_time;
  m_mvhd_timescale = params.writer->getMvhdTimescale();
  m_time_from_epoch = params.writer->getTimeFromEpoch();
  m_track_id = params.track_id;
  if (m_track_id == 0) {
    throw std::runtime_error("OpusTrack::OpusTrack(): invalid track_id=0");
  }
  m_writer = params.writer;
}

void OpusTrack::makeStsdBoxInfo(BoxInfo* stbl) {
  auto stsd = new BoxInfo({.parent = stbl, .box = new box::Stsd({.entry_count = 1})});
  auto opus = new BoxInfo({.parent = stsd,
                           .box = new box::AudioSampleEntry({
                               .type = BoxType("Opus"),
                               .data_reference_index = 1,
                               .entry_version = 0,
                               .channel_count = 2,
                               .sample_size = 16,
                               .sample_rate = 48000L << 16,
                           })});
  new BoxInfo({.parent = opus,
               .box = new box::DOps({.output_channel_count = 2,
                                     .pre_skip = static_cast<std::uint16_t>(m_pre_skip),
                                     .input_sample_rate = 48000,
                                     .output_gain = 0,
                                     .channel_mapping_family = 0})});
}

void OpusTrack::appendTrakBoxInfo(BoxInfo* moov) {
  finalize();

  auto trak = makeTrakBoxInfo(moov);
  makeTkhdBoxInfo(trak);

  auto edts = makeEdtsBoxInfo(trak);
  makeElstBoxInfo(edts);

  auto mdia = makeMdiaBoxInfo(trak);
  makeMdhdBoxInfo(mdia);
  makeHdlrBoxInfo(mdia);

  auto minf = makeMinfBoxInfo(mdia);
  makeSmhdBoxInfo(minf);
  makeDinfBoxInfo(minf);

  auto stbl = makeStblBoxInfo(minf);
  makeStsdBoxInfo(stbl);
  makeSttsBoxInfo(stbl);
  makeStscBoxInfo(stbl);
  makeStszBoxInfo(stbl);
  makeOffsetBoxInfo(stbl);
  makeSgpdBoxInfo(stbl);
  makeSbgpBoxInfo(stbl);
}

void OpusTrack::makeSgpdBoxInfo(BoxInfo* stbl) {
  // https://github.com/VFR-maniac/Mp4Opus/issues/4
  // ffmpeg and vimeo seem to use roll_distance[0] = -4
  // https://vfrmaniac.fushizen.eu/contents/opus_in_isobmff.html uses roll_distance[0] = -2
  new BoxInfo({.parent = stbl,
               .box = new box::Sgpd({.roll_distances = {box::RollDistance({.roll_distance = m_roll_distance})}})});
}

void OpusTrack::makeSbgpBoxInfo(BoxInfo* stbl) {
  new BoxInfo({.parent = stbl,
               .box = new box::Sbgp({.entries = {box::SbgpEntry(
                                         {.sample_count = static_cast<std::uint32_t>(std::size(m_mdat_sample_sizes)),
                                          .group_description_index = 1})}})});
}

void OpusTrack::addData(const std::uint64_t timestamp, const std::vector<std::uint8_t>& data, bool is_key) {
  addMdatData(timestamp, data, is_key);
}

void OpusTrack::addData(const std::uint64_t timestamp,
                        const std::uint8_t* data,
                        const std::size_t data_size,
                        bool is_key) {
  addMdatData(timestamp, data, data_size, is_key);
}

}  // namespace shiguredo::mp4::track
