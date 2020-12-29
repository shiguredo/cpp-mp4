#include <array>
#include <cstdint>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/box_type.hpp"

BOOST_AUTO_TEST_SUITE(box_type)

BOOST_AUTO_TEST_CASE(box_type_to_string) {
  BOOST_REQUIRE_EQUAL("1234", shiguredo::mp4::BoxType(std::array<std::uint8_t, 4>{'1', '2', '3', '4'}).toString());
  BOOST_REQUIRE_EQUAL("abcd", shiguredo::mp4::BoxType(std::array<std::uint8_t, 4>{'a', 'b', 'c', 'd'}).toString());
  BOOST_REQUIRE_EQUAL("xx x", shiguredo::mp4::BoxType(std::array<std::uint8_t, 4>{'x', 'x', ' ', 'x'}).toString());
  BOOST_REQUIRE_EQUAL("xx-x", shiguredo::mp4::BoxType(std::array<std::uint8_t, 4>{'x', 'x', '-', 'x'}).toString());
  BOOST_REQUIRE_EQUAL("xx©x", shiguredo::mp4::BoxType(std::array<std::uint8_t, 4>{'x', 'x', 0xa9, 'x'}).toString());
  BOOST_REQUIRE_EQUAL("0x7878ab78",
                      shiguredo::mp4::BoxType(std::array<std::uint8_t, 4>{'x', 'x', 0xab, 'x'}).toString());
}

BOOST_AUTO_TEST_SUITE_END()
