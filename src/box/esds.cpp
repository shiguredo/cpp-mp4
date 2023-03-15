#include "shiguredo/mp4/box/esds.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <istream>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

ESDescriptor::ESDescriptor(const ESDescriptorParameters& params)
    : m_sub_descriptors_size(params.sub_descriptors_size),
      m_ESID(params.ESID),
      m_stream_dependence_flag(params.stream_dependence_flag),
      m_url_flag(params.url_flag),
      m_ocr_stream_flag(params.ocr_stream_flag),
      m_stream_priority(params.stream_priority),
      m_depends_on_esid(params.depends_on_esid),
      m_url_string(params.url_string),
      m_OCRESID(params.OCRESID) {
  m_tag = ESDescrTag;
}

ESDescriptor::ESDescriptor() {
  m_tag = ESDescrTag;
}

std::uint64_t ESDescriptor::writeData(bitio::Writer* writer) const {
  auto wbits = bitio::write_int<std::int8_t>(writer, m_tag);
  wbits += bitio::write_uvarint<std::uint32_t>(writer, getDataSizeWithSubDescriptors());
  wbits += bitio::write_uint<std::uint16_t>(writer, m_ESID);
  wbits += write_bool(writer, m_stream_dependence_flag);
  wbits += write_bool(writer, m_url_flag);
  wbits += write_bool(writer, m_ocr_stream_flag);
  wbits += bitio::write_int<std::int8_t>(writer, m_stream_priority, 5);
  if (m_stream_dependence_flag) {
    wbits += bitio::write_uint<std::uint16_t>(writer, m_depends_on_esid);
  }
  if (m_url_flag) {
    auto size = std::size(m_url_string);
    if (size > std::numeric_limits<std::uint8_t>::max()) {
      throw std::runtime_error(fmt::format("ESDescriptor::writeData(): url_string is too long: {}", size));
    }
    wbits += bitio::write_uint<std::uint8_t>(writer, static_cast<std::uint8_t>(std::size(m_url_string)));
    wbits += bitio::write_vector_uint<std::uint8_t>(writer, m_url_string);
  }
  if (m_ocr_stream_flag) {
    wbits += bitio::write_uint<std::uint16_t>(writer, m_OCRESID);
  }

  return wbits;
}

std::uint64_t ESDescriptor::getSize() const {
  return 1 + bitio::uvarint_size<std::uint32_t>(getDataSize()) + getDataSize();
}

std::uint32_t ESDescriptor::getDataSizeWithSubDescriptors() const {
  if (m_sub_descriptors_size > 0) {
    return getDataSize() + m_sub_descriptors_size;
  }
  return getDataSize() + std::accumulate(std::begin(m_sub_descriptors), std::end(m_sub_descriptors), 0U,
                                         [](const std::uint32_t a, auto d) {
                                           return a + 5U  // tag + sizeof(varint)
                                                  + d->getDataSizeWithSubDescriptors();
                                         });
}

std::uint32_t ESDescriptor::getDataSize() const {
  std::uint32_t size = 3;
  if (m_stream_dependence_flag) {
    size += 2;
  }
  if (m_url_flag) {
    size += 1 + std::size(m_url_string);
  }
  if (m_ocr_stream_flag) {
    size += 2;
  }
  return size;
}

std::uint64_t ESDescriptor::readData(bitio::Reader* reader) {
  std::uint32_t data_size_with_sub_descriptors;
  auto rbits = bitio::read_uvarint<std::uint32_t>(reader, &data_size_with_sub_descriptors);
  rbits += bitio::read_uint<std::uint16_t>(reader, &m_ESID);
  rbits += read_bool(reader, &m_stream_dependence_flag);
  rbits += read_bool(reader, &m_url_flag);
  rbits += read_bool(reader, &m_ocr_stream_flag);
  rbits += bitio::read_int<std::int8_t>(reader, &m_stream_priority, 5);
  if (m_stream_dependence_flag) {
    rbits += bitio::read_uint<std::uint16_t>(reader, &m_depends_on_esid);
  }
  if (m_url_flag) {
    std::uint8_t size;
    rbits += bitio::read_uint<std::uint8_t>(reader, &size);
    rbits += bitio::read_vector_uint<std::uint8_t>(reader, static_cast<std::size_t>(size), &m_url_string);
  }
  if (m_ocr_stream_flag) {
    rbits += bitio::read_uint<std::uint16_t>(reader, &m_OCRESID);
  }
  m_sub_descriptors_size = data_size_with_sub_descriptors - getDataSize();

  return rbits;
}

std::string ESDescriptor::toString() const {
  std::string s = fmt::format(
      "{{Tag=ESDescr Size={} ESID={} StreamDependenceFlag={} UrlFlag={} "
      "OcrStreamFlag={} StreamPriority={}",
      getDataSizeWithSubDescriptors(), m_ESID, m_stream_dependence_flag, m_url_flag, m_ocr_stream_flag,
      m_stream_priority);

  if (m_stream_dependence_flag) {
    s += fmt::format(" DependsOnESID={}", m_depends_on_esid);
  }
  if (m_url_flag) {
    std::string url(std::begin(m_url_string), std::end(m_url_string));
    s += fmt::format(R"( URLLength={:#x} URLString="{}")", std::size(m_url_string), url);
  }
  if (m_ocr_stream_flag) {
    s += fmt::format(" OCRESID={}", m_OCRESID);
  }

  return s + "}";
}

DecoderConfigDescriptor::DecoderConfigDescriptor() {
  m_tag = DecoderConfigDescrTag;
}

DecoderConfigDescriptor::DecoderConfigDescriptor(const DecoderConfigDescriptorParameters& params)
    : m_object_type_indication(params.object_type_indication),
      m_stream_type(params.stream_type),
      m_upstream(params.upstream),
      m_reserved(params.reserved),
      m_buffer_size_db(params.buffer_size_db),
      m_max_bitrate(params.max_bitrate),
      m_avg_bitrate(params.avg_bitrate) {
  m_tag = DecoderConfigDescrTag;
}

std::uint64_t DecoderConfigDescriptor::writeData(bitio::Writer* writer) const {
  auto wbits = bitio::write_int<std::int8_t>(writer, m_tag);
  wbits += bitio::write_uvarint<std::uint32_t>(writer, getDataSizeWithSubDescriptors());
  wbits += bitio::write_uint<std::uint8_t>(writer, m_object_type_indication);
  wbits += bitio::write_int<std::int8_t>(writer, m_stream_type, 6);
  wbits += write_bool(writer, m_upstream);
  wbits += write_bool(writer, m_reserved);
  wbits += bitio::write_uint<std::uint32_t>(writer, m_buffer_size_db, 24);
  wbits += bitio::write_uint<std::uint32_t>(writer, m_max_bitrate);
  wbits += bitio::write_uint<std::uint32_t>(writer, m_avg_bitrate);
  return wbits;
}

std::uint64_t DecoderConfigDescriptor::getSize() const {
  return 1 + getDataSize() + bitio::uvarint_size<std::uint32_t>(getDataSize());
}

std::uint32_t DecoderConfigDescriptor::getDataSizeWithSubDescriptors() const {
  if (m_sub_descriptors_size > 0) {
    return getDataSize() + m_sub_descriptors_size;
  }

  return getDataSize() + std::accumulate(std::begin(m_sub_descriptors), std::end(m_sub_descriptors), 0U,
                                         [](const std::uint32_t a, auto d) { return a + d->getSize(); });
}

std::uint32_t DecoderConfigDescriptor::getDataSize() const {
  return 13;
}

std::uint64_t DecoderConfigDescriptor::readData(bitio::Reader* reader) {
  std::uint32_t data_size_with_sub_descriptors;
  auto rbits = bitio::read_uvarint<std::uint32_t>(reader, &data_size_with_sub_descriptors);
  rbits += bitio::read_uint<std::uint8_t>(reader, &m_object_type_indication);
  rbits += bitio::read_int<std::int8_t>(reader, &m_stream_type, 6);
  rbits += read_bool(reader, &m_upstream);
  rbits += read_bool(reader, &m_reserved);
  rbits += bitio::read_uint<std::uint32_t>(reader, &m_buffer_size_db, 24);
  rbits += bitio::read_uint<std::uint32_t>(reader, &m_max_bitrate);
  rbits += bitio::read_uint<std::uint32_t>(reader, &m_avg_bitrate);
  m_sub_descriptors_size = data_size_with_sub_descriptors - getDataSize();
  return rbits;
}
std::string DecoderConfigDescriptor::toString() const {
  return fmt::format(
      "{{Tag=DecoderConfigDescr Size={} ObjectTypeIndication={:#x} "
      "StreamType={} UpStream={} Reserved={} BufferSizeDB={} MaxBitrate={} "
      "AvgBitrate={}}}",
      getDataSize(), m_object_type_indication, m_stream_type, m_upstream, m_reserved, m_buffer_size_db, m_max_bitrate,
      m_avg_bitrate);
}

DecSpecificInfo::DecSpecificInfo() {
  m_tag = DecSpecificInfoTag;
}

DecSpecificInfo::DecSpecificInfo(const DecSpecificInfoParameters& params) : m_data(params.data) {
  m_tag = DecSpecificInfoTag;
}

std::uint64_t DecSpecificInfo::writeData(bitio::Writer* writer) const {
  auto wbits = bitio::write_int<std::int8_t>(writer, m_tag);
  wbits += bitio::write_uvarint<std::uint32_t>(writer, static_cast<std::uint32_t>(std::size(m_data)));
  return wbits + bitio::write_vector_uint<std::uint8_t>(writer, m_data);
}

std::uint64_t DecSpecificInfo::getSize() const {
  return 1 + bitio::uvarint_size<std::uint32_t>(getDataSize()) + std::size(m_data);
}

std::uint32_t DecSpecificInfo::getDataSize() const {
  return static_cast<std::uint32_t>(std::size(m_data));
}

std::uint32_t DecSpecificInfo::getDataSizeWithSubDescriptors() const {
  return static_cast<std::uint32_t>(std::size(m_data));
}

std::uint64_t DecSpecificInfo::readData(bitio::Reader* reader) {
  std::uint32_t size;
  auto rbits = bitio::read_uvarint<std::uint32_t>(reader, &size);
  return rbits + bitio::read_vector_uint<std::uint8_t>(reader, size, &m_data);
}

std::string DecSpecificInfo::toString() const {
  return fmt::format("{{Tag=DecSpecificInfo Size={} Data=[{:#x}]}}", std::size(m_data), fmt::join(m_data, ", "));
}

SLConfigDescr::SLConfigDescr() {
  m_tag = SLConfigDescrTag;
}

SLConfigDescr::SLConfigDescr(const SLConfigDescrParameters& params) : m_data(params.data) {
  m_tag = SLConfigDescrTag;
}

std::uint64_t SLConfigDescr::writeData(bitio::Writer* writer) const {
  auto wbits = bitio::write_int<std::int8_t>(writer, m_tag);
  wbits += bitio::write_uvarint<std::uint32_t>(writer, static_cast<std::uint32_t>(std::size(m_data)));
  return wbits + bitio::write_vector_uint<std::uint8_t>(writer, m_data);
}

std::uint64_t SLConfigDescr::getSize() const {
  return 1 + bitio::uvarint_size<std::uint32_t>(getDataSize()) + std::size(m_data);
}

std::uint32_t SLConfigDescr::getDataSize() const {
  return static_cast<std::uint32_t>(std::size(m_data));
}

std::uint32_t SLConfigDescr::getDataSizeWithSubDescriptors() const {
  return static_cast<std::uint32_t>(std::size(m_data));
}

std::uint64_t SLConfigDescr::readData(bitio::Reader* reader) {
  std::uint32_t size;
  auto rbits = bitio::read_uvarint<std::uint32_t>(reader, &size);
  return rbits + bitio::read_vector_uint<std::uint8_t>(reader, size, &m_data);
}

std::string SLConfigDescr::toString() const {
  return fmt::format("{{Tag=SLConfigDescr Size={} Data=[{:#x}]}}", std::size(m_data), fmt::join(m_data, ", "));
}

BoxType box_type_esds() {
  return BoxType("esds");
}

Esds::Esds() {
  m_type = box_type_esds();
}

Esds::Esds(const EsdsParameters& params) : m_descriptors(params.descriptors) {
  m_type = box_type_esds();
}

std::string Esds::toStringOnlyData() const {
  std::vector<std::string> descriptors;
  std::transform(std::begin(m_descriptors), std::end(m_descriptors), std::back_inserter(descriptors),
                 [](const auto& e) { return e->toString(); });

  return fmt::format("{} Descriptors=[{}]", getVersionAndFlagsString(), fmt::join(descriptors, ", "));
}

std::uint64_t Esds::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += std::accumulate(std::begin(m_descriptors), std ::end(m_descriptors), 0UL,
                           [&writer](const auto a, const auto d) { return a + d->writeData(&writer); });
  return wbits;
}

std::uint64_t Esds::getDataSize() const {
  return std::accumulate(std::begin(m_descriptors), std::end(m_descriptors), 4UL,
                         [](const std::uint64_t a, const auto d) { return a + d->getSize(); });
}

std::uint64_t Esds::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  m_descriptors.clear();
  while (true) {
    const auto offset_to_end = shiguredo::mp4::stream::get_istream_offset_to_end(is);
    if (offset_to_end == 0) {
      return rbits;
    }
    std::int8_t tag;
    rbits += bitio::read_int<std::int8_t>(&reader, &tag);
    std::shared_ptr<Descriptor> p;
    switch (tag) {
      case ESDescrTag:
        p = std::make_shared<ESDescriptor>();
        break;
      case DecoderConfigDescrTag:
        p = std::make_shared<DecoderConfigDescriptor>();
        break;
      case DecSpecificInfoTag:
        p = std::make_shared<DecSpecificInfo>();
        break;
      case SLConfigDescrTag:
        p = std::make_shared<SLConfigDescr>();
        break;
      default:
        spdlog::info("Esds::readData(): unknown tag: {}", tag);
        return rbits + static_cast<std::uint64_t>(offset_to_end) * 8;
    }
    rbits += p->readData(&reader);
    m_descriptors.push_back(p);
  }
  return rbits;
}

void ESDescriptor::addSubDescriptor(std::shared_ptr<Descriptor> desc) {
  m_sub_descriptors.push_back(desc);
}

void DecoderConfigDescriptor::addSubDescriptor(std::shared_ptr<Descriptor> desc) {
  m_sub_descriptors.push_back(desc);
}

}  // namespace shiguredo::mp4::box
