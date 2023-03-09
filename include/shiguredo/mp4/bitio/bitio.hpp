#pragma once

#include <fmt/core.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"

namespace shiguredo::mp4::bitio {

template <typename T>
std::uint64_t write_int(Writer* writer, const T val, const std::uint64_t size = sizeof(T) * 8) {
  const bool signed_bit = val < 0;
  std::uint64_t wbits = 0;
  for (std::uint64_t i = 0; i < size; i += 8) {
    T v = val;
    std::uint64_t s = 8;
    if (size > i + 8) {
      v = v >> (size - (i + 8));
    } else if (size < i + 8) {
      s = size - i;
    }
    if (i == 0) {
      if (signed_bit) {
        v |= 0x1 << (s - 1);
      } else {
        v &= (0x1 << (s - 1)) - 1;
      }
    }

    std::vector<std::uint8_t> data{static_cast<std::uint8_t>(v)};
    writer->writeBits(data, s);
    wbits += s;
  }
  return wbits;
}

template <typename T>
std::uint64_t read_int(Reader* reader, T* var, const std::uint64_t size = sizeof(T) * 8) {
  if (size == 0) {
    throw std::invalid_argument(fmt::format("bitio::read_int(): size must not be zero: {}", size));
  }
  std::vector<std::uint8_t> data;
  reader->readBits(&data, size);

  bool sign_bit = false;
  if (!data.empty()) {
    std::int8_t sign_mask = static_cast<std::int8_t>(0x01 << ((size - 1) % 8));
    sign_bit = (data[0] & sign_mask) != 0;
    if (sign_bit) {
      data[0] |= ~(sign_mask - 1);
    }
  }

  std::uint64_t val = 0;
  if (sign_bit) {
    val = static_cast<std::uint64_t>(~0);
  }
  for (auto b : data) {
    val <<= 8;
    val |= static_cast<std::uint64_t>(b);
  }
  *var = static_cast<T>(val);
  return size;
}

template <typename T>
std::uint64_t write_uint(Writer* writer, const T val, const std::uint64_t size = sizeof(T) * 8) {
  std::uint64_t wbits = 0;
  for (std::uint64_t i = 0; i < size; i += 8) {
    T v = val;
    std::uint64_t s = 8;
    if (size > i + 8) {
      v = v >> (size - (i + 8));
    } else if (size < i + 8) {
      s = size - i;
    }
    std::vector<std::uint8_t> data{static_cast<std::uint8_t>(v)};
    writer->writeBits(data, s);
    wbits += s;
  }
  return wbits;
}

template <typename T>
std::uint64_t read_uint(Reader* reader, T* var, const std::uint64_t size = sizeof(T) * 8) {
  if (size == 0) {
    throw std::invalid_argument(fmt::format("bitio::read_uint(): size must not be zero: {}", size));
  }
  std::vector<std::uint8_t> data;
  reader->readBits(&data, size);

  std::uint64_t val = 0;
  for (auto b : data) {
    val <<= 8;
    val |= static_cast<std::uint64_t>(b);
  }
  *var = static_cast<T>(val);
  return size;
}

std::uint64_t write_string(Writer*, const std::string&);
std::uint64_t read_string(Reader*, std::string*, const std::uint64_t);

std::uint64_t write_pascal_string(Writer*, const std::string&);
std::uint64_t read_pascal_string(Reader*, std::string*, const std::uint64_t);

std::uint64_t write_bool(Writer*, const bool);
std::uint64_t read_bool(Reader*, bool*);

std::uint64_t write_array_uint8_4(Writer*, const std::array<std::uint8_t, 4>&);
std::uint64_t read_array_uint8_4(Reader*, std::array<std::uint8_t, 4>*);

template <typename T>
std::uint64_t write_vector_uint(Writer* writer, const std::vector<T>& v) {
  std::uint64_t wbits = 0;
  for (const auto c : v) {
    wbits += write_uint<T>(writer, c);
  }
  return wbits;
}

template <typename T>
std::uint64_t read_vector_uint(Reader* reader, const std::size_t size, std::vector<T>* v) {
  std::uint64_t rbits = 0;
  v->resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    rbits += read_uint<T>(reader, &((*v)[i]));
  }
  return rbits;
}

template <typename T>
std::uint64_t uvarint_size(const T u) {
  return sizeof(u);
}

template <typename T>
std::uint64_t write_uvarint(Writer* writer, const T u) {
  std::uint64_t val = static_cast<std::uint64_t>(u);
  for (std::int8_t i = 21; i >= 0; i -= 7) {
    std::uint8_t data = static_cast<std::uint8_t>((val >> i) & 0x7f);
    if (i > 0) {
      data |= 0x80;
    }
    std::vector<std::uint8_t> v{data};
    writer->writeBits(v, 8);
  }

  return 4 * 8;
}

template <typename T>
std::uint64_t read_uvarint(Reader* reader, T* var) {
  T val = 0;
  std::uint64_t rbits = 0;
  while (true) {
    std::vector<std::uint8_t> v;
    reader->readBits(&v, 8);
    rbits += 8;
    val = (val << 7) + static_cast<T>(v[0] & 0x7f);
    if ((v[0] & 0x80) == 0) {
      *var = val;
      return rbits;
    }
  }
}

}  // namespace shiguredo::mp4::bitio
