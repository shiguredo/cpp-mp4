#include "shiguredo/mp4/box/ilst.hpp"

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_ilst() {
  return BoxType("ilst");
}

Ilst::Ilst() {
  m_type = box_type_ilst();
}

IlstMeta::IlstMeta(const IlstMetaParameters& params) {
  m_type = params.type;
}

std::string Ilst::toStringOnlyData() const {
  return "";
}

std::uint64_t Ilst::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t Ilst::readData(std::istream&) {
  return 0;
}

std::string IlstMeta::toStringOnlyData() const {
  return "";
}

std::uint64_t IlstMeta::writeData(std::ostream&) const {
  return 0;
}

std::uint64_t IlstMeta::readData(std::istream&) {
  return 0;
}

}  // namespace shiguredo::mp4::box
