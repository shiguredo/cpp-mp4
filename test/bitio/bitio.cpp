#include <algorithm>
#include <cstdint>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/bitio/reader.hpp"

BOOST_AUTO_TEST_SUITE(marshal)

struct UnmarshalPascalStringTestCase {
  const std::string name;
  const std::vector<std::uint8_t> bin;
  const std::string str;
};

UnmarshalPascalStringTestCase unmarshal_pascal_string_test_cases[] = {
    {
        "NormalString",
        {'s', 'h', 'i', 'g', 'u', 'r', 'e', 'd', 'o', 0},
        "shiguredo",
    },
    {
        "EmptyString",
        {},
        "",
    },
    {
        "AppleQuickTimePascalStringWithEmpty",
        {0x00},
        "",
    },
    {
        "AppleQuickTimePascalString",
        {9, 's', 'h', 'i', 'g', 'u', 'r', 'e', 'd', 'o'},
        "shiguredo",
    },
    {
        "AppleQuickTimePascalStringLong",
        {' ', 'a', ' ', '1', 's', 't', ' ', 'b', 'y', 't', 'e', ' ', 'e', 'q', 'u', 'a', 'l',
         's', ' ', 't', 'o', ' ', 't', 'h', 'i', 's', ' ', 'l', 'e', 'n', 'g', 't', 'h'},
        "a 1st byte equals to this length",
    },
};

BOOST_AUTO_TEST_CASE(unmarshal_pascal_string) {
  for (const auto& tc : unmarshal_pascal_string_test_cases) {
    BOOST_TEST_MESSAGE(tc.name);
    std::stringstream ss;
    std::copy(std::begin(tc.bin), std::end(tc.bin), std::ostreambuf_iterator<char>(ss));
    shiguredo::mp4::bitio::Reader reader(ss);

    std::string s;
    shiguredo::mp4::bitio::read_pascal_string(&reader, &s, std::size(tc.bin) * 8);
    BOOST_REQUIRE_EQUAL(tc.str, s);
  }
}

BOOST_AUTO_TEST_SUITE_END()
