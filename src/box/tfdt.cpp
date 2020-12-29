#include "shiguredo/mp4/box/tfdt.hpp"

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

BoxType box_type_tfdt() {
  return BoxType("tfdt");
}

Tfdt::Tfdt() {
  m_type = box_type_tfdt();
}

Tfdt::Tfdt(const TfdtParameters& params) : m_base_media_decode_time(params.base_media_decode_time) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_tfdt();
}

std::string Tfdt::toStringOnlyData() const {
  return fmt::format("{} BaseMediaDecodeTime={}", getVersionAndFlagsString(), m_base_media_decode_time);
}

std::uint64_t Tfdt::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  return wbits + bitio::write_uint<std::uint64_t>(&writer, m_base_media_decode_time, m_version == 0 ? 32 : 64);
}

std::uint64_t Tfdt::getDataSize() const {
  return 8 + (m_version == 0 ? 0 : 4);
}

std::uint64_t Tfdt::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  return rbits + bitio::read_uint<std::uint64_t>(&reader, &m_base_media_decode_time, m_version == 0 ? 32 : 64);
}

}  // namespace shiguredo::mp4::box
