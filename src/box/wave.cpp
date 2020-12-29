#include "shiguredo/mp4/box/wave.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_wave() {
  return BoxType("wave");
}

Wave::Wave() {
  m_type = box_type_wave();
}

std::string Wave::toStringOnlyData() const {
  return "";
}

std::uint64_t Wave::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Wave::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
