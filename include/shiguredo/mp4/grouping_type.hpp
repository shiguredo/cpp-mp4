#pragma once

#include <array>
#include <cstdint>

namespace shiguredo::mp4 {

using GroupingType = std::array<std::uint8_t, 4>;

const GroupingType GroupingTypeRoll{'r', 'o', 'l', 'l'};
const GroupingType GroupingTypeProl{'p', 'r', 'o', 'l'};
const GroupingType GroupingTypeAlst{'a', 'l', 's', 't'};
const GroupingType GroupingTypeRap{'r', 'a', 'p', ' '};
const GroupingType GroupingTypeTele{'t', 'e', 'l', 'e'};

}  // namespace shiguredo::mp4
