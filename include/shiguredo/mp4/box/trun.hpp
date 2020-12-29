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

struct TrunEntryParameters {
  const std::uint32_t sample_duration = 0;
  const std::uint32_t sample_size = 0;
  const std::uint32_t sample_flags = 0;
  const std::int64_t sample_composition_time_offset = 0;
};

class TrunEntry {
 public:
  TrunEntry() = default;
  explicit TrunEntry(const TrunEntryParameters&);

  std::string toString(const std::uint32_t) const;
  std::uint64_t writeData(bitio::Writer*, const std::uint32_t) const;
  std::uint64_t getSize(const std::uint32_t) const;
  std::uint64_t readData(bitio::Reader*, const std::uint32_t);

 private:
  std::uint32_t m_sample_duration;
  std::uint32_t m_sample_size;
  std::uint32_t m_sample_flags;
  std::int64_t m_sample_composition_time_offset;
};

struct TrunParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::int32_t data_offset = 0;
  const std::uint32_t first_sample_flags = 0;
  const std::vector<TrunEntry> entries = {};
};

BoxType box_type_trun();

class Trun : public FullBox {
 public:
  Trun();
  explicit Trun(const TrunParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::int32_t m_data_offset;
  std::uint32_t m_first_sample_flags;
  std::vector<TrunEntry> m_entries;
};

}  // namespace shiguredo::mp4::box
