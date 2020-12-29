#include "shiguredo/mp4/box/dinf.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_dinf() {
  return BoxType("dinf");
}

Dinf::Dinf() {
  m_type = box_type_dinf();
}

std::string Dinf::toStringOnlyData() const {
  return "";
}

std::uint64_t Dinf::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Dinf::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
