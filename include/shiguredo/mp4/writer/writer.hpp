#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "shiguredo/mp4/box/data.hpp"
#include "shiguredo/mp4/box/ftyp.hpp"

namespace shiguredo::mp4 {

class BoxInfo;

namespace box {

class Mvhd;

}  // namespace box

namespace track {

class Track;

}

}  // namespace shiguredo::mp4

namespace shiguredo::mp4::writer {

class Writer {
 public:
  virtual ~Writer();

  virtual void writeFtypBox() = 0;
  virtual void writeMoovBox() = 0;

  virtual void addMdatData(const std::uint8_t*, const std::size_t) = 0;
  void addMdatData(const std::vector<std::uint8_t>&);
  virtual std::uint64_t tellCurrentMdatOffset() = 0;

  virtual void appendTrakAndUdtaBoxInfo(const std::vector<shiguredo::mp4::track::Track*>&) = 0;

  void appendUdtaBoxInfo(const box::DataParameters& data_params = {
                             .data = {'L', 'a', 'v', 'f', '5', '9', '.', '2', '7', '.', '1', '0', '0'}});
  void addBoxesUnderMoov(BoxInfo*);

  BoxInfo* getMoovBoxInfo() const;
  std::uint64_t getTimeFromEpoch() const;
  std::uint32_t getMvhdTimescale() const;
  std::uint32_t getAndUpdateNextTrackID();

 protected:
  std::uint32_t m_mvhd_timescale;
  float m_duration;
  std::uint64_t m_time_from_epoch = 0;
  std::uint64_t m_ftyp_size;
  std::uint64_t m_mdat_data_size = 0;
  std::uint32_t m_next_track_id = 1;

  BoxInfo* m_moov_box_info = nullptr;
  box::Mvhd* m_mvhd_box;

  virtual void setOffsetAndSize() = 0;
};

}  // namespace shiguredo::mp4::writer
