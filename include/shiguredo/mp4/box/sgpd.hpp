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

struct RollDistanceParameters {
  std::uint32_t description_length = 0;
  std::int16_t roll_distance;
};

class RollDistance {
 public:
  RollDistance() = default;
  explicit RollDistance(const RollDistanceParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t getSize() const;
  std::uint64_t readData(bitio::Reader*, const bool);

 private:
  std::uint32_t m_description_length;
  std::int16_t m_roll_distance;
  bool m_with_length = false;
};

struct AlternativeStartupEntryOptParameters {
  const std::uint16_t num_output_samples;
  const std::uint16_t num_total_samples;
};

class AlternativeStartupEntryOpt {
 public:
  AlternativeStartupEntryOpt() = default;
  explicit AlternativeStartupEntryOpt(const AlternativeStartupEntryOptParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t readData(bitio::Reader*);

 private:
  std::uint16_t m_num_output_samples;
  std::uint16_t m_num_total_samples;
};

struct AlternativeStartupEntryParameters {
  const std::uint32_t description_length = 0;
  const std::uint16_t roll_count;
  const std::uint16_t first_output_sample;
  const std::vector<std::uint32_t> sample_offsets;
  const std::vector<AlternativeStartupEntryOpt> opts = {};
};

class AlternativeStartupEntry {
 public:
  AlternativeStartupEntry() = default;
  explicit AlternativeStartupEntry(const AlternativeStartupEntryParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t getSize() const;
  std::uint64_t readData(bitio::Reader*, const std::uint32_t, const bool);

 private:
  std::uint32_t m_description_length;
  bool m_with_length = false;
  std::uint16_t m_roll_count;
  std::uint16_t m_first_output_sample;
  std::vector<std::uint32_t> m_sample_offsets;
  std::vector<AlternativeStartupEntryOpt> m_opts;
};

struct VisualRandomAccessEntryParameters {
  const std::uint32_t description_length = 0;
  const bool num_leading_samples_known;
  const std::uint8_t num_leading_samples;
};

class VisualRandomAccessEntry {
 public:
  VisualRandomAccessEntry() = default;
  explicit VisualRandomAccessEntry(const VisualRandomAccessEntryParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t getSize() const;
  std::uint64_t readData(bitio::Reader*, const bool);

 private:
  std::uint32_t m_description_length;
  bool m_num_leading_samples_known;
  std::uint8_t m_num_leading_samples;
  bool m_with_length = false;
};

struct TemporalLevelEntryParameters {
  std::uint32_t description_length = 0;
  bool level_independently_decodable;
};

class TemporalLevelEntry {
 public:
  TemporalLevelEntry() = default;
  explicit TemporalLevelEntry(const TemporalLevelEntryParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t getSize() const;
  std::uint64_t readData(bitio::Reader*, const bool);

 private:
  std::uint32_t m_description_length;
  bool m_level_independently_decodable;
  std::uint8_t m_reserved = 0;
  bool m_with_length = false;
};

struct SgpdParameters {
  const std::uint8_t version = 1;
  const std::uint32_t flags = 0x000000;
  const GroupingType grouping_type = GroupingTypeRoll;
  const std::uint32_t default_length = 2;
  const std::uint32_t default_sample_description_index = 0;
  const std::vector<RollDistance> roll_distances = {};
  const std::vector<AlternativeStartupEntry> alternative_startup_entries = {};
  const std::vector<VisualRandomAccessEntry> visual_random_access_entries = {};
  const std::vector<TemporalLevelEntry> temporal_level_entries = {};
};

BoxType box_type_sgpd();

class Sgpd : public FullBox {
 public:
  Sgpd();
  explicit Sgpd(const SgpdParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  GroupingType m_grouping_type;
  std::uint32_t m_default_length;
  std::uint32_t m_default_sample_description_index;
  std::vector<RollDistance> m_roll_distances;
  std::vector<AlternativeStartupEntry> m_alternative_startup_entries;
  std::vector<VisualRandomAccessEntry> m_visual_random_access_entries;
  std::vector<TemporalLevelEntry> m_temporal_level_entries;

  bool noDefaultLength() const;
};

}  // namespace shiguredo::mp4::box
