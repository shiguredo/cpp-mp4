#include "shiguredo/mp4/box/moof.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_moof() {
  return BoxType("moof");
}

Moof::Moof() {
  m_type = box_type_moof();
}

std::string Moof::toStringOnlyData() const {
  return "";
}

std::uint64_t Moof::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Moof::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
