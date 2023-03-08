#include "shiguredo/mp4/box/fiel.hpp"

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

BoxType box_type_fiel() {
  return BoxType("fiel");
}

Fiel::Fiel(const FielParameters& params) : m_field_count(params.field_count), m_field_ordering(params.field_ordering) {
  m_type = box_type_fiel();
}

Fiel::Fiel() {
  m_type = box_type_fiel();
}

std::string Fiel::toStringOnlyData() const {
  return fmt::format("FieldCount={} FieldOrdering={}", m_field_count, m_field_ordering);
}

std::uint64_t Fiel::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  return bitio::write_uint<std::uint8_t>(&writer, m_field_count) +
         bitio::write_uint<std::uint8_t>(&writer, m_field_ordering);
}

std::uint64_t Fiel::getDataSize() const {
  return 2;
}

std::uint64_t Fiel::readData(std::istream& is) {
  bitio::Reader reader(is);
  return bitio::read_uint<std::uint8_t>(&reader, &m_field_count) +
         bitio::read_uint<std::uint8_t>(&reader, &m_field_ordering);
}

}  // namespace shiguredo::mp4::box
