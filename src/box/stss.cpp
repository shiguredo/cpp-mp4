#include "shiguredo/mp4/box/stss.hpp"

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

BoxType box_type_stss() {
  return BoxType("stss");
}

Stss::Stss() {
  m_type = box_type_stss();
}

Stss::Stss(const StssParmeters& params) : m_sample_numbers(params.sample_numbers) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_stss();
}

std::string Stss::toStringOnlyData() const {
  return fmt::format("{} EntryCount={} SampleNumbers=[{}]", getVersionAndFlagsString(), std::size(m_sample_numbers),
                     fmt::join(m_sample_numbers, ", "));
}

std::uint64_t Stss::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  std::uint32_t entry_count = static_cast<std::uint32_t>(std::size(m_sample_numbers));
  return wbits += bitio::write_uint<std::uint32_t>(&writer, entry_count) +
                  bitio::write_vector_uint<std::uint32_t>(&writer, m_sample_numbers);
}

std::uint64_t Stss::getDataSize() const {
  return 8 + std::size(m_sample_numbers) * 4;
}

std::uint64_t Stss::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  std::uint32_t entry_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &entry_count);
  return rbits += bitio::read_vector_uint<std::uint32_t>(&reader, entry_count, &m_sample_numbers);
}

}  // namespace shiguredo::mp4::box
