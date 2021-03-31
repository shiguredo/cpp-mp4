#include "shiguredo/mp4/box/colr.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <array>
#include <iterator>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_colr() {
  return BoxType("colr");
}

Colr::Colr() {
  m_type = box_type_colr();
}

Colr::Colr(const ColrParameters& params)
    : m_colour_type(params.colour_type),
      m_colour_primaries(params.colour_primaries),
      m_transfer_characteristics(params.transfer_characteristics),
      m_matrix_coefficents(params.matrix_coefficents),
      m_full_range_flag(params.full_range_flag),
      m_profile(params.profile),
      m_unknown(params.unknown) {
  m_type = box_type_colr();
}

std::string Colr::toStringOnlyData() const {
  if (m_colour_type == std::array<std::uint8_t, 4>{'n', 'c', 'l', 'x'}) {
    return nclxToString();
  }
  if (m_colour_type == std::array<std::uint8_t, 4>{'r', 'I', 'C', 'C'} ||
      m_colour_type == std::array<std::uint8_t, 4>{'p', 'r', 'o', 'f'}) {
    return profileToString();
  }
  return unknownToString();
}

std::string Colr::nclxToString() const {
  return fmt::format(
      "ColourType=\"{:c}{:c}{:c}{:c}\" ColourPrimaries={} "
      "TransferCharacteristics={} "
      "MatrixCoefficients={} FullRangeFlag={}",
      m_colour_type[0], m_colour_type[1], m_colour_type[2], m_colour_type[3], m_colour_primaries,
      m_transfer_characteristics, m_matrix_coefficents, m_full_range_flag);
}

std::string Colr::profileToString() const {
  return fmt::format("ColourType=\"{:c}{:c}{:c}{:c}\" Profile=[{:#x}]", m_colour_type[0], m_colour_type[1],
                     m_colour_type[2], m_colour_type[3], fmt::join(m_profile, ", "));
}

std::string Colr::unknownToString() const {
  return fmt::format("ColourType=\"{:c}{:c}{:c}{:c}\" Unknown=[{:#x}]", m_colour_type[0], m_colour_type[1],
                     m_colour_type[2], m_colour_type[3], fmt::join(m_unknown, ", "));
}

std::uint64_t Colr::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = 0;
  for (std::size_t i = 0; i < 4; ++i) {
    wbits += bitio::write_uint<std::uint8_t>(&writer, m_colour_type[i]);
  }
  if (m_colour_type == std::array<std::uint8_t, 4>{'n', 'c', 'l', 'x'}) {
    return wbits + nclxMarshal(&writer);
  }
  if (m_colour_type == std::array<std::uint8_t, 4>{'r', 'I', 'C', 'C'} ||
      m_colour_type == std::array<std::uint8_t, 4>{'p', 'r', 'o', 'f'}) {
    return wbits + bitio::write_vector_uint<std::uint8_t>(&writer, m_profile);
  }
  return wbits + bitio::write_vector_uint<std::uint8_t>(&writer, m_unknown);
}

std::uint64_t Colr::nclxMarshal(bitio::Writer* writer) const {
  std::uint64_t wbits = 0;
  wbits += bitio::write_uint<std::uint16_t>(writer, m_colour_primaries);
  wbits += bitio::write_uint<std::uint16_t>(writer, m_transfer_characteristics);
  wbits += bitio::write_uint<std::uint16_t>(writer, m_matrix_coefficents);
  wbits += write_bool(writer, m_full_range_flag);
  wbits += bitio::write_uint<std::uint8_t>(writer, m_reserved, 7);
  return wbits;
}

std::uint64_t Colr::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = bitio::read_uint<std::uint8_t>(&reader, &m_colour_type[0]);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_colour_type[1]);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_colour_type[2]);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_colour_type[3]);

  if (m_colour_type == std::array<std::uint8_t, 4>{'n', 'c', 'l', 'x'}) {
    return rbits + nclxUnwriteData(&reader);
  }

  const auto offset_to_end = static_cast<std::size_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));

  if (m_colour_type == std::array<std::uint8_t, 4>{'r', 'I', 'C', 'C'} ||
      m_colour_type == std::array<std::uint8_t, 4>{'p', 'r', 'o', 'f'}) {
    return rbits + bitio::read_vector_uint<std::uint8_t>(&reader, offset_to_end, &m_profile);
  }

  return rbits + bitio::read_vector_uint<std::uint8_t>(&reader, offset_to_end, &m_unknown);
}

std::uint64_t Colr::nclxUnwriteData(bitio::Reader* reader) {
  std::uint64_t rbits = bitio::read_uint<std::uint16_t>(reader, &m_colour_primaries);
  rbits += bitio::read_uint<std::uint16_t>(reader, &m_transfer_characteristics);
  rbits += bitio::read_uint<std::uint16_t>(reader, &m_matrix_coefficents);
  rbits += read_bool(reader, &m_full_range_flag);
  rbits += bitio::read_uint<std::uint8_t>(reader, &m_reserved, 7);
  return rbits;
}

std::uint64_t Colr::getDataSize() const {
  if (m_colour_type == std::array<std::uint8_t, 4>{'n', 'c', 'l', 'x'}) {
    return 11;
  }
  if (m_colour_type == std::array<std::uint8_t, 4>{'r', 'I', 'C', 'C'} ||
      m_colour_type == std::array<std::uint8_t, 4>{'p', 'r', 'o', 'f'}) {
    return 4 + std::size(m_profile);
  }
  return 4 + std::size(m_unknown);
}

}  // namespace shiguredo::mp4::box
