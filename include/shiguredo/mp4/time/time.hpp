#pragma once

#include <cstdint>
#include <string>

namespace shiguredo::mp4::time {

const std::uint64_t SECONDS_19040101_to_19700101 = 2082844800;  // 24107 * 86400

std::string format_epoch_19040101(const std::uint64_t t);
std::uint64_t convert_to_epoch_19040101(const std::uint64_t t);

}  // namespace shiguredo::mp4::time
