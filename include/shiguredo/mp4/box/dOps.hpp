#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::bitio {

class Reader;
class Writer;

}  // namespace shiguredo::mp4::bitio

namespace shiguredo::mp4::box {

struct DOpsChannelMappingTableParameters {
  const std::uint8_t stream_count = 0;
  const std::uint8_t coupled_count = 0;
  const std::vector<std::uint8_t> channel_mapping = {};
};

class DOpsChannelMappingTable {
 public:
  DOpsChannelMappingTable() = default;
  explicit DOpsChannelMappingTable(const DOpsChannelMappingTableParameters&);
  ~DOpsChannelMappingTable() = default;

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t getSize() const;
  std::uint64_t readData(bitio::Reader*, const std::uint8_t);

 private:
  std::uint8_t m_stream_count = 0;
  std::uint8_t m_coupled_count = 0;
  std::vector<std::uint8_t> m_channel_mapping = {};
};

BoxType box_type_dOps();

struct DOpsParameters {
  const std::uint8_t version = 0;
  const std::uint8_t output_channel_count;
  const std::uint16_t pre_skip;
  const std::uint32_t input_sample_rate;
  const std::int16_t output_gain;
  const std::uint8_t channel_mapping_family;
  const DOpsChannelMappingTable channel_mapping_table = {};
};

class DOps : public Box {
 public:
  DOps();
  explicit DOps(const DOpsParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint8_t m_version;
  std::uint8_t m_output_channel_count;
  std::uint16_t m_pre_skip;
  std::uint32_t m_input_sample_rate;
  std::int16_t m_output_gain;
  std::uint8_t m_channel_mapping_family;
  DOpsChannelMappingTable m_channel_mapping_table;
};

}  // namespace shiguredo::mp4::box
