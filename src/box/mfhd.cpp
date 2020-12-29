#include "shiguredo/mp4/box/mfhd.hpp"

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

BoxType box_type_mfhd() {
  return BoxType("mfhd");
}

Mfhd::Mfhd() {
  m_type = box_type_mfhd();
}

Mfhd::Mfhd(const MfhdParameters& params) : m_sequence_number(params.sequence_number) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_mfhd();
}

std::string Mfhd::toStringOnlyData() const {
  return fmt::format("{} SequenceNumber={}", getVersionAndFlagsString(), m_sequence_number);
}

std::uint64_t Mfhd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  return wbits + bitio::write_uint<std::uint32_t>(&writer, m_sequence_number);
}

std::uint64_t Mfhd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  return rbits + bitio::read_uint<std::uint32_t>(&reader, &m_sequence_number);
}

std::uint64_t Mfhd::getDataSize() const {
  return 8;
}

}  // namespace shiguredo::mp4::box
