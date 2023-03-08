#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct FielParameters {
  const std::uint8_t field_count = 1;
  const std::uint8_t field_ordering = 0;
};

BoxType box_type_fiel();

class Fiel : public Box {
 public:
  Fiel();
  explicit Fiel(const FielParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint8_t m_field_count;
  std::uint8_t m_field_ordering;
};

}  // namespace shiguredo::mp4::box
