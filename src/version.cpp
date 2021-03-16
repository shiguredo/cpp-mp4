#include <fmt/core.h>

#include <string>

#include "shiguredo/mp4/constants.hpp"
#include "shiguredo/mp4/version.hpp"

namespace shiguredo::mp4 {

std::string get_version_string() {
  return fmt::format("{}.{}", Constants::VERSION_YEAR, Constants::VERSION_NUMBER_OF_RELEASES);
}

}  // namespace shiguredo::mp4
