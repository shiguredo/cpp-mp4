#include "shiguredo/mp4/box/tfhd.hpp"

#include <fmt/core.h>

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_tfhd() {
  return BoxType("tfhd");
}

Tfhd::Tfhd() {
  m_type = box_type_tfhd();
}

Tfhd::Tfhd(const TfhdParameters& params)
    : m_track_id(params.track_id),
      m_base_data_offset(params.base_data_offset),
      m_sample_description_index(params.sample_description_index),
      m_default_sample_duration(params.default_sample_duration),
      m_default_sample_size(params.default_sample_size),
      m_default_sample_flags(params.default_sample_flags) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_tfhd();
}

std::string Tfhd::toStringOnlyData() const {
  std::string s = fmt::format("{} TrackID={}", getVersionAndFlagsString(), m_track_id);
  std::uint32_t flags = getFlags();
  if (flags & TfhdBaseDataOffsetPresent) {
    s += fmt::format(" BaseDataOffset={}", m_base_data_offset);
  }
  if (flags & TfhdSampleDescriptionIndexPresent) {
    s += fmt::format(" SampleDescriptionIndex={}", m_sample_description_index);
  }
  if (flags & TfhdDefaultSampleDurationPresent) {
    s += fmt::format(" DefaultSampleDuration={}", m_default_sample_duration);
  }
  if (flags & TfhdDefaultSampleSizePresent) {
    s += fmt::format(" DefaultSampleSize={}", m_default_sample_size);
  }
  if (flags & TfhdDefaultSampleFlagsPresent) {
    s += fmt::format(" DefaultSampleFlags={:#x}", m_default_sample_flags);
  }
  return s;
}

std::uint64_t Tfhd::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_track_id);
  std::uint32_t flags = getFlags();
  if (flags & TfhdBaseDataOffsetPresent) {
    wbits += bitio::write_uint<std::uint64_t>(&writer, m_base_data_offset);
  }
  if (flags & TfhdSampleDescriptionIndexPresent) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_sample_description_index);
  }
  if (flags & TfhdDefaultSampleDurationPresent) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_sample_duration);
  }
  if (flags & TfhdDefaultSampleSizePresent) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_sample_size);
  }
  if (flags & TfhdDefaultSampleFlagsPresent) {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_default_sample_flags);
  }
  return wbits;
}

std::uint64_t Tfhd::getDataSize() const {
  std::uint64_t size = 8;
  std::uint32_t flags = getFlags();
  if (flags & TfhdBaseDataOffsetPresent) {
    size += 8;
  }
  if (flags & TfhdSampleDescriptionIndexPresent) {
    size += 4;
  }
  if (flags & TfhdDefaultSampleDurationPresent) {
    size += 4;
  }
  if (flags & TfhdDefaultSampleSizePresent) {
    size += 4;
  }
  if (flags & TfhdDefaultSampleFlagsPresent) {
    size += 4;
  }
  return size;
}

std::uint64_t Tfhd::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_track_id);
  std::uint32_t flags = getFlags();
  if (flags & TfhdBaseDataOffsetPresent) {
    rbits += bitio::read_uint<std::uint64_t>(&reader, &m_base_data_offset);
  }
  if (flags & TfhdSampleDescriptionIndexPresent) {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_sample_description_index);
  }
  if (flags & TfhdDefaultSampleDurationPresent) {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_sample_duration);
  }
  if (flags & TfhdDefaultSampleSizePresent) {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_sample_size);
  }
  if (flags & TfhdDefaultSampleFlagsPresent) {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_default_sample_flags);
  }
  return rbits;
}

}  // namespace shiguredo::mp4::box
