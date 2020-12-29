#include "shiguredo/mp4/box/stsc.hpp"

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

BoxType box_type_stsc() {
  return BoxType("stsc");
}

Stsc::Stsc() {
  m_type = box_type_stsc();
}

Stsc::Stsc(const StscParameters& params) : m_entries(params.entries) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_stsc();
}

std::string Stsc::toStringOnlyData() const {
  std::vector<std::string> entries;
  std::transform(std::begin(m_entries), std::end(m_entries), std::back_inserter(entries),
                 [](const auto& e) { return e.toString(); });
  return fmt::format("{} EntryCount={} Entries=[{}]", getVersionAndFlagsString(), std::size(entries),
                     fmt::join(entries, ", "));
}

std::uint64_t Stsc::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  std::uint32_t entry_count = static_cast<std::uint32_t>(std::size(m_entries));
  wbits += bitio::write_uint<std::uint32_t>(&writer, entry_count);
  wbits += std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL,
                           [&writer](const auto a, const auto& e) { return a + e.writeData(&writer); });
  return wbits;
}

std::uint64_t Stsc::getDataSize() const {
  return 8 + 12 * std::size(m_entries);
}

std::uint64_t Stsc::readData(std::istream& is) {
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

StscEntry::StscEntry(const StscEntryParameters& params)
    : m_first_chunk(params.first_chunk),
      m_sample_per_chunk(params.samples_per_chunk),
      m_sample_description_index(params.sample_description_index) {}

std::string StscEntry::toString() const {
  return fmt::format("{{FirstChunk={} SamplesPerChunk={} SampleDescriptionIndex={}}}", m_first_chunk,
                     m_sample_per_chunk, m_sample_description_index);
}

std::uint64_t StscEntry::writeData(bitio::Writer* writer) const {
  return bitio::write_uint<std::uint32_t>(writer, m_first_chunk) +
         bitio::write_uint<std::uint32_t>(writer, m_sample_per_chunk) +
         bitio::write_uint<std::uint32_t>(writer, m_sample_description_index);
}

std::uint64_t StscEntry::readData(bitio::Reader* reader) {
  return bitio::read_uint<std::uint32_t>(reader, &m_first_chunk) +
         bitio::read_uint<std::uint32_t>(reader, &m_sample_per_chunk) +
         bitio::read_uint<std::uint32_t>(reader, &m_sample_description_index);
}

std::ostream& operator<<(std::ostream& os, const StscEntry& entry) {
  os << entry.toString();
  return os;
}

}  // namespace shiguredo::mp4::box
