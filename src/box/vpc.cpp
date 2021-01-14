#include "shiguredo/mp4/box/vpc.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cstdint>
#include <istream>
#include <ostream>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_vpcc() {
  return BoxType("vpcC");
}

VPCodecConfiguration::VPCodecConfiguration() {
  m_type = box_type_vpcc();
}

std::string VPCodecConfiguration::toStringOnlyData() const {
  return fmt::format(
      "{} Profile={} Level={} BitDepth={} ChromaSubSampling={} VideoFullRangeFlag={} ColourPrimaries={} "
      "TransferCharacteristics={} MatrixCoefficents={} CodecInitializationData=[{:#x}]",
      getVersionAndFlagsString(), m_profile, m_level, m_bit_depth, m_chroma_sub_sampling, m_video_full_range_flag,
      m_colour_primaries, m_transfer_characteristics, m_matrix_coefficents,
      fmt::join(m_codec_initialization_data, ", "));
}

std::uint64_t VPCodecConfiguration::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_profile);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_level);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_bit_depth, 4);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_chroma_sub_sampling, 3);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_video_full_range_flag, 1);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_colour_primaries);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_transfer_characteristics);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_matrix_coefficents);
  if (m_version == 0) {
    return wbits;
  }
  std::uint16_t size = static_cast<std::uint16_t>(std::size(m_codec_initialization_data));
  wbits += bitio::write_uint<std::uint16_t>(&writer, size);
  return wbits + bitio::write_vector_uint<std::uint8_t>(&writer, m_codec_initialization_data);
}

std::uint64_t VPCodecConfiguration::getDataSize() const {
  if (m_version == 0) {
    return 10;
  }
  return 12 + std::size(m_codec_initialization_data);
}

std::uint64_t VPCodecConfiguration::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_profile);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_level);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_bit_depth, 4);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_chroma_sub_sampling, 3);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_video_full_range_flag, 1);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_colour_primaries);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_transfer_characteristics);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_matrix_coefficents);
  if (m_version == 0) {
    return rbits;
  }
  std::uint16_t size;
  rbits += bitio::read_uint<std::uint16_t>(&reader, &size);
  return rbits +
         bitio::read_vector_uint<std::uint8_t>(&reader, static_cast<std::size_t>(size), &m_codec_initialization_data);
}

VPCodecConfiguration::VPCodecConfiguration(const VPCodecConfigurationParameters& params)
    : m_profile(params.profile),
      m_level(params.level),
      m_bit_depth(params.bit_depth),
      m_chroma_sub_sampling(params.chroma_sub_sampling),
      m_video_full_range_flag(params.video_full_range_flag),
      m_colour_primaries(params.colour_primaries),
      m_transfer_characteristics(params.transfer_characteristics),
      m_matrix_coefficents(params.matrix_coefficents),
      m_codec_initialization_data(params.codec_initialization_data) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_vpcc();
}

}  // namespace shiguredo::mp4::box
