#include "shiguredo/mp4/box/av1c.hpp"

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
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_av1c() {
  return BoxType("av1C");
}

AV1CodecConfiguration::AV1CodecConfiguration() {
  m_type = box_type_av1c();
}

std::string AV1CodecConfiguration::toStringOnlyData() const {
  auto s = fmt::format(
      "Marker={} Version={} SeqProfile={} SeqLevelIdx0={} SeqTier0={} HighBitdepth={} "
      "TwelveBit={} Monochrome={} ChromaSubsamplingX={} ChromaSubsamplingY={} ChromaSamplePosition={} "
      "InitialPresentationDelayPresent={}",
      m_marker, m_version, m_seq_profile, m_seq_level_idx_0, m_seq_tier_0, m_high_bitdepth, m_twelve_bit, m_monochrome,
      m_chroma_subsampling_x, m_chroma_subsampling_y, m_chroma_sample_position, m_initial_presentation_delay_present);

  if (m_initial_presentation_delay_present) {
    s += fmt::format(" InitialPresentationDelayMinusOne={}", m_initial_presentation_delay_minus_one);
  }

  return s + fmt::format(" ConfigOBUs=[{:#x}]", fmt::join(m_config_OBUs, ", "));
}

std::uint64_t AV1CodecConfiguration::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = bitio::write_uint<std::uint8_t>(&writer, m_marker, 1);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_version, 7);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_seq_profile, 3);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_seq_level_idx_0, 5);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_seq_tier_0, 1);
  wbits += bitio::write_bool(&writer, m_high_bitdepth);
  wbits += bitio::write_bool(&writer, m_twelve_bit);
  wbits += bitio::write_bool(&writer, m_monochrome);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_chroma_subsampling_x, 1);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_chroma_subsampling_y, 1);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_chroma_sample_position, 2);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_reserved, 3);
  wbits += bitio::write_bool(&writer, m_initial_presentation_delay_present);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_initial_presentation_delay_minus_one, 4);
  return wbits + bitio::write_vector_uint<std::uint8_t>(&writer, m_config_OBUs);
}

std::uint64_t AV1CodecConfiguration::getDataSize() const {
  return 4 + std::size(m_config_OBUs);
}

std::uint64_t AV1CodecConfiguration::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = bitio::read_uint<std::uint8_t>(&reader, &m_marker, 1);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_version, 7);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_seq_profile, 3);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_seq_level_idx_0, 5);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_seq_tier_0, 1);
  rbits += bitio::read_bool(&reader, &m_high_bitdepth);
  rbits += bitio::read_bool(&reader, &m_twelve_bit);
  rbits += bitio::read_bool(&reader, &m_monochrome);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_chroma_subsampling_x, 1);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_chroma_subsampling_y, 1);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_chroma_sample_position, 2);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_reserved, 3);
  rbits += bitio::read_bool(&reader, &m_initial_presentation_delay_present);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_initial_presentation_delay_minus_one, 4);
  const auto offset_to_end = shiguredo::mp4::stream::get_istream_offset_to_end(is);
  return rbits +
         bitio::read_vector_uint<std::uint8_t>(&reader, static_cast<std::size_t>(offset_to_end), &m_config_OBUs);
}

AV1CodecConfiguration::AV1CodecConfiguration(const AV1CodecConfigurationParameters& params)
    : m_marker(params.marker),
      m_version(params.version),
      m_seq_profile(params.seq_profile),
      m_seq_level_idx_0(params.seq_level_idx_0),
      m_seq_tier_0(params.seq_tier_0),
      m_high_bitdepth(params.high_bitdepth),
      m_twelve_bit(params.twelve_bit),
      m_monochrome(params.monochrome),
      m_chroma_subsampling_x(params.chroma_subsampling_x),
      m_chroma_subsampling_y(params.chroma_subsampling_y),
      m_chroma_sample_position(params.chroma_sample_position),
      m_initial_presentation_delay_present(params.initial_presentation_delay_present),
      m_initial_presentation_delay_minus_one(params.initial_presentation_delay_minus_one),
      m_config_OBUs(params.config_OBUs) {
  m_type = box_type_av1c();
}

}  // namespace shiguredo::mp4::box
