#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/brand.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_ftyp();

struct FtypParameters {
  const Brand major_brand;
  const std::uint32_t minor_version;
  const std::vector<Brand> compatible_brands;
};

class Ftyp : public Box {
 public:
  Ftyp();
  explicit Ftyp(const FtypParameters& params);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  Brand m_major_brand;
  std::uint32_t m_minor_version;
  std::vector<Brand> m_compatible_brands;
};

}  // namespace shiguredo::mp4::box
