#include "shiguredo/mp4/box/ctts.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cstddef>
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

CttsEntry::CttsEntry(const std::uint32_t t_sample_cont, const std::int64_t t_sample_offset)
    : m_sample_count(t_sample_cont), m_sample_offset(t_sample_offset) {}

std::string CttsEntry::toString() const {
  return fmt::format("{{SampleCount={} SampleOffset={}}}", m_sample_count, m_sample_offset);
}

std::uint64_t CttsEntry::writeData(bitio::Writer* writer) const {
  std::uint64_t wbits = bitio::write_uint<std::uint32_t>(writer, m_sample_count);
  wbits += bitio::write_int<std::int64_t>(writer, m_sample_offset, 32);
  return wbits;
}

std::uint64_t CttsEntry::readData(bitio::Reader* reader) {
  std::uint64_t rbits = bitio::read_uint<std::uint32_t>(reader, &m_sample_count);
  rbits += bitio::read_int<std::int64_t>(reader, &m_sample_offset, 32);
  return rbits;
}

BoxType box_type_ctts() {
  return BoxType("ctts");
}

Ctts::Ctts() {
  m_type = box_type_ctts();
}

Ctts::Ctts(const CttsParameters& params) : m_entries(params.entries) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_ctts();
}

std::string Ctts::toStringOnlyData() const {
  std::vector<std::string> entries;
  std::transform(std::begin(m_entries), std::end(m_entries), std::back_inserter(entries),
                 [](const auto& e) { return e.toString(); });

  return fmt::format("{} EntryCount={} Entries=[{}]", getVersionAndFlagsString(), std::size(m_entries),
                     fmt::join(entries, ", "));
}

std::uint64_t Ctts::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(std::size(m_entries)));
  wbits += std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL,
                           [&writer](const auto a, const auto& e) { return a + e.writeData(&writer); });
  return wbits;
}

std::uint64_t Ctts::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  std::uint32_t entry_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &entry_count);
  m_entries.resize(entry_count);
  for (size_t i = 0; i < entry_count; ++i) {
    rbits += m_entries[i].readData(&reader);
  }
  return rbits;
}

std::uint64_t Ctts::getDataSize() const {
  return 8 + 8 * std::size(m_entries);
}

}  // namespace shiguredo::mp4::box
