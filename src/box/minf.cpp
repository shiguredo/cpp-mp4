#include "shiguredo/mp4/box/minf.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_minf() {
  return BoxType("minf");
}

Minf::Minf() {
  m_type = box_type_minf();
}

std::string Minf::toStringOnlyData() const {
  return "";
}

std::uint64_t Minf::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Minf::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
