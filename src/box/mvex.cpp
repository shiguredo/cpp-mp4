#include "shiguredo/mp4/box/mvex.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_mvex() {
  return BoxType("mvex");
}

Mvex::Mvex() {
  m_type = box_type_mvex();
}

std::string Mvex::toStringOnlyData() const {
  return "";
}

std::uint64_t Mvex::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Mvex::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
