#pragma once

#include <array>
#include <cstdint>

namespace shiguredo::mp4 {

using Brand = std::array<std::uint8_t, 4>;

const Brand BrandIsom{'i', 's', 'o', 'm'};
const Brand BrandIso2{'i', 's', 'o', '2'};
const Brand BrandMp41{'m', 'p', '4', '1'};
const Brand BrandAvc1{'a', 'v', 'c', '1'};

}  // namespace shiguredo::mp4
