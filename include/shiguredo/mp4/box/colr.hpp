#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::bitio {

class Reader;
class Writer;

}  // namespace shiguredo::mp4::bitio

namespace shiguredo::mp4::box {

struct ColrParameters {
  const std::array<std::uint8_t, 4> colour_type;
  const std::uint16_t colour_primaries;
  const std::uint16_t transfer_characteristics;
  const std::uint16_t matrix_coefficients;
  const bool full_range_flag;
  const std::vector<std::uint8_t> profile = {};
  const std::vector<std::uint8_t> unknown = {};
};

BoxType box_type_colr();

class Colr : public Box {
 public:
  Colr();
  explicit Colr(const ColrParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::array<std::uint8_t, 4> m_colour_type;
  std::uint16_t m_colour_primaries;
  std::uint16_t m_transfer_characteristics;
  std::uint16_t m_matrix_coefficients;
  bool m_full_range_flag;
  std::uint8_t m_reserved = 0;
  std::vector<std::uint8_t> m_profile;
  std::vector<std::uint8_t> m_unknown;

  std::string nclxToString() const;
  std::string profileToString() const;
  std::string unknownToString() const;

  std::uint64_t nclxMarshal(bitio::Writer*) const;

  std::uint64_t nclxUnwriteData(bitio::Reader*);
};

}  // namespace shiguredo::mp4::box
