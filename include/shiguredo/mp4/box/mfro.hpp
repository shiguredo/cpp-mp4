#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct MfroParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint32_t size;
};

BoxType box_type_mfro();

class Mfro : public FullBox {
 public:
  Mfro();
  explicit Mfro(const MfroParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint32_t m_size;
};

}  // namespace shiguredo::mp4::box
