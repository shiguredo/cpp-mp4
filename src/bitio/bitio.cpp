#include "shiguredo/mp4/bitio/bitio.hpp"

#include <fmt/core.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"

namespace shiguredo::mp4::bitio {

std::uint64_t write_string(Writer* writer, const std::string& str) {
  std::uint64_t wbits = 0;
  for (auto c : str) {
    std::vector<std::uint8_t> v{static_cast<std::uint8_t>(c)};
    writer->writeBits(v, 8);
    wbits += 8;
  }
  std::vector<std::uint8_t> v{0x00};
  writer->writeBits(v, 8);
  wbits += 8;
  return wbits;
}

std::uint64_t read_string(Reader* reader, std::string* str, std::uint64_t max_rbits) {
  std::uint64_t rbits = 0;
  str->clear();
  while (rbits < max_rbits) {
    std::vector<std::uint8_t> v;
    reader->readBits(&v, 8);
    rbits += 8;
    if (v[0] == 0x00) {
      break;
    }
    *str += static_cast<char>(v[0]);
  }
  return rbits;
}

std::uint64_t write_pascal_string(Writer* writer, const std::string& str) {
  std::size_t size = std::size(str);
  if (size > 0xff) {
    throw std::runtime_error("bitio::write_pascal_string(): size of pascal string should be equal or less than 0xff");
  }
  std::uint64_t wbits = 0;
  std::vector<std::uint8_t> v{static_cast<std::uint8_t>(size)};
  writer->writeBits(v, 8);
  wbits += 8;
  for (auto c : str) {
    std::vector<std::uint8_t> vec{static_cast<std::uint8_t>(c)};
    writer->writeBits(vec, 8);
    wbits += 8;
  }
  return wbits;
}

std::uint64_t read_pascal_string(Reader* reader, std::string* str, std::uint64_t max_rbits) {
  if (max_rbits < 2 * 8) {
    return read_string(reader, str, max_rbits);
  }

  std::istream& is = reader->getIStream();

  auto offset = is.tellg();
  if (!is.good()) {
    throw std::runtime_error(
        fmt::format("bitio::read_pascal_string(): istream::tellg() failed: rdstate={}", is.rdstate()));
  }

  std::vector<std::uint8_t> v;
  reader->readBits(&v, 8);
  if (v[0] * 8 != (max_rbits - 8)) {
    is.seekg(offset, std::ios_base::beg);
    if (!is.good()) {
      throw std::runtime_error(
          fmt::format("bitio::read_pascal_string(): istream::seekg() failed: rdstate={}", is.rdstate()));
    }
    return read_string(reader, str, max_rbits);
  }

  std::uint64_t rbits = 0;
  str->clear();
  while (rbits < v[0] * 8) {
    std::vector<std::uint8_t> vec;
    reader->readBits(&vec, 8);
    rbits += 8;
    *str += static_cast<char>(vec[0]);
  }
  return rbits + 8;
}

std::uint64_t write_bool(Writer* writer, const bool b) {
  std::uint8_t val = b ? 0xff : 0x00;
  std::vector<std::uint8_t> v{val};
  writer->writeBits(v, 1);
  return 1;
}

std::uint64_t read_bool(Reader* reader, bool* b) {
  std::vector<std::uint8_t> v;
  reader->readBits(&v, 1);
  *b = v[0] != 0x00;
  return 1;
}

std::uint64_t write_array_uint8_4(Writer* writer, const std::array<uint8_t, 4>& v) {
  std::uint64_t wbits = 0;
  for (const auto c : v) {
    wbits += write_uint<std::uint8_t>(writer, c);
  }
  return wbits;
}

std::uint64_t read_array_uint8_4(Reader* reader, std::array<uint8_t, 4>* v) {
  for (std::size_t i = 0; i < 4; ++i) {
    read_uint<std::uint8_t>(reader, &((*v)[i]));
  }
  return 32;
}

}  // namespace shiguredo::mp4::bitio
