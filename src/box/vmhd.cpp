#include "shiguredo/mp4/box/vmhd.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cstdint>
#include <istream>
#include <ostream>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_vmhd() {
  return BoxType("vmhd");
}

Vmhd::Vmhd() {
  m_type = box_type_vmhd();
}

Vmhd::Vmhd(const VmhdParameters& params) : m_graphicsmode(params.graphicsmode), m_opcolor(params.opcolor) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_vmhd();
}

std::string Vmhd::toStringOnlyData() const {
  return fmt::format("{} Graphicsmode={} Opcolor=[{}]", getVersionAndFlagsString(), m_graphicsmode,
                     fmt::join(m_opcolor, ", "));
}

std::uint64_t Vmhd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_graphicsmode);
  for (const auto c : m_opcolor) {
    wbits += bitio::write_uint<std::uint16_t>(&writer, c);
  }
  return wbits;
}

std::uint64_t Vmhd::getDataSize() const {
  return 12;
}

std::uint64_t Vmhd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_graphicsmode);
  for (std::size_t i = 0; i < std::size(m_opcolor); ++i) {
    rbits += bitio::read_uint<std::uint16_t>(&reader, &(m_opcolor[i]));
  }
  return rbits;
}

}  // namespace shiguredo::mp4::box
