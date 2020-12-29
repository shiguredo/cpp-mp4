#include "shiguredo/mp4/box/urn.hpp"

#include <fmt/core.h>

#include <cstdint>
#include <istream>
#include <iterator>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_urn() {
  return BoxType("urn ");
}

Urn::Urn() {
  m_type = box_type_urn();
  setFlags(UrnSelfContainedFlags);
}

Urn::Urn(const UrnParameters& params) : m_name(params.name), m_location(params.location) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_urn();
}

std::string Urn::toStringOnlyData() const {
  if (checkFlag(UrnSelfContainedFlags)) {
    return getVersionAndFlagsString();
  }
  return fmt::format("{} Name=\"{}\" Location=\"{}\"", getVersionAndFlagsString(), m_name, m_location);
}

std::uint64_t Urn::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  if (checkFlag(UrnSelfContainedFlags)) {
    return wbits;
  }
  wbits += bitio::write_string(&writer, m_name);
  wbits += bitio::write_string(&writer, m_location);
  return wbits;
}

std::uint64_t Urn::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  if (checkFlag(UrnSelfContainedFlags)) {
    return rbits;
  }
  auto offset_to_end = static_cast<std::uint64_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
  auto name_rbits = bitio::read_string(&reader, &m_name, (offset_to_end - 1) * 8);
  rbits += name_rbits;
  rbits += bitio::read_string(&reader, &m_location, offset_to_end * 8 - name_rbits);
  return rbits;
}

std::uint64_t Urn::getDataSize() const {
  if (checkFlag(UrnSelfContainedFlags)) {
    return 4;
  }
  return 4 + std::size(m_name) + 1 + std::size(m_location) + 1;
}

}  // namespace shiguredo::mp4::box
