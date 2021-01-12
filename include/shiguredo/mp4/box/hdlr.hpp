#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct HdlrParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint32_t pre_defined = 0;
  const std::array<std::uint8_t, 4> handler_type;
  const std::string name;
  const std::vector<std::uint8_t> padding = {};
};

BoxType box_type_hdlr();

class Hdlr : public FullBox {
 public:
  Hdlr();
  explicit Hdlr(const HdlrParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::uint32_t m_pre_defined = 0;
  std::array<std::uint8_t, 4> m_handler_type;
  std::array<std::uint8_t, 4> m_manufacturer = {0, 0, 0, 0};  // Reserved. Set to 0.
  std::array<std::uint8_t, 4> m_flags = {0, 0, 0, 0};         // Reserved. Set to 0.
  std::array<std::uint8_t, 4> m_flags_mask = {0, 0, 0, 0};    // Reserved. Set to 0
  std::string m_name;
  std::vector<std::uint8_t> m_padding = {};
};

}  // namespace shiguredo::mp4::box
