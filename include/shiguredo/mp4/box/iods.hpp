#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct IodsParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::uint8_t type_tag;
  const std::uint32_t descriptor_length = 7;
  const std::uint16_t OD_ID;
  const std::uint8_t OD_profile_level;
  const std::uint8_t scene_profile_level;
  const std::uint8_t audio_profile_level;
  const std::uint8_t video_profile_level;
  const std::uint8_t graphics_profile_level;
};

BoxType box_type_iods();

class Iods : public FullBox {
 public:
  Iods();
  explicit Iods(const IodsParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::uint8_t m_type_tag;
  std::uint32_t m_descriptor_length;
  std::uint16_t m_OD_ID;
  std::uint8_t m_OD_profile_level;
  std::uint8_t m_scene_profile_level;
  std::uint8_t m_audio_profile_level;
  std::uint8_t m_video_profile_level;
  std::uint8_t m_graphics_profile_level;
};

}  // namespace shiguredo::mp4::box
