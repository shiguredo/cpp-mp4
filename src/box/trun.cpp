#include "shiguredo/mp4/box/trun.hpp"

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

BoxType box_type_trun() {
  return BoxType("trun");
}

Trun::Trun() {
  m_type = box_type_trun();
}

Trun::Trun(const TrunParameters& params)
    : m_data_offset(params.data_offset), m_first_sample_flags(params.first_sample_flags), m_entries(params.entries) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_trun();
}

std::string Trun::toStringOnlyData() const {
  std::string s = fmt::format("{} SampleCount={}", getVersionAndFlagsString(), std::size(m_entries));
  const std::uint32_t flags = getFlags();
  if (flags & 0x1) {
    s += fmt::format(" DataOffset={}", m_data_offset);
  }
  if (flags & 0x4) {
    s += fmt::format(" FirstSampleFlags={:#x}", m_first_sample_flags);
  }
  std::vector<std::string> entries;
  std::transform(std::begin(m_entries), std::end(m_entries), std::back_inserter(entries),
                 [flags](const auto& e) { return e.toString(flags); });
  return s + fmt::format(" Entries=[{}]", fmt::join(entries, ", "));
}

std::uint64_t Trun::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  std::uint32_t sample_count = static_cast<std::uint32_t>(std::size(m_entries));
  wbits += bitio::write_uint<std::uint32_t>(&writer, sample_count);
  const std::uint32_t flags = getFlags();
  if (flags & 0x1) {
    wbits += bitio::write_int<std::int32_t>(&writer, m_data_offset);
  }
  if (flags & 0x4) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_first_sample_flags);
  }
  wbits += std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL,
                           [&writer, &flags](const auto a, const auto& e) { return a + e.writeData(&writer, flags); });
  return wbits;
}

std::uint64_t Trun::getDataSize() const {
  std::uint64_t size = 8;
  const std::uint32_t flags = getFlags();
  if (flags & 0x1) {
    size += 4;
  }
  if (flags & 0x4) {
    size += 4;
  }
  size += std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL,
                          [&flags](const auto a, const auto& e) { return a + e.getSize(flags); });

  return size;
}

std::uint64_t Trun::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  std::uint32_t sample_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &sample_count);
  const std::uint32_t flags = getFlags();
  if (flags & 0x1) {
    rbits += bitio::read_int<std::int32_t>(&reader, &m_data_offset);
  }
  if (flags & 0x4) {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_first_sample_flags);
  }
  m_entries.resize(sample_count);
  for (std::size_t i = 0; i < sample_count; ++i) {
    rbits += m_entries[i].readData(&reader, flags);
  }
  return rbits;
}

TrunEntry::TrunEntry(const TrunEntryParameters& params)
    : m_sample_duration(params.sample_duration),
      m_sample_size(params.sample_size),
      m_sample_flags(params.sample_flags),
      m_sample_composition_time_offset(params.sample_composition_time_offset) {}

std::string TrunEntry::toString(const std::uint32_t flags) const {
  std::vector<std::string> ss;

  if (flags & 0x100) {
    ss.push_back(fmt::format("SampleDuration={}", m_sample_duration));
  }
  if (flags & 0x200) {
    ss.push_back(fmt::format("SampleSize={}", m_sample_size));
  }
  if (flags & 0x400) {
    ss.push_back(fmt::format("SampleFlags={:#x}", m_sample_flags));
  }
  if (flags & 0x800) {
    ss.push_back(fmt::format("SampleCompositionTimeOffset={}", m_sample_composition_time_offset));
  }

  return fmt::format("{{{}}}", fmt::join(ss, " "));
}

std::uint64_t TrunEntry::writeData(bitio::Writer* writer, const std::uint32_t flags) const {
  std::uint64_t wbits = 0;
  if (flags & 0x100) {
    wbits += bitio::write_uint<std::uint32_t>(writer, m_sample_duration);
  }
  if (flags & 0x200) {
    wbits += bitio::write_uint<std::uint32_t>(writer, m_sample_size);
  }
  if (flags & 0x400) {
    wbits += bitio::write_uint<std::uint32_t>(writer, m_sample_flags);
  }
  if (flags & 0x800) {
    wbits += bitio::write_int<std::int64_t>(writer, m_sample_composition_time_offset, 32);
  }
  return wbits;
}

std::uint64_t TrunEntry::getSize(const std::uint32_t flags) const {
  std::uint64_t size = 0;
  if (flags & 0x100) {
    size += 4;
  }
  if (flags & 0x200) {
    size += 4;
  }
  if (flags & 0x400) {
    size += 4;
  }
  if (flags & 0x800) {
    size += 4;
  }
  return size;
}

std::uint64_t TrunEntry::readData(bitio::Reader* reader, const std::uint32_t flags) {
  std::uint64_t rbits = 0;
  if (flags & 0x100) {
    rbits += bitio::read_uint<std::uint32_t>(reader, &m_sample_duration);
  }
  if (flags & 0x200) {
    rbits += bitio::read_uint<std::uint32_t>(reader, &m_sample_size);
  }
  if (flags & 0x400) {
    rbits += bitio::read_uint<std::uint32_t>(reader, &m_sample_flags);
  }
  if (flags & 0x800) {
    rbits += bitio::read_int<std::int64_t>(reader, &m_sample_composition_time_offset, 32);
  }
  return rbits;
}

}  // namespace shiguredo::mp4::box
