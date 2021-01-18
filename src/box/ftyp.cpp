#include "shiguredo/mp4/box/ftyp.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <ext/alloc_traits.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/brand.hpp"
#include "shiguredo/mp4/stream/stream.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_ftyp() {
  return BoxType("ftyp");
}

Ftyp::Ftyp() {
  m_type = box_type_ftyp();
}

Ftyp::Ftyp(const FtypParameters& params)
    : m_major_brand(params.major_brand),
      m_minor_version(params.minor_version),
      m_compatible_brands(params.compatible_brands) {
  m_type = box_type_ftyp();
}

std::string Ftyp::toStringOnlyData() const {
  std::vector<std::string> brands;
  std::transform(std::begin(m_compatible_brands), std::end(m_compatible_brands), std::back_inserter(brands),
                 [](const auto& b) {
                   std::string brand(std::begin(b), std::end(b));
                   return fmt::format(R"({{CompatibleBrand="{}"}})", brand);
                 });
  std::string major_brand(std::begin(m_major_brand), std::end(m_major_brand));

  return fmt::format(R"(MajorBrand="{}" MinorVersion={} CompatibleBrands=[{}])", major_brand, m_minor_version,
                     fmt::join(brands, ", "));
}

std::uint64_t Ftyp::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  auto wbits = bitio::write_array_uint8_4(&writer, m_major_brand);
  wbits += bitio::write_uint<std::uint32_t>(&writer, m_minor_version);
  for (const auto& b : m_compatible_brands) {
    wbits += bitio::write_array_uint8_4(&writer, b);
  }
  return wbits;
}

std::uint64_t Ftyp::readData(std::istream& is) {
  bitio::Reader reader(is);
  auto rbits = bitio::read_array_uint8_4(&reader, &m_major_brand);
  rbits += bitio::read_uint<std::uint32_t>(&reader, &m_minor_version);
  const std::size_t offset_to_end = static_cast<std::size_t>(shiguredo::mp4::stream::get_istream_offset_to_end(is));
  if (offset_to_end % 4 != 0) {
    throw std::runtime_error("Ftyp::readData(): box has invalid length");
  }
  m_compatible_brands.resize(offset_to_end / 4);
  for (std::size_t i = 0; i * 4 < offset_to_end; ++i) {
    Brand brand;
    rbits += bitio::read_array_uint8_4(&reader, &brand);
    m_compatible_brands[i] = brand;
  }

  return rbits;
}

std::uint64_t Ftyp::getDataSize() const {
  return 8 + std::size(m_compatible_brands) * 4;
}

}  // namespace shiguredo::mp4::box
