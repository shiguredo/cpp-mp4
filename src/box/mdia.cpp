#include "shiguredo/mp4/box/mdia.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_mdia() {
  return BoxType("mdia");
}

Mdia::Mdia() {
  m_type = box_type_mdia();
}

std::string Mdia::toStringOnlyData() const {
  return "";
}

std::uint64_t Mdia::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Mdia::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
