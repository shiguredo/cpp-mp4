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

struct StdpSampleParameters {
  const std::uint8_t is_leading;
  const std::uint8_t sample_depends_on;
  const std::uint8_t sample_is_depended_on;
  const std::uint8_t sample_has_redundancy;
};

class SdtpSample {
 public:
  SdtpSample() = default;
  explicit SdtpSample(const StdpSampleParameters&);

  std::string toString() const;

  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t readData(bitio::Reader*);

 private:
  std::uint8_t m_is_leading;
  std::uint8_t m_sample_depends_on;
  std::uint8_t m_sample_is_depended_on;
  std::uint8_t m_sample_has_redundancy;
};

struct SdtpParameters {
  const std::vector<SdtpSample> samples;
};

BoxType box_type_sdtp();

class Sdtp : public FullBox {
 public:
  Sdtp();
  explicit Sdtp(const SdtpParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::vector<SdtpSample> m_samples;
};

}  // namespace shiguredo::mp4::box
