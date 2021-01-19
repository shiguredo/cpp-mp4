#include <cstdint>
#include <string>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/time/time.hpp"

BOOST_AUTO_TEST_SUITE(mp4_time)

struct TimeTestCase {
  const std::uint64_t from_epoch_19700101;
  const std::uint64_t from_epoch_19040101;
  const std::string formated;
};

TimeTestCase time_test_cases[] = {
    {
        0,
        2082844800,
        "1970-01-01T00:00:00Z",
    },
    {
        0x7fffffff,
        4230328447,
        "2038-01-19T03:14:07Z",
    },
    {
        0xffffffff,
        6377812095,
        "2106-02-07T06:28:15Z",
    },
    {
        0x100000000,
        6377812096,
        "0000-00-00T00:00:00Z",
    },
};

BOOST_AUTO_TEST_CASE(epoch) {
  BOOST_TEST_MESSAGE("epoch");
  for (const auto& tc : time_test_cases) {
    const auto from_epoch_19040101 = shiguredo::mp4::time::convert_to_epoch_19040101(tc.from_epoch_19700101);
    BOOST_REQUIRE_EQUAL(tc.from_epoch_19040101, from_epoch_19040101);
    BOOST_REQUIRE_EQUAL(tc.formated, shiguredo::mp4::time::format_epoch_19040101(from_epoch_19040101));
  }
}

BOOST_AUTO_TEST_SUITE_END()
