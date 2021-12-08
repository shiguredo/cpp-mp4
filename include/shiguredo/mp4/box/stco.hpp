#pragma once

#include <compare>
#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct StcoParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::vector<std::uint32_t> chunk_offsets;
};

BoxType box_type_stco();

class Stco : public FullBox {
 public:
  Stco();
  explicit Stco(const StcoParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t readData(std::istream&) override;
  std::uint64_t getDataSize() const override;

  auto operator<=>(const Stco&) const = default;

 private:
  std::vector<std::uint32_t> m_chunk_offsets;
};

std::ostream& operator<<(std::ostream&, const Stco&);

}  // namespace shiguredo::mp4::box
