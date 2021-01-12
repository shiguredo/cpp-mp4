#include "shiguredo/mp4/box/pixel_aspect_ratio.hpp"

#include <fmt/core.h>

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_pasp() {
  return BoxType("pasp");
}

PixelAspectRatio::PixelAspectRatio() {
  m_type = box_type_pasp();
}

PixelAspectRatio::PixelAspectRatio(const PixelAspectRatioParameters& params)
    : m_h_spacing(params.h_spacing), m_v_spacing(params.v_spacing) {
  m_type = box_type_pasp();
}

std::string PixelAspectRatio::toStringOnlyData() const {
  return fmt::format("HSpacing={} VSpacing={}", m_h_spacing, m_v_spacing);
}

std::uint64_t PixelAspectRatio::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  auto wbits = bitio::write_uint<std::uint32_t>(&writer, m_h_spacing);
  return wbits + bitio::write_uint<std::uint32_t>(&writer, m_v_spacing);
}

std::uint64_t PixelAspectRatio::readData(std::istream& is) {
  bitio::Reader reader(is);
  auto rbits = bitio::read_uint<std::uint32_t>(&reader, &m_h_spacing);
  return rbits + bitio::read_uint<std::uint32_t>(&reader, &m_v_spacing);
}

std::uint64_t PixelAspectRatio::getDataSize() const {
  return 8;
}

}  // namespace shiguredo::mp4::box
