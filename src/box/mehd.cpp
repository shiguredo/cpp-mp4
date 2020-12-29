#include "shiguredo/mp4/box/mehd.hpp"

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

BoxType box_type_mehd() {
  return BoxType("mehd");
}

Mehd::Mehd() {
  m_type = box_type_mehd();
}

Mehd::Mehd(const MehdParameters& params) : m_fragment_duration(params.fragment_duration) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_mehd();
}

std::string Mehd::toStringOnlyData() const {
  return fmt::format("{} FragmentDuration={}", getVersionAndFlagsString(), m_fragment_duration);
}

std::uint64_t Mehd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  return wbits + bitio::write_uint<std::uint64_t>(&writer, m_fragment_duration, m_version == 0 ? 32 : 64);
}

std::uint64_t Mehd::getDataSize() const {
  return 4 + (m_version == 0 ? 4 : 8);
}

std::uint64_t Mehd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  return rbits + bitio::read_uint<std::uint64_t>(&reader, &m_fragment_duration, m_version == 0 ? 32 : 64);
}

}  // namespace shiguredo::mp4::box
