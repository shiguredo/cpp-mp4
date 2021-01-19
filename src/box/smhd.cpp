#include "shiguredo/mp4/box/smhd.hpp"

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

BoxType box_type_smhd() {
  return BoxType("smhd");
}

Smhd::Smhd() {
  m_type = box_type_smhd();
}

Smhd::Smhd(const SmhdParameters& params) : m_balance(params.balance) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_smhd();
}

std::string Smhd::toStringOnlyData() const {
  return fmt::format("{} Balance={:.2f}", getVersionAndFlagsString(), getBalance());
}

std::uint64_t Smhd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  const std::uint64_t wbits = writeVersionAndFlag(&writer);
  return wbits + bitio::write_uint<std::int16_t>(&writer, m_balance) +
         bitio::write_uint<std ::uint16_t>(&writer, m_reserved);
}

std::uint64_t Smhd::getDataSize() const {
  return 8;
}

std::uint64_t Smhd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  return rbits + bitio::read_uint<std::int16_t>(&reader, &m_balance) +
         bitio::read_uint<std::uint16_t>(&reader, &m_reserved);
}

float Smhd::getBalance() const {
  return static_cast<float>(m_balance) / (1 << 8);
}

}  // namespace shiguredo::mp4::box
