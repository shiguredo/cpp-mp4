#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_edts();

class Edts : public Box {
 public:
  Edts();

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t readData(std::istream& is) override;
};

}  // namespace shiguredo::mp4::box
