#include "shiguredo/mp4/box/stsz.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cstdint>
#include <istream>
#include <iterator>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_stsz() {
  return BoxType("stsz");
}

Stsz::Stsz() {
  m_type = box_type_stsz();
}

Stsz::Stsz(const StszParameters& params) : m_sample_size(params.sample_size), m_entry_sizes(params.entry_sizes) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_stsz();
}

std::string Stsz::toStringOnlyData() const {
  return fmt::format("{} SampleSize={} SampleCount={} EntrySizes=[{}]", getVersionAndFlagsString(), m_sample_size,
                     std::size(m_entry_sizes), fmt::join(m_entry_sizes, ", "));
}

std::uint64_t Stsz::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_sample_size);
  wbits += bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(std::size(m_entry_sizes)));
  if (m_sample_size == 0) {
    wbits += bitio::write_vector_uint<std::uint32_t>(&writer, m_entry_sizes);
  }
  return wbits;
}

std::uint64_t Stsz::getDataSize() const {
  return 12 + (m_sample_size == 0 ? 4 * std::size(m_entry_sizes) : 0);
}

std::uint64_t Stsz::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_sample_size);
  std::uint32_t sample_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &sample_count);
  if (m_sample_size == 0) {
    rbits += bitio::read_vector_uint<std::uint32_t>(&reader, sample_count, &m_entry_sizes);
  } else {
    m_entry_sizes.clear();
  }
  return rbits;
}

}  // namespace shiguredo::mp4::box
