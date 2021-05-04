#include "shiguredo/mp4/box/sample_entry.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <array>
#include <cstdint>
#include <istream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

std::uint64_t SampleEntry::writeReservedAndDataReferenceIndex(bitio::Writer* writer) const {
  std::uint64_t wbits = 0;
  for (std::size_t i = 0; i < std::size(m_reserved); ++i) {
    wbits += bitio::write_uint<std::uint8_t>(writer, m_reserved[i]);
  }
  return wbits + bitio::write_uint<std::uint16_t>(writer, m_data_reference_index);
}

std::uint64_t SampleEntry::readReservedAndDataReferenceIndex(bitio::Reader* reader) {
  std::uint64_t rbits = 0;
  for (std::size_t i = 0; i < std::size(m_reserved); ++i) {
    rbits += bitio::read_uint<std::uint8_t>(reader, &(m_reserved[i]));
  }

  return rbits + bitio::read_uint<std::uint16_t>(reader, &m_data_reference_index);
}

VisualSampleEntry::VisualSampleEntry(const BoxType& t_type) {
  m_type = t_type;
}

VisualSampleEntry::VisualSampleEntry(const VisualSampleEntryParameters& params)
    : m_pre_defined(params.pre_defined),
      m_pre_defined2(params.pre_defined2),
      m_width(params.width),
      m_height(params.height),
      m_horizresolution(params.horizresolution),
      m_vertresolution(params.vertresolution),
      m_frame_count(params.frame_count),
      m_compressorname(params.compressorname),
      m_depth(params.depth),
      m_pre_defined3(params.pre_defined3) {
  m_type = params.type;
  m_data_reference_index = params.data_reference_index;
}

std::string VisualSampleEntry::toStringOnlyData() const {
  std::size_t size = static_cast<std::size_t>(m_compressorname[0]);
  if (size > 31) {
    size = 31;
  }
  std::string compressorname(std::begin(m_compressorname) + 1, std::begin(m_compressorname) + 1 + size);

  return fmt::format(
      R"(DataReferenceIndex={} PreDefined={} PreDefined2=[{}] Width={} Height={} Horizresolution={} Vertresolution={} FrameCount={} Compressorname="{}" Depth={} PreDefined3={})",
      m_data_reference_index, m_pre_defined, fmt::join(m_pre_defined2, ", "), m_width, m_height, m_horizresolution,
      m_vertresolution, m_frame_count, compressorname, m_depth, m_pre_defined3);
}

std::uint64_t VisualSampleEntry::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeReservedAndDataReferenceIndex(&writer);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_pre_defined);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_reserved2);
  for (const auto i : m_pre_defined2) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, i);
  }
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_width);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_height);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_horizresolution);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_vertresolution);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_reserved3);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_frame_count);
  for (const auto c : m_compressorname) {
    wbits += bitio::write_uint<std::uint8_t>(&writer, c);
  }
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_depth);
  return wbits + bitio::write_int<std::int16_t>(&writer, m_pre_defined3);
}

std::uint64_t VisualSampleEntry::getDataSize() const {
  return 8 + 4 + 3 * 4 + 18 + 32 + 4;
}

std::uint64_t VisualSampleEntry::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readReservedAndDataReferenceIndex(&reader);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_pre_defined);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_reserved2);
  for (std::size_t i = 0; i < std::size(m_pre_defined2); ++i) {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &(m_pre_defined2[i]));
  }
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_width);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_height);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_horizresolution);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_vertresolution);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_reserved3);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_frame_count);
  for (std::size_t i = 0; i < std::size(m_compressorname); ++i) {
    rbits += bitio::read_uint<std::uint8_t>(&reader, &(m_compressorname[i]));
  }
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_depth);
  return rbits + bitio::read_int<std::int16_t>(&reader, &m_pre_defined3);
}

AudioSampleEntry::AudioSampleEntry(const BoxType& t_type) {
  m_type = t_type;
}

AudioSampleEntry::AudioSampleEntry(const AudioSampleEntryParameters& params)
    : m_entry_version(params.entry_version),
      m_channel_count(params.channel_count),
      m_sample_size(params.sample_size),
      m_pre_defined(params.pre_defined),
      m_sample_rate(params.sample_rate),
      m_is_quick_time_compatible(params.is_quick_time_compatible),
      m_quick_time_data(params.quick_time_data),
      m_under_wave(params.under_wave) {
  m_type = params.type;
  m_data_reference_index = params.data_reference_index;
  checkLengthOfQuickTimeData(std::size(m_quick_time_data));
}

std::string AudioSampleEntry::toStringOnlyData() const {
  if (m_under_wave) {
    return fmt::format("QuickTimeData=[{:#x}]", fmt::join(m_quick_time_data, ", "));
  }
  std::string s = fmt::format(
      "DataReferenceIndex={} EntryVersion={} ChannelCount={} SampleSize={} "
      "PreDefined={} SampleRate={}",
      m_data_reference_index, m_entry_version, m_channel_count, m_sample_size, m_pre_defined, m_sample_rate);
  if (!m_is_quick_time_compatible) {
    return s;
  }
  return s + fmt::format(" QuickTimeData=[{:#x}]", fmt::join(m_quick_time_data, ", "));
}

std::uint64_t AudioSampleEntry::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeReservedAndDataReferenceIndex(&writer);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_entry_version);
  for (const auto r : m_reserved2) {
    wbits += bitio::write_uint<std::uint16_t>(&writer, r);
  }
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_channel_count);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_sample_size);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_pre_defined);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_reserved3);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_sample_rate);
  if (!m_is_quick_time_compatible) {
    return wbits;
  }
  return wbits + bitio::write_vector_uint<std::uint8_t>(&writer, m_quick_time_data);
}

std::uint64_t AudioSampleEntry::getDataSize() const {
  std::uint64_t size = 8 + 8 + 8 + 4;
  if (!m_is_quick_time_compatible) {
    return size;
  }
  return size + std::size(m_quick_time_data);
}

std::uint64_t AudioSampleEntry::readData(std::istream& is) {
  bitio::Reader reader(is);
  if (m_under_wave) {
    const std::uint64_t offset_to_end =
        static_cast<std::uint64_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
    return bitio::read_vector_uint<std::uint8_t>(&reader, offset_to_end, &m_quick_time_data);
  }
  std::uint64_t rbits = readReservedAndDataReferenceIndex(&reader);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_entry_version);
  for (std::size_t i = 0; i < std::size(m_reserved2); ++i) {
    rbits += bitio::read_uint<std::uint16_t>(&reader, &(m_reserved2[i]));
  }
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_channel_count);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_sample_size);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_pre_defined);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_reserved3);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_sample_rate);
  if (m_type == BoxType("Opus") || m_entry_version == 0) {
    m_is_quick_time_compatible = false;
    return rbits;
  }
  const std::uint64_t offset_to_end = static_cast<std::uint64_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
  if (offset_to_end == 0) {
    m_is_quick_time_compatible = false;
    return rbits;
  }
  m_is_quick_time_compatible = true;
  spdlog::debug("AudioSampleEntry::readData(): offset_to_end: {}", offset_to_end);
  return rbits +
         bitio::read_vector_uint<std::uint8_t>(&reader, getLengthOfQuickTimeData(offset_to_end), &m_quick_time_data);
}

std::uint64_t AudioSampleEntry::getLengthOfQuickTimeData(std::uint64_t length) const {
  if (!m_is_quick_time_compatible && length > 0) {
    throw std::runtime_error(
        fmt::format("AudioSampleEntry::getLengthOfQuickTimeData(): invalid quick_time_data length: "
                    "m_is_quick_time_compatible=false length={}",
                    length));
  }
  if (m_under_wave) {
    return length;
  }
  if (m_entry_version == 1) {
    return 16;
  }
  if (m_entry_version == 2) {
    return 36;
  }
  return length;
}

void AudioSampleEntry::checkLengthOfQuickTimeData(std::uint64_t length) const {
  if (!m_is_quick_time_compatible && length > 0) {
    throw std::runtime_error(
        fmt::format("AudioSampleEntry::checkLengthOfQuickTimeData(): invalid quick_time_data length: "
                    "m_is_quick_time_compatible=false length={}",
                    length));
  }
  if (m_under_wave) {
    return;
  }
  if (m_entry_version == 1 && length != 16) {
    throw std::runtime_error(fmt::format(
        "AudioSampleEntry::checkLengthOfQuickTimeData(): invalid quick_time_data length: m_entry_version=1 length={}",
        length));
  }
  if (m_entry_version == 2 && length != 36) {
    throw std::runtime_error(fmt::format(
        "AudioSampleEntry::checkLengthOfQuickTimeData(): invalid quick_time_data length: m_entry_version=2 length={}",
        length));
  }
}

void AudioSampleEntry::setUnderWave(bool under_wave) {
  m_under_wave = under_wave;
}

}  // namespace shiguredo::mp4::box
