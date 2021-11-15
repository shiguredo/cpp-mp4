#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/constants.hpp"

namespace shiguredo::mp4 {

struct BoxHeaderParameters {
  const std::uint64_t offset = 0;
  const std::uint64_t size = 0;
  const std::uint64_t header_size = Constants::SMALL_HEADER_SIZE;
  const BoxType type;
  const bool extend_to_eof = false;
};

class BoxHeader {
 public:
  explicit BoxHeader(const BoxHeaderParameters& t_type);

  void setOffsetAndDataSize(const std::uint64_t, const std::uint64_t);
  std::uint64_t write(std::ostream& os);

  void seekToData(std::istream& is);
  void seekToEnd(std::istream& is);

#if __cplusplus >= 202002L
  auto operator<=>(const BoxHeader&) const = default;
#endif

  std::uint64_t getOffset() const;
  std::uint64_t getSize() const;
  std::uint64_t getDataSize() const;
  std::uint64_t getHeaderSize() const;
  BoxType getType() const;
  std::string toString() const;

 private:
  std::uint64_t m_offset;
  std::uint64_t m_size = 0;
  std::uint64_t m_header_size = Constants::SMALL_HEADER_SIZE;
  BoxType m_type;
  bool m_extend_to_eof = false;
};

BoxHeader* read_box_header(std::istream&);

}  // namespace shiguredo::mp4
