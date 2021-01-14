#include "shiguredo/mp4/box_types.hpp"

#include <string>

#include <boost/functional/factory.hpp>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_map.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4 {

void register_box_map(BoxMap* boxMap) {
  boxMap->addBoxDef(boost::factory<box::Btrt*>(), BoxType("btrt"), {});
  boxMap->addBoxDef(boost::factory<box::Co64*>(), BoxType("co64"), {0});
  boxMap->addBoxDef(boost::factory<box::Colr*>(), BoxType("colr"), {});
  boxMap->addBoxDef(boost::factory<box::Ctts*>(), BoxType("ctts"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Data*>(), BoxType("data"), {});
  boxMap->addBoxDef(boost::factory<box::Dinf*>(), BoxType("dinf"), {});
  boxMap->addBoxDef(boost::factory<box::DOps*>(), BoxType("dOps"), {});
  boxMap->addBoxDef(boost::factory<box::Dref*>(), BoxType("dref"), {0});
  boxMap->addBoxDef(boost::factory<box::Edts*>(), BoxType("edts"), {});
  boxMap->addBoxDef(boost::factory<box::Elst*>(), BoxType("elst"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Emsg*>(), BoxType("emsg"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Esds*>(), BoxType("esds"), {0});
  boxMap->addBoxDef(boost::factory<box::Free*>(), BoxType("free"), {});
  boxMap->addBoxDef(boost::factory<box::Ftyp*>(), BoxType("ftyp"), {});
  boxMap->addBoxDef(boost::factory<box::Hdlr*>(), BoxType("hdlr"), {0});
  boxMap->addBoxDef(boost::factory<box::Ilst*>(), BoxType("ilst"), {});
  boxMap->addBoxDef(boost::factory<box::Iods*>(), BoxType("iods"), {0});
  boxMap->addBoxDef(boost::factory<box::Mdat*>(), BoxType("mdat"), {});
  boxMap->addBoxDef(boost::factory<box::Mdhd*>(), BoxType("mdhd"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Mdia*>(), BoxType("mdia"), {});
  boxMap->addBoxDef(boost::factory<box::Mehd*>(), BoxType("mehd"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Meta*>(), BoxType("meta"), {0});
  boxMap->addBoxDef(boost::factory<box::Mfhd*>(), BoxType("mfhd"), {0});
  boxMap->addBoxDef(boost::factory<box::Mfra*>(), BoxType("mfra"), {});
  boxMap->addBoxDef(boost::factory<box::Mfro*>(), BoxType("mfro"), {0});
  boxMap->addBoxDef(boost::factory<box::Minf*>(), BoxType("minf"), {});
  boxMap->addBoxDef(boost::factory<box::Moof*>(), BoxType("moof"), {});
  boxMap->addBoxDef(boost::factory<box::Moov*>(), BoxType("moov"), {});
  boxMap->addBoxDef(boost::factory<box::Mvex*>(), BoxType("mvex"), {});
  boxMap->addBoxDef(boost::factory<box::Mvhd*>(), BoxType("mvhd"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Pssh*>(), BoxType("pssh"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Sbgp*>(), BoxType("sbgp"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Schi*>(), BoxType("schi"), {});
  boxMap->addBoxDef(boost::factory<box::Sdtp*>(), BoxType("sdtp"), {0});
  boxMap->addBoxDef(boost::factory<box::Sgpd*>(), BoxType("sgpd"), {1, 2});
  boxMap->addBoxDef(boost::factory<box::Sidx*>(), BoxType("sidx"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Sinf*>(), BoxType("sinf"), {});
  boxMap->addBoxDef(boost::factory<box::Skip*>(), BoxType("skip"), {});
  boxMap->addBoxDef(boost::factory<box::Smhd*>(), BoxType("smhd"), {0});
  boxMap->addBoxDef(boost::factory<box::Stbl*>(), BoxType("stbl"), {});
  boxMap->addBoxDef(boost::factory<box::Stco*>(), BoxType("stco"), {});
  boxMap->addBoxDef(boost::factory<box::Stsc*>(), BoxType("stsc"), {0});
  boxMap->addBoxDef(boost::factory<box::Stsd*>(), BoxType("stsd"), {0});
  boxMap->addBoxDef(boost::factory<box::Stss*>(), BoxType("stss"), {0});
  boxMap->addBoxDef(boost::factory<box::Stsz*>(), BoxType("stsz"), {0});
  boxMap->addBoxDef(boost::factory<box::Stts*>(), BoxType("stts"), {0});
  boxMap->addBoxDef(boost::factory<box::Styp*>(), BoxType("styp"), {});
  boxMap->addBoxDef(boost::factory<box::Tfdt*>(), BoxType("tfdt"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Tfhd*>(), BoxType("tfhd"), {0});
  boxMap->addBoxDef(boost::factory<box::Tfra*>(), BoxType("tfra"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Tkhd*>(), BoxType("tkhd"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Traf*>(), BoxType("traf"), {});
  boxMap->addBoxDef(boost::factory<box::Trak*>(), BoxType("trak"), {});
  boxMap->addBoxDef(boost::factory<box::Trex*>(), BoxType("trex"), {0});
  boxMap->addBoxDef(boost::factory<box::Trun*>(), BoxType("trun"), {0, 1});
  boxMap->addBoxDef(boost::factory<box::Udta*>(), BoxType("udta"), {});
  boxMap->addBoxDef(boost::factory<box::Vmhd*>(), BoxType("vmhd"), {0});
  boxMap->addBoxDef(boost::factory<box::Url*>(), BoxType("url "), {0});
  boxMap->addBoxDef(boost::factory<box::Urn*>(), BoxType("urn "), {0});
  boxMap->addBoxDef(boost::factory<box::Wave*>(), BoxType("wave"), {0});

  boxMap->addBoxDef(boost::factory<box::VisualSampleEntry*>(), BoxType("avc1"), {0});
  boxMap->addBoxDef(boost::factory<box::VisualSampleEntry*>(), BoxType("encv"), {0});
  boxMap->addBoxDef(boost::factory<box::VisualSampleEntry*>(), BoxType("vp08"), {0});
  boxMap->addBoxDef(boost::factory<box::VisualSampleEntry*>(), BoxType("vp09"), {0});
  boxMap->addBoxDef(boost::factory<box::VisualSampleEntry*>(), BoxType("vp10"), {0});
  boxMap->addBoxDef(boost::factory<box::VisualSampleEntry*>(), BoxType("av01"), {0});
  boxMap->addBoxDef(boost::factory<box::AudioSampleEntry*>(), BoxType("mp4a"), {0});
  boxMap->addBoxDef(boost::factory<box::AudioSampleEntry*>(), BoxType("enca"), {0});
  boxMap->addBoxDef(boost::factory<box::AudioSampleEntry*>(), BoxType("Opus"), {0});
  boxMap->addBoxDef(boost::factory<box::AVCDecoderConfiguration*>(), BoxType("avcC"), {0});
  boxMap->addBoxDef(boost::factory<box::VPCodecConfiguration*>(), BoxType("vpcC"), {1, 0});
  boxMap->addBoxDef(boost::factory<box::PixelAspectRatio*>(), BoxType("pasp"), {0});

  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("----"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("aART"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("akID"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("apID"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("atID"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("cmID"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("cnID"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("covr"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("cpil"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("cprt"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("desc"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("disk"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("egid"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("geID"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("gnre"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("pcst"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("pgap"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("plID"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("purd"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("purl"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("rtng"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("sfID"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("soaa"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("soal"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("soar"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("soco"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("sonm"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("sosn"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("stik"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("tmpo"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("trkn"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("tven"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("tves"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("tvnn"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("tvsh"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType("tvsn"), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'A', 'R', 'T'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'a', 'l', 'b'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'c', 'm', 't'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'c', 'o', 'm'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'd', 'a', 'y'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'g', 'e', 'n'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'g', 'r', 'p'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'n', 'a', 'm'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 't', 'o', 'o'}), {});
  boxMap->addBoxDef(boost::factory<box::IlstMeta*>(), BoxType(std::array<std::uint8_t, 4>{0xa9, 'w', 'r', 't'}), {});
}

}  // namespace shiguredo::mp4
