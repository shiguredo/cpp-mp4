#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct PixelAspectRatioParameters {
  const std::uint32_t h_spacing = 1;
  const std::uint32_t v_spacing = 1;
};

BoxType box_type_pasp();

class PixelAspectRatio : public AnyTypeBox {
 public:
  PixelAspectRatio();
  explicit PixelAspectRatio(const PixelAspectRatioParameters&);

  std::string toStringOnlyData() const override;
  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint32_t m_h_spacing;
  std::uint32_t m_v_spacing;
};

}  // namespace shiguredo::mp4::box
