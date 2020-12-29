#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct FreeParameters {
  const std::vector<std::uint8_t> data;
};

BoxType box_type_free();

class Free : public Box {
 public:
  Free();
  explicit Free(const FreeParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::vector<std::uint8_t> m_data;
};

}  // namespace shiguredo::mp4::box
