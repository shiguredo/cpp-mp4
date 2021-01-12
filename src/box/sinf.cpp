#include "shiguredo/mp4/box/sinf.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_sinf() {
  return BoxType("sinf");
}

Sinf::Sinf() {
  m_type = box_type_sinf();
}

std::string Sinf::toStringOnlyData() const {
  return "";
}

std::uint64_t Sinf::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Sinf::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
