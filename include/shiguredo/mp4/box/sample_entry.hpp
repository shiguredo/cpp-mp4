#pragma once

#include <array>
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

class SampleEntry : public AnyTypeBox {
 protected:
  std::uint64_t writeReservedAndDataReferenceIndex(bitio::Writer*) const;
  std::uint64_t readReservedAndDataReferenceIndex(bitio::Reader*);

  std::array<std::uint8_t, 6> m_reserved = {0, 0, 0, 0, 0, 0};
  std::uint16_t m_data_reference_index;
};

struct VisualSampleEntryParameters {
  const BoxType type;
  const std::uint16_t data_reference_index;
  const std::uint16_t pre_defined = 0;
  const std::array<std::uint32_t, 3> pre_defined2 = {0, 0, 0};
  const std::uint16_t width;
  const std::uint16_t height;
  const std::uint32_t horizresolution = 72 << 16;
  const std::uint32_t vertresolution = 72 << 16;
  const std::uint16_t frame_count = 1;
  const std::array<std::uint8_t, 32> compressorname = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };
  const std::uint16_t depth = 24;
  const std::int16_t pre_defined3 = -1;
};

class VisualSampleEntry : public SampleEntry {
 public:
  VisualSampleEntry() = default;
  explicit VisualSampleEntry(const BoxType&);
  explicit VisualSampleEntry(const VisualSampleEntryParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint16_t m_pre_defined;
  std::uint16_t m_reserved2 = 0;
  std::array<std::uint32_t, 3> m_pre_defined2;
  std::uint16_t m_width;
  std::uint16_t m_height;
  std::uint32_t m_horizresolution;
  std::uint32_t m_vertresolution;
  std::uint32_t m_reserved3 = 0;
  std::uint16_t m_frame_count;
  std::array<std::uint8_t, 32> m_compressorname;
  std::uint16_t m_depth;
  std::int16_t m_pre_defined3;
};

struct AudioSampleEntryParameters {
  const BoxType type;
  const std::uint16_t data_reference_index;
  const std::uint16_t entry_version;
  const std::uint16_t channel_count;
  const std::uint16_t sample_size;
  const std::uint16_t pre_defined = 0;
  const std::uint32_t sample_rate;
  const bool is_quick_time_compatible = false;
  const std::vector<std::uint8_t> quick_time_data = {};
  const bool under_wave = false;
};

class AudioSampleEntry : public SampleEntry {
 public:
  AudioSampleEntry() = default;
  explicit AudioSampleEntry(const BoxType&);
  explicit AudioSampleEntry(const AudioSampleEntryParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

  void setUnderWave(bool);

 private:
  std::uint16_t m_entry_version;
  std::array<std::uint16_t, 3> m_reserved2 = {0, 0, 0};
  std::uint16_t m_channel_count;
  std::uint16_t m_sample_size;
  std::uint16_t m_pre_defined;
  std::uint16_t m_reserved3 = 0;
  std::uint32_t m_sample_rate;
  bool m_is_quick_time_compatible;
  std::vector<std::uint8_t> m_quick_time_data;
  bool m_under_wave;

  void checkLengthOfQuickTimeData(std::uint64_t size) const;
  std::uint64_t getLengthOfQuickTimeData(std::uint64_t size) const;
};

}  // namespace shiguredo::mp4::box
