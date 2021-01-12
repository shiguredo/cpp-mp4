#include <cstdint>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/box/co64.hpp"
#include "shiguredo/mp4/box/stco.hpp"
#include "shiguredo/mp4/box/stsc.hpp"
#include "shiguredo/mp4/box/stts.hpp"
#include "shiguredo/mp4/track/track.hpp"

BOOST_AUTO_TEST_SUITE(track)

struct MakeSttsEntriesTestCase {
  std::vector<std::uint32_t> sample_duraitons;
  std::vector<shiguredo::mp4::box::SttsEntry> entries;
};

MakeSttsEntriesTestCase make_stts_entries_test_cases[] = {
    {
        {10, 10, 10},
        {shiguredo::mp4::box::SttsEntry({.sample_count = 3, .sample_duration = 10})},
    },
    {
        {10, 5, 10},
        {shiguredo::mp4::box::SttsEntry({.sample_count = 1, .sample_duration = 10}),
         shiguredo::mp4::box::SttsEntry({.sample_count = 1, .sample_duration = 5}),
         shiguredo::mp4::box::SttsEntry({.sample_count = 1, .sample_duration = 10})},
    },
    {
        {10, 10, 5},
        {
            shiguredo::mp4::box::SttsEntry({.sample_count = 2, .sample_duration = 10}),
            shiguredo::mp4::box::SttsEntry({.sample_count = 1, .sample_duration = 5}),
        },
    },
    {
        {},
        {},
    },
};

BOOST_AUTO_TEST_CASE(make_stts_entries) {
  BOOST_TEST_MESSAGE("make_stts_entries");
  for (const auto& tc : make_stts_entries_test_cases) {
    std::vector<shiguredo::mp4::box::SttsEntry> entries;
    shiguredo::mp4::track::make_stts_entries(&entries, tc.sample_duraitons);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(std::begin(tc.entries), std::end(tc.entries), std::begin(entries),
                                    std::end(entries));
  }
}

struct MakeStscEntriesTestCase {
  std::vector<shiguredo::mp4::track::ChunkInfo> chunk_offsets;
  std::vector<shiguredo::mp4::box::StscEntry> entries;
};

MakeStscEntriesTestCase make_stsc_entries_test_cases[] = {
    {{
         shiguredo::mp4::track::ChunkInfo{.number_of_samples = 10},
         shiguredo::mp4::track::ChunkInfo{.number_of_samples = 10},
     },
     {
         shiguredo::mp4::box::StscEntry({.first_chunk = 1, .samples_per_chunk = 10}),
     }},
    {{
         shiguredo::mp4::track::ChunkInfo{.number_of_samples = 10},
         shiguredo::mp4::track::ChunkInfo{.number_of_samples = 5},
         shiguredo::mp4::track::ChunkInfo{.number_of_samples = 10},
     },
     {
         shiguredo::mp4::box::StscEntry({.first_chunk = 1, .samples_per_chunk = 10}),
         shiguredo::mp4::box::StscEntry({.first_chunk = 2, .samples_per_chunk = 5}),
         shiguredo::mp4::box::StscEntry({.first_chunk = 3, .samples_per_chunk = 10}),
     }},
    {{
         shiguredo::mp4::track::ChunkInfo{.number_of_samples = 10},
         shiguredo::mp4::track::ChunkInfo{.number_of_samples = 10},
         shiguredo::mp4::track::ChunkInfo{.number_of_samples = 5},
     },
     {
         shiguredo::mp4::box::StscEntry({.first_chunk = 1, .samples_per_chunk = 10}),
         shiguredo::mp4::box::StscEntry({.first_chunk = 3, .samples_per_chunk = 5}),
     }},
    {
        {},
        {},
    },
};

BOOST_AUTO_TEST_CASE(make_stsc_entries) {
  BOOST_TEST_MESSAGE("make_stsc_entries");
  for (const auto& tc : make_stsc_entries_test_cases) {
    std::vector<shiguredo::mp4::box::StscEntry> entries;
    shiguredo::mp4::track::make_stsc_entries(&entries, tc.chunk_offsets);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(std::begin(tc.entries), std::end(tc.entries), std::begin(entries),
                                    std::end(entries));
  }
}

struct MakeOffsetBoxTestCase {
  std::vector<shiguredo::mp4::track::ChunkInfo> chunk_offsets;
  bool use_co64;
  shiguredo::mp4::Box* box;
};

MakeOffsetBoxTestCase make_offset_box_test_cases[] = {
    {
        {},
        false,
        new shiguredo::mp4::box::Stco({.chunk_offsets = {}}),
    },
    {
        {
            shiguredo::mp4::track::ChunkInfo{.offset = 10},
            shiguredo::mp4::track::ChunkInfo{.offset = 15},
            shiguredo::mp4::track::ChunkInfo{.offset = 20},
        },
        false,
        new shiguredo::mp4::box::Stco({.chunk_offsets = {10, 15, 20}}),
    },
    {
        {
            shiguredo::mp4::track::ChunkInfo{.offset = 10},
            shiguredo::mp4::track::ChunkInfo{.offset = 15},
            shiguredo::mp4::track::ChunkInfo{.offset = 0xffffffff},
        },
        false,
        new shiguredo::mp4::box::Stco({.chunk_offsets = {10, 15, 0xffffffff}}),
    },
    {
        {
            shiguredo::mp4::track::ChunkInfo{.offset = 10},
            shiguredo::mp4::track::ChunkInfo{.offset = 15},
            shiguredo::mp4::track::ChunkInfo{.offset = 0x100000000},
        },
        true,
        new shiguredo::mp4::box::Co64({.chunk_offsets = {10, 15, 0x100000000}}),
    },
};

BOOST_AUTO_TEST_CASE(make_offset_box) {
  BOOST_TEST_MESSAGE("make_offset_box");
  for (const auto& tc : make_offset_box_test_cases) {
    auto box = shiguredo::mp4::track::make_offset_box(tc.chunk_offsets);
    if (tc.use_co64) {
      shiguredo::mp4::box::Co64* expected = dynamic_cast<shiguredo::mp4::box::Co64*>(tc.box);
      shiguredo::mp4::box::Co64* actual = dynamic_cast<shiguredo::mp4::box::Co64*>(box);
      BOOST_REQUIRE(expected != nullptr);
      BOOST_REQUIRE(actual != nullptr);
      BOOST_REQUIRE_EQUAL(*expected, *actual);
    } else {
      shiguredo::mp4::box::Stco* expected = dynamic_cast<shiguredo::mp4::box::Stco*>(tc.box);
      shiguredo::mp4::box::Stco* actual = dynamic_cast<shiguredo::mp4::box::Stco*>(box);
      BOOST_REQUIRE(expected != nullptr);
      BOOST_REQUIRE(actual != nullptr);
      BOOST_REQUIRE_EQUAL(*expected, *actual);
    }
    delete tc.box;
    delete box;
  }
}

BOOST_AUTO_TEST_SUITE_END()
