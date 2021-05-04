#include "shiguredo/mp4/box/elst.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

ElstEntry::ElstEntry(const ElstEntryParameters& params)
    : m_track_duration(params.track_duration), m_media_time(params.media_time), m_media_rate(params.media_rate) {}

std::string ElstEntry::toString() const {
  return fmt::format("{{TrackDuration={} MediaTime={} MediaRate={:.4f}}}", m_track_duration, m_media_time,
                     getMediaRate());
}

std::uint64_t ElstEntry::writeData(bitio::Writer* writer, const std::uint8_t version) const {
  std::uint64_t wbits = bitio::write_uint<std::uint64_t>(writer, m_track_duration, version == 0 ? 32 : 64);
  wbits += bitio::write_int<std::int64_t>(writer, m_media_time, version == 0 ? 32 : 64);
  wbits += bitio::write_int<std::int32_t>(writer, m_media_rate);
  return wbits;
}

std::uint64_t ElstEntry::readData(bitio::Reader* reader, const std::uint8_t version) {
  std::uint64_t rbits = bitio::read_uint<std::uint64_t>(reader, &m_track_duration, version == 0 ? 32 : 64);
  rbits += bitio::read_int<std::int64_t>(reader, &m_media_time, version == 0 ? 32 : 64);
  rbits += bitio::read_int<std::int32_t>(reader, &m_media_rate);
  return rbits;
}

double ElstEntry::getMediaRate() const {
  return static_cast<double>(m_media_rate) / (1 << 16);
}

BoxType box_type_elst() {
  return BoxType("elst");
}

Elst::Elst() {
  m_type = box_type_elst();
}

Elst::Elst(const ElstParameters& params) : m_entries(params.entries) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_elst();
}

std::string Elst::toStringOnlyData() const {
  std::vector<std::string> entries;
  std::transform(std::begin(m_entries), std::end(m_entries), std::back_inserter(entries),
                 [](const auto& e) { return e.toString(); });

  return fmt::format("{} EntryCount={} Entries=[{}]", getVersionAndFlagsString(), std::size(m_entries),
                     fmt::join(entries, ", "));
}

std::uint64_t Elst::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(std::size(m_entries)));

  wbits += std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL,
                           [&](const auto a, const auto& e) { return a + e.writeData(&writer, m_version); });
  return wbits;
}

std::uint64_t Elst::getDataSize() const {
  return 8 + std::size(m_entries) * (m_version == 0 ? 12 : 20);
}

std::uint64_t Elst::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  std::uint32_t entry_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &entry_count);
  m_entries.resize(entry_count);
  for (std::size_t i = 0; i < entry_count; ++i) {
    ElstEntry entry;
    rbits += entry.readData(&reader, m_version);
    m_entries[i] = entry;
  }
  return rbits;
}

}  // namespace shiguredo::mp4::box
