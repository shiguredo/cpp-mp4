#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/grouping_type.hpp"

namespace shiguredo::mp4::bitio {

class Reader;
class Writer;

}  // namespace shiguredo::mp4::bitio

namespace shiguredo::mp4::box {

struct SbgpEntryParameters {
  const std::uint32_t sample_count;
  const std::uint32_t group_description_index;
};

class SbgpEntry {
 public:
  SbgpEntry() = default;
  explicit SbgpEntry(const SbgpEntryParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t readData(bitio::Reader*);

 private:
  std::uint32_t m_sample_count;
  std::uint32_t m_group_description_index;
};

struct SbgpParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const GroupingType grouping_type = GroupingTypeRoll;
  const std::uint32_t grouping_type_parameter = 0;
  const std::vector<SbgpEntry> entries;
};

BoxType box_type_sbgp();

class Sbgp : public FullBox {
 public:
  Sbgp();
  explicit Sbgp(const SbgpParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  GroupingType m_grouping_type;
  std::uint32_t m_grouping_type_parameter;
  std::vector<SbgpEntry> m_entries;
};

}  // namespace shiguredo::mp4::box
