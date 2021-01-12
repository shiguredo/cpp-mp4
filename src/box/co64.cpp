#include "shiguredo/mp4/box/co64.hpp"

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

BoxType box_type_co64() {
  return BoxType("co64");
}

Co64::Co64() {
  m_type = box_type_co64();
}

Co64::Co64(const Co64Parameters& params) : m_chunk_offsets(params.chunk_offsets) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_co64();
}

std::string Co64::toStringOnlyData() const {
  return fmt::format("{} EntryCount={} ChunkOffsets=[{:#x}]", getVersionAndFlagsString(), std::size(m_chunk_offsets),
                     fmt::join(m_chunk_offsets, ", "));
}

std::uint64_t Co64::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(std::size(m_chunk_offsets)));
  wbits += bitio::write_vector_uint<std::uint64_t>(&writer, m_chunk_offsets);
  return wbits;
}

std::uint64_t Co64::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);

  std::uint32_t entry_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &entry_count);
  rbits += bitio::read_vector_uint<std::uint64_t>(&reader, entry_count, &m_chunk_offsets);
  return rbits;
}

std::uint64_t Co64::getDataSize() const {
  return 8 + std::size(m_chunk_offsets) * 8;
}

std::ostream& operator<<(std::ostream& os, const Co64& co64) {
  os << co64.toString();
  return os;
}

}  // namespace shiguredo::mp4::box
