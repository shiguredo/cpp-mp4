#include "shiguredo/mp4/box/tfra.hpp"

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

BoxType box_type_tfra() {
  return BoxType("tfra");
}

Tfra::Tfra() {
  m_type = box_type_tfra();
}

Tfra::Tfra(const TfraParameters& params)
    : m_track_id(params.track_id),
      m_length_size_of_traf_num(params.length_size_of_traf_num),
      m_length_size_of_trun_num(params.length_size_of_trun_num),
      m_length_size_of_sample_num(params.length_size_of_sample_num),
      m_entries(params.entries) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_tfra();
}

std::string Tfra::toStringOnlyData() const {
  std::vector<std::string> entries;
  std::transform(std::begin(m_entries), std::end(m_entries), std::back_inserter(entries),
                 [](const auto& e) { return e.toString(); });
  return fmt::format(
      "{} TrackID={} LengthSizeOfTrafNum={:#x} LengthSizeOfTrunNum={:#x} LengthSizeOfSampleNum={:#x} NumberOfEntry={} "
      "Entries=[{}]",
      getVersionAndFlagsString(), m_track_id, m_length_size_of_traf_num, m_length_size_of_trun_num,
      m_length_size_of_sample_num, std::size(entries), fmt::join(entries, ", "));
}

std::uint64_t Tfra::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_track_id);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_reserved, 26);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_length_size_of_traf_num, 2);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_length_size_of_trun_num, 2);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_length_size_of_sample_num, 2);
  std::uint32_t entry_count = static_cast<std::uint32_t>(std::size(m_entries));
  wbits += bitio::write_uint<std::uint32_t>(&writer, entry_count);
  wbits += std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL, [&](const auto a, const auto& e) {
    return a + e.writeData(&writer, m_version, m_length_size_of_traf_num, m_length_size_of_trun_num,
                           m_length_size_of_sample_num);
  });
  return wbits;
}

std::uint64_t Tfra::getDataSize() const {
  return 16 + std::accumulate(std::begin(m_entries), std::end(m_entries), 0UL, [&](const auto a, const auto& e) {
           return a + e.getSize(m_version, m_length_size_of_traf_num, m_length_size_of_trun_num,
                                m_length_size_of_sample_num);
         });
}

std::uint64_t Tfra::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_track_id);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_reserved, 26);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_length_size_of_traf_num, 2);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_length_size_of_trun_num, 2);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_length_size_of_sample_num, 2);
  std::uint32_t entry_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &entry_count);
  m_entries.resize(entry_count);
  for (std::uint32_t i = 0; i < entry_count; ++i) {
    rbits += m_entries[i].readData(&reader, m_version, m_length_size_of_traf_num, m_length_size_of_trun_num,
                                   m_length_size_of_sample_num);
  }
  return rbits;
}

TfraEntry::TfraEntry(const TfraEntryParameters& params)
    : m_time(params.time),
      m_moof_offset(params.moof_offset),
      m_traf_number(params.traf_number),
      m_trun_number(params.trun_number),
      m_sample_number(params.sample_number) {}

std::string TfraEntry::toString() const {
  return fmt::format("{{Time={} MoofOffset={} TrafNumber={} TrunNumber={} SampleNumber={}}}", m_time, m_moof_offset,
                     m_traf_number, m_trun_number, m_sample_number);
}

std::uint64_t TfraEntry::writeData(bitio::Writer* writer,
                                   const std::uint8_t version,
                                   const std::uint8_t length_size_of_traf_num,
                                   const std::uint8_t length_size_of_trun_num,
                                   const std::uint8_t length_size_of_sample_num) const {
  auto wbits = bitio::write_uint<std::uint64_t>(writer, m_time, version == 0 ? 32 : 64) +
               bitio::write_uint<std::uint64_t>(writer, m_moof_offset, version == 0 ? 32 : 64);
  std::uint64_t size_of_traf_num = (static_cast<std::uint64_t>(length_size_of_traf_num) + 1) * 8;
  wbits += bitio::write_uint<std::uint32_t>(writer, m_traf_number, size_of_traf_num);
  std::uint64_t size_of_trun_num = (static_cast<std::uint64_t>(length_size_of_trun_num) + 1) * 8;
  wbits += bitio::write_uint<std::uint32_t>(writer, m_trun_number, size_of_trun_num);
  std::uint64_t size_of_sample_num = (static_cast<std::uint64_t>(length_size_of_sample_num) + 1) * 8;
  wbits += bitio::write_uint<std::uint32_t>(writer, m_sample_number, size_of_sample_num);
  return wbits;
}

std::uint64_t TfraEntry::getSize(const std::uint8_t version,
                                 const std::uint8_t length_size_of_traf_num,
                                 const std::uint8_t length_size_of_trun_num,
                                 const std::uint8_t length_size_of_sample_num) const {
  return 8 + (version == 0 ? 0 : 8) + (static_cast<std::uint64_t>(length_size_of_traf_num) + 1) +
         (static_cast<std::uint64_t>(length_size_of_trun_num) + 1) +
         (static_cast<std::uint64_t>(length_size_of_sample_num) + 1);
}

std::uint64_t TfraEntry::readData(bitio::Reader* reader,
                                  const std::uint8_t version,
                                  const std::uint8_t length_size_of_traf_num,
                                  const std::uint8_t length_size_of_trun_num,
                                  const std::uint8_t length_size_of_sample_num) {
  auto rbits = bitio::read_uint<std::uint64_t>(reader, &m_time, version == 0 ? 32 : 64) +
               bitio::read_uint<std::uint64_t>(reader, &m_moof_offset, version == 0 ? 32 : 64);
  std::uint64_t size_of_traf_num = (static_cast<std::uint64_t>(length_size_of_traf_num) + 1) * 8;
  rbits += bitio::read_uint<std::uint32_t>(reader, &m_traf_number, size_of_traf_num);
  std::uint64_t size_of_trun_num = (static_cast<std::uint64_t>(length_size_of_trun_num) + 1) * 8;
  rbits += bitio::read_uint<std::uint32_t>(reader, &m_trun_number, size_of_trun_num);
  std::uint64_t size_of_sample_num = (static_cast<std::uint64_t>(length_size_of_sample_num) + 1) * 8;
  rbits += bitio::read_uint<std::uint32_t>(reader, &m_sample_number, size_of_sample_num);

  return rbits;
}

}  // namespace shiguredo::mp4::box
