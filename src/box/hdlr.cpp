#include "shiguredo/mp4/box/hdlr.hpp"

#include <fmt/core.h>

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

BoxType box_type_hdlr() {
  return BoxType("hdlr");
}

Hdlr::Hdlr() {
  m_type = box_type_hdlr();
}

Hdlr::Hdlr(const HdlrParameters& params)
    : m_pre_defined(params.pre_defined),
      m_handler_type(params.handler_type),
      m_name(params.name),
      m_padding(params.padding) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_hdlr();
}

std::string Hdlr::toStringOnlyData() const {
  std::string type(std::begin(m_handler_type), std::end(m_handler_type));
  return fmt::format(R"({} PreDefined={} HandlerType="{}" Name="{}")", getVersionAndFlagsString(), m_pre_defined, type,
                     m_name);
}

std::uint64_t Hdlr::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, static_cast<std::uint32_t>(m_pre_defined));
  wbits += bitio::write_array_uint8_4(&writer, m_handler_type);
  wbits += bitio::write_array_uint8_4(&writer, m_manufacturer);
  wbits += bitio::write_array_uint8_4(&writer, m_flags);
  wbits += bitio::write_array_uint8_4(&writer, m_flags_mask);
  wbits += bitio::write_string(&writer, m_name);
  return wbits + bitio::write_vector_uint<std::uint8_t>(&writer, m_padding);
}

std::uint64_t Hdlr::getDataSize() const {
  return 24 + std::size(m_name) + 1 + std::size(m_padding);
}

std::uint64_t Hdlr::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_pre_defined);
  rbits += bitio::read_array_uint8_4(&reader, &m_handler_type);
  rbits += bitio::read_array_uint8_4(&reader, &m_manufacturer);
  rbits += bitio::read_array_uint8_4(&reader, &m_flags);
  rbits += bitio::read_array_uint8_4(&reader, &m_flags_mask);

  auto offset_to_end = static_cast<std::uint64_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
  std::uint64_t name_rbits;
  if (m_pre_defined != 0) {
    name_rbits = bitio::read_pascal_string(&reader, &m_name, offset_to_end * 8);
  } else {
    name_rbits = bitio::read_string(&reader, &m_name, offset_to_end * 8);
  }
  if (offset_to_end * 8 < name_rbits || name_rbits % 8 != 0) {
    throw std::runtime_error(
        fmt::format("Hdlr::readData(): invalid name_rbits: offset_to_end={} name_rbits={}", offset_to_end, name_rbits));
  }
  rbits += name_rbits;
  return rbits + bitio::read_vector_uint<std::uint8_t>(&reader, (offset_to_end * 8) - name_rbits, &m_padding);
}

}  // namespace shiguredo::mp4::box
