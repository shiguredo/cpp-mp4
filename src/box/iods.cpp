#include "shiguredo/mp4/box/iods.hpp"

#include <fmt/core.h>

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"
#include "shiguredo/mp4/bitio/writer.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

BoxType box_type_iods() {
  return BoxType("iods");
}

Iods::Iods() {
  m_type = box_type_iods();
}

Iods::Iods(const IodsParameters& params)
    : m_type_tag(params.type_tag),
      m_descriptor_length(params.descriptor_length),
      m_OD_ID(params.OD_ID),
      m_OD_profile_level(params.OD_profile_level),
      m_scene_profile_level(params.scene_profile_level),
      m_audio_profile_level(params.audio_profile_level),
      m_video_profile_level(params.video_profile_level),
      m_graphics_profile_level(params.graphics_profile_level) {
  setVersion(params.version);
  setFlags(params.flags);
  m_type = box_type_iods();
}

std::string Iods::toStringOnlyData() const {
  return fmt::format(
      "{} TypeTag={:#x} ODID={} ODProfileLevel={:#x} SceneProfileLevel={:#x} AudioProfileLevel={:#x} "
      "VideoProfileLevel={:#x} "
      "GraphicsProfileLevel={:#x}",
      getVersionAndFlagsString(), m_type_tag, m_OD_ID, m_OD_profile_level, m_scene_profile_level, m_audio_profile_level,
      m_video_profile_level, m_graphics_profile_level);
}

std::uint64_t Iods::writeData(std::ostream& os) const {
  bitio::Writer writer(os);
  std::uint64_t wbits = writeVersionAndFlag(&writer);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_type_tag);
  wbits += bitio::write_uvarint<std::uint32_t>(&writer, m_descriptor_length);
  wbits += bitio::write_uint<std::uint16_t>(&writer, m_OD_ID);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_OD_profile_level);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_scene_profile_level);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_audio_profile_level);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_video_profile_level);
  wbits += bitio::write_uint<std::uint8_t>(&writer, m_graphics_profile_level);
  return wbits;
}

std::uint64_t Iods::getDataSize() const {
  return 12 + bitio::uvarint_size(m_descriptor_length);
}

std::uint64_t Iods::readData(std::istream& is) {
  bitio::Reader reader(is);
  std::uint64_t rbits = readVersionAndFlag(&reader);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_type_tag);
  rbits += bitio::read_uvarint<std::uint32_t>(&reader, &m_descriptor_length);
  rbits += bitio::read_uint<std::uint16_t>(&reader, &m_OD_ID);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_OD_profile_level);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_scene_profile_level);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_audio_profile_level);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_video_profile_level);
  rbits += bitio::read_uint<std::uint8_t>(&reader, &m_graphics_profile_level);
  return rbits + (m_descriptor_length - 7) * 8;
}

}  // namespace shiguredo::mp4::box
