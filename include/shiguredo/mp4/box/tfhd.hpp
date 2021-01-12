#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

enum TfhdFlags : std::uint32_t {
  TfhdBaseDataOffsetPresent = 0x000001,
  TfhdSampleDescriptionIndexPresent = 0x000002,
  TfhdDefaultSampleDurationPresent = 0x000008,
  TfhdDefaultSampleSizePresent = 0x000010,
  TfhdDefaultSampleFlagsPresent = 0x000020,
  TfhdDurationIsEmpty = 0x010000,
  TfhdDefaultBaseIsMoof = 0x020000,
};

struct TfhdParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint32_t track_id;
  const std::uint64_t base_data_offset = 0;
  const std::uint32_t sample_description_index = 0;
  const std::uint32_t default_sample_duration = 0;
  const std::uint32_t default_sample_size = 0;
  const std::uint32_t default_sample_flags = 0;
};

BoxType box_type_tfhd();

class Tfhd : public FullBox {
 public:
  Tfhd();
  explicit Tfhd(const TfhdParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint32_t m_track_id;
  std::uint64_t m_base_data_offset;
  std::uint32_t m_sample_description_index;
  std::uint32_t m_default_sample_duration;
  std::uint32_t m_default_sample_size;
  std::uint32_t m_default_sample_flags;
};

}  // namespace shiguredo::mp4::box
