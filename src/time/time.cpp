#include "shiguredo/mp4/time/time.hpp"

#include <bits/types/struct_tm.h>
#include <fmt/chrono.h>
#include <fmt/core.h>

#include <ctime>
#include <limits>

namespace shiguredo::mp4::time {

std::string format_epoch_19040101(const std::uint64_t t) {
  std::time_t from1970 = static_cast<std::time_t>(t - SECONDS_19040101_to_19700101);
  if (from1970 > std::numeric_limits<std::uint32_t>::max()) {
    // cannot handle big values.
    return "0000-00-00T00:00:00Z";
  }
  std::tm tm;

  return fmt::format("{:%FT%TZ}", *gmtime_r(&from1970, &tm));
}

std::uint64_t convert_to_epoch_19040101(const std::uint64_t t) {
  return t + SECONDS_19040101_to_19700101;
}

}  // namespace shiguredo::mp4::time
