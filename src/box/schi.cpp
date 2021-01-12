#include "shiguredo/mp4/box/schi.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_schi() {
  return BoxType("schi");
}

Schi::Schi() {
  m_type = box_type_schi();
}

std::string Schi::toStringOnlyData() const {
  return "";
}

std::uint64_t Schi::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Schi::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
