#include "shiguredo/mp4/track/mp3.hpp"

#include <memory>
#include <stdexcept>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

MP3Track::MP3Track(const MP3TrackParameters& params)
    : m_buffer_size_db(params.buffer_size_db), m_max_bitrate(params.max_bitrate), m_avg_bitrate(params.avg_bitrate) {
  m_timescale = params.timescale;
  m_duration = params.duration;
  m_media_time = params.media_time;
  m_mvhd_timescale = params.writer->getMvhdTimescale();
  m_time_from_epoch = params.writer->getTimeFromEpoch();
  m_track_id = params.track_id;
  if (m_track_id == 0) {
    throw std::runtime_error("MP3Track::MP3Track(): invalid track_id=0");
  }
  m_writer = params.writer;
}

void MP3Track::makeStsdBoxInfo(BoxInfo* stbl) {
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
      new box::DecoderConfigDescriptor({.object_type_indication = 0x6b,  // Audio ISO/IEC 11172-3 (MPEG-1 Audio)
                                        .stream_type = 0x05,             // AudioStream
                                        .buffer_size_db = m_buffer_size_db,
                                        .max_bitrate = m_max_bitrate,
                                        .avg_bitrate = m_avg_bitrate}));
  std::shared_ptr<box::SLConfigDescr> scd(
      new box::SLConfigDescr({.data = {0x02}}));  // predefined, Reserved for use in MP4 files
  std::shared_ptr<box::ESDescriptor> esd(new box::ESDescriptor({.ESID = 0}));
  esd->addSubDescriptor(dcd);
  esd->addSubDescriptor(scd);
  new BoxInfo({.parent = mp4a, .box = new box::Esds({.descriptors = {esd, dcd, scd}})});
}

void MP3Track::appendTrakBoxInfo(BoxInfo* moov) {
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
}

void MP3Track::addData(const std::uint64_t timestamp, const std::vector<std::uint8_t>& data, bool is_key) {
  addMdatData(timestamp, data, is_key);
}

void MP3Track::addData(const std::uint64_t timestamp,
                       const std::uint8_t* data,
                       const std::size_t data_size,
                       bool is_key) {
  addMdatData(timestamp, data, data_size, is_key);
}

}  // namespace shiguredo::mp4::track
