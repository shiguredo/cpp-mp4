#include "shiguredo/mp4/box/emsg.hpp"

#include <fmt/core.h>

#include <cstddef>
#include <cstdint>
#include <istream>
#include <iterator>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_emsg() {
  return BoxType("emsg");
}

Emsg::Emsg() {
  m_type = box_type_emsg();
}

Emsg::Emsg(const EmsgParameters& params)
    : m_scheme_uri(params.scheme_uri),
      m_value(params.value),
      m_timescale(params.timescale),
      m_presentation_time(params.presentation_time),
      m_event_duration(params.event_duration),
      m_id(params.id),
      m_message_data(params.message_data) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_emsg();
}

std::string Emsg::toStringOnlyData() const {
  std::string message_data(std::begin(m_message_data), std::end(m_message_data));
  if (m_version == 0) {
    return fmt::format(
        R"({} SchemeIdUri="{}" Value="{}" Timescale={} PresentationTimeDelta={} EventDuration={} Id={} MessageData="{}")",
        getVersionAndFlagsString(), m_scheme_uri, m_value, m_timescale, m_presentation_time, m_event_duration, m_id,
        message_data);
  }
  return fmt::format(
      R"({} SchemeIdUri="{}" Value="{}" Timescale={} PresentationTime={} EventDuration={} Id={} MessageData="{}")",
      getVersionAndFlagsString(), m_scheme_uri, m_value, m_timescale, m_presentation_time, m_event_duration, m_id,
      message_data);
}

std::uint64_t Emsg::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  if (m_version == 0) {
    wbits += bitio::write_string(&writer, m_scheme_uri);
    wbits += bitio::write_string(&writer, m_value);
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_timescale);
    wbits += bitio::write_uint<std::uint64_t>(&writer, m_presentation_time, 32);
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_event_duration);
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_id);
  } else {
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_timescale);
    wbits += bitio::write_uint<std::uint64_t>(&writer, m_presentation_time);
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_event_duration);
    wbits += bitio::write_uint<std::uint32_t>(&writer, m_id);
    wbits += bitio::write_string(&writer, m_scheme_uri);
    wbits += bitio::write_string(&writer, m_value);
  }
  wbits += bitio::write_vector_uint<std::uint8_t>(&writer, m_message_data);
  return wbits;
}

std::uint64_t Emsg::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  if (m_version == 0) {
    auto offset_to_end = static_cast<std::uint64_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
    auto scheme_uri_rbits = bitio::read_string(&reader, &m_scheme_uri, offset_to_end * 8);
    rbits += scheme_uri_rbits;
    rbits += bitio::read_string(&reader, &m_value, offset_to_end * 8 - scheme_uri_rbits);
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_timescale);
    rbits += bitio::read_uint<std::uint64_t>(&reader, &m_presentation_time, 32);
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_event_duration);
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_id);
  } else {
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_timescale);
    rbits += bitio::read_uint<std::uint64_t>(&reader, &m_presentation_time);
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_event_duration);
    rbits += bitio::read_uint<std::uint32_t>(&reader, &m_id);
    auto offset_to_end = static_cast<std::uint64_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
    auto scheme_uri_rbits = bitio::read_string(&reader, &m_scheme_uri, offset_to_end * 8);
    rbits += scheme_uri_rbits;
    rbits += bitio::read_string(&reader, &m_value, offset_to_end * 8 - scheme_uri_rbits);
  }
  size_t offset_to_end = static_cast<std::size_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
  rbits += bitio::read_vector_uint<std::uint8_t>(&reader, offset_to_end, &m_message_data);
  return rbits;
}

std::uint64_t Emsg::getDataSize() const {
  return 4 + std::size(m_scheme_uri) + 1 + std::size(m_value) + 1 + (m_version == 0 ? 16 : 20) +
         std::size(m_message_data);
}

}  // namespace shiguredo::mp4::box
