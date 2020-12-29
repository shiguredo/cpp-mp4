#include "shiguredo/mp4/box/mvhd.hpp"

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

BoxType box_type_mvhd() {
  return BoxType("mvhd");
}

Mvhd::Mvhd() {
  m_type = box_type_mvhd();
}

Mvhd::Mvhd(const MvhdParameters& params)
    : m_creation_time(params.creation_time),
      m_modification_time(params.modification_time),
      m_timescale(params.timescale),
      m_duration(params.duration),
      m_rate(params.rate),
      m_volume(params.volume),
      m_matrix(params.matrix),
      m_pre_defined(params.pre_defined),
      m_next_track_id(params.next_track_id) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_mvhd();
}

double Mvhd::getRate() const {
  return static_cast<double>(m_rate) / (1 << 16);
}

std::string Mvhd::toStringOnlyData() const {
  return fmt::format(R"({} CreationTime="{}" ModificationTime="{}" Timescale={} Duration={} )"
                     "Rate={:.4f} Volume={} Matrix=[{:#x}] PreDefined=[{}] NextTrackID={}",
                     getVersionAndFlagsString(), time::format_epoch_19040101(m_creation_time),
                     time::format_epoch_19040101(m_modification_time), m_timescale, m_duration, getRate(), m_volume,
                     fmt::join(m_matrix, ", "), fmt::join(m_pre_defined, ", "), m_next_track_id);
}

std::uint64_t Mvhd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_creation_time, m_version == 0 ? 32 : 64);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_modification_time, m_version == 0 ? 32 : 64);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_timescale);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_duration, m_version == 0 ? 32 : 64);
  wbits += bitio::write_int<std::int32_t>(&writer, m_rate);
  wbits += bitio::write_int<std::int16_t>(&writer, m_volume);
  wbits += bitio::write_int<std::int16_t>(&writer, m_reserved);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_reserved2);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_reserved3);
  for (const auto i : m_matrix) {
    wbits += bitio::write_int<std::int32_t>(&writer, i);
  }
  for (const auto i : m_pre_defined) {
    wbits += bitio::write_int<std::int32_t>(&writer, i);
  }
  return wbits + bitio::write_uint<std::uint32_t>(&writer, m_next_track_id);
}

std::uint64_t Mvhd::getDataSize() const {
  if (m_version == 0) {
    return 100;
  }
  return 112;
}

std::uint64_t Mvhd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_creation_time, m_version == 0 ? 32 : 64);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_modification_time, m_version == 0 ? 32 : 64);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_timescale);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_duration, m_version == 0 ? 32 : 64);
  rbits += bitio::read_int<std::int32_t>(&reader, &m_rate);
  rbits += bitio::read_int<std::int16_t>(&reader, &m_volume);
  rbits += bitio::read_int<std::int16_t>(&reader, &m_reserved);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_reserved2);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_reserved3);
  for (std::size_t i = 0; i < std::size(m_matrix); ++i) {
    rbits += bitio::read_int<std::int32_t>(&reader, &(m_matrix[i]));
  }
  for (std::size_t i = 0; i < std::size(m_pre_defined); ++i) {
    rbits += bitio::read_int<std::int32_t>(&reader, &(m_pre_defined[i]));
  }
  return rbits + bitio::read_uint<std::uint32_t>(&reader, &m_next_track_id);
}

void Mvhd::setNextTrackID(const std::uint32_t next_track_id) {
  m_next_track_id = next_track_id;
}

}  // namespace shiguredo::mp4::box
