#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/brand.hpp"

namespace shiguredo::mp4::box {

struct StypParameters {
  const Brand major_brand;
  const std::uint32_t minor_version;
  const std::vector<Brand> compatible_brands;
};

BoxType box_type_styp();

class Styp : public Box {
 public:
  Styp();
  explicit Styp(const StypParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  Brand m_major_brand;
  std::uint32_t m_minor_version;
  std::vector<Brand> m_compatible_brands;
};

}  // namespace shiguredo::mp4::box
