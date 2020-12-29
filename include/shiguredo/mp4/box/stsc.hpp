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

struct StscEntryParameters {
  const std::uint32_t first_chunk;
  const std::uint32_t samples_per_chunk;
  const std::uint32_t sample_description_index = 1;
};

class StscEntry {
 public:
  StscEntry() = default;
  explicit StscEntry(const StscEntryParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t readData(bitio::Reader*);
  auto operator<=>(const StscEntry&) const = default;

 private:
  std::uint32_t m_first_chunk;
  std::uint32_t m_sample_per_chunk;
  std::uint32_t m_sample_description_index;
};

std::ostream& operator<<(std::ostream&, const StscEntry&);

struct StscParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::vector<StscEntry> entries;
};

BoxType box_type_stsc();

class Stsc : public FullBox {
 public:
  Stsc();
  explicit Stsc(const StscParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::vector<StscEntry> m_entries;
};

}  // namespace shiguredo::mp4::box
