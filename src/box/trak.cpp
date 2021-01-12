#include "shiguredo/mp4/box/trak.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_trak() {
  return BoxType("trak");
}

Trak::Trak() {
  m_type = box_type_trak();
}

std::string Trak::toStringOnlyData() const {
  return "";
}

std::uint64_t Trak::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Trak::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
