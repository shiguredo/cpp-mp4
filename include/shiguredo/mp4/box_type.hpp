#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace shiguredo::mp4 {

class BoxType {
 public:
  explicit BoxType(const std::string&);
  explicit BoxType(const std::array<std::uint8_t, 4>&);
  BoxType() = default;

  void setData(const std::uint8_t, const std::uint8_t, const std::uint8_t, const std::uint8_t);

  std::array<std::uint8_t, 4> getData() const;
  auto operator<=>(const BoxType&) const = default;

  std::string toString() const;
  bool matchWith(const BoxType& other) const;

 private:
  std::array<std::uint8_t, 4> m_data;
};

BoxType box_type_any();
}  // namespace shiguredo::mp4
