#pragma once

#include <compare>
#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct Co64Parameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::vector<std::uint64_t> chunk_offsets;
};

BoxType box_type_co64();

class Co64 : public FullBox {
 public:
  Co64();
  explicit Co64(const Co64Parameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t readData(std::istream& is) override;
  std::uint64_t getDataSize() const override;

  auto operator<=>(const Co64&) const = default;

 private:
  std::vector<std::uint64_t> m_chunk_offsets;
};

std::ostream& operator<<(std::ostream&, const Co64&);

}  // namespace shiguredo::mp4::box
