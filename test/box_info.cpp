#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_info.hpp"

BOOST_AUTO_TEST_SUITE(box_info)

BOOST_AUTO_TEST_CASE(box_info_tree) {
  auto moov = new shiguredo::mp4::BoxInfo({.box = new shiguredo::mp4::box::Moov()});

  new shiguredo::mp4::BoxInfo(
      {.parent = moov,
       .box = new shiguredo::mp4::box::Mvhd(
           {.creation_time = 0, .modification_time = 0, .timescale = 1000, .duration = 15000, .next_track_id = 2})});
  moov->adjustOffsetAndSize(100);
  BOOST_REQUIRE_EQUAL(R"([moov] Offset=100 Size=116
  [mvhd] Offset=108 Size=108 Version=0 Flags=0x000000 CreationTime="1904-01-01T00:00:00Z" ModificationTime="1904-01-01T00:00:00Z" Timescale=1000 Duration=15000 Rate=1.0000 Volume=256 Matrix=[0x10000, 0x0, 0x0, 0x0, 0x10000, 0x0, 0x0, 0x0, 0x40000000] PreDefined=[0, 0, 0, 0, 0, 0] NextTrackID=2)",
                      moov->toString());

  delete moov;
}

BOOST_AUTO_TEST_SUITE_END()
