#include "shiguredo/mp4/box/url.hpp"

#include <fmt/core.h>

#include <cstdint>
#include <istream>
#include <iterator>
#include <string>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_url() {
  return BoxType("url ");
}

Url::Url() {
  m_type = box_type_url();
  setFlags(UrlSelfContainedFlags);
}

Url::Url(const UrlParameters& params) : m_location(params.location) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_url();
}

std::string Url::toStringOnlyData() const {
  if (checkFlag(UrlSelfContainedFlags)) {
    return getVersionAndFlagsString();
  }
  return fmt::format("{} Location=\"{}\"", getVersionAndFlagsString(), m_location);
}

std::uint64_t Url::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  if (!checkFlag(UrlSelfContainedFlags)) {
    wbits += bitio::write_string(&writer, m_location);
  }
  return wbits;
}

std::uint64_t Url::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  if (!checkFlag(UrlSelfContainedFlags)) {
    const auto offset_to_end = static_cast<std::uint64_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
    rbits += bitio::read_string(&reader, &m_location, offset_to_end * 8);
  }
  return rbits;
}

std::uint64_t Url::getDataSize() const {
  if (checkFlag(UrlSelfContainedFlags)) {
    return 4;
  }
  return 4 + std::size(m_location) + 1;
}

}  // namespace shiguredo::mp4::box
