#include "shiguredo/mp4/bitio/writer.hpp"

#include <fmt/core.h>

#include <cstdint>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace shiguredo::mp4::bitio {

Writer::Writer(std::ostream& t_os) : m_os(t_os) {}

std::streamsize Writer::write(const std::vector<std::uint8_t>& p) {
  if (m_width != 0) {
    throw std::invalid_argument("bitio::Writer::write(): width is not 0");
  }

  auto size = static_cast<std::streamsize>(std::size(p));
  m_os.write(reinterpret_cast<const char*>(p.data()), size);

  if (!m_os.good()) {
    throw std::runtime_error(
        fmt::format("bitio::Writer::write(): ostream::write() failed: rdstate={}", m_os.rdstate()));
  }

  return size;
}

void Writer::writeBits(const std::vector<std::uint8_t>& data, const std::uint64_t width) {
  auto length = std::size(data) * 8;
  auto offset = length - width;
  for (auto i = offset; i < length; ++i) {
    auto oi = i >> 3;
    writeBit(((data[oi] >> (7 - i % 8)) & 0x01) != 0);
  }
}

void Writer::writeBit(const bool bit) {
  if (bit) {
    m_octet |= (0x1 << (7 - m_width));
  }
  ++m_width;

  if (m_width == 8) {
    char data[1] = {static_cast<char>(m_octet)};
    m_os.write(data, 1);
    if (!m_os.good()) {
      throw std::runtime_error(
          fmt::format("bitio::Writer::writeBit(): ostream::write() failed: rdstate={}", m_os.rdstate()));
    }

    m_octet = 0x00;
    m_width = 0;
  }
}

}  // namespace shiguredo::mp4::bitio
