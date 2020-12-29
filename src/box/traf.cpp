#include "shiguredo/mp4/box/traf.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_traf() {
  return BoxType("traf");
}

Traf::Traf() {
  m_type = box_type_traf();
}

std::string Traf::toStringOnlyData() const {
  return "";
}

std::uint64_t Traf::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Traf::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
