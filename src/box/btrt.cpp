#include "shiguredo/mp4/box/btrt.hpp"

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

BoxType box_type_btrt() {
  return BoxType("btrt");
}

Btrt::Btrt(const BtrtParameters& params)
    : m_decoding_buffer_size(params.decoding_buffer_size),
      m_max_bitrate(params.max_bitrate),
      m_avg_bitrate(params.avg_bitrate) {
  m_type = box_type_btrt();
}

Btrt::Btrt() {
  m_type = box_type_btrt();
}

std::string Btrt::toStringOnlyData() const {
  return fmt::format("DecodingBufferSize={} MaxBitrate={} AvgBitrate={}", m_decoding_buffer_size, m_max_bitrate,
                     m_avg_bitrate);
}

std::uint64_t Btrt::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  return bitio::write_uint<std::uint32_t>(&writer, m_decoding_buffer_size) +
         bitio::write_uint<std::uint32_t>(&writer, m_max_bitrate) +
         bitio::write_uint<std::uint32_t>(&writer, m_avg_bitrate);
}

std::uint64_t Btrt::getDataSize() const {
  return 12;
}

std::uint64_t Btrt::readData(std::istream& is) {
  bitio::Reader reader(is);
  return bitio::read_uint<std::uint32_t>(&reader, &m_decoding_buffer_size) +
         bitio::read_uint<std::uint32_t>(&reader, &m_max_bitrate) +
         bitio::read_uint<std::uint32_t>(&reader, &m_avg_bitrate);
}

}  // namespace shiguredo::mp4::box
