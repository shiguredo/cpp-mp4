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

struct SidxReferenceParameters {
  const bool reference_type;
  const std::uint32_t reference_size;
  const std::uint32_t subsegument_duration;
  const bool starts_with_sap;
  const std::uint8_t sap_type;
  const std::uint32_t sap_delta_time;
};

class SidxReference {
 public:
  SidxReference() = default;
  explicit SidxReference(const SidxReferenceParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t readData(bitio::Reader*);

 private:
  bool m_reference_type;
  std::uint32_t m_reference_size;
  std::uint32_t m_subsegument_duration;
  bool m_starts_with_sap;
  std::uint8_t m_sap_type;
  std::uint32_t m_sap_delta_time;
};

struct SidxParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint32_t reference_id;
  const std::uint32_t timescale;
  const std::uint64_t earliest_presentation_time;
  const std::uint64_t first_offset;
  const std::vector<SidxReference> references;
};

BoxType box_type_sidx();

class Sidx : public FullBox {
 public:
  Sidx();
  explicit Sidx(const SidxParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint32_t m_reference_id;
  std::uint32_t m_timescale;
  std::uint64_t m_earliest_presentation_time;
  std::uint64_t m_first_offset;
  std::uint16_t m_reserved = 0;
  std::vector<SidxReference> m_references;
};

}  // namespace shiguredo::mp4::box
