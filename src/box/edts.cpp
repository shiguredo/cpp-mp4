#include "shiguredo/mp4/box/edts.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_edts() {
  return BoxType("edts");
}

Edts::Edts() {
  m_type = box_type_edts();
}

std::string Edts::toStringOnlyData() const {
  return "";
}

std::uint64_t Edts::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Edts::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
