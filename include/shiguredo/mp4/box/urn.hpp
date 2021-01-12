#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

const std::uint32_t UrnSelfContainedFlags = 0x000001;

struct UrnParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::string name = "";
  const std::string location = "";
};

BoxType box_type_urn();

class Urn : public FullBox {
 public:
  Urn();
  explicit Urn(const UrnParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::string m_name;
  std::string m_location;
};

}  // namespace shiguredo::mp4::box
