#include "shiguredo/mp4/box/udta.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_udta() {
  return BoxType("udta");
}

Udta::Udta() {
  m_type = box_type_udta();
}

std::string Udta::toStringOnlyData() const {
  return "";
}

std::uint64_t Udta::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Udta::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
