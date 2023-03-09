#include "shiguredo/mp4/writer/faststart_writer.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
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

FaststartWriter::FaststartWriter(std::ostream& t_os, const FaststartWriterParameters& params)
    : m_os(t_os), m_ftyp_params(params.ftyp_params) {
  m_mvhd_timescale = params.mvhd_timescale;
  m_duration = params.duration;
  std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
  m_time_from_epoch = time::convert_to_epoch_19040101(
      static_cast<std::uint64_t>(duration_cast<std::chrono::seconds>(p.time_since_epoch()).count()));

  char* mdat_filename = ::strdup(params.mdat_path_templete.c_str());
  int fd = ::mkstemp(mdat_filename);
  if (fd == -1) {
    throw std::runtime_error("FaststartWriter::FaststartWriter(): cannot open an intermediate file: mkstemp()");
  }

  m_mdat_path = mdat_filename;
  std::free(mdat_filename);
  spdlog::debug("FaststartWriter::FaststartWriter(): m_mdat_path: {}", m_mdat_path.string());
  m_mdat_fd = ::fdopen(fd, "w+");
  if (m_mdat_fd == nullptr) {
    throw std::runtime_error(
        fmt::format("FaststartWriter::FaststartWriter(): cannot open an intermediate file: fdopen(), m_mdat_path={}",
                    m_mdat_path.string()));
  }
  if (params.write_free_box_after_moov) {
    m_free_size = 8;
  }
}

void FaststartWriter::recreateMoovBoxInfo() {
  if (m_moov_box_info) {
    delete m_moov_box_info;
  }

  m_moov_box_info = new BoxInfo({.box = new box::Moov()});
  const std::uint64_t mvhd_duration = static_cast<std::uint64_t>(static_cast<float>(m_mvhd_timescale) * m_duration);
  m_mvhd_box = new box::Mvhd({.creation_time = m_time_from_epoch,
                              .modification_time = m_time_from_epoch,
                              .timescale = m_mvhd_timescale,
                              .duration = mvhd_duration,
                              .next_track_id = m_next_track_id});
  new BoxInfo({.parent = m_moov_box_info, .box = m_mvhd_box});
  setOffsetAndSize();
}

void FaststartWriter::writeFtypBox() {
  BoxInfo* ftyp = new BoxInfo({.box = new box::Ftyp(m_ftyp_params)});

  ftyp->adjustOffsetAndSize(0);
  ftyp->write(m_os);
  m_ftyp_size = ftyp->getSize();
  delete ftyp;
}

void FaststartWriter::writeFreeBoxAfterMoovBox() {
  if (m_free_size == 0) {
    return;
  }
  BoxInfo* free = new BoxInfo({.box = new box::Free()});
  const std::uint64_t offset = getFtypSize() + getMoovSize();
  free->adjustOffsetAndSize(offset);
  m_os.seekp(static_cast<std::streamoff>(offset), std::ios_base::beg);
  free->write(m_os);
  delete free;
}

void FaststartWriter::writeMdatHeader() {
  const std::uint64_t offset = getFtypSize() + getMoovSize() + m_free_size;
  m_os.seekp(static_cast<std::streamoff>(offset), std::ios_base::beg);
  if (!m_os.good()) {
    throw std::runtime_error(
        fmt::format("FaststartWriter::writeMdatHeader(): ostream::seekp() failed: rdstate={}", m_os.rdstate()));
  }
  const auto mdat_header_size = getMdatHeaderSize();
  BoxHeader mdat({.offset = offset,
                  .size = m_mdat_data_size + mdat_header_size,
                  .header_size = mdat_header_size,
                  .type = BoxType("mdat")});
  mdat.write(m_os);
  if (!m_os.good()) {
    throw std::runtime_error(
        fmt::format("FaststartWriter::writeMdatHeader(): ostream::write() failed: rdstate={}", m_os.rdstate()));
  }
}

void FaststartWriter::addMdatData(const std::uint8_t* data, const std::size_t data_size) {
  if (const auto ret = fwrite(data, 1, data_size, m_mdat_fd); ret != data_size) {
    throw std::runtime_error(
        fmt::format("FaststartWriter::addMdatData(): fwrite() failed: data_size={} ret={}", data_size, ret));
  }
  m_mdat_data_size += static_cast<std::uint64_t>(data_size);
}

void FaststartWriter::setOffsetAndSize() {
  m_moov_box_info->adjustOffsetAndSize(m_ftyp_size);
}

std::uint64_t FaststartWriter::tellCurrentMdatOffset() {
  const auto offset = ::ftell(m_mdat_fd);
  if (offset == -1) {
    throw std::runtime_error("FaststartWriter::tellCurrentMdatOffset(): ftell() failed");
  }
  spdlog::trace("FaststartWriter::tellCurrentMdatOffset(): tell offset: {}", offset);
  return static_cast<std::uint64_t>(offset);
}

void FaststartWriter::writeMoovBox() {
  m_os.seekp(static_cast<std::streamoff>(m_ftyp_size), std::ios_base::beg);
  if (!m_os.good()) {
    throw std::runtime_error(
        fmt::format("FaststartWriter::writeFtypBox(): ostream::seekp() failed: rdstate={}", m_os.rdstate()));
  }

  m_mvhd_box->setNextTrackID(m_next_track_id);

  m_moov_box_info->write(m_os);

  writeFreeBoxAfterMoovBox();
}

std::uint64_t FaststartWriter::getFtypSize() const {
  return m_ftyp_size;
}

std::uint64_t FaststartWriter::getMoovSize() const {
  if (!m_moov_box_info) {
    return 0;
  }
  return m_moov_box_info->getSize();
}

std::uint64_t FaststartWriter::getMdatHeaderSize() const {
  if (m_mdat_data_size > (std::numeric_limits<std::uint32_t>::max() - 8)) {
    return Constants::LARGE_HEADER_SIZE;
  }
  return Constants::SMALL_HEADER_SIZE;
}

void FaststartWriter::copyMdatData() {
  if (const auto ret = std::fseek(m_mdat_fd, 0, SEEK_SET); ret == -1) {
    throw std::runtime_error("fseek failed");
  }

  char buffer[COPY_MDAT_DATA_BUFFER_SIZE];
  while (!std::feof(m_mdat_fd)) {
    std::size_t size = std::fread(buffer, 1, COPY_MDAT_DATA_BUFFER_SIZE, m_mdat_fd);
    std::copy_n(buffer, size, std::ostreambuf_iterator<char>(m_os));
    if (!m_os.good()) {
      throw std::runtime_error(
          fmt::format("FaststartWriter::copyMdatData(): copying mdat data failed: rdstate={}", m_os.rdstate()));
    }
  }

  if (auto ret = std::fclose(m_mdat_fd); ret == EOF) {
    throw std::runtime_error(
        fmt::format("FaststartWriter::copyMdatData(): cannot close the intermediate file: {}", m_mdat_path.string()));
  }

  deleteIntermediateFile();
}

void FaststartWriter::appendTrakAndUdtaBoxInfo(const std::vector<shiguredo::mp4::track::Track*>& tracks) {
  std::uint64_t prev_size = 0;
  std::uint64_t size = getFtypSize() + getMoovSize() + getMdatHeaderSize() + m_free_size;
  spdlog::trace("FaststartWriter::appendTrakAndUdtaBoxInfo()");
  do {
    spdlog::trace("  FaststartWriter::appendTrakAndUdtaBoxInfo(): size: {} {} {} {}", getFtypSize(), getMoovSize(),
                  getMdatHeaderSize(), m_free_size);
    recreateMoovBoxInfo();
    auto diff = size - prev_size;
    spdlog::trace("  FaststartWriter::appendTrakAndUdtaBoxInfo(): diff: {} {} {}", size, prev_size, diff);
    for (auto t : tracks) {
      t->resetChunkOffsets(diff);
      t->appendTrakBoxInfo(getMoovBoxInfo());
    }
    appendUdtaBoxInfo();
    prev_size = size;
    setOffsetAndSize();
    spdlog::trace("  FaststartWriter::appendTrakAndUdtaBoxInfo(): size(2): {} {} {} {}", getFtypSize(), getMoovSize(),
                  getMdatHeaderSize(), m_free_size);
    size = getFtypSize() + getMoovSize() + getMdatHeaderSize() + m_free_size;
    if (size < prev_size) {
      throw std::runtime_error(fmt::format(
          "FaststartWriter::appendTrakAndUdtaBoxInfo(): shrink moov size: prev_size={} size={}", prev_size, size));
    }
  } while (prev_size != size);
}

void FaststartWriter::deleteIntermediateFile() {
  if (const bool result = std::filesystem::remove(m_mdat_path); !result) {
    throw std::runtime_error(fmt::format(
        "FaststartWriter::deleteIntermediateFile(): cannot remove the intermediate file: {}", m_mdat_path.string()));
  }
}

std::filesystem::path FaststartWriter::getIntermediateFilePath() {
  return m_mdat_path;
}

}  // namespace shiguredo::mp4::writer
