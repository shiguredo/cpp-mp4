#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

#include "shiguredo/mp4/box/ftyp.hpp"
#include "shiguredo/mp4/brand.hpp"
#include "shiguredo/mp4/writer/writer.hpp"

namespace shiguredo::mp4::track {

class Track;

}

namespace shiguredo::mp4::writer {

struct SimpleWriterParameters {
  const std::uint32_t mvhd_timescale = 1000;
  const float duration;
  const box::FtypParameters ftyp_params{.major_brand = BrandIsom,
                                        .minor_version = 512,
                                        .compatible_brands = {BrandIsom, BrandIso2, BrandMp41}};
};

class SimpleWriter : public Writer {
 public:
  SimpleWriter(std::ostream&, const SimpleWriterParameters&);

  void writeFtypBox() override;
  void writeMoovBox() override;
  void writeFreeBoxAndMdatHeader();

  void addMdatData(const std::uint8_t*, const std::size_t) override;
  std::uint64_t tellCurrentMdatOffset() override;

  void appendTrakAndUdtaBoxInfo(const std::vector<shiguredo::mp4::track::Track*>&) override;

 private:
  std::ostream& m_os;
  const box::FtypParameters m_ftyp_params;

  void setOffsetAndSize() override;
};

}  // namespace shiguredo::mp4::writer
