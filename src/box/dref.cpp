#include "shiguredo/mp4/box/dref.hpp"

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

BoxType box_type_dref() {
  return BoxType("dref");
}

Dref::Dref() {
  m_type = box_type_dref();
}

Dref::Dref(const DrefParameters& params) : m_entry_count(params.entry_count) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_dref();
}

std::string Dref::toStringOnlyData() const {
  return fmt::format("{} EntryCount={}", getVersionAndFlagsString(), m_entry_count);
}

std::uint64_t Dref::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  return wbits + bitio::write_uint<std::uint32_t>(&writer, m_entry_count);
}

std::uint64_t Dref::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  return rbits + bitio::read_uint<std::uint32_t>(&reader, &m_entry_count);
}

std::uint64_t Dref::getDataSize() const {
  return 8;
}

}  // namespace shiguredo::mp4::box
