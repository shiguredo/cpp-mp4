#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct SkipParameters {
  const std::vector<std::uint8_t> data;
};

BoxType box_type_skip();

class Skip : public Box {
 public:
  Skip();
  explicit Skip(const SkipParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::vector<std::uint8_t> m_data;
};

}  // namespace shiguredo::mp4::box
