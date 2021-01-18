#include "shiguredo/mp4/writer/simple_writer.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_header.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/time/time.hpp"
#include "shiguredo/mp4/track/track.hpp"

namespace shiguredo::mp4::writer {

SimpleWriter::SimpleWriter(std::ostream& t_os, const SimpleWriterParameters& params)
    : m_os(t_os), m_ftyp_params(params.ftyp_params) {
  m_mvhd_timescale = params.mvhd_timescale;
  m_duration = params.duration;
  std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
  m_time_from_epoch = time::convert_to_epoch_19040101(
      static_cast<std::uint64_t>(duration_cast<std::chrono::seconds>(p.time_since_epoch()).count()));

  const std::uint64_t mvhd_duration = static_cast<std::uint64_t>(static_cast<float>(m_mvhd_timescale) * m_duration);

  m_moov_box_info = new BoxInfo({.box = new box::Moov()});
  m_mvhd_box = new box::Mvhd({.creation_time = m_time_from_epoch,
                              .modification_time = m_time_from_epoch,
                              .timescale = m_mvhd_timescale,
                              .duration = mvhd_duration,
                              .next_track_id = m_next_track_id});
  new BoxInfo({.parent = m_moov_box_info, .box = m_mvhd_box});
}

void SimpleWriter::writeFtypBox() {
  BoxInfo* ftyp = new BoxInfo({.box = new box::Ftyp(m_ftyp_params)});

  ftyp->adjustOffsetAndSize(0);
  ftyp->write(m_os);
  m_ftyp_size = ftyp->getSize();
  delete ftyp;

  m_os.seekp(static_cast<std::streamoff>(m_ftyp_size + Constants::LARGE_HEADER_SIZE), std::ios_base::beg);
  if (!m_os.good()) {
    throw std::runtime_error(
        fmt::format("SimpleWriter::writeFreeBoxAndMdatHeader(): ostream::seekp() failed: rdstate={}", m_os.rdstate()));
  }
}

void SimpleWriter::writeFreeBoxAndMdatHeader() {
  m_os.seekp(static_cast<std::streamoff>(m_ftyp_size), std::ios_base::beg);
  if (!m_os.good()) {
    throw std::runtime_error(
        fmt::format("SimpleWriter::writeFreeBoxAndMdatHeader(): ostream::seekp() failed: rdstate={}", m_os.rdstate()));
  }
  if (m_mdat_data_size > (std::numeric_limits<std::uint32_t>::max() - 8)) {
    BoxHeader mdat({.offset = m_ftyp_size,
                    .size = m_mdat_data_size + Constants::LARGE_HEADER_SIZE,
                    .header_size = Constants::LARGE_HEADER_SIZE,
                    .type = BoxType("mdat")});
    mdat.write(m_os);
    if (!m_os.good()) {
      throw std::runtime_error(fmt::format(
          "SimpleWriter::writeFreeBoxAndMdatHeader(): ostream::write() failed: rdstate={}", m_os.rdstate()));
    }
  } else {
    BoxInfo* free = new BoxInfo({.box = new box::Free()});
    free->adjustOffsetAndSize(m_ftyp_size);
    free->write(m_os);
    if (!m_os.good()) {
      throw std::runtime_error(fmt::format(
          "SimpleWriter::writeFreeBoxAndMdatHeader(): ostream::write() failed: rdstate={}", m_os.rdstate()));
    }
    BoxHeader mdat({.offset = m_ftyp_size + free->getSize(),
                    .size = m_mdat_data_size + Constants::SMALL_HEADER_SIZE,
                    .header_size = Constants::SMALL_HEADER_SIZE,
                    .type = BoxType("mdat")});
    delete free;
    mdat.write(m_os);
    if (!m_os.good()) {
      throw std::runtime_error(fmt::format(
          "SimpleWriter::writeFreeBoxAndMdatHeader(): ostream::write() failed: rdstate={}", m_os.rdstate()));
    }
  }
}

void SimpleWriter::addMdatData(const std::uint8_t* data, const std::size_t data_size) {
  std::copy_n(data, data_size, std::ostreambuf_iterator<char>(m_os));
  if (!m_os.good()) {
    throw std::runtime_error(
        fmt::format("SimpleWriter::addMdatData(): ostream::write() failed: rdstate={}", m_os.rdstate()));
  }
  m_mdat_data_size += static_cast<std::uint64_t>(data_size);
}

void SimpleWriter::setOffsetAndSize() {
  const std::uint64_t offset = m_ftyp_size + Constants::LARGE_HEADER_SIZE + m_mdat_data_size;
  m_moov_box_info->adjustOffsetAndSize(offset);
}

std::uint64_t SimpleWriter::tellCurrentMdatOffset() {
  const auto offset = m_os.tellp();
  if (!m_os.good()) {
    throw std::runtime_error(
        fmt::format("SimpleWriter::tellCurrentMdatOffset(): ostream::tellp() failed: rdstate={}", m_os.rdstate()));
  }
  return static_cast<std::uint64_t>(offset);
}

void SimpleWriter::writeMoovBox() {
  setOffsetAndSize();
  m_mvhd_box->setNextTrackID(m_next_track_id);

  m_moov_box_info->write(m_os);
}

void SimpleWriter::appendTrakAndUdtaBoxInfo(const std::vector<shiguredo::mp4::track::Track*>& tracks) {
  for (auto t : tracks) {
    t->appendTrakBoxInfo(getMoovBoxInfo());
  }
  appendUdtaBoxInfo();
}

}  // namespace shiguredo::mp4::writer
