#include "shiguredo/mp4/track/aac.hpp"

#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

AACTrack::AACTrack(const AACTrackParameters& params)
    : m_buffer_size_db(params.buffer_size_db), m_max_bitrate(params.max_bitrate), m_avg_bitrate(params.avg_bitrate) {
  m_timescale = params.timescale;
  m_duration = params.duration;
  m_media_time = params.media_time;
  m_mvhd_timescale = params.writer->getMvhdTimescale();
  m_time_from_epoch = params.writer->getTimeFromEpoch();
  m_track_id = params.track_id;
  if (m_track_id == 0) {
    throw std::runtime_error("AACTrack::AACTrack(): invalid track_id=0");
  }
  m_writer = params.writer;
}

void AACTrack::makeStsdBoxInfo(BoxInfo* stbl) {
  auto stsd = new BoxInfo({.parent = stbl, .box = new box::Stsd({.entry_count = 1})});
  auto mp4a = new BoxInfo({.parent = stsd,
                           .box = new box::AudioSampleEntry({
                               .type = BoxType("mp4a"),
                               .data_reference_index = 1,
                               .entry_version = 0,
                               .channel_count = 2,
                               .sample_size = 16,
                               .sample_rate = m_timescale << 16,
                           })});
  std::shared_ptr<box::DecoderConfigDescriptor> dcd(
      new box::DecoderConfigDescriptor({.object_type_indication = 0x40,  // Audio ISO/IEC 14496-3 (MPEG-4 Audio)
                                        .stream_type = 0x05,             // AudioStream
                                        .buffer_size_db = m_buffer_size_db,
                                        .max_bitrate = m_max_bitrate,
                                        .avg_bitrate = m_avg_bitrate}));

  // AAC Sequence header
  // https://titanwolf.org/Network/Articles/Article?AID=973232a4-5330-4b50-bfa8-8bde11c31399
  std::shared_ptr<box::DecSpecificInfo> dsi(
      new box::DecSpecificInfo({.data = {0x11, 0x90}}));  // AACLC, @48khz, channel cfg 2
  std::shared_ptr<box::SLConfigDescr> scd(
      new box::SLConfigDescr({.data = {0x02}}));  // predefined, Reserved for use in MP4 files
  std::shared_ptr<box::ESDescriptor> esd(
      new box::ESDescriptor({.ESID = static_cast<std::uint16_t>(0xffff & m_track_id)}));
  esd->addSubDescriptor(dcd);
  dcd->addSubDescriptor(dsi);
  esd->addSubDescriptor(scd);
  new BoxInfo({.parent = mp4a, .box = new box::Esds({.descriptors = {esd, dcd, dsi, scd}})});
  new BoxInfo(
      {.parent = mp4a,
       .box = new box::Btrt({.decoding_buffer_size = 0, .max_bitrate = m_max_bitrate, .avg_bitrate = m_avg_bitrate})});
}

void AACTrack::appendTrakBoxInfo(BoxInfo* moov) {
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

void AACTrack::makeSgpdBoxInfo(BoxInfo* stbl) {
  // https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFAppenG/QTFFAppenG.html
  new BoxInfo({.parent = stbl,
               .box = new box::Sgpd({.roll_distances = {box::RollDistance({.roll_distance = m_roll_distance})}})});
}

void AACTrack::makeSbgpBoxInfo(BoxInfo* stbl) {
  new BoxInfo({.parent = stbl,
               .box = new box::Sbgp({.entries = {box::SbgpEntry(
                                         {.sample_count = static_cast<std::uint32_t>(std::size(m_mdat_sample_sizes)),
                                          .group_description_index = 1})}})});
}

void AACTrack::addData(const std::uint64_t timestamp, const std::vector<std::uint8_t>& data, bool is_key) {
  addMdatData(timestamp, data, is_key);
}

void AACTrack::addData(const std::uint64_t timestamp,
                       const std::uint8_t* data,
                       const std::size_t data_size,
                       bool is_key) {
  addMdatData(timestamp, data, data_size, is_key);
}

}  // namespace shiguredo::mp4::track
