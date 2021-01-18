#include <array>
#include <cstdint>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/endian/endian.hpp"

BOOST_AUTO_TEST_SUITE(endian)

struct Uint32TestCase {
  const std::uint32_t number;
  const std::array<std::uint8_t, 4> array;
};

Uint32TestCase uint32_test_cases[] = {
    {0, {0x00, 0x00, 0x00, 0x00}},
    {0x12345678, {0x12, 0x34, 0x56, 0x78}},
    {0x123456, {0x00, 0x12, 0x34, 0x56}},
    {0xffffffff, {0xff, 0xff, 0xff, 0xff}},
};

BOOST_AUTO_TEST_CASE(uint32) {
  BOOST_TEST_MESSAGE("uint32");
  for (const auto& tc : uint32_test_cases) {
    const auto array = shiguredo::mp4::endian::uint32_to_be(tc.number);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(std::begin(tc.array), std::end(tc.array), std::begin(array), std::end(array));
    const auto number = shiguredo::mp4::endian::be_to_uint32(array);
    BOOST_REQUIRE_EQUAL(tc.number, number);
    const auto number2 = shiguredo::mp4::endian::be_to_uint32(array[0], array[1], array[2], array[3]);
    BOOST_REQUIRE_EQUAL(tc.number, number2);
  }
}

struct Uint64TestCase {
  const std::uint64_t number;
  const std::array<std::uint8_t, 8> array;
};

Uint64TestCase uint64_test_cases[] = {
    {0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x123456789abcdef0, {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0}},
    {0x123456789abcdef, {0x1, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}},
    {0x123456789abcde, {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde}},
    {0xffffffffffffffff, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
};

BOOST_AUTO_TEST_CASE(uint64) {
  BOOST_TEST_MESSAGE("uint64");
  for (const auto& tc : uint64_test_cases) {
    const auto array = shiguredo::mp4::endian::uint64_to_be(tc.number);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(std::begin(tc.array), std::end(tc.array), std::begin(array), std::end(array));
    const auto number = shiguredo::mp4::endian::be_to_uint64(array);
    BOOST_REQUIRE_EQUAL(tc.number, number);
    const auto number2 = shiguredo::mp4::endian::be_to_uint64(array[0], array[1], array[2], array[3], array[4],
                                                              array[5], array[6], array[7]);
    BOOST_REQUIRE_EQUAL(tc.number, number2);
  }
}

BOOST_AUTO_TEST_SUITE_END()
