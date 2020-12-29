#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct TfdtParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint64_t base_media_decode_time;
};

BoxType box_type_tfdt();

class Tfdt : public FullBox {
 public:
  Tfdt();
  explicit Tfdt(const TfdtParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint64_t m_base_media_decode_time;
};

}  // namespace shiguredo::mp4::box
