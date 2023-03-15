#include "shiguredo/mp4/writer/writer.hpp"

#include <array>
#include <cstdint>
#include <iterator>
#include <string>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::writer {

Writer::~Writer() {
  if (m_moov_box_info) {
    delete m_moov_box_info;
  }
}

void Writer::addMdatData(const std::vector<std::uint8_t>& data) {
  addMdatData(data.data(), std::size(data));
}

BoxInfo* Writer::getMoovBoxInfo() const {
  return m_moov_box_info;
}

std::uint64_t Writer::getTimeFromEpoch() const {
  return 0;
}

std::uint32_t Writer::getMvhdTimescale() const {
  return m_mvhd_timescale;
}

std::uint32_t Writer::getAndUpdateNextTrackID() {
  const auto track_id = m_next_track_id;
  ++m_next_track_id;
  return track_id;
}

void Writer::appendUdtaBoxInfo(const box::DataParameters& data_params) {
  auto udta = new BoxInfo({.parent = m_moov_box_info, .box = new box::Udta()});
  auto meta = new BoxInfo({.parent = udta, .box = new box::Meta()});
  new BoxInfo({.parent = meta, .box = new box::Hdlr({.handler_type = {'m', 'd', 'i', 'r'}, .name = ""})});
  auto ilst = new BoxInfo({.parent = meta, .box = new box::Ilst()});
  auto ctoo =
      new BoxInfo({.parent = ilst,
                   .box = new box::IlstMeta({.type = BoxType(std::array<std::uint8_t, 4>({0xa9, 't', 'o', 'o'}))})});
  new BoxInfo({.parent = ctoo, .box = new box::Data(data_params)});
}

}  // namespace shiguredo::mp4::writer
