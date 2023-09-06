#include <cstdint>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/track/h264.hpp"

BOOST_AUTO_TEST_SUITE(h264)

struct FindNextNalUnitNormalTestCase {
  const std::vector<std::uint8_t> data;
  const std::size_t start;
  const shiguredo::mp4::track::NalUnit nal_unit;
};

FindNextNalUnitNormalTestCase find_next_nal_unit_normal_test_case[] = {
    {
        {0, 0, 0, 1, 67, 1, 2},
        0,
        shiguredo::mp4::track::NalUnit{4, 0, 7, 67},
    },
    {
        {0, 0, 0, 1, 68, 1, 2, 3},
        0,
        shiguredo::mp4::track::NalUnit{4, 0, 8, 68},
    },
    {
        {0, 0, 0, 1, 68, 1, 2, 3, 4, 0, 0, 1, 65, 1, 2, 3},
        0,
        shiguredo::mp4::track::NalUnit{4, 0, 9, 68},
    },
    {
        {0, 0, 0, 1, 68, 1, 2, 3, 4, 0, 0, 1, 65, 1, 2, 3},
        9,
        shiguredo::mp4::track::NalUnit{3, 9, 16, 65},
    },
    {
        {0, 0, 1, 61, 1, 2, 3},
        0,
        shiguredo::mp4::track::NalUnit{3, 0, 7, 61},
    },
};

BOOST_AUTO_TEST_CASE(find_next_nal_unit_normal) {
  BOOST_TEST_MESSAGE("find_next_nal_unit_normal");
  for (const auto& tc : find_next_nal_unit_normal_test_case) {
    auto nal_unit = shiguredo::mp4::track::find_next_nal_unit(tc.data.data(), tc.data.size(), tc.start);
    BOOST_REQUIRE_EQUAL(tc.nal_unit, nal_unit);
  }
}

struct FindNextNalUnitRuntimeErrorTestCase {
  const std::vector<std::uint8_t> data;
  const std::size_t start;
};

FindNextNalUnitRuntimeErrorTestCase find_next_nal_unit_runtime_error_test_case[] = {
    {
        {0, 0, 0, 1, 68, 1, 2, 3},
        2,
    },
    {
        {0, 0, 0, 1, 68, 1, 2, 3},
        3,
    },
    {
        {0, 0, 0, 1, 68, 1, 2, 3},
        4,
    },
    {
        {0, 0, 0, 1, 68, 1, 2, 3},
        10,
    },
    {
        {0, 0, 0, 1, 68, 1, 2, 3, 4, 0, 0, 1, 65, 1, 2, 3},
        8,
    },
    {
        {0, 0, 0, 1, 68, 1, 2, 3, 4, 0, 0, 1, 65, 1, 2, 3},
        10,
    },
    {
        {0, 0, 1, 61, 1, 2, 3},
        1,
    },
};

BOOST_AUTO_TEST_CASE(find_next_nal_unit_runtime_error) {
  BOOST_TEST_MESSAGE("find_next_nal_unit_runtime_error");
  for (const auto& tc : find_next_nal_unit_runtime_error_test_case) {
    BOOST_REQUIRE_THROW(shiguredo::mp4::track::find_next_nal_unit(tc.data.data(), tc.data.size(), tc.start),
                        std::runtime_error);
  }
}

BOOST_AUTO_TEST_SUITE_END()
