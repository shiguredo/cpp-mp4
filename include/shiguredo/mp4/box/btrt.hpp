#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct BtrtParameters {
  const std::uint32_t decoding_buffer_size;
  const std::uint32_t max_bitrate;
  const std::uint32_t avg_bitrate;
};

BoxType box_type_btrt();

class Btrt : public Box {
 public:
  Btrt();
  explicit Btrt(const BtrtParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint32_t m_decoding_buffer_size;
  std::uint32_t m_max_bitrate;
  std::uint32_t m_avg_bitrate;
};

}  // namespace shiguredo::mp4::box
