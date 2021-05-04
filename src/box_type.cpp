#include "shiguredo/mp4/box_type.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace shiguredo::mp4 {

BoxType::BoxType(const std::string& str) {
  if (std::size(str) != 4) {
    throw std::invalid_argument(fmt::format("BoxType::BoxType(): invalid box type id length: [{}]", str));
  }
  m_data[0] = static_cast<std::uint8_t>(str[0]);
  m_data[1] = static_cast<std::uint8_t>(str[1]);
  m_data[2] = static_cast<std::uint8_t>(str[2]);
  m_data[3] = static_cast<std::uint8_t>(str[3]);
}

BoxType::BoxType(const std::array<std::uint8_t, 4>& t_data) : m_data(t_data) {}

void BoxType::setData(const std::uint8_t d0, const std::uint8_t d1, const std::uint8_t d2, const std::uint8_t d3) {
  m_data[0] = d0;
  m_data[1] = d1;
  m_data[2] = d2;
  m_data[3] = d3;
}

std::array<std::uint8_t, 4> BoxType::getData() const {
  return m_data;
}

bool BoxType::operator<(const BoxType& r) const {
  auto r_data = r.getData();
  for (std::size_t i = 0; i < m_data.size(); i++)
  {
    if (m_data[i] < r_data[i]) {
      return true;
    }
  }
  return false;
}

bool BoxType::operator==(const BoxType& r) const {
  auto r_data = r.getData();
  for (std::size_t i = 0; i < m_data.size(); i++)
  {
    if (m_data[i] != r_data[i]) {
      return false;
    }
  }
  return true;
}

std::string BoxType::toString() const {
  if (std::all_of(std::begin(m_data), std::end(m_data),
                  [](const std::uint8_t ch) { return std::isprint(ch) != 0 || ch == 0xa9; })) {
    auto s = std::string(std::begin(m_data), std::end(m_data));
    return std::regex_replace(s, std::regex("\x00a9"), "©");
  }
  return fmt::format("0x{:02x}{:02x}{:02x}{:02x}", m_data[0], m_data[1], m_data[2], m_data[3]);
}

bool BoxType::matchWith(const BoxType& other) const {
  const auto boxTypeAny = box_type_any();
  if (*this == boxTypeAny || other == boxTypeAny) {
    return true;
  }
  return *this == other;
}

BoxType box_type_any() {
  static BoxType boxTypeAny(std::array<std::uint8_t, 4>{0x00, 0x00, 0x00, 0x00});
  return boxTypeAny;
}

}  // namespace shiguredo::mp4
