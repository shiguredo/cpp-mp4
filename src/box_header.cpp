#include "shiguredo/mp4/box_header.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <istream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <vector>

#include "shiguredo/mp4/endian/endian.hpp"

namespace shiguredo::mp4 {

BoxHeader* read_box_header(std::istream& is) {
  std::uint64_t offset = static_cast<std::uint64_t>(is.tellg());
  if (!is.good()) {
    throw std::runtime_error(fmt::format("read_box_header(): istream::tellg() failed: rdstate={}", is.rdstate()));
  }
  is.seekg(0, std::ios_base::end);
  if (!is.good()) {
    throw std::runtime_error(fmt::format("read_box_header(): istream::seekg() failed: rdstate={}", is.rdstate()));
  }
  auto offset_eof = static_cast<std::uint64_t>(is.tellg());
  if (!is.good()) {
    throw std::runtime_error(fmt::format("read_box_header(): istream::tellg() failed: rdstate={}", is.rdstate()));
  }
  if ((offset_eof - offset) < Constants::SMALL_HEADER_SIZE) {
    throw std::invalid_argument(
        fmt::format("read_box_header(): stream has not enough data (1): {} {}", offset_eof, offset));
  }

  is.seekg(static_cast<std::streamoff>(offset), std::ios_base::beg);
  if (!is.good()) {
    throw std::runtime_error(fmt::format("read_box_header(): istream::seekg() failed: rdstate={}", is.rdstate()));
  }
  std::array<std::uint8_t, 8> buf;
  std::copy_n(std::istreambuf_iterator<char>(is), 8, std::begin(buf));
  std::uint64_t header_size = Constants::SMALL_HEADER_SIZE;
  std::uint64_t size = endian::be_to_uint32(buf[0], buf[1], buf[2], buf[3]);
  BoxType type;
  type.setData(buf[4], buf[5], buf[6], buf[7]);
  bool extend_to_eof = false;

  if (size == 0) {
    size = offset_eof - offset;
    extend_to_eof = true;
  } else if (size == 1) {
    if ((offset_eof - offset) < Constants::LARGE_HEADER_SIZE) {
      throw std::invalid_argument(
          fmt::format("read_box_header(): stream has not enough data (2): {}", offset_eof - offset));
    }
    is.seekg(static_cast<std::streamoff>(offset + Constants::SMALL_HEADER_SIZE), std::ios_base::beg);
    if (!is.good()) {
      throw std::runtime_error(fmt::format("read_box_header(): istream::seekg() failed: rdstate={}", is.rdstate()));
    }
    std::copy_n(std::istreambuf_iterator<char>(is), Constants::LARGE_HEADER_SIZE - Constants::SMALL_HEADER_SIZE,
                std::begin(buf));
    header_size = Constants::LARGE_HEADER_SIZE;
    size = endian::be_to_uint64(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
  }
  return new BoxHeader(
      {.offset = offset, .size = size, .header_size = header_size, .type = type, .extend_to_eof = extend_to_eof});
}

BoxHeader::BoxHeader(const BoxHeaderParameters& params)
    : m_offset(params.offset),
      m_size(params.size),
      m_header_size(params.header_size),
      m_type(params.type),
      m_extend_to_eof(params.extend_to_eof) {}

std::uint64_t BoxHeader::write(std::ostream& os) {
  os.seekp(static_cast<std::streamoff>(m_offset), std::ios_base::beg);
  if (!os.good()) {
    throw std::runtime_error(fmt::format("BoxHeader::write() ostream::seekp() failed: rdstate={}", os.rdstate()));
  }
  std::vector<std::uint8_t> data;

  if (m_extend_to_eof) {
    data.resize(Constants::SMALL_HEADER_SIZE);
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
  } else if (m_size <= std::numeric_limits<std::uint32_t>::max() && m_header_size != Constants::LARGE_HEADER_SIZE) {
    data.resize(Constants::SMALL_HEADER_SIZE);
    auto a = endian::uint32_to_be(static_cast<uint32_t>(m_size));
    std::copy(std::begin(a), std::end(a), std::begin(data));
  } else {
    data.resize(Constants::LARGE_HEADER_SIZE);
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 1;
    auto a = endian::uint64_to_be(m_size);
    std::copy(std::begin(a), std::end(a), std::begin(data) + 8);
  }
  auto type_data = m_type.getData();
  data[4] = type_data[0];
  data[5] = type_data[1];
  data[6] = type_data[2];
  data[7] = type_data[3];

  std::copy(std::begin(data), std::end(data), std::ostreambuf_iterator<char>(os));
  m_size += std::size(data) - m_header_size;
  m_header_size = std::size(data);
  return m_size;
}

void BoxHeader::seekToData(std::istream& is) {
  is.seekg(static_cast<std::streamoff>(m_offset + m_header_size), std::ios_base::beg);
  if (!is.good()) {
    throw std::runtime_error(fmt::format("BoxHeader::seekToData() istream::seekg() failed: rdstate={}", is.rdstate()));
  }
}

std::uint64_t BoxHeader::getOffset() const {
  return m_offset;
}

std::uint64_t BoxHeader::getSize() const {
  return m_size;
}

std::uint64_t BoxHeader::getDataSize() const {
  return m_size - m_header_size;
}

std::uint64_t BoxHeader::getHeaderSize() const {
  return m_header_size;
}

BoxType BoxHeader::getType() const {
  return m_type;
}

std::string BoxHeader::toString() const {
  return fmt::format("[{}] Offset={} Size={}", m_type.toString(), m_offset, m_size);
}

void BoxHeader::seekToEnd(std::istream& is) {
  is.seekg(static_cast<std::streamoff>(m_offset + m_size), std::ios_base::beg);
  if (!is.good()) {
    throw std::runtime_error(fmt::format("BoxHeader::seekToEnd(): istream::seekg() failed: rdstate={}", is.rdstate()));
  }
}

void BoxHeader::setOffsetAndDataSize(const std::uint64_t t_offset, const std::uint64_t t_data_size) {
  m_offset = t_offset;
  if (t_data_size > (std::numeric_limits<std::uint32_t>::max() - Constants::SMALL_HEADER_SIZE)) {
    m_header_size = Constants::LARGE_HEADER_SIZE;
  } else {
    m_header_size = Constants::SMALL_HEADER_SIZE;
  }
  m_size = t_data_size + m_header_size;
}

}  // namespace shiguredo::mp4
