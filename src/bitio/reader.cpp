#include "shiguredo/mp4/bitio/reader.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <cstdint>
#include <istream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace shiguredo::mp4::bitio {

Reader::Reader(std::istream& t_is) : m_is(t_is) {}

std::streamsize Reader::read(std::vector<std::uint8_t>* p) {
  if (m_width != 0) {
    throw std::invalid_argument("bitio::Reader::read(): m_width must be 0: m_width=" + std::to_string(m_width));
  }
  auto size = static_cast<std::streamsize>(p->size());
  m_is.read(reinterpret_cast<char*>(p->data()), size);
  if (!m_is.good()) {
    throw std::runtime_error(fmt::format("bitio::Reader::read(): istream::read() failed: rdstate={}", m_is.rdstate()));
  }
  return size;
}

void Reader::readBits(std::vector<std::uint8_t>* data, std::uint64_t size) {
  auto bytes = (size + 7) >> 3;
  data->resize(bytes);
  std::fill(std::begin(*data), std::end(*data), 0);
  auto offset = (bytes << 3) - (size);

  for (std::uint64_t i = 0; i < size; ++i) {
    auto bit = readBit();
    auto byte_index = (offset + i) >> 3;
    auto bit_index = 7 - (offset + i) % 8;
    if (bit) {
      (*data)[byte_index] |= (0x1 << bit_index);
    }
  }
}

bool Reader::readBit() {
  if (m_width == 0) {
    char buf[1];
    m_is.read(buf, 1);
    if (!m_is.good()) {
      throw std::runtime_error(
          fmt::format("bitio::Reader::readBit(): istream::read() failed: rdstate={}", m_is.rdstate()));
    }
    m_octet = static_cast<std::uint8_t>(buf[0]);
    m_width = 8;
  }
  m_width--;
  return ((m_octet >> m_width) & 0x01) != 0;
}

std::uint64_t Reader::seek(std::uint64_t offset, std::ios_base::seekdir way) {
  if ((way == std::ios_base::cur) && (m_width != 0)) {
    throw std::invalid_argument("bitio::Reader::seek(): m_width must be 0: m_width=" + std::to_string(m_width));
  }
  m_is.seekg(static_cast<std::streamoff>(offset), way);
  if (!m_is.good()) {
    throw std::runtime_error(fmt::format("bitio::Reader::seek(): istream::seekg() failed: rdstate={}", m_is.rdstate()));
  }
  m_width = 0;
  return static_cast<std::uint64_t>(m_is.tellg());
}

void Reader::setOctet(std::uint8_t octet) {
  m_octet = octet;
}

std::uint8_t Reader::getOctet() const {
  return m_octet;
}

void Reader::setWidth(std::uint8_t width) {
  m_width = width;
}

std::istream& Reader::getIStream() const {
  return m_is;
}

}  // namespace shiguredo::mp4::bitio
