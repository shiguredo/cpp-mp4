#include "shiguredo/mp4/track/h264.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

H264Track::H264Track(const H264TrackParameters& params) {
  m_timescale = params.timescale;
  m_duration = params.duration;
  m_media_time = params.media_time;
  m_mvhd_timescale = params.writer->getMvhdTimescale();
  m_time_from_epoch = params.writer->getTimeFromEpoch();
  m_track_id = params.track_id;
  if (m_track_id == 0) {
    throw std::runtime_error("H264Track::H264Track(): invalid track_id=0");
  }
  m_width = params.width;
  m_height = params.height;
  m_writer = params.writer;
  m_configuration_version = params.configuration_version;
  m_profile = params.configuration_version;
  m_profile_compatibility = params.profile_compatibility;
  m_level = params.level;
}

void H264Track::makeStsdBoxInfo(BoxInfo* stbl) {
  auto stsd = new BoxInfo({.parent = stbl, .box = new box::Stsd({.entry_count = 1})});
  auto avc1 = new BoxInfo({.parent = stsd,
                           .box = new box::VisualSampleEntry({
                               .type = BoxType("avc1"),
                               .data_reference_index = 1,
                               .width = static_cast<std::uint16_t>(m_width),
                               .height = static_cast<std::uint16_t>(m_height),
                           })});

  auto avcc = new box::AVCDecoderConfiguration({
      .configuration_version = m_configuration_version,
      .profile = m_profile,
      .profile_compatibility = m_profile_compatibility,
      .level = m_level,
      .sequence_parameter_sets = m_sequence_parameter_sets,
      .picture_parameter_sets = m_picture_parameter_sets,
  });
  new BoxInfo({.parent = avc1, .box = avcc});
}

void H264Track::appendTrakBoxInfo(BoxInfo* moov) {
  finalize();

  auto trak = makeTrakBoxInfo(moov);
  makeTkhdBoxInfo(trak);

  auto edts = makeEdtsBoxInfo(trak);
  makeElstBoxInfo(edts);

  auto mdia = makeMdiaBoxInfo(trak);
  makeMdhdBoxInfo(mdia);
  makeHdlrBoxInfo(mdia);

  auto minf = makeMinfBoxInfo(mdia);
  makeVmhdBoxInfo(minf);
  makeDinfBoxInfo(minf);

  auto stbl = makeStblBoxInfo(minf);
  makeStsdBoxInfo(stbl);
  makeSttsBoxInfo(stbl);
  makeStssBoxInfo(stbl);
  makeStscBoxInfo(stbl);
  makeStszBoxInfo(stbl);
  makeOffsetBoxInfo(stbl);
}

void H264Track::addData(const std::uint64_t timestamp, const std::vector<std::uint8_t>& data, bool is_key) {
  addData(timestamp, data.data(), data.size(), is_key);
}

void H264Track::addData(const std::uint64_t timestamp,
                        const std::uint8_t* data,
                        const std::size_t data_size,
                        bool is_key) {
  std::size_t start = 0;
  while (1) {
    auto nu = find_next_nal_unit(data, data_size, start);
    std::vector<uint8_t> sps_data = {};
    std::vector<uint8_t> pps_data = {};
    std::size_t video_data_size;
    std::uint8_t* mdat_data;
    switch (nu.header & 0x1f) {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        // VCL
        video_data_size = nu.end - nu.start - nu.start_code_size;
        mdat_data = new std::uint8_t[video_data_size + 4];
        mdat_data[0] = static_cast<std::uint8_t>(video_data_size >> 24);
        mdat_data[1] = static_cast<std::uint8_t>(video_data_size >> 16);
        mdat_data[2] = static_cast<std::uint8_t>(video_data_size >> 8);
        mdat_data[3] = static_cast<std::uint8_t>(video_data_size & 0xff);

        std::copy_n(data + nu.start + nu.start_code_size, video_data_size, mdat_data + 4);

        addMdatData(timestamp, mdat_data, video_data_size + 4, is_key);
        delete[] mdat_data;
        break;
      case 7:
        // SPS
        std::copy_n(data + nu.start + nu.start_code_size, nu.end - nu.start - nu.start_code_size,
                    std::back_inserter(sps_data));
        appendSequenceParameterSets(shiguredo::mp4::box::AVCParameterSet({.nal_unit = sps_data}));
        break;
      case 8:
        // PPS
        std::copy_n(data + nu.start + nu.start_code_size, nu.end - nu.start - nu.start_code_size,
                    std::back_inserter(pps_data));
        appendPictureParameterSets(shiguredo::mp4::box::AVCParameterSet({.nal_unit = pps_data}));
        break;
      default:
        throw std::runtime_error(
            fmt::format("unsuppoted NalUnit type: header={:02x}, type={:02x}", nu.header, nu.header & 0x1f));
    }

    if (nu.end >= data_size) {
      return;
    }
    start = nu.end;
  }
}

bool operator==(NalUnit const& left, NalUnit const& right) {
  return left.start_code_size == right.start_code_size && left.start == right.start && left.end == right.end &&
         left.header == right.header;
}

std::ostream& operator<<(std::ostream& os, const NalUnit& nu) {
  os << "start_code_size: " << nu.start_code_size << " start: " << nu.start << " end: " << nu.end
     << " header: " << static_cast<uint32_t>(nu.header);
  return os;
}

NalUnit find_next_nal_unit(const std::uint8_t* data, const std::size_t data_size, const std::size_t start) {
  // start から NalUnit が始まっていることを期待する
  auto index = start;
  std::size_t start_code_size;
  std::uint8_t header = 0;
  while (index + 5 < data_size) {
    if (data[index] == 0 && data[index + 1] == 0 && data[index + 2] == 0 & data[index + 3] == 1) {
      if (header == 0) {
        start_code_size = 4;
        header = data[index + 4];
      } else {
        return NalUnit{start_code_size, start, index, header};
      }
      index += 4;
      continue;
    }
    if (data[index] == 0 && data[index + 1] == 0 && data[index + 2] == 1) {
      if (header == 0) {
        start_code_size = 3;
        header = data[index + 3];
      } else {
        return NalUnit{start_code_size, start, index, header};
      }
      index += 3;
      continue;
    }
    if (index == start && header == 0) {
      throw std::runtime_error(fmt::format("not start with nalunit: start={}", start));
    }
    ++index;
  }
  if (header == 0) {
    throw std::runtime_error("nalunit not found");
  }
  return NalUnit{start_code_size, start, data_size, header};
}

void H264Track::appendSequenceParameterSets(const shiguredo::mp4::box::AVCParameterSet& params) {
  m_sequence_parameter_sets.emplace_back(params);
}

void H264Track::appendPictureParameterSets(const shiguredo::mp4::box::AVCParameterSet& params) {
  m_picture_parameter_sets.emplace_back(params);
}

}  // namespace shiguredo::mp4::track
