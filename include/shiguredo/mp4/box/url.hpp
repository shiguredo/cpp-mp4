#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

const std::uint32_t UrlSelfContainedFlags = 0x000001;

struct UrlParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::string location = "";
};

BoxType box_type_url();

class Url : public FullBox {
 public:
  Url();
  explicit Url(const UrlParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::string m_location;
};

}  // namespace shiguredo::mp4::box
