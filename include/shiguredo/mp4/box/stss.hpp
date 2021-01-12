#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct StssParmeters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::vector<std::uint32_t>& sample_numbers;
};

BoxType box_type_stss();

class Stss : public FullBox {
 public:
  Stss();
  explicit Stss(const StssParmeters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::vector<std::uint32_t> m_sample_numbers;
};

}  // namespace shiguredo::mp4::box
