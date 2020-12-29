#include "shiguredo/mp4/box/sbgp.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cstdint>
#include <istream>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

SbgpEntry::SbgpEntry(const SbgpEntryParameters& params)
    : m_sample_count(params.sample_count), m_group_description_index(params.group_description_index) {}

std::string SbgpEntry::toString() const {
  return fmt::format("{{SampleCount={} GroupDescriptionIndex={}}}", m_sample_count, m_group_description_index);
}

std::uint64_t SbgpEntry::writeData(bitio::Writer* writer) const {
  auto wbits = bitio::write_uint<std::uint32_t>(writer, m_sample_count);
  return wbits + bitio::write_uint<std::uint32_t>(writer, m_group_description_index);
}

std::uint64_t SbgpEntry::readData(bitio::Reader* reader) {
  auto rbits = bitio::read_uint<std::uint32_t>(reader, &m_sample_count);
  return rbits + bitio::read_uint<std::uint32_t>(reader, &m_group_description_index);
}

BoxType box_type_sbgp() {
  return BoxType("sbgp");
}

Sbgp::Sbgp() {
  m_type = box_type_sbgp();
}

Sbgp::Sbgp(const SbgpParameters& params)
    : m_grouping_type(params.grouping_type),
      m_grouping_type_parameter(params.grouping_type_parameter),
      m_entries(params.entries) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_sbgp();
}

std::string Sbgp::toStringOnlyData() const {
  std::string grouping_type(std::begin(m_grouping_type), std::end(m_grouping_type));
  std::string s = fmt::format(R"({} GroupingType="{}")", getVersionAndFlagsString(), grouping_type);
  if (m_version != 0) {
    s += fmt::format(" GroupingTypeParameter={}", m_grouping_type_parameter);
  }
  std::vector<std::string> entries;
  std::transform(std::begin(m_entries), std::end(m_entries), std::back_inserter(entries),
                 [](const auto& e) { return e.toString(); });
  return s + fmt::format(" EntryCount={} Entries=[{}]", std::size(m_entries), fmt::join(entries, ", "));
}

std::uint64_t Sbgp::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_array_uint8_4(&writer, m_grouping_type);
  if (m_version != 0) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_grouping_type_parameter);
  }
  wbits += bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(std::size(m_entries)));
  wbits += std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL,
                           [&writer](const std::uint64_t a, auto e) { return a + e.writeData(&writer); });
  return wbits;
}

std::uint64_t Sbgp::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_array_uint8_4(&reader, &m_grouping_type);
  if (m_version != 0) {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_grouping_type_parameter);
  }

  std::uint32_t size;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &size);
  m_entries.resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    rbits += m_entries[i].readData(&reader);
  }

  return rbits;
}

std::uint64_t Sbgp::getDataSize() const {
  return 12 + 8 * std::size(m_entries) + (m_version == 0 ? 0 : 4);
}

}  // namespace shiguredo::mp4::box
