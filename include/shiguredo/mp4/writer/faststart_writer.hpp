#pragma once

#include <cstdint>
#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box/ftyp.hpp"
#include "shiguredo/mp4/brand.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

class Track;

}

namespace shiguredo::mp4::writer {

const std::size_t COPY_MDAT_DATA_BUFFER_SIZE = 1024 * 1024;

struct FaststartWriterParameters {
  const std::uint32_t mvhd_timescale = 1000;
  const float duration;
  const std::string mdat_path_templete = "mdatXXXXXX";
  const box::FtypParameters ftyp_params{.major_brand = BrandIsom,
                                        .minor_version = 512,
                                        .compatible_brands = {BrandIsom, BrandIso2, BrandMp41}};
  const bool write_free_box_after_moov = true;
};

class FaststartWriter : public Writer {
 public:
  FaststartWriter(std::ostream&, const FaststartWriterParameters&);

  void writeFtypBox() override;
  void writeMoovBox() override;

  void addMdatData(const std::uint8_t*, const std::size_t) override;
  std::uint64_t tellCurrentMdatOffset() override;

  void appendTrakAndUdtaBoxInfo(const std::vector<shiguredo::mp4::track::Track*>&) override;
  void writeMdatHeader();
  void copyMdatData();
  void deleteIntermediateFile();
  std::filesystem::path getIntermediateFilePath();

 private:
  std::ostream& m_os;
  const box::FtypParameters m_ftyp_params;
  std::filesystem::path m_mdat_path;
  std::FILE* m_mdat_fd;
  std::uint64_t m_free_size = 0;

  void setOffsetAndSize() override;

  void recreateMoovBoxInfo();
  std::uint64_t getFtypSize() const;
  std::uint64_t getMoovSize() const;
  std::uint64_t getMdatHeaderSize() const;
  void writeFreeBoxAfterMoovBox();
};

}  // namespace shiguredo::mp4::writer
