#include "shiguredo/mp4/box/mdhd.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <array>
#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/time/time.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_mdhd() {
  return BoxType("mdhd");
}

Mdhd::Mdhd() {
  m_type = box_type_mdhd();
}

Mdhd::Mdhd(const MdhdParameters& params)
    : m_creation_time(params.creation_time),
      m_modification_time(params.modification_time),
      m_timescale(params.timescale),
      m_duration(params.duration),
      m_pad(params.pad),
      m_language(params.language),
      m_pre_defined(params.pre_defined) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_mdhd();
}

std::string Mdhd::toStringOnlyData() const {
  return fmt::format(
      R"({} CreationTime="{}" ModificationTime="{}" Timescale={} Duration={} Pad={} Language="{:c}" PreDefined={})",
      getVersionAndFlagsString(), time::format_epoch_19040101(m_creation_time),
      time::format_epoch_19040101(m_modification_time), m_timescale, m_duration, m_pad, fmt::join(m_language, ""),
      m_pre_defined);
}

std::uint64_t Mdhd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_creation_time, m_version == 0 ? 32 : 64);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_modification_time, m_version == 0 ? 32 : 64);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_timescale);
  wbits += bitio::write_uint<std::uint64_t>(&writer, m_duration, m_version == 0 ? 32 : 64);
  wbits += write_bool(&writer, m_pad);
  wbits += bitio::write_uint<std::uint16_t>(&writer, encode_language(m_language), 15);
  return wbits + bitio::write_uint<std::uint16_t>(&writer, m_pre_defined);
}

std::uint64_t Mdhd::getDataSize() const {
  if (m_version == 0) {
    return 24;
  }
  return 36;
}

std::uint64_t Mdhd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_creation_time, m_version == 0 ? 32 : 64);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_modification_time, m_version == 0 ? 32 : 64);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_timescale);
  rbits += bitio::read_uint<std::uint64_t>(&reader, &m_duration, m_version == 0 ? 32 : 64);
  rbits += read_bool(&reader, &m_pad);
  std::uint16_t language;
  rbits += bitio::read_uint<std::uint16_t>(&reader, &language, 15);
  m_language = decode_language(language);
  return rbits + bitio::read_uint<std::uint16_t>(&reader, &m_pre_defined);
}

std::uint16_t encode_language(const std::array<std::uint8_t, 3>& lng) {
  return static_cast<std::uint16_t>(((lng[0] - 0x60) << 10) + ((lng[1] - 0x60) << 5) + ((lng[2] - 0x60)));
}

std::array<std::uint8_t, 3> decode_language(const uint16_t enc) {
  std::array<std::uint8_t, 3> lng;
  lng[0] = ((enc >> 10) & 0x1f) + 0x60;
  lng[1] = ((enc >> 5) & 0x1f) + 0x60;
  lng[2] = (enc & 0x1f) + 0x60;
  return lng;
}

}  // namespace shiguredo::mp4::box
