#include "shiguredo/mp4/box/mfro.hpp"

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

BoxType box_type_mfro() {
  return BoxType("mfro");
}

Mfro::Mfro() {
  m_type = box_type_mfro();
}

Mfro::Mfro(const MfroParameters& params) : m_size(params.size) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_mfro();
}

std::string Mfro::toStringOnlyData() const {
  return fmt::format("{} Size={}", getVersionAndFlagsString(), m_size);
}

std::uint64_t Mfro::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  return wbits + bitio::write_uint<std::uint32_t>(&writer, m_size);
}

std::uint64_t Mfro::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  return rbits + bitio::read_uint<std::uint32_t>(&reader, &m_size);
}

std::uint64_t Mfro::getDataSize() const {
  return 8;
}

}  // namespace shiguredo::mp4::box
