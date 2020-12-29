#include "shiguredo/mp4/box/stco.hpp"

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

BoxType box_type_stco() {
  return BoxType("stco");
}

Stco::Stco() {
  m_type = box_type_stco();
}

Stco::Stco(const StcoParameters& params) : m_chunk_offsets(params.chunk_offsets) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_stco();
}

std::string Stco::toStringOnlyData() const {
  return fmt::format("{} EntryCount={} ChunkOffsets=[{:#x}]", getVersionAndFlagsString(), std::size(m_chunk_offsets),
                     fmt::join(m_chunk_offsets, ", "));
}

std::uint64_t Stco::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  return wbits + bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(std::size(m_chunk_offsets))) +
         bitio::write_vector_uint<std::uint32_t>(&writer, m_chunk_offsets);
}

std::uint64_t Stco::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  std::uint32_t entry_count;
  rbits += bitio::read_uint<std::uint32_t>(&reader, &entry_count);
  return rbits + bitio::read_vector_uint<std::uint32_t>(&reader, entry_count, &m_chunk_offsets);
}

std::uint64_t Stco::getDataSize() const {
  return 8 + std::size(m_chunk_offsets) * 4;
}

std::ostream& operator<<(std::ostream& os, const Stco& stco) {
  os << stco.toString();
  return os;
}

}  // namespace shiguredo::mp4::box
