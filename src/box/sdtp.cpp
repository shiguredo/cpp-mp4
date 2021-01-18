#include "shiguredo/mp4/box/sdtp.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_sdtp() {
  return BoxType("sdtp");
}

Sdtp::Sdtp() {
  m_type = box_type_sdtp();
}

Sdtp::Sdtp(const SdtpParameters& params) : m_samples(params.samples) {
  m_type = box_type_sdtp();
}

std::string Sdtp::toStringOnlyData() const {
  std::vector<std::string> samples;
  std::transform(std::begin(m_samples), std::end(m_samples), std::back_inserter(samples),
                 [](const auto& s) { return s.toString(); });

  return fmt::format("{} Samples=[{}]", getVersionAndFlagsString(), fmt::join(samples, ", "));
}

std::uint64_t Sdtp::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += std::accumulate(std::begin(m_samples), std::end(m_samples), 0UL,
                           [&writer](const auto a, const auto& s) { return a + s.writeData(&writer); });
  return wbits;
}

std::uint64_t Sdtp::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  const std::size_t offset_to_end = static_cast<std::size_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
  m_samples.resize(offset_to_end);
  for (std::size_t i = 0; i < offset_to_end; ++i) {
    rbits += m_samples[i].readData(&reader);
  }
  return rbits;
}

std::uint64_t Sdtp::getDataSize() const {
  return 4 + std::size(m_samples);
}

SdtpSample::SdtpSample(const StdpSampleParameters& params)
    : m_is_leading(params.is_leading),
      m_sample_depends_on(params.sample_depends_on),
      m_sample_is_depended_on(params.sample_is_depended_on),
      m_sample_has_redundancy(params.sample_has_redundancy) {}

std::string SdtpSample::toString() const {
  return fmt::format("{{IsLeading={:#x} SampleDependsOn={:#x} SampleIsDependedOn={:#x} SampleHasRedundancy={:#x}}}",
                     m_is_leading, m_sample_depends_on, m_sample_is_depended_on, m_sample_has_redundancy);
}

std::uint64_t SdtpSample::writeData(bitio::Writer* writer) const {
  std::uint64_t wbits = bitio::write_uint<std::uint8_t>(writer, m_is_leading, 2);
  wbits += bitio::write_uint<std::uint8_t>(writer, m_sample_depends_on, 2);
  wbits += bitio::write_uint<std::uint8_t>(writer, m_sample_is_depended_on, 2);
  wbits += bitio::write_uint<std::uint8_t>(writer, m_sample_has_redundancy, 2);
  return wbits;
}

std::uint64_t SdtpSample::readData(bitio::Reader* reader) {
  std::uint64_t rbits = bitio::read_uint<std::uint8_t>(reader, &m_is_leading, 2);
  rbits += bitio::read_uint<std::uint8_t>(reader, &m_sample_depends_on, 2);
  rbits += bitio::read_uint<std::uint8_t>(reader, &m_sample_is_depended_on, 2);
  rbits += bitio::read_uint<std::uint8_t>(reader, &m_sample_has_redundancy, 2);
  return rbits;
}

}  // namespace shiguredo::mp4::box
