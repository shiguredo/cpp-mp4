#include "shiguredo/mp4/stream/stream.hpp"

#include <fmt/core.h>

#include <stdexcept>

namespace shiguredo::mp4::stream {

std::streamoff get_istream_offset_to_end(std::istream& is) {
  const auto offset = is.tellg();
  if (!is.good()) {
    throw std::runtime_error(
        fmt::format("stream::get_istream_offset_to_end(): istream::tellg() failed: rdstate={}", is.rdstate()));
  }
  is.seekg(0, std::ios_base::end);
  if (!is.good()) {
    throw std::runtime_error(
        fmt::format("stream::get_istream_offset_to_end(): istream::seekg() failed: rdstate={}", is.rdstate()));
  }
  const auto end_offset = is.tellg();
  if (!is.good()) {
    throw std::runtime_error(
        fmt::format("stream::get_istream_offset_to_end(): istream::tellg() failed: rdstate={}", is.rdstate()));
  }
  is.seekg(offset, std::ios_base::beg);
  if (!is.good()) {
    throw std::runtime_error(
        fmt::format("stream::get_istream_offset_to_end(): istream::seekg() failed: rdstate={}", is.rdstate()));
  }
  return end_offset - offset;
}

}  // namespace shiguredo::mp4::stream
