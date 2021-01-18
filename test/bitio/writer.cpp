#include <cstdint>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/bitio/writer.hpp"

BOOST_AUTO_TEST_SUITE(writer)

BOOST_AUTO_TEST_CASE(writer_basic) {
  std::stringstream ss;
  shiguredo::mp4::bitio::Writer writer(ss);

  // 1101,1010
  //  ^^^ ^^^^
  writer.writeBits({0xda}, 7);

  // 0000,0111,0110,0011,1101,0101
  //         ^ ^^^^ ^^^^ ^^^^ ^^^^
  writer.writeBits({0x07, 0x63, 0xd5}, 17);

  const auto ret = writer.write({0xa4, 0x6f});
  BOOST_REQUIRE_EQUAL(2, ret);

  // 0000,0111,0110,1001,1110,0011
  //         ^ ^^^^ ^^^^ ^^^^ ^^^^
  writer.writeBits({0x07, 0x69, 0xe3}, 17);

  writer.writeBit(true);
  writer.writeBit(false);

  // 1111,0111
  //    ^ ^^^^
  writer.writeBits({0xf7}, 5);

  const std::uint8_t expected[] = {
      0xb5, 0x63, 0xd5,  // 1011,0101,0110,0011,1101,0101
      0xa4, 0x6f,        //
      0xb4, 0xf1, 0xd7,  // 1011,0100,1111,0001,1101,0111
  };

  const auto s = ss.str();
  const std::vector<std::uint8_t> actual(std::begin(s), std::end(s));
  BOOST_REQUIRE_EQUAL(8, std::size(actual));

  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + 8, actual.data(), actual.data() + std::size(actual));
}

BOOST_AUTO_TEST_SUITE_END()
