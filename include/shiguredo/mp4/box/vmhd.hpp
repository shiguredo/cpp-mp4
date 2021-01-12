#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct VmhdParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint16_t graphicsmode = 0;
  const std::array<std::uint16_t, 3> opcolor = {0, 0, 0};
};

BoxType box_type_vmhd();

class Vmhd : public FullBox {
 public:
  Vmhd();
  explicit Vmhd(const VmhdParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint16_t m_graphicsmode;
  std::array<std::uint16_t, 3> m_opcolor;
};

}  // namespace shiguredo::mp4::box
