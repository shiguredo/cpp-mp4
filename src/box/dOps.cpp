#include "shiguredo/mp4/box/dOps.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cstdint>
#include <istream>
#include <iterator>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_dOps() {
  return BoxType("dOps");
}

DOps::DOps() {
  m_type = box_type_dOps();
}

std::string DOps::toStringOnlyData() const {
  std::string s = fmt::format(
      "Version={} OutputChannelCount={} PreSkip={} InputSampleRate={} OutputGain={} ChannelMappingFamily={}", m_version,
      m_output_channel_count, m_pre_skip, m_input_sample_rate, m_output_gain, m_channel_mapping_family);
  if (m_channel_mapping_family == 0) {
    return s;
  }
  return s + " " + m_channel_mapping_table.toString();
}

std::uint64_t DOps::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  auto wbits = bitio::write_uint<std::uint8_t>(&writer, m_version);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_output_channel_count);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_pre_skip);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_input_sample_rate);
  wbits += bitio::write_int<std::int16_t>(&writer, m_output_gain);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_channel_mapping_family);
  if (m_channel_mapping_family == 0) {
    return wbits;
  }
  return wbits + m_channel_mapping_table.writeData(&writer);
}

std::uint64_t DOps::getDataSize() const {
  if (m_channel_mapping_family == 0) {
    return 11;
  }
  return 11 + m_channel_mapping_table.getSize();
}

std::uint64_t DOps::readData(std::istream& is) {
  bitio::Reader reader(is);
  auto rbits = bitio::read_uint<std::uint8_t>(&reader, &m_version);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_output_channel_count);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_pre_skip);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_input_sample_rate);
  rbits += bitio::read_int<std::int16_t>(&reader, &m_output_gain);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_channel_mapping_family);
  if (m_channel_mapping_family == 0) {
    return rbits;
  }
  return rbits + m_channel_mapping_table.readData(&reader, m_output_channel_count);
}

DOpsChannelMappingTable::DOpsChannelMappingTable(const DOpsChannelMappingTableParameters& params)
    : m_stream_count(params.stream_count),
      m_coupled_count(params.coupled_count),
      m_channel_mapping(params.channel_mapping) {}

std::string DOpsChannelMappingTable::toString() const {
  return fmt::format("StreamCount={} CoupledCount={} ChannelMapping=[{}]", m_stream_count, m_coupled_count,
                     fmt::join(m_channel_mapping, ", "));
}

std::uint64_t DOpsChannelMappingTable::writeData(bitio::Writer* writer) const {
  auto wbits = bitio::write_uint<std::uint8_t>(writer, m_stream_count) +
               bitio::write_uint<std::uint8_t>(writer, m_coupled_count);
  return wbits + bitio::write_vector_uint<std::uint8_t>(writer, m_channel_mapping);
}

std::uint64_t DOpsChannelMappingTable::getSize() const {
  return 2 + std::size(m_channel_mapping);
}

std::uint64_t DOpsChannelMappingTable::readData(bitio::Reader* reader, const std::uint8_t count) {
  auto rbits = bitio::read_uint<std::uint8_t>(reader, &m_stream_count) +
               bitio::read_uint<std::uint8_t>(reader, &m_coupled_count);
  return rbits + bitio::read_vector_uint<std::uint8_t>(reader, count, &m_channel_mapping);
}

DOps::DOps(const DOpsParameters& params)
    : m_version(params.version),
      m_output_channel_count(params.output_channel_count),
      m_pre_skip(params.pre_skip),
      m_input_sample_rate(params.input_sample_rate),
      m_output_gain(params.output_gain),
      m_channel_mapping_family(params.channel_mapping_family),
      m_channel_mapping_table(params.channel_mapping_table) {
  m_type = box_type_dOps();
}

}  // namespace shiguredo::mp4::box
