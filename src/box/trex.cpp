#include "shiguredo/mp4/box/trex.hpp"

#include <fmt/core.h>

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_trex() {
  return BoxType("trex");
}

Trex::Trex() {
  m_type = box_type_trex();
}

Trex::Trex(const TrexParameters& params)
    : m_track_id(params.track_id),
      m_default_sample_description_index(params.default_sample_description_index),
      m_default_sample_duration(params.default_sample_duration),
      m_default_sample_size(params.default_sample_size),
      m_default_sample_flags(params.default_sample_flags) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_trex();
}

std::string Trex::toStringOnlyData() const {
  return fmt::format(
      "{} TrackID={} DefaultSampleDescriptionIndex={} DefaultSampleDuration={} DefaultSampleSize={} "
      "DefaultSampleFlags={:#x}",
      getVersionAndFlagsString(), m_track_id, m_default_sample_description_index, m_default_sample_duration,
      m_default_sample_size, m_default_sample_flags);
}

std::uint64_t Trex::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_track_id);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_sample_description_index);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_sample_duration);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_sample_size);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_sample_flags);
  return wbits;
}

std::uint64_t Trex::getDataSize() const {
  return 24;
}

std::uint64_t Trex::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_track_id);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_sample_description_index);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_sample_duration);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_sample_size);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_sample_flags);
  return rbits;
}

}  // namespace shiguredo::mp4::box
