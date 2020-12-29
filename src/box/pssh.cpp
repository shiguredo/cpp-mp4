#include "shiguredo/mp4/box/pssh.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <ext/alloc_traits.h>

#include <algorithm>
#include <cstdint>
#include <istream>
#include <iterator>
#include <numeric>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

PsshKID::PsshKID(const PsshKIDParameters& params) : m_kid(params.kid) {}

std::string PsshKID::toString() const {
  return fmt::format(R"("{:02x}")", fmt::join(m_kid, ""));
}

std::uint64_t PsshKID::writeData(bitio::Writer* writer) const {
  std::uint64_t wbits = 0;
  for (std::size_t i = 0; i < std::size(m_kid); ++i) {
    wbits += bitio::write_uint<std::uint8_t>(writer, m_kid[i]);
  }
  return wbits;
}

std::uint64_t PsshKID::readData(bitio::Reader* reader) {
  std::uint64_t rbits = 0;
  for (std::size_t i = 0; i < std::size(m_kid); ++i) {
    rbits += bitio::read_uint<std::uint8_t>(reader, &m_kid[i]);
  }
  return rbits;
}

BoxType box_type_pssh() {
  return BoxType("pssh");
}

Pssh::Pssh() {
  m_type = box_type_pssh();
}

Pssh::Pssh(const PsshParameters& params) : m_system_id(params.system_id), m_kids(params.kids), m_data(params.data) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_pssh();
}

std::string Pssh::toStringOnlyData() const {
  auto s = fmt::format(R"({} SystemID="{:02x}")", getVersionAndFlagsString(), fmt::join(m_system_id, ""));
  if (m_version != 0) {
    std::vector<std::string> kids;
    std::transform(std::begin(m_kids), std::end(m_kids), std::back_inserter(kids),
                   [](const auto& k) { return k.toString(); });
    s += fmt::format(" KIDCount={} KIDs=[{}]", std::size(m_kids), fmt::join(kids, " "));
  }
  s += fmt::format(" DataSize={} Data=[{:#x}]", std::size(m_data), fmt::join(m_data, ", "));
  return s;
}

std::uint64_t Pssh::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  for (const auto i : m_system_id) {
    wbits += bitio::write_uint<std::uint8_t>(&writer, i);
  }

  if (m_version != 0) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(std::size(m_kids)));
    wbits += std::accumulate(std::begin(m_kids), std::end(m_kids), 0UL,
                             [&writer](const auto a, const auto& k) { return a + k.writeData(&writer); });
  }
  wbits += bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(std::size(m_data)));
  wbits += bitio::write_vector_uint<std::uint8_t>(&writer, m_data);
  return wbits;
}

std::uint64_t Pssh::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  for (std::size_t i = 0; i < std::size(m_system_id); ++i) {
    rbits += bitio::read_uint<std::uint8_t>(&reader, &(m_system_id[i]));
  }

  if (m_version != 0) {
    std::uint32_t kid_count;
    rbits += bitio::read_uint<std::uint32_t>(&reader, &kid_count);
    m_kids.resize(static_cast<std::size_t>(kid_count));
    for (std::uint32_t i = 0; i < kid_count; ++i) {
      PsshKID kid;
      rbits += kid.readData(&reader);
      m_kids[i] = kid;
    }
  }

  std::uint32_t data_size;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &data_size);
  rbits += bitio::read_vector_uint<std::uint8_t>(&reader, data_size, &m_data);
  return rbits;
}

std::uint64_t Pssh::getDataSize() const {
  if (m_version == 0) {
    return 24 + std::size(m_data);
  }

  return 28 + std::size(m_data) + std::size(m_kids) * 16;
}

}  // namespace shiguredo::mp4::box
