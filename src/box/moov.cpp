#include "shiguredo/mp4/box/moov.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_moov() {
  return BoxType("moov");
}

Moov::Moov() {
  m_type = box_type_moov();
}

std::string Moov::toStringOnlyData() const {
  return "";
}

std::uint64_t Moov::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Moov::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
