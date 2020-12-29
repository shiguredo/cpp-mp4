#include "shiguredo/mp4/box/meta.hpp"

#include <array>
#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_meta() {
  return BoxType("meta");
}

Meta::Meta() {
  m_type = box_type_meta();
}

std::string Meta::toStringOnlyData() const {
  return getVersionAndFlagsString();
}

std::uint64_t Meta::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  return writeVersionAndFlag(&writer);
}

std::uint64_t Meta::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  if ((m_version | m_flags[0] | m_flags[1] | m_flags[2]) != 0) {
    m_version = 0;
    m_flags = {0, 0, 0};
  }

  return rbits;
}

}  // namespace shiguredo::mp4::box
