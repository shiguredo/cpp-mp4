#include <algorithm>
#include <cstdint>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/bitio/reader.hpp"

BOOST_AUTO_TEST_SUITE(reader)

BOOST_AUTO_TEST_CASE(reader_basic) {
  std::stringstream ss;
  shiguredo::mp4::bitio::Reader reader(ss);

  std::uint8_t buf[] = {
      0xb5, 0x63, 0xd5,  // 1011,0101,0110,0011,1101,0101
      0xa4, 0x6f,        //
      0xb4, 0xf1, 0xd7,  // 1011,0100,1111,0001,1101,0111
  };

  ss << buf;

  std::vector<std::uint8_t> data;
  // 0101,1010
  reader.readBits(&data, 7);
  BOOST_REQUIRE_EQUAL(1, std::size(data));
  std::uint8_t expected1[] = {0x5a};
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected1, expected1 + 1, data.data(), data.data() + std::size(data));

  reader.readBits(&data, 17);
  BOOST_REQUIRE_EQUAL(3, std::size(data));
  std::uint8_t expected2[] = {0x01, 0x63, 0xd5};
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected2, expected2 + 3, data.data(), data.data() + std::size(data));

  data.resize(2);
  auto size = reader.read(&data);
  BOOST_REQUIRE_EQUAL(2, size);
  std::uint8_t expected3[] = {0xa4, 0x6f};

  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected3, expected3 + 2, data.data(), data.data() + std::size(data));

  reader.readBits(&data, 17);
  BOOST_REQUIRE_EQUAL(3, std::size(data));
  std::uint8_t expected4[] = {0x01, 0x69, 0xe3};
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected4, expected4 + 3, data.data(), data.data() + std::size(data));

  auto bit1 = reader.readBit();
  BOOST_REQUIRE(bit1);
  auto bit2 = reader.readBit();
  BOOST_REQUIRE(!bit2);

  reader.readBits(&data, 5);
  BOOST_REQUIRE_EQUAL(1, std::size(data));
  std::uint8_t expected5[] = {0x17};
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected5, expected5 + 1, data.data(), data.data() + std::size(data));
}

struct ReadBitsTestCase {
  std::string name;
  std::uint8_t octet;
  std::uint8_t width;
  std::vector<std::uint8_t> input;
  std::size_t size;
  bool throw_exception;
  std::vector<std::uint8_t> expected;
};
ReadBitsTestCase read_bits_test_cases[] = {
    {"no width", 0, 0, {0x6c, 0xa5}, 10, false, {0x01, 0xb2}},
    {"width 3", 0x6c, 3, {0xa5}, 10, false, {0x02, 0x52}},
    {"reach to end of box", 0, 0, {0x6c, 0xa5}, 16, false, {0x6c, 0xa5}},
    {"overrun", 0, 0, {0x6c, 0xa5}, 17, true, {0x6c, 0xa5}},
};

BOOST_AUTO_TEST_CASE(reader_read_bits) {
  for (const auto& tc : read_bits_test_cases) {
    std::stringstream ss;
    shiguredo::mp4::bitio::Reader reader(ss);

    std::copy(std::begin(tc.input), std::end(tc.input), std::ostreambuf_iterator<char>(ss));
    reader.setOctet(tc.octet);
    reader.setWidth(tc.width);
    std::vector<std::uint8_t> data;
    if (tc.throw_exception) {
      BOOST_REQUIRE_THROW(reader.readBits(&data, tc.size), std::runtime_error);
      continue;
    }
    BOOST_REQUIRE_NO_THROW(reader.readBits(&data, tc.size));
    BOOST_REQUIRE_EQUAL(std::size(tc.expected), std::size(data));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(std::begin(tc.expected), std::end(tc.expected), data.data(),
                                    data.data() + std::size(data));
  }
}

BOOST_AUTO_TEST_CASE(reader_read_bit) {
  std::stringstream ss;
  shiguredo::mp4::bitio::Reader reader(ss);
  ss.put(0x6c);
  ss.put(static_cast<char>(0xa5));

  std::pair<bool, std::uint8_t> outputs[] = {
      {false, 0x6c}, {true, 0x6c},  {true, 0x6c},  {false, 0x6c}, {true, 0x6c}, {true, 0x6c},
      {false, 0x6c}, {false, 0x6c}, {true, 0xa5},  {false, 0xa5}, {true, 0xa5}, {false, 0xa5},
      {false, 0xa5}, {true, 0xa5},  {false, 0xa5}, {true, 0xa5},
  };

  for (const auto& o : outputs) {
    auto bit = reader.readBit();
    BOOST_REQUIRE_EQUAL(o.first, bit);
    BOOST_REQUIRE_EQUAL(o.second, reader.getOctet());
  }
}

BOOST_AUTO_TEST_CASE(reader_read_invalid_argument) {
  std::stringstream ss;
  shiguredo::mp4::bitio::Reader reader(ss);
  std::uint8_t buf[] = {
      0x6c, 0x82, 0x41, 0x35, 0x71, 0xa4, 0xcd, 0x9f,
  };
  ss << buf;

  std::vector<std::uint8_t> data;
  data.resize(2);
  BOOST_REQUIRE_NO_THROW(reader.read(&data));
  BOOST_REQUIRE_NO_THROW(reader.readBits(&data, 3));
  BOOST_REQUIRE_THROW(reader.read(&data), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(reader_seek_invalid_argument) {
  std::stringstream ss;
  shiguredo::mp4::bitio::Reader reader(ss);
  std::uint8_t buf[] = {
      0x6c, 0x82, 0x41, 0x35, 0x71, 0xa4, 0xcd, 0x9f,
  };
  ss << buf;

  std::vector<std::uint8_t> data;
  BOOST_REQUIRE_NO_THROW(reader.seek(2, std::ios_base::cur));
  BOOST_REQUIRE_NO_THROW(reader.readBits(&data, 3));
  BOOST_REQUIRE_THROW(reader.seek(2, std::ios_base::cur), std::invalid_argument);
  BOOST_REQUIRE_NO_THROW(reader.seek(0, std::ios_base::beg));
  BOOST_REQUIRE_NO_THROW(reader.readBits(&data, 3));

  std::uint8_t expected[] = {0x03};
  BOOST_REQUIRE_EQUAL(1, std::size(data));
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + 1, data.data(), data.data() + std::size(data));
}

BOOST_AUTO_TEST_SUITE_END()
