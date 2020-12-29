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

struct ElstEntryParameters {
  const std::uint64_t track_duration;
  const std::int64_t media_time = 0;
  const std::int32_t media_rate = 1 << 16;
};

class ElstEntry {
 public:
  ElstEntry() = default;
  explicit ElstEntry(const ElstEntryParameters&);

  std::string toString() const;
  std::uint64_t writeData(bitio::Writer*, const std::uint8_t version) const;
  std::uint64_t getSize(const std::uint8_t version) const;
  std::uint64_t readData(bitio::Reader*, const std::uint8_t version);

  double getMediaRate() const;

  std::uint64_t m_track_duration;
  std::int64_t m_media_time;
  std::int32_t m_media_rate;
};

BoxType box_type_elst();

struct ElstParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::vector<ElstEntry> entries;
};

class Elst : public FullBox {
 public:
  Elst();
  explicit Elst(const ElstParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::vector<ElstEntry> m_entries;
};

}  // namespace shiguredo::mp4::box
