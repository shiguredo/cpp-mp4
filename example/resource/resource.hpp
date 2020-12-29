#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Resource {
  const std::uint64_t timestamp;
  const std::vector<std::uint8_t> data;
  bool is_key;
};

void load_resources_from_csv(std::vector<Resource>* resources, const std::string& filename);
