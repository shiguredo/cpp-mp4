#include "shiguredo/mp4/box/stsd.hpp"

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

BoxType box_type_stsd() {
  return BoxType("stsd");
}

Stsd::Stsd() {
  m_type = box_type_stsd();
}

Stsd::Stsd(const StsdParameters& params) : m_entry_count(params.entry_count) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_stsd();
}

std::string Stsd::toStringOnlyData() const {
  return fmt::format("{} EntryCount={}", getVersionAndFlagsString(), m_entry_count);
}

std::uint64_t Stsd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  return wbits + bitio::write_uint<std::uint32_t>(&writer, m_entry_count);
}

std::uint64_t Stsd::getDataSize() const {
  return 8;
}

std::uint64_t Stsd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  return rbits + bitio::read_uint<std::uint32_t>(&reader, &m_entry_count);
}

}  // namespace shiguredo::mp4::box
