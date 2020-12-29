#include "shiguredo/mp4/box/stbl.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_stbl() {
  return BoxType("stbl");
}

Stbl::Stbl() {
  m_type = box_type_stbl();
}

std::string Stbl::toStringOnlyData() const {
  return "";
}

std::uint64_t Stbl::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Stbl::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
