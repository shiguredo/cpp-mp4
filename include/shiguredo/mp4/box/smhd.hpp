#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct SmhdParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::int16_t balance = 0;
};

BoxType box_type_smhd();

class Smhd : public FullBox {
 public:
  Smhd();
  explicit Smhd(const SmhdParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

  float getBalance() const;

 private:
  std::int16_t m_balance;
  std::uint16_t m_reserved = 0;
};

}  // namespace shiguredo::mp4::box
