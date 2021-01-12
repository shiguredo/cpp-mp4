#include "shiguredo/mp4/box/sgpd.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <istream>
#include <iterator>
#include <memory>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/grouping_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_sgpd() {
  return BoxType("sgpd");
}

Sgpd::Sgpd() {
  m_type = box_type_sgpd();
}

Sgpd::Sgpd(const SgpdParameters& params)
    : m_grouping_type(params.grouping_type),
      m_default_length(params.default_length),
      m_default_sample_description_index(params.default_sample_description_index),
      m_roll_distances(params.roll_distances),
      m_alternative_startup_entries(params.alternative_startup_entries),
      m_visual_random_access_entries(params.visual_random_access_entries),
      m_temporal_level_entries(params.temporal_level_entries) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_sgpd();
}

std::string Sgpd::toStringOnlyData() const {
  std::string grouping_type(std::begin(m_grouping_type), std::end(m_grouping_type));
  std::string s = fmt::format(R"({} GroupingType="{}")", getVersionAndFlagsString(), grouping_type);
  if (m_version == 1) {
    s += fmt::format(" DefaultLength={}", m_default_length);
  } else {
    s += fmt::format(" DefaultSampleDescriptionIndex={}", m_default_sample_description_index);
  }
  auto no_default_length = noDefaultLength();
  if (m_grouping_type == GroupingTypeRoll || m_grouping_type == GroupingTypeProl) {
    std::vector<std::string> distances;
    std::transform(std::begin(m_roll_distances), std::end(m_roll_distances), std::back_inserter(distances),
                   [](const auto& d) { return d.toString(); });
    if (no_default_length) {
      s += fmt::format(" EntryCount={} RollDistancesL=[{}]", std::size(distances), fmt::join(distances, ", "));
      return s;
    }
    s += fmt::format(" EntryCount={} RollDistances=[{}]", std::size(distances), fmt::join(distances, ", "));
    return s;
  }
  if (m_grouping_type == GroupingTypeAlst) {
    std::vector<std::string> entries;
    std::transform(std::begin(m_alternative_startup_entries), std::end(m_alternative_startup_entries),
                   std::back_inserter(entries), [](const auto& e) { return e.toString(); });
    if (no_default_length) {
      s += fmt::format(" EntryCount={} AlternativeStartupEntriesL=[{}]", std::size(entries), fmt::join(entries, ", "));
      return s;
    }
    s += fmt::format(" EntryCount={} AlternativeStartupEntries=[{}]", std::size(entries), fmt::join(entries, ", "));
    return s;
  }
  if (m_grouping_type == GroupingTypeRap) {
    std::vector<std::string> entries;
    std::transform(std::begin(m_visual_random_access_entries), std::end(m_visual_random_access_entries),
                   std::back_inserter(entries), [](const auto& e) { return e.toString(); });
    if (no_default_length) {
      s += fmt::format(" EntryCount={} VisualRandomAccessEntriesL=[{}]", std::size(entries), fmt::join(entries, ", "));
      return s;
    }
    s += fmt::format(" EntryCount={} VisualRandomAccessEntries=[{}]", std::size(entries), fmt::join(entries, ", "));
    return s;
  }

  if (m_grouping_type == GroupingTypeTele) {
    std::vector<std::string> entries;
    std::transform(std::begin(m_temporal_level_entries), std::end(m_temporal_level_entries),
                   std::back_inserter(entries), [](const auto& e) { return e.toString(); });
    if (no_default_length) {
      s += fmt::format(" EntryCount={} TemporalLevelEntriesL=[{}]", std::size(entries), fmt::join(entries, ", "));
      return s;
    }
    s += fmt::format(" EntryCount={} TemporalLevelEntries=[{}]", std::size(entries), fmt::join(entries, ", "));
    return s;
  }

  return s;
}

std::uint64_t Sgpd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_array_uint8_4(&writer, m_grouping_type);
  if (m_version == 1) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_length);
  } else {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_sample_description_index);
  }
  if (m_grouping_type == GroupingTypeRoll || m_grouping_type == GroupingTypeProl) {
    std::uint32_t size = static_cast<std::uint32_t>(std::size(m_roll_distances));
    wbits += bitio::write_uint<std::uint32_t>(&writer, size);
    wbits += std::accumulate(std::begin(m_roll_distances), std::end(m_roll_distances), 0UL,
                             [&writer](const auto a, auto d) { return a + d.writeData(&writer); });
    return wbits;
  }
  if (m_grouping_type == GroupingTypeAlst) {
    std::uint32_t size = static_cast<std::uint32_t>(std::size(m_alternative_startup_entries));
    wbits += bitio::write_uint<std::uint32_t>(&writer, size);
    wbits += std::accumulate(std::begin(m_alternative_startup_entries), std::end(m_alternative_startup_entries), 0UL,
                             [&writer](const auto a, auto e) { return a + e.writeData(&writer); });
    return wbits;
  }
  if (m_grouping_type == GroupingTypeRap) {
    std::uint32_t size = static_cast<std::uint32_t>(std::size(m_visual_random_access_entries));
    wbits += bitio::write_uint<std::uint32_t>(&writer, size);
    wbits += std::accumulate(std::begin(m_visual_random_access_entries), std::end(m_visual_random_access_entries), 0UL,
                             [&writer](const auto a, auto e) { return a + e.writeData(&writer); });
    return wbits;
  }
  if (m_grouping_type == GroupingTypeTele) {
    std::uint32_t size = static_cast<std::uint32_t>(std::size(m_temporal_level_entries));
    wbits += bitio::write_uint<std::uint32_t>(&writer, size);
    wbits += std::accumulate(std::begin(m_temporal_level_entries), std::end(m_temporal_level_entries), 0UL,
                             [&writer](const auto a, auto e) { return a + e.writeData(&writer); });
    return wbits;
  }
  return wbits;
}

std::uint64_t Sgpd::getDataSize() const {
  if (m_grouping_type == GroupingTypeRoll || m_grouping_type == GroupingTypeProl) {
    return 16 + std::accumulate(std::begin(m_roll_distances), std::end(m_roll_distances), 0UL,
                                [](const auto a, const auto& e) { return a + e.getSize(); });
  }
  if (m_grouping_type == GroupingTypeAlst) {
    return 16 + std::accumulate(std::begin(m_alternative_startup_entries), std::end(m_alternative_startup_entries), 0UL,
                                [](const auto a, const auto& e) { return a + e.getSize(); });
  }
  if (m_grouping_type == GroupingTypeRap) {
    return 16 + std::accumulate(std::begin(m_visual_random_access_entries), std::end(m_visual_random_access_entries),
                                0UL, [](const auto a, const auto& e) { return a + e.getSize(); });
  }
  if (m_grouping_type == GroupingTypeTele) {
    return 16 + std::accumulate(std::begin(m_temporal_level_entries), std::end(m_temporal_level_entries), 0UL,
                                [](const auto a, const auto& e) { return a + e.getSize(); });
  }
  return 12;
}

std::uint64_t Sgpd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_array_uint8_4(&reader, &m_grouping_type);
  if (m_version == 1) {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_length);
  } else {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_sample_description_index);
  }
  auto no_default_length = noDefaultLength();
  if (m_grouping_type == GroupingTypeRoll || m_grouping_type == GroupingTypeProl) {
    std::uint32_t size;
    rbits += bitio::read_uint<std::uint32_t>(&reader, &size);
    m_roll_distances.resize(size);
    for (std::size_t i = 0; i < size; ++i) {
      rbits += m_roll_distances[i].readData(&reader, no_default_length);
    }
    return rbits;
  }
  if (m_grouping_type == GroupingTypeAlst) {
    std::uint32_t size;
    rbits += bitio::read_uint<std::uint32_t>(&reader, &size);
    m_alternative_startup_entries.resize(size);
    for (std::size_t i = 0; i < size; ++i) {
      rbits += m_alternative_startup_entries[i].readData(&reader, m_default_length, no_default_length);
    }
    return rbits;
  }
  if (m_grouping_type == GroupingTypeRap) {
    std::uint32_t size;
    rbits += bitio::read_uint<std::uint32_t>(&reader, &size);
    m_visual_random_access_entries.resize(size);
    for (std::size_t i = 0; i < size; ++i) {
      rbits += m_visual_random_access_entries[i].readData(&reader, no_default_length);
    }
    return rbits;
  }
  if (m_grouping_type == GroupingTypeTele) {
    std::uint32_t size;
    rbits += bitio::read_uint<std::uint32_t>(&reader, &size);
    m_temporal_level_entries.resize(size);
    for (std::size_t i = 0; i < size; ++i) {
      rbits += m_temporal_level_entries[i].readData(&reader, no_default_length);
    }
    return rbits;
  }
  return rbits;
}

bool Sgpd::noDefaultLength() const {
  return m_version == 1 && m_default_length == 0;
}

RollDistance::RollDistance(const RollDistanceParameters& params)
    : m_description_length(params.description_length), m_roll_distance(params.roll_distance) {
  m_with_length = m_description_length > 0;
}

std::string RollDistance::toString() const {
  if (m_with_length) {
    return fmt::format("{{DescriptionLength={} RollDistance={}}}", m_description_length, m_roll_distance);
  }
  return fmt::format("{}", m_roll_distance);
}

std::uint64_t RollDistance::writeData(bitio::Writer* writer) const {
  std::uint64_t wbits = 0;
  if (m_with_length) {
    wbits += bitio::write_uint<std::uint32_t>(writer, m_description_length);
  }
  return wbits + bitio::write_int<std::int16_t>(writer, m_roll_distance);
}

std::uint64_t RollDistance::getSize() const {
  return 2 + (m_with_length ? 4 : 0);
}

std::uint64_t RollDistance::readData(bitio::Reader* reader, const bool with_length) {
  m_with_length = with_length;
  std::uint64_t rbits = 0;
  if (m_with_length) {
    rbits += bitio::read_uint<std::uint32_t>(reader, &m_description_length);
  }
  return rbits + bitio::read_int<std::int16_t>(reader, &m_roll_distance);
}

AlternativeStartupEntryOpt::AlternativeStartupEntryOpt(const AlternativeStartupEntryOptParameters& params)
    : m_num_output_samples(params.num_output_samples), m_num_total_samples(params.num_total_samples) {}

std::string AlternativeStartupEntryOpt::toString() const {
  return fmt::format("{{NumOutputSamples={} NumTotalSamples={}}}", m_num_output_samples, m_num_total_samples);
}

std::uint64_t AlternativeStartupEntryOpt::writeData(bitio::Writer* writer) const {
  return bitio::write_uint<std::uint16_t>(writer, m_num_output_samples) +
         bitio::write_uint<std::uint16_t>(writer, m_num_total_samples);
}

std::uint64_t AlternativeStartupEntryOpt::readData(bitio::Reader* reader) {
  return bitio::read_uint<std::uint16_t>(reader, &m_num_output_samples) +
         bitio::read_uint<std::uint16_t>(reader, &m_num_total_samples);
}

AlternativeStartupEntry::AlternativeStartupEntry(const AlternativeStartupEntryParameters& params)
    : m_description_length(params.description_length),
      m_roll_count(params.roll_count),
      m_first_output_sample(params.first_output_sample),
      m_sample_offsets(params.sample_offsets),
      m_opts(params.opts) {
  m_with_length = m_description_length > 0;
}

std::string AlternativeStartupEntry::toString() const {
  std::vector<std::string> opts;
  std::transform(std::begin(m_opts), std::end(m_opts), std::back_inserter(opts),
                 [](const auto& opt) { return opt.toString(); });

  std::string s;
  if (m_with_length) {
    s = fmt::format("{{DescriptionLength={} ", m_description_length);
  } else {
    s = "{";
  }

  return s + fmt::format("RollCount={} FirstOutputSample={} SampleOffset=[{}] Opts=[{}]}}", m_roll_count,
                         m_first_output_sample, fmt::join(m_sample_offsets, ", "), fmt::join(opts, ", "));
}

std::uint64_t AlternativeStartupEntry::writeData(bitio::Writer* writer) const {
  std::uint64_t wbits = 0;
  if (m_with_length) {
    wbits += bitio::write_uint<std::uint32_t>(writer, m_description_length);
  }
  wbits += bitio::write_uint<std::uint16_t>(writer, m_roll_count);
  wbits += bitio::write_uint<std::uint16_t>(writer, m_first_output_sample);
  wbits += bitio::write_vector_uint<std::uint32_t>(writer, m_sample_offsets);
  wbits += std::accumulate(std::begin(m_opts), std::end(m_opts), 0UL,
                           [&writer](const auto a, auto o) { return a + o.writeData(writer); });
  return wbits;
}

std::uint64_t AlternativeStartupEntry::getSize() const {
  return 4 + 4 * std::size(m_sample_offsets) + 4 * std::size(m_opts) + (m_with_length ? 4 : 0);
}

std::uint64_t AlternativeStartupEntry::readData(bitio::Reader* reader,
                                                const std::uint32_t length,
                                                const bool with_length) {
  m_with_length = with_length;
  std::uint64_t rbits = 0;
  if (m_with_length) {
    rbits += bitio::read_uint<std::uint32_t>(reader, &m_description_length);
  }
  rbits += bitio::read_uint<std::uint16_t>(reader, &m_roll_count);
  rbits += bitio::read_uint<std::uint16_t>(reader, &m_first_output_sample);
  rbits += bitio::read_vector_uint<std::uint32_t>(reader, m_roll_count, &m_sample_offsets);
  std::size_t size;
  std::int64_t offset;
  if (m_with_length) {
    offset = static_cast<std::int64_t>(m_description_length * 8) - static_cast<std::int64_t>(rbits - 32);
  } else {
    offset = static_cast<std::int64_t>(length * 8) - static_cast<std::int64_t>(rbits);
  }
  if ((offset < 0) || (offset % 32 != 0)) {
    throw std::runtime_error(
        fmt::format("AlternativeStartupEntry::readData(): invalid offset of AlternativeStartupEntry: {}", offset));
  }
  size = static_cast<std::size_t>(offset / 32);
  m_opts.resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    rbits += m_opts[i].readData(reader);
  }
  return rbits;
}

VisualRandomAccessEntry::VisualRandomAccessEntry(const VisualRandomAccessEntryParameters& params)
    : m_description_length(params.description_length),
      m_num_leading_samples_known(params.num_leading_samples_known),
      m_num_leading_samples(params.num_leading_samples) {
  m_with_length = m_description_length > 0;
}

std::string VisualRandomAccessEntry::toString() const {
  std::string s;
  if (m_with_length) {
    s = fmt::format("{{DescriptionLength={} ", m_description_length);
  } else {
    s = "{";
  }
  return s + fmt::format("NumLeadingSamplesKnown={} NumLeadingSamples={:#x}}}", m_num_leading_samples_known,
                         m_num_leading_samples);
}

std::uint64_t VisualRandomAccessEntry::writeData(bitio::Writer* writer) const {
  std::uint64_t wbits = 0;
  if (m_with_length) {
    wbits += bitio::write_uint<std::uint32_t>(writer, m_description_length);
  }
  wbits += write_bool(writer, m_num_leading_samples_known);
  return wbits + bitio::write_uint<std::uint8_t>(writer, m_num_leading_samples, 7);
}

std::uint64_t VisualRandomAccessEntry::getSize() const {
  return 1 + (m_with_length ? 4 : 0);
}

std::uint64_t VisualRandomAccessEntry::readData(bitio::Reader* reader, const bool with_length) {
  m_with_length = with_length;
  std::uint64_t rbits = 0;
  if (m_with_length) {
    rbits += bitio::read_uint<std::uint32_t>(reader, &m_description_length);
  }
  rbits += read_bool(reader, &m_num_leading_samples_known);
  return rbits + bitio::read_uint<std::uint8_t>(reader, &m_num_leading_samples, 7);
}

TemporalLevelEntry::TemporalLevelEntry(const TemporalLevelEntryParameters& params)
    : m_description_length(params.description_length),
      m_level_independently_decodable(params.level_independently_decodable) {
  m_with_length = m_description_length > 0;
}

std::string TemporalLevelEntry::toString() const {
  std::string s;
  if (m_with_length) {
    s = fmt::format("{{DescriptionLength={} ", m_description_length);
  } else {
    s = "{";
  }
  return s + fmt::format("LevelIndependentlyRecodable={}}}", m_level_independently_decodable);
}

std::uint64_t TemporalLevelEntry::writeData(bitio::Writer* writer) const {
  std::uint64_t wbits = 0;
  if (m_with_length) {
    wbits += bitio::write_uint<std::uint32_t>(writer, m_description_length);
  }
  wbits += write_bool(writer, m_level_independently_decodable);
  return wbits + bitio::write_uint<std::uint8_t>(writer, m_reserved, 7);
}

std::uint64_t TemporalLevelEntry::getSize() const {
  return 1 + (m_with_length ? 4 : 0);
}

std::uint64_t TemporalLevelEntry::readData(bitio::Reader* reader, const bool with_length) {
  m_with_length = with_length;
  std::uint64_t rbits = 0;
  if (m_with_length) {
    rbits += bitio::read_uint<std::uint32_t>(reader, &m_description_length);
  }
  rbits += read_bool(reader, &m_level_independently_decodable);
  return rbits + bitio::read_uint<std::uint8_t>(reader, &m_reserved, 7);
}

}  // namespace shiguredo::mp4::box
