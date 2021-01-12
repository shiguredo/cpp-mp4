#include "resource/resource.hpp"

#include <fmt/core.h>
#include <rapidcsv.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

std::vector<std::uint8_t> hex2bin(const std::string& hex_data) {
  std::vector<std::uint8_t> ret;
  ret.resize(std::size(hex_data) / 2);
  for (size_t i = 0; i < std::size(ret); ++i) {
    std::int16_t tmp;
    if (std::istringstream(hex_data.substr(i * 2, 2)) >> std::hex >> tmp) {
      ret[i] = static_cast<std::uint8_t>(tmp);
    }
  }

  return ret;
}

void load_resources_from_csv(std::vector<Resource>* resources, const std::string& filename) {
  resources->clear();
  rapidcsv::Document doc(filename, rapidcsv::LabelParams(-1, -1));
  resources->reserve(doc.GetRowCount());
  for (size_t i = 0; i < doc.GetRowCount(); ++i) {
    std::uint64_t timestamp = doc.GetCell<std::uint64_t>(0, i);
    std::size_t data_length = doc.GetCell<std::uint64_t>(1, i);
    std::string hex_data = doc.GetCell<std::string>(2, i);
    std::vector<std::uint8_t> data = hex2bin(hex_data);
    if (data_length != std::size(data)) {
      throw new std::runtime_error(fmt::format("load_resources_from_csv(): data is invalid: timestamp={}", timestamp));
    }
    bool is_key = true;
    if (doc.GetColumnCount() > 3) {
      is_key = doc.GetCell<bool>(3, i, [](const std::string& s, bool& val) {
        if (s == "true") {
          val = true;
        } else {
          val = false;
        }
      });
    }
    resources->push_back(Resource{.timestamp = timestamp, .data = data, .is_key = is_key});
  }
}
