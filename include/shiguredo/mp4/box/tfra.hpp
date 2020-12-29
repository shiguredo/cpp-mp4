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

struct TfraEntryParameters {
  const std::uint64_t time;
  const std::uint64_t moof_offset;
  const std::uint32_t traf_number;
  const std::uint32_t trun_number;
  const std::uint32_t sample_number;
};

class TfraEntry {
 public:
  TfraEntry() = default;
  explicit TfraEntry(const TfraEntryParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*,
                          const std::uint8_t,
                          const std::uint8_t,
                          const std::uint8_t,
                          const std::uint8_t) const;
  std::uint64_t getSize(const std::uint8_t, const std::uint8_t, const std::uint8_t, const std::uint8_t) const;
  std::uint64_t readData(bitio::Reader*,
                         const std::uint8_t,
                         const std::uint8_t,
                         const std::uint8_t,
                         const std::uint8_t);

 private:
  std::uint64_t m_time;
  std::uint64_t m_moof_offset;
  std::uint32_t m_traf_number;
  std::uint32_t m_trun_number;
  std::uint32_t m_sample_number;
};

struct TfraParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint32_t track_id;
  const std::uint8_t length_size_of_traf_num;
  const std::uint8_t length_size_of_trun_num;
  const std::uint8_t length_size_of_sample_num;
  const std::vector<TfraEntry> entries;
};

BoxType box_type_tfra();

class Tfra : public FullBox {
 public:
  Tfra();
  explicit Tfra(const TfraParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint32_t m_track_id;
  std::uint32_t m_reserved = 0;
  std::uint8_t m_length_size_of_traf_num;
  std::uint8_t m_length_size_of_trun_num;
  std::uint8_t m_length_size_of_sample_num;
  std::vector<TfraEntry> m_entries;
};

}  // namespace shiguredo::mp4::box
