#include <algorithm>
#include <cstdint>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/box_header.hpp"

BOOST_AUTO_TEST_SUITE(box_header)

struct BoxHeaderConstructorTestCase {
  std::string name;
  std::vector<std::uint8_t> buf;
  std::streamoff seek;
  bool throw_exception;
  shiguredo::mp4::BoxHeader expected;
};

BoxHeaderConstructorTestCase box_header_constructor_test_cases[] = {
    {"no offset",
     {0x00, 0x01, 0x23, 0x45, 't', 'e', 's', 't'},
     0,
     false,
     shiguredo::mp4::BoxHeader(
         {.offset = 0, .size = 0x12345, .header_size = 8, .type = shiguredo::mp4::BoxType("test")})},
    {"has offset",
     {0x00, 0x00, 0x00, 0x01, 0x23, 0x45, 't', 'e', 's', 't'},
     2,
     false,
     shiguredo::mp4::BoxHeader(
         {.offset = 2, .size = 0x12345, .header_size = 8, .type = shiguredo::mp4::BoxType("test")})},
    {"large-size",
     {0x00, 0x00, 0x00, 0x01, 't', 'e', 's', 't', 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef},
     0,
     false,
     shiguredo::mp4::BoxHeader(
         {.offset = 0, .size = 0x123456789abcdef, .header_size = 16, .type = shiguredo::mp4::BoxType("test")})},
    {"extend to eof",
     {
         0x00, 0x00, 0x00, 0x00, 't',  'e',  's',  't',  0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     },
     0,
     false,
     shiguredo::mp4::BoxHeader(
         {.offset = 0, .size = 20, .header_size = 8, .type = shiguredo::mp4::BoxType("test"), .extend_to_eof = true})},
    {"end-of-file",
     {
         0x00,
         0x01,
         0x23,
         0x45,
         't',
         'e',
         's',
     },
     0,
     true,
     shiguredo::mp4::BoxHeader({.offset = 0, .size = 0, .header_size = 0, .type = shiguredo::mp4::BoxType("test")})},
};

BOOST_AUTO_TEST_CASE(box_header_constructor) {
  for (const auto& tc : box_header_constructor_test_cases) {
    BOOST_TEST_MESSAGE(tc.name);
    std::stringstream ss;
    std::copy(std::begin(tc.buf), std::end(tc.buf), std::ostreambuf_iterator<char>(ss));
    ss.seekg(tc.seek, std::ios_base::beg);

    if (tc.throw_exception) {
      BOOST_REQUIRE_THROW(shiguredo::mp4::read_box_header(ss), std::invalid_argument);
      continue;
    }
    shiguredo::mp4::BoxHeader* bi = shiguredo::mp4::read_box_header(ss);
    BOOST_REQUIRE(tc.expected == *bi);
  }
}

struct BoxHeaderWriteTestCase {
  std::string name;
  std::vector<std::uint8_t> pre;
  shiguredo::mp4::BoxHeader bi;
  shiguredo::mp4::BoxHeader expected_bi;
  std::vector<std::uint8_t> expected_bytes;
};

BoxHeaderWriteTestCase box_header_write_test_cases[] = {
    {"small-size",
     {},
     shiguredo::mp4::BoxHeader(
         {.offset = 0, .size = 0x12345, .header_size = 8, .type = shiguredo::mp4::BoxType("test")}),
     shiguredo::mp4::BoxHeader(
         {.offset = 0, .size = 0x12345, .header_size = 8, .type = shiguredo::mp4::BoxType("test")}),
     {0x00, 0x01, 0x23, 0x45, 't', 'e', 's', 't'}},
    {"large-size",
     {},
     shiguredo::mp4::BoxHeader(
         {.offset = 0, .size = 0x123456789abc, .header_size = 8, .type = shiguredo::mp4::BoxType("test")}),
     shiguredo::mp4::BoxHeader(
         {.offset = 0, .size = 0x123456789abc + 8, .header_size = 16, .type = shiguredo::mp4::BoxType("test")}),
     {0x00, 0x00, 0x00, 0x01, 't', 'e', 's', 't', 0x00, 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc}},
    {"extend to eof",
     {},
     shiguredo::mp4::BoxHeader({.offset = 0,
                                .size = 0x123,
                                .header_size = 8,
                                .type = shiguredo::mp4::BoxType("test"),
                                .extend_to_eof = true}),
     shiguredo::mp4::BoxHeader({.offset = 0,
                                .size = 0x123,
                                .header_size = 8,
                                .type = shiguredo::mp4::BoxType("test"),
                                .extend_to_eof = true}),
     {0x00, 0x00, 0x00, 0x00, 't', 'e', 's', 't'}},
    {"with offset",
     {0x00, 0x00, 0x00},
     shiguredo::mp4::BoxHeader(
         {.offset = 3, .size = 0x12345, .header_size = 8, .type = shiguredo::mp4::BoxType("test")}),
     shiguredo::mp4::BoxHeader(
         {.offset = 3, .size = 0x12345, .header_size = 8, .type = shiguredo::mp4::BoxType("test")}),
     {0x00, 0x00, 0x00,  // pre inserted
      0x00, 0x01, 0x23, 0x45, 't', 'e', 's', 't'}},
};

BOOST_AUTO_TEST_CASE(box_header_write) {
  for (auto& tc : box_header_write_test_cases) {
    BOOST_TEST_MESSAGE(tc.name);
    std::stringstream ss;
    std::copy(std::begin(tc.pre), std::end(tc.pre), std::ostreambuf_iterator<char>(ss));
    tc.bi.write(ss);
    BOOST_REQUIRE(tc.expected_bi == tc.bi);
    auto s = ss.str();
    std::vector<std::uint8_t> actual_bytes(std::begin(s), std::end(s));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(std::begin(tc.expected_bytes), std::end(tc.expected_bytes), actual_bytes.data(),
                                    actual_bytes.data() + std::size(actual_bytes));
  }
}

BOOST_AUTO_TEST_SUITE_END()
