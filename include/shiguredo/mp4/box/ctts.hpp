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

BoxType box_type_ctts();

class CttsEntry {
 public:
  CttsEntry() = default;
  CttsEntry(const std::uint32_t, const std::int64_t);

  std::string toString() const;
  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t readData(bitio::Reader*);

 private:
  std::uint32_t m_sample_count;
  std::int64_t m_sample_offset;  // size = 32
};

struct CttsParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::vector<CttsEntry> entries;
};

class Ctts : public FullBox {
 public:
  Ctts();
  explicit Ctts(const CttsParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::vector<CttsEntry> m_entries;
};

}  // namespace shiguredo::mp4::box
