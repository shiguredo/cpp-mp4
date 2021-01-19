#include "shiguredo/mp4/box/data.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <istream>
#include <iterator>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_data() {
  return BoxType("data");
}

Data::Data() {
  m_type = box_type_data();
}

Data::Data(const DataParameters& params)
    : m_data_type(params.data_type), m_data_lang(params.data_lang), m_data(params.data) {
  m_type = box_type_data();
}

std::string Data::toStringOnlyData() const {
  std::string s = fmt::format("DataType={} DataLang={} ", data_type_to_string(m_data_type), m_data_lang);
  if (m_data_type == StringUTF8) {
    std::vector<std::uint8_t> escaped;
    std::transform(std::begin(m_data), std::end(m_data), std::back_inserter(escaped), [](const std::uint8_t c) {
      if (std::isprint(c)) {
        return c;
      }
      return static_cast<std::uint8_t>('.');
    });
    std::string d(std::begin(escaped), std::end(escaped));
    return s + fmt::format(R"(Data="{}")", d);
  }

  return s + fmt::format("Data=[{:#x}]", fmt::join(m_data, ", "));
}

std::uint64_t Data::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  auto wbits = bitio::write_uint<std::uint32_t>(&writer, m_data_type);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_data_lang);
  return wbits + bitio::write_vector_uint<std::uint8_t>(&writer, m_data);
}

std::uint64_t Data::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint32_t data_type;
  auto rbits = bitio::read_uint<std::uint32_t>(&reader, &data_type);
  m_data_type = static_cast<DataType>(data_type);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_data_lang);
  const std::size_t offset_to_end = static_cast<std::size_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
  return rbits + bitio::read_vector_uint<std::uint8_t>(&reader, offset_to_end, &m_data);
}

std::uint64_t Data::getDataSize() const {
  return 8 + std::size(m_data);
}

}  // namespace shiguredo::mp4::box
