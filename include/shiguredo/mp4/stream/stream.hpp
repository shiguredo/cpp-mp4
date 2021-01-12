#pragma once

#include <istream>

namespace shiguredo::mp4::stream {

std::streamoff get_istream_offset_to_end(std::istream&);

}  // namespace shiguredo::mp4::stream
