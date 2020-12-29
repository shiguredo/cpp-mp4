#include "shiguredo/mp4/box/skip.hpp"

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
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_skip() {
  return BoxType("skip");
}

Skip::Skip() {
  m_type = box_type_skip();
}

Skip::Skip(const SkipParameters& params) : m_data(params.data) {
  m_type = box_type_skip();
}

std::string Skip::toStringOnlyData() const {
  return fmt::format("Data=[{:#x}]", fmt::join(m_data, ", "));
}

std::uint64_t Skip::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  return bitio::write_vector_uint<std::uint8_t>(&writer, m_data);
}

std::uint64_t Skip::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::size_t offset_to_end = static_cast<std::size_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
  return bitio::read_vector_uint<std::uint8_t>(&reader, offset_to_end, &m_data);
}

std::uint64_t Skip::getDataSize() const {
  return std::size(m_data);
}

}  // namespace shiguredo::mp4::box
