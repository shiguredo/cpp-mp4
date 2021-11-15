#include "shiguredo/mp4/box/avc.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

bool is_avc_high_profile(const std::uint8_t profile) {
  switch (profile) {
    case AVCScalableHighProfile:
      [[fallthrough]];
    case AVCHighProfile:
      [[fallthrough]];
    case AVCHigh10Profile:
      [[fallthrough]];
    case AVCHighMultiviewProfile:
      [[fallthrough]];
    case AVCHigh422Profile:
      [[fallthrough]];
    case AVCStereoHighProfile:
      [[fallthrough]];
    case AVCHighMultiviewDepthProfile:
      [[fallthrough]];
    case AVCHigh444Profile:
      [[fallthrough]];
    case AVCHigh444PredictiveProfile:
      return true;
    default:
      return false;
  }
}

AVCParameterSet::AVCParameterSet(const AVCParameterSetParameters& params) : m_nal_unit(params.nal_unit) {}

std::string AVCParameterSet::toString() const {
  return fmt::format("{{Length={} NALUnit=[{:#x}]}}", std::size(m_nal_unit), fmt::join(m_nal_unit, ", "));
}

std::uint64_t AVCParameterSet::writeData(bitio::Writer* writer) const {
  auto wbits = bitio::write_uint<std::uint16_t>(writer, static_cast<std::uint16_t>(std::size(m_nal_unit)));
  wbits += bitio::write_vector_uint<std::uint8_t>(writer, m_nal_unit);
  return wbits;
}

std::uint64_t AVCParameterSet::readData(bitio::Reader* reader) {
  std::uint16_t size;
  auto rbits = bitio::read_uint<std::uint16_t>(reader, &size);
  rbits += bitio::read_vector_uint<std::uint8_t>(reader, size, &m_nal_unit);
  return rbits;
}

std::uint64_t AVCParameterSet::getSize() const {
  return 2 + std::size(m_nal_unit);
}

BoxType box_type_avcc() {
  return BoxType("avcC");
}

AVCDecoderConfiguration::AVCDecoderConfiguration() {
  m_type = box_type_avcc();
}

AVCDecoderConfiguration::AVCDecoderConfiguration(const AVCDecoderConfigurationParameters& params)
    : m_configuration_version(params.configuration_version),
      m_profile(params.profile),
      m_profile_compatibility(params.profile_compatibility),
      m_level(params.level),
      m_length_size_minus_one(params.length_size_minus_one),
      m_sequence_parameter_sets(params.sequence_parameter_sets),
      m_picture_parameter_sets(params.picture_parameter_sets),
      m_high_profile_fields_enabled(params.high_profile_fields_enabled),
      m_chroma_format(params.chroma_format),
      m_bit_depth_luma_minus8(params.bit_depth_luma_minus8),
      m_bit_depth_chroma_minus8(params.bit_depth_chroma_minus8),
      m_sequence_parameter_sets_ext(params.sequence_parameter_sets_ext) {
  m_type = box_type_avcc();
}

std::string AVCDecoderConfiguration::toStringOnlyData() const {
  std::vector<std::string> sequence_parameter_sets;
  std::transform(std::begin(m_sequence_parameter_sets), std::end(m_sequence_parameter_sets),
                 std::back_inserter(sequence_parameter_sets), [](const auto& set) { return set.toString(); });

  std::vector<std::string> picture_parameter_sets;
  std::transform(std::begin(m_picture_parameter_sets), std::end(m_picture_parameter_sets),
                 std::back_inserter(picture_parameter_sets), [](const auto& set) { return set.toString(); });

  std::string s = fmt::format(
      "ConfigurationVersion={:#x} Profile={:#x} ProfileCompatibility={:#x} "
      "Level={:#x} LengthSizeMinusOne={:#x} NumOfSequenceParameterSets={:#x} "
      "SequenceParameterSets=[{}] NumOfPictureParameterSets={:#x} "
      "PictureParameterSets=[{}]",
      m_configuration_version, m_profile, m_profile_compatibility, m_level, m_length_size_minus_one,
      std::size(sequence_parameter_sets), fmt::join(sequence_parameter_sets, ", "), std::size(picture_parameter_sets),
      fmt::join(picture_parameter_sets, ", "));

  if (is_avc_high_profile(m_profile) && m_high_profile_fields_enabled) {
    std::vector<std::string> sequence_parameter_sets_ext;
    std::transform(std::begin(m_sequence_parameter_sets_ext), std::end(m_sequence_parameter_sets_ext),
                   std::back_inserter(sequence_parameter_sets_ext), [](const auto& set) { return set.toString(); });
    s += fmt::format(
        " ChromaFormat={:#x} BitDepthLumaMinus8={:#x} "
        "BitDepthChromaMinus8={:#x} NumOfSequenceParameterSetExt={:#x} "
        "SequenceParameterSetsExt=[{}]",
        m_chroma_format, m_bit_depth_luma_minus8, m_bit_depth_chroma_minus8, std::size(sequence_parameter_sets_ext),
        fmt::join(sequence_parameter_sets_ext, ", "));
  }

  return s;
}

std::uint64_t AVCDecoderConfiguration::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  auto wbits = bitio::write_uint<std::uint8_t>(&writer, m_configuration_version);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_profile);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_profile_compatibility);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_level);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_reserved, 6);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_length_size_minus_one, 2);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_reserved2, 3);
  std::uint8_t size_of_seq = static_cast<std::uint8_t>(std::size(m_sequence_parameter_sets));
  wbits += bitio::write_uint<std::uint8_t>(&writer, size_of_seq, 5);
  wbits += std::accumulate(std::begin(m_sequence_parameter_sets), std::end(m_sequence_parameter_sets), 0UL,
                           [&writer](const auto a, const auto& set) { return a + set.writeData(&writer); });
  std::uint8_t size_of_pic = static_cast<std::uint8_t>(std::size(m_picture_parameter_sets));
  wbits += bitio::write_uint<std::uint8_t>(&writer, size_of_pic);
  wbits += std::accumulate(std::begin(m_picture_parameter_sets), std::end(m_picture_parameter_sets), 0UL,
                           [&writer](const auto a, const auto& set) { return a + set.writeData(&writer); });
  if (!m_high_profile_fields_enabled) {
    return wbits;
  }
  if (!is_avc_high_profile(m_profile)) {
    throw std::runtime_error(
        "AVCDecoderConfiguration::writeData(): m_profile and m_high_profile_fields_enabled are not inconsistent");
  }
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_reserved3, 6);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_chroma_format, 2);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_reserved4, 5);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_bit_depth_luma_minus8, 3);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_reserved5, 5);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_bit_depth_chroma_minus8, 3);
  std::uint8_t size_of_ext = static_cast<std::uint8_t>(std::size(m_sequence_parameter_sets_ext));
  wbits += bitio::write_uint<std::uint8_t>(&writer, size_of_ext);
  wbits += std::accumulate(std::begin(m_sequence_parameter_sets_ext), std::end(m_sequence_parameter_sets_ext), 0UL,
                           [&writer](const auto a, const auto& set) { return a + set.writeData(&writer); });
  return wbits;
}

std::uint64_t AVCDecoderConfiguration::readData(std::istream& is) {
  bitio::Reader reader(is);
  auto rbits = bitio::read_uint<std::uint8_t>(&reader, &m_configuration_version);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_profile);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_profile_compatibility);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_level);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_reserved, 6);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_length_size_minus_one, 2);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_reserved2, 3);
  std::uint8_t size_of_seq;
  rbits += bitio::read_uint<std::uint8_t>(&reader, &size_of_seq, 5);
  m_sequence_parameter_sets.resize(size_of_seq);
  for (std::size_t i = 0; i < size_of_seq; ++i) {
    rbits += m_sequence_parameter_sets[i].readData(&reader);
  }
  std::uint8_t size_of_pic;
  rbits += bitio::read_uint<std::uint8_t>(&reader, &size_of_pic);
  m_picture_parameter_sets.resize(size_of_pic);
  for (std::size_t i = 0; i < size_of_pic; ++i) {
    rbits += m_picture_parameter_sets[i].readData(&reader);
  }

  m_high_profile_fields_enabled = false;
  if (!is_avc_high_profile(m_profile)) {
    return rbits;
  }
  const auto offset_to_end = shiguredo::mp4::stream::get_istream_offset_to_end(is);
  if (offset_to_end < AVCHighProfileMinimumSize) {
    spdlog::debug("AVCDecoderConfiguration::readData: offset_to_end: {}", offset_to_end);
    return rbits + static_cast<std::uint64_t>(offset_to_end) * 8;
  }
  m_high_profile_fields_enabled = true;
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_reserved3, 6);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_chroma_format, 2);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_reserved4, 5);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_bit_depth_luma_minus8, 3);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_reserved5, 5);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_bit_depth_chroma_minus8, 3);
  std::uint8_t size_of_ext;
  rbits += bitio::read_uint<std::uint8_t>(&reader, &size_of_ext);
  m_sequence_parameter_sets_ext.resize(size_of_ext);
  for (std::size_t i = 0; i < size_of_ext; ++i) {
    rbits += m_sequence_parameter_sets_ext[i].readData(&reader);
  }

  return rbits;
}

std::uint64_t AVCDecoderConfiguration::getDataSize() const {
  std::uint64_t size = 6;
  size += std::accumulate(std::begin(m_sequence_parameter_sets), std::end(m_sequence_parameter_sets), 0UL,
                          [](const auto s, const auto& a) { return s + a.getSize(); });
  size += 1;
  size += std::accumulate(std::begin(m_picture_parameter_sets), std::end(m_picture_parameter_sets), 0UL,
                          [](const auto s, const auto& a) { return s + a.getSize(); });
  if (!m_high_profile_fields_enabled) {
    return size;
  }
  size += 4;
  size += std::accumulate(std::begin(m_sequence_parameter_sets_ext), std::end(m_sequence_parameter_sets_ext), 0UL,
                          [](const auto s, const auto& a) { return s + a.getSize(); });
  return size;
}

}  // namespace shiguredo::mp4::box
