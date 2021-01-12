#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct MehdParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint64_t fragment_duration;
};

BoxType box_type_mehd();

class Mehd : public FullBox {
 public:
  Mehd();
  explicit Mehd(const MehdParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint64_t m_fragment_duration;
};

}  // namespace shiguredo::mp4::box
