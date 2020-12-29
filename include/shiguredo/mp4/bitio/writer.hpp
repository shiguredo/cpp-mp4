#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

namespace shiguredo::mp4::bitio {

class Writer {
 public:
  explicit Writer(std::ostream& t_os);

  std::streamsize write(const std::vector<std::uint8_t>& p);
  void writeBits(const std::vector<std::uint8_t>& data, std::uint64_t width);
  void writeBit(bool bit);

 private:
  std::ostream& m_os;
  std::uint8_t m_octet = 0x00;
  std::uint64_t m_width = 0;
};

}  // namespace shiguredo::mp4::bitio
