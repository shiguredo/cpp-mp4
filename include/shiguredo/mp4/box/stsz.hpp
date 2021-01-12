#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct StszParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint32_t sample_size = 0;
  const std::vector<std::uint32_t> entry_sizes;
};

BoxType box_type_stsz();

class Stsz : public FullBox {
 public:
  Stsz();
  explicit Stsz(const StszParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint32_t m_sample_size;
  std::vector<std::uint32_t> m_entry_sizes;
};

}  // namespace shiguredo::mp4::box
