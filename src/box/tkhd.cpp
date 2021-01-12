#include "shiguredo/mp4/box/tkhd.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cstdint>
#include <istream>
#include <iterator>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/time/time.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_tkhd() {
  return BoxType("tkhd");
}

Tkhd::Tkhd() {
  m_type = box_type_tkhd();
}

Tkhd::Tkhd(const TkhdParameters& params)
    : m_creation_time(params.creation_time),
      m_modification_time(params.modification_time),
      m_track_id(params.track_id),
      m_duration(params.duration),
      m_layer(params.layer),
      m_alternate_group(params.alternate_group),
      m_volume(params.volume),
      m_matrix(params.matrix),
      m_width(params.width),
      m_height(params.height) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_tkhd();
}

std::string Tkhd::toStringOnlyData() const {
  return fmt::format(
      R"({} CreationTime="{}" ModificationTime="{}" TrackID={} Duration={} Layer={} AlternateGroup={} Volume={} )"
      "Matrix=[{:#x}] Width={:.4f} Height={:.4f}",
      getVersionAndFlagsString(), time::format_epoch_19040101(m_creation_time),
      time::format_epoch_19040101(m_modification_time), m_track_id, m_duration, m_layer, m_alternate_group, m_volume,
      fmt::join(m_matrix, ", "), getWidth(), getHeight());
}

std::uint64_t Tkhd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_creation_time, m_version == 0 ? 32 : 64);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_modification_time, m_version == 0 ? 32 : 64);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_track_id);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_reserved);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_duration, m_version == 0 ? 32 : 64);

  wbits += bitio::write_uint<std::uint32_t>(&writer, m_reserved2);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_reserved3);

  wbits += bitio::write_int<std::int16_t>(&writer, m_layer);
  wbits += bitio::write_int<std::int16_t>(&writer, m_alternate_group);
  wbits += bitio::write_int<std::int16_t>(&writer, m_volume);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_reserved4);

  for (const auto i : m_matrix) {
    wbits += bitio::write_int<std::int32_t>(&writer, i);
  }

  wbits += bitio::write_uint<std::uint32_t>(&writer, m_width);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_height);

  return wbits;
}

std::uint64_t Tkhd::getDataSize() const {
  if (m_version == 0) {
    return 84;
  }
  return 96;
}

std::uint64_t Tkhd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_creation_time, m_version == 0 ? 32 : 64);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_modification_time, m_version == 0 ? 32 : 64);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_track_id);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_reserved);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_duration, m_version == 0 ? 32 : 64);

  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_reserved2);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_reserved3);

  rbits += bitio::read_int<std::int16_t>(&reader, &m_layer);
  rbits += bitio::read_int<std::int16_t>(&reader, &m_alternate_group);
  rbits += bitio::read_int<std::int16_t>(&reader, &m_volume);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_reserved4);

  for (std::size_t i = 0; i < std::size(m_matrix); ++i) {
    rbits += bitio::read_int<std::int32_t>(&reader, &(m_matrix[i]));
  }

  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_width);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_height);

  return rbits;
}

double Tkhd::getWidth() const {
  return static_cast<double>(m_width) / (1 << 16);
}

double Tkhd::getHeight() const {
  return static_cast<double>(m_height) / (1 << 16);
}

}  // namespace shiguredo::mp4::box
