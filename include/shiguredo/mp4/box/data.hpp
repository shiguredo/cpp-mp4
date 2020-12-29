#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <string_view>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

// https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/Metadata/Metadata.html#//apple_ref/doc/uid/TP40000939-CH1-SW35
enum DataType : std::uint32_t {
  Binary = 0,
  StringUTF8 = 1,
  StringUTF16 = 2,
  StringMac = 3,
  JPEG = 14,
  SignedIntBigEndian = 21,
  Float32BigEndian = 22,
  Float64BigEndian = 23,
};

constexpr std::string_view data_type_to_string(DataType type) {
  using namespace std::string_view_literals; /* NOLINT */
  switch (type) {
    case DataType::Binary:
      return "BINARY"sv;
    case DataType::StringUTF8:
      return "UTF8"sv;
    case DataType::StringUTF16:
      return "UTF16"sv;
    case DataType::StringMac:
      return "MAC_STR"sv;
    case DataType::JPEG:
      return "JPEG"sv;
    case DataType::SignedIntBigEndian:
      return "INT"sv;
    case DataType::Float32BigEndian:
      return "FLOAT32"sv;
    case DataType::Float64BigEndian:
      return "FLOAT64"sv;
  }
  return "";
}

struct DataParameters {
  const DataType data_type = DataType::StringUTF8;
  const std::uint32_t data_lang = 0;
  const std::vector<std::uint8_t> data;
};

BoxType box_type_data();

class Data : public Box {
 public:
  Data();
  explicit Data(const DataParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  DataType m_data_type;
  std::uint32_t m_data_lang;
  std::vector<std::uint8_t> m_data;
};

}  // namespace shiguredo::mp4::box
