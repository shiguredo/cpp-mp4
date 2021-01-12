#pragma once

#include <cstdint>
#include <istream>
#include <vector>

namespace shiguredo::mp4::bitio {

class Reader {
 public:
  explicit Reader(std::istream& t_is);

  std::streamsize read(std::vector<std::uint8_t>* p);
  void readBits(std::vector<std::uint8_t>* data, std::uint64_t size);
  bool readBit();
  std::uint64_t seek(std::uint64_t offset, std::ios_base::seekdir way);

  void setOctet(std::uint8_t octet);
  std::uint8_t getOctet() const;

  void setWidth(std::uint8_t width);

  std::istream& getIStream() const;

 private:
  std::istream& m_is;
  std::uint8_t m_octet = 0x00;
  std::uint8_t m_width = 0;
};

}  // namespace shiguredo::mp4::bitio
