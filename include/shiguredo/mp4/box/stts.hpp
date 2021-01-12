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

struct SttsEntryParameters {
  const std::uint32_t sample_count;
  const std::uint32_t sample_duration;
};

class SttsEntry {
 public:
  SttsEntry() = default;
  explicit SttsEntry(const SttsEntryParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t readData(bitio::Reader*);
  auto operator<=>(const SttsEntry&) const = default;

 private:
  std::uint32_t m_sample_count;
  std::uint32_t m_sample_duration;
};

std::ostream& operator<<(std::ostream&, const SttsEntry&);

struct SttsParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::vector<SttsEntry> entries;
};

BoxType box_type_stts();

class Stts : public FullBox {
 public:
  Stts();
  explicit Stts(const SttsParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::vector<SttsEntry> m_entries;
};

}  // namespace shiguredo::mp4::box
