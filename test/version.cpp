#include <string>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/version.hpp"

BOOST_AUTO_TEST_SUITE(version)

BOOST_AUTO_TEST_CASE(version_string) {
  BOOST_REQUIRE_EQUAL("2022.1.0", shiguredo::mp4::get_version_string());
}

BOOST_AUTO_TEST_SUITE_END()
