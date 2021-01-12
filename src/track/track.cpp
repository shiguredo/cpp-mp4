#include "shiguredo/mp4/track/track.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

void Track::terminateCurrentChunk() {
  if (m_current_chunk_info.number_of_samples > 0) {
    m_chunk_infos.push_back(m_current_chunk_info);
    m_current_chunk_info = {.offset = 0, .number_of_samples = 0};
  }
}

void Track::addMdatData(const std::uint64_t timestamp, const std::vector<std::uint8_t>& data, bool is_key) {
  addMdatData(timestamp, data.data(), std::size(data), is_key);
}

void Track::addMdatData(const std::uint64_t timestamp,
                        const std::uint8_t* data,
                        const std::size_t data_size,
                        bool is_key) {
  if (!m_current_chunk_info.initialized) {
    m_current_chunk_info.initialized = true;
    m_current_chunk_info.offset = m_writer->tellCurrentMdatOffset();
  }
  ++m_current_chunk_info.number_of_samples;

  if (!std::empty(m_mdat_sample_sizes)) {
    spdlog::trace("Track::addMdatData(): duration: {} {}", timestamp, m_prev_timestamp);
    m_sample_durations.push_back(static_cast<std::uint32_t>(timestamp - m_prev_timestamp));
  }
  m_prev_timestamp = timestamp;

  m_writer->addMdatData(data, data_size);
  m_mdat_sample_sizes.push_back(static_cast<std::uint32_t>(data_size));
  if (m_handler_type == HandlerType::vide && is_key) {
    m_key_sample_numbers.push_back(static_cast<std::uint32_t>(std::size(m_mdat_sample_sizes)));
  }
}

void Track::finalize() {
  if (m_finalized) {
    return;
  }
  m_finalized = true;
  if (m_current_chunk_info.number_of_samples > 0) {
    m_chunk_infos.push_back(m_current_chunk_info);
  }

  if (!std::empty(m_mdat_sample_sizes)) {
    m_sample_durations.push_back(static_cast<std::uint32_t>(getDurationInTimescale() - m_prev_timestamp));
  }
}

void Track::makeDinfBoxInfo(BoxInfo* minf) {
  auto dinf = new BoxInfo({.parent = minf, .box = new box::Dinf()});
  auto dref = new BoxInfo({.parent = dinf, .box = new box::Dref({.entry_count = 1})});
  new BoxInfo({.parent = dref, .box = new box::Url({.flags = box::UrlSelfContainedFlags})});
}

std::array<std::uint8_t, 4> Track::getHandlerTypeArray() {
  if (m_handler_type == HandlerType::vide) {
    return {'v', 'i', 'd', 'e'};
  } else if (m_handler_type == HandlerType::soun) {
    return {'s', 'o', 'u', 'n'};
  }
  throw std::logic_error(fmt::format("Track::getHandlerTypeArray(): invalid handler type: {}", m_handler_type));
}

void make_stts_entries(std::vector<box::SttsEntry>* entries, const std::vector<std::uint32_t>& sample_durations) {
  entries->clear();
  entries->reserve(std::size(sample_durations));
  if (!std::empty(sample_durations)) {
    auto duration = sample_durations[0];
    std::uint32_t count = 1;
    for (std::size_t i = 1; i < std::size(sample_durations); ++i) {
      if (duration == sample_durations[i]) {
        ++count;
      } else {
        entries->push_back(box::SttsEntry({.sample_count = count, .sample_duration = duration}));
        duration = sample_durations[i];
        count = 1;
      }
    }
    if (count > 0) {
      entries->push_back(box::SttsEntry({.sample_count = count, .sample_duration = duration}));
    }
  }
}

void Track::makeSttsBoxInfo(BoxInfo* stbl) {
  std::vector<box::SttsEntry> stts_entries;
  make_stts_entries(&stts_entries, m_sample_durations);

  new BoxInfo({.parent = stbl, .box = new box::Stts({.entries = {stts_entries}})});
}

void make_stsc_entries(std::vector<box::StscEntry>* entries, const std::vector<ChunkInfo>& chunk_offsets) {
  entries->clear();
  entries->reserve(std::size(chunk_offsets));
  std::int64_t samples_per_chunk = -1;
  std::uint32_t chunk = 1;
  for (const auto& i : chunk_offsets) {
    if (samples_per_chunk != static_cast<std::int64_t>(i.number_of_samples)) {
      entries->push_back(box::StscEntry(
          {.first_chunk = chunk, .samples_per_chunk = i.number_of_samples, .sample_description_index = 1}));
      samples_per_chunk = static_cast<std::int64_t>(i.number_of_samples);
    }
    ++chunk;
  }
}

void Track::makeStscBoxInfo(BoxInfo* stbl) {
  std::vector<box::StscEntry> stsc_entries;
  make_stsc_entries(&stsc_entries, m_chunk_infos);
  new BoxInfo({.parent = stbl, .box = new box::Stsc({.entries = stsc_entries})});
}

Box* make_offset_box(const std::vector<ChunkInfo>& chunk_infos) {
  bool use_co64 = false;
  if (!std::empty(chunk_infos)) {
    auto last = chunk_infos.back();
    if (last.offset > (std::numeric_limits<std::uint32_t>::max())) {
      use_co64 = true;
    }
  }

  if (use_co64) {
    std::vector<std::uint64_t> chunk_offsets;
    std::transform(std::begin(chunk_infos), std::end(chunk_infos), std::back_inserter(chunk_offsets),
                   [](const auto e) { return e.offset; });
    return new box::Co64({.chunk_offsets = chunk_offsets});
  } else {
    std::vector<std::uint32_t> chunk_offsets;
    std::transform(std::begin(chunk_infos), std::end(chunk_infos), std::back_inserter(chunk_offsets),
                   [](const auto e) { return static_cast<std::uint32_t>(e.offset); });
    return new box::Stco({.chunk_offsets = chunk_offsets});
  }
}

void Track::makeOffsetBoxInfo(BoxInfo* stbl) {
  new BoxInfo({.parent = stbl, .box = make_offset_box(m_chunk_infos)});
}

void Track::setMediaTime(const std::int64_t media_time) {
  m_media_time = media_time;
}

std::uint64_t Track::getDurationInTimescale() const {
  return static_cast<std::uint64_t>(static_cast<float>(m_timescale) * m_duration);
}

std::uint64_t Track::getDurationInMvhdTimescale() const {
  return static_cast<std::uint64_t>(static_cast<float>(m_mvhd_timescale) * m_duration);
}

BoxInfo* Track::makeTrakBoxInfo(BoxInfo* moov) {
  return new BoxInfo({.parent = moov, .box = new box::Trak()});
}

BoxInfo* Track::makeEdtsBoxInfo(BoxInfo* trak) {
  return new BoxInfo({.parent = trak, .box = new box::Edts()});
}

void Track::makeElstBoxInfo(BoxInfo* edts) {
  new BoxInfo({.parent = edts,
               .box = new box::Elst({.entries = {box::ElstEntry({.track_duration = getDurationInMvhdTimescale(),
                                                                 .media_time = m_media_time})}})});
}

BoxInfo* Track::makeMdiaBoxInfo(BoxInfo* trak) {
  return new BoxInfo({.parent = trak, .box = new box::Mdia()});
}

void Track::makeMdhdBoxInfo(BoxInfo* mdia) {
  new BoxInfo({.parent = mdia,
               .box = new box::Mdhd({.creation_time = m_time_from_epoch,
                                     .modification_time = m_time_from_epoch,
                                     .timescale = m_timescale,
                                     .duration = getDurationInTimescale()})});
}

void Track::makeHdlrBoxInfo(BoxInfo* mdia, const std::string& name) {
  new BoxInfo({.parent = mdia, .box = new box::Hdlr({.handler_type = getHandlerTypeArray(), .name = name})});
}

BoxInfo* Track::makeMinfBoxInfo(BoxInfo* mdia) {
  return new BoxInfo({.parent = mdia, .box = new box::Minf()});
}

BoxInfo* Track::makeStblBoxInfo(BoxInfo* minf) {
  return new BoxInfo({.parent = minf, .box = new box::Stbl()});
}

void Track::makeStszBoxInfo(BoxInfo* stbl) {
  new BoxInfo({.parent = stbl, .box = new box::Stsz({.entry_sizes = m_mdat_sample_sizes})});
}

void Track::makeStssBoxInfo(BoxInfo* stbl) {
  new BoxInfo({.parent = stbl, .box = new box::Stss({.sample_numbers = m_key_sample_numbers})});
}

std::uint64_t Track::getTimescale() const {
  return m_timescale;
}

void Track::resetChunkOffsets(std::uint64_t diff) {
  finalize();
  for (auto& co : m_chunk_infos) {
    co.offset += diff;
  }
}

}  // namespace shiguredo::mp4::track
