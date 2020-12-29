#include "shiguredo/mp4/box/sidx.hpp"

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

BoxType box_type_sidx() {
  return BoxType("sidx");
}

Sidx::Sidx() {
  m_type = box_type_sidx();
}

Sidx::Sidx(const SidxParameters& params)
    : m_reference_id(params.reference_id),
      m_timescale(params.timescale),
      m_earliest_presentation_time(params.earliest_presentation_time),
      m_first_offset(params.first_offset),
      m_references(params.references) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_sidx();
}

std::string Sidx::toStringOnlyData() const {
  std::vector<std::string> references;
  std::transform(std::begin(m_references), std::end(m_references), std::back_inserter(references),
                 [](const auto& r) { return r.toString(); });

  return fmt::format(
      "{} ReferenceID={} Timescale={} EarliestPresentationTime={} "
      "FirstOffset={} ReferenceCount={} References=[{}]",
      getVersionAndFlagsString(), m_reference_id, m_timescale, m_earliest_presentation_time, m_first_offset,
      std::size(references), fmt::join(references, ", "));
}

std::uint64_t Sidx::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_reference_id);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_timescale);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_earliest_presentation_time, m_version == 0 ? 32 : 64);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_first_offset, m_version == 0 ? 32 : 64);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_reserved);
  std::uint16_t size = static_cast<std::uint16_t>(std::size(m_references));
  wbits += bitio::write_uint<std::uint16_t>(&writer, size);
  wbits += std::accumulate(std::begin(m_references), std::end(m_references), 0UL,
                           [&writer](const auto a, const auto& r) { return a + r.writeData(&writer); });

  return wbits;
}

std::uint64_t Sidx::getDataSize() const {
  return 24 + 12 * std::size(m_references) + (m_version == 0 ? 0 : 8);
}

std::uint64_t Sidx::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_reference_id);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_timescale);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_earliest_presentation_time, m_version == 0 ? 32 : 64);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_first_offset, m_version == 0 ? 32 : 64);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_reserved);
  std::uint16_t size;
  rbits += bitio::read_uint<std::uint16_t>(&reader, &size);
  m_references.resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    rbits += m_references[i].readData(&reader);
  }

  return rbits;
}

SidxReference::SidxReference(const SidxReferenceParameters& params)
    : m_reference_type(params.reference_type),
      m_reference_size(params.reference_size),
      m_subsegument_duration(params.subsegument_duration),
      m_starts_with_sap(params.starts_with_sap),
      m_sap_type(params.sap_type),
      m_sap_delta_time(params.sap_delta_time) {}

std::string SidxReference::toString() const {
  return fmt::format(
      "{{ReferenceType={} ReferencedSize={} SubsegmentDuration={} "
      "StartsWithSAP={} SAPType={} SAPDeltaTime={}}}",
      m_reference_type, m_reference_size, m_subsegument_duration, m_starts_with_sap, m_sap_type, m_sap_delta_time);
}

std::uint64_t SidxReference::writeData(bitio::Writer* writer) const {
  auto wbits = write_bool(writer, m_reference_type);
  wbits += bitio::write_uint<std::uint32_t>(writer, m_reference_size, 31);
  wbits += bitio::write_uint<std::uint32_t>(writer, m_subsegument_duration);
  wbits += write_bool(writer, m_starts_with_sap);
  wbits += bitio::write_uint<std::uint8_t>(writer, m_sap_type, 3);
  return wbits + bitio::write_uint<std::uint32_t>(writer, m_sap_delta_time, 28);
}

std::uint64_t SidxReference::readData(bitio::Reader* reader) {
  auto rbits = read_bool(reader, &m_reference_type);
  rbits += bitio::read_uint<std::uint32_t>(reader, &m_reference_size, 31);
  rbits += bitio::read_uint<std::uint32_t>(reader, &m_subsegument_duration);
  rbits += read_bool(reader, &m_starts_with_sap);
  rbits += bitio::read_uint<std::uint8_t>(reader, &m_sap_type, 3);
  return rbits + bitio::read_uint<std::uint32_t>(reader, &m_sap_delta_time, 28);
}

}  // namespace shiguredo::mp4::box
