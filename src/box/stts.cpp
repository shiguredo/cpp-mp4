#include "shiguredo/mp4/box/stts.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cstdint>
#include <istream>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_stts() {
  return BoxType("stts");
}

Stts::Stts() {
  m_type = box_type_stts();
}

Stts::Stts(const SttsParameters& params) : m_entries(params.entries) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_stts();
}

std::string Stts::toStringOnlyData() const {
  std::vector<std::string> entries;
  std::transform(std::begin(m_entries), std::end(m_entries), std::back_inserter(entries),
                 [](const auto& e) { return e.toString(); });
  return fmt::format("{} EntryCount={} Entries=[{}]", getVersionAndFlagsString(), std::size(entries),
                     fmt::join(entries, ", "));
}

std::uint64_t Stts::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  std::uint32_t entry_count = static_cast<std::uint32_t>(std::size(m_entries));
  wbits += bitio::write_uint<std::uint32_t>(&writer, entry_count);
  wbits += std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL,
                           [&writer](const auto a, const auto& e) { return a + e.writeData(&writer); });
  return wbits;
}

std::uint64_t Stts::getDataSize() const {
  return 8 + 8 * std::size(m_entries);
}

std::uint64_t Stts::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  std::uint32_t entry_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &entry_count);
  m_entries.resize(entry_count);
  for (std::uint32_t i = 0; i < entry_count; ++i) {
    rbits += m_entries[i].readData(&reader);
  }
  return rbits;
}

SttsEntry::SttsEntry(const SttsEntryParameters& params)
    : m_sample_count(params.sample_count), m_sample_duration(params.sample_duration) {}

std::string SttsEntry::toString() const {
  return fmt::format("{{SampleCount={} SampleDuration={}}}", m_sample_count, m_sample_duration);
}

std::uint64_t SttsEntry::writeData(bitio::Writer* writer) const {
  return bitio::write_uint<std::uint32_t>(writer, m_sample_count) +
         bitio::write_uint<std::uint32_t>(writer, m_sample_duration);
}

std::uint64_t SttsEntry::readData(bitio::Reader* reader) {
  return bitio::read_uint<std::uint32_t>(reader, &m_sample_count) +
         bitio::read_uint<std::uint32_t>(reader, &m_sample_duration);
}

std::ostream& operator<<(std::ostream& os, const SttsEntry& entry) {
  os << entry.toString();
  return os;
}

}  // namespace shiguredo::mp4::box
