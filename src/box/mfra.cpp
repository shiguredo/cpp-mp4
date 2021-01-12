#include "shiguredo/mp4/box/mfra.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_mfra() {
  return BoxType("mfra");
}

Mfra::Mfra() {
  m_type = box_type_mfra();
}

std::string Mfra::toStringOnlyData() const {
  return "";
}

std::uint64_t Mfra::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Mfra::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
