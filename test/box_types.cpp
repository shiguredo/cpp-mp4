#include <cstdint>
#include <sstream>
#include <string>

#include <boost/test/unit_test.hpp>

#include "shiguredo/mp4/box/boxes.hpp"
#include "shiguredo/mp4/box_map.hpp"
#include "shiguredo/mp4/box_type.hpp"
#include "shiguredo/mp4/box_types.hpp"

BOOST_AUTO_TEST_SUITE(box)

struct BoxTypesTestCase {
  std::string name;
  shiguredo::mp4::Box* src;
  shiguredo::mp4::Box* dst;
  std::vector<std::uint8_t> bin;
  std::string str;
};

BoxTypesTestCase box_test_cases[] = {
    {"co64",
     new shiguredo::mp4::box::Co64({.chunk_offsets = {0x0123456789abcdef, 0x89abcdef01234567}}),
     new shiguredo::mp4::box::Co64(),
     {
         0,                                               // version
         0x00, 0x00, 0x00,                                // flags
         0x00, 0x00, 0x00, 0x02,                          // entry count
         0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,  // chunk offset
         0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67,  // chunk offset
     },
     "Version=0 Flags=0x000000 EntryCount=2 "
     "ChunkOffsets=[0x123456789abcdef, 0x89abcdef01234567]"},
    {"colr: nclx",
     new shiguredo::mp4::box::Colr({.colour_type = {'n', 'c', 'l', 'x'},
                                    .colour_primaries = 0x0123,
                                    .transfer_characteristics = 0x2345,
                                    .matrix_coefficents = 0x4567,
                                    .full_range_flag = true}),
     new shiguredo::mp4::box::Colr(),
     {
         'n', 'c', 'l', 'x', 0x01, 0x23,  // ColourPrimaries
         0x23, 0x45,                      // TransferCharacteristics
         0x45, 0x67,                      // MatrixCoefficients
         0x80,                            // FullRangeFlag, Reserved
     },
     R"(ColourType="nclx" ColourPrimaries=291 TransferCharacteristics=9029 )"
     "MatrixCoefficients=17767 FullRangeFlag=true"},
    {
        "colr: rICC",
        new shiguredo::mp4::box::Colr({.colour_type = {'r', 'I', 'C', 'C'}, .profile = {0x12, 0x34, 0x56, 0x78, 0xab}}),
        new shiguredo::mp4::box::Colr(),
        {
            'r',
            'I',
            'C',
            'C',
            0x12,
            0x34,
            0x56,
            0x78,
            0xab,
        },
        R"(ColourType="rICC" Profile=[0x12, 0x34, 0x56, 0x78, 0xab])",
    },
    {
        "colr: nclc",
        new shiguredo::mp4::box::Colr({.colour_type = {'n', 'c', 'l', 'c'}, .unknown = {0x01, 0x23, 0x45}}),
        new shiguredo::mp4::box::Colr(),
        {
            'n',
            'c',
            'l',
            'c',
            0x01,
            0x23,
            0x45,
        },
        R"(ColourType="nclc" Unknown=[0x1, 0x23, 0x45])",
    },
    {
        "ctts: version 0",
        new shiguredo::mp4::box::Ctts({.entries =
                                           {
                                               {0x01234567, 0x12345678},
                                               {0x89abcdef, 0x789abcde},
                                           }}),
        new shiguredo::mp4::box::Ctts(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x01, 0x23, 0x45, 0x67,  // sample count
            0x12, 0x34, 0x56, 0x78,  // sample offset
            0x89, 0xab, 0xcd, 0xef,  // sample count
            0x78, 0x9a, 0xbc, 0xde,  // sample offset
        },
        "Version=0 Flags=0x000000 EntryCount=2 "
        "Entries=[{SampleCount=19088743 "
        "SampleOffset=305419896}, {SampleCount=2309737967 "
        "SampleOffset=2023406814}]",
    },
    {
        "ctts: version 1",
        new shiguredo::mp4::box::Ctts({.version = 1,
                                       .entries =
                                           {
                                               {0x01234567, 0x12345678},
                                               {0x89abcdef, -0x789abcde},
                                           }}),
        new shiguredo::mp4::box::Ctts(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x01, 0x23, 0x45, 0x67,  // sample count
            0x12, 0x34, 0x56, 0x78,  // sample offset
            0x89, 0xab, 0xcd, 0xef,  // sample count
            0x87, 0x65, 0x43, 0x22,  // sample offset
        },
        "Version=1 Flags=0x000000 EntryCount=2 "
        "Entries=[{SampleCount=19088743 "
        "SampleOffset=305419896}, {SampleCount=2309737967 "
        "SampleOffset=-2023406814}]",
    },
    {
        "dinf",
        new shiguredo::mp4::box::Dinf(),
        new shiguredo::mp4::box::Dinf(),
        {},
        "",
    },
    {
        "dref",
        new shiguredo::mp4::box::Dref({.entry_count = 0x12345678}),
        new shiguredo::mp4::box::Dref(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x12, 0x34, 0x56, 0x78,  // entry count
        },
        "Version=0 Flags=0x000000 EntryCount=305419896",
    },
    {
        "url no location",
        new shiguredo::mp4::box::Url(),
        new shiguredo::mp4::box::Url(),
        {
            0,                 // version
            0x00, 0x00, 0x01,  // flags
        },
        "Version=0 Flags=0x000001",
    },
    {
        "url location",
        new shiguredo::mp4::box::Url({.location = "tst"}),
        new shiguredo::mp4::box::Url(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x74, 0x73, 0x74, 0x00,  // location
        },
        R"(Version=0 Flags=0x000000 Location="tst")",
    },
    {
        "urn no name/location",
        new shiguredo::mp4::box::Urn(),
        new shiguredo::mp4::box::Urn(),
        {
            0,                 // version
            0x00, 0x00, 0x01,  // flags
        },
        "Version=0 Flags=0x000001",
    },
    {
        "urn name/location",
        new shiguredo::mp4::box::Urn({.name = "111", .location = "tst"}),
        new shiguredo::mp4::box::Urn(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x31, 0x31, 0x31, 0x00,  // name
            0x74, 0x73, 0x74, 0x00,  // location
        },
        R"(Version=0 Flags=0x000000 Name="111" Location="tst")",
    },
    {
        "edts",
        new shiguredo::mp4::box::Edts{},
        new shiguredo::mp4::box::Edts{},
        {},
        "",
    },
    {"elst: version 0",
     new shiguredo::mp4::box::Elst(
         {.entries =
              {
                  shiguredo::mp4::box::ElstEntry(
                      {.track_duration = 0x0100000a, .media_time = 0x0100000b, .media_rate = 1 << 16}),
                  shiguredo::mp4::box::ElstEntry(
                      {.track_duration = 0x0200000a, .media_time = 0x0200000b, .media_rate = 2 << 16}),
              }}),
     new shiguredo::mp4::box::Elst(),
     {
         0,                       // version
         0x00, 0x00, 0x00,        // flags
         0x00, 0x00, 0x00, 0x02,  // entry count
         0x01, 0x00, 0x00, 0x0a,  // segment duration v0
         0x01, 0x00, 0x00, 0x0b,  // media time v0
         0x00, 0x01, 0x00, 0x00,  // media rate
         0x02, 0x00, 0x00, 0x0a,  // segment duration v0
         0x02, 0x00, 0x00, 0x0b,  // media time v0
         0x00, 0x02, 0x00, 0x00,  // media rate
     },
     "Version=0 Flags=0x000000 EntryCount=2 Entries=["
     "{TrackDuration=16777226 MediaTime=16777227 "
     "MediaRate=1.0000}, {TrackDuration=33554442 MediaTime="
     "33554443 MediaRate=2.0000}]"},
    {
        "elst: version 1",
        new shiguredo::mp4::box::Elst({.version = 1,
                                       .entries = {shiguredo::mp4::box::ElstEntry({
                                                       .track_duration = 0x010000000000000a,
                                                       .media_time = 0x010000000000000b,
                                                       .media_rate = 1 << 16,
                                                   }),
                                                   shiguredo::mp4::box::ElstEntry({
                                                       .track_duration = 0x020000000000000a,
                                                       .media_time = 0x020000000000000b,
                                                       .media_rate = 3 << 16,
                                                   })}}),
        new shiguredo::mp4::box::Elst(),
        {
            1,                                               // version
            0x00, 0x00, 0x00,                                // flags
            0x00, 0x00, 0x00, 0x02,                          // entry count
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a,  // segment duration v1
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b,  // media time v1
            0x00, 0x01, 0x00, 0x00,                          // media rate
            0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a,  // segment duration v1
            0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b,  // media time v1
            0x00, 0x03, 0x00, 0x00                           // media rate
        },
        "Version=1 Flags=0x000000 EntryCount=2 "
        "Entries=[{TrackDuration=72057594037927946 "
        "MediaTime=72057594037927947 MediaRate=1.0000}, "
        "{TrackDuration=144115188075855882 "
        "MediaTime=144115188075855883 "
        "MediaRate=3.0000}]",
    },
    {
        "emsg: version 0",
        new shiguredo::mp4::box::Emsg({.scheme_uri = "urn:test",
                                       .value = "foo",
                                       .timescale = 1000,
                                       .presentation_time = 123,
                                       .event_duration = 3000,
                                       .id = 0xabcd,
                                       .message_data = {'s', 'h', 'i', 'g', 'u', 'r', 'e', 'd', 'o'}}),
        new shiguredo::mp4::box::Emsg(),
        {
            0,                                                     // version
            0x00, 0x00, 0x00,                                      // flags
            0x75, 0x72, 0x6e, 0x3a, 0x74, 0x65, 0x73, 0x74, 0x00,  // scheme id uri
            0x66, 0x6f, 0x6f, 0x00,                                // value
            0x00, 0x00, 0x03, 0xe8,                                // timescale
            0x00, 0x00, 0x00, 0x7b,                                // presentation time delta
            0x00, 0x00, 0x0b, 0xb8,                                // event duration
            0x00, 0x00, 0xab, 0xcd,                                // id
            0x73, 0x68, 0x69, 0x67, 0x75, 0x72, 0x65, 0x64, 0x6f   // message data
        },
        R"(Version=0 Flags=0x000000 SchemeIdUri="urn:test" Value="foo" Timescale=1000 PresentationTimeDelta=123 EventDuration=3000 Id=43981 MessageData="shiguredo")",
    },
    {
        "emsg: version 1",
        new shiguredo::mp4::box::Emsg({.version = 1,
                                       .scheme_uri = "urn:test",
                                       .value = "foo",
                                       .timescale = 1000,
                                       .presentation_time = 123,
                                       .event_duration = 3000,
                                       .id = 0xabcd,
                                       .message_data = {'s', 'h', 'i', 'g', 'u', 'r', 'e', 'd', 'o'}}),
        new shiguredo::mp4::box::Emsg(),
        {
            1,                                               // version
            0x00, 0x00, 0x00,                                // flags
            0x00, 0x00, 0x03, 0xe8,                          // timescale
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7b,  // presentation time
            0x00, 0x00, 0x0b, 0xb8,                          // event duration
            0x00, 0x00, 0xab, 0xcd,                          // id
            0x75, 0x72, 0x6e, 0x3a, 0x74, 0x65, 0x73, 0x74,
            0x00,                                                 // scheme id uri
            0x66, 0x6f, 0x6f, 0x00,                               // value
            0x73, 0x68, 0x69, 0x67, 0x75, 0x72, 0x65, 0x64, 0x6f  // message data
        },
        R"(Version=1 Flags=0x000000 SchemeIdUri="urn:test" Value="foo" Timescale=1000 PresentationTime=123 EventDuration=3000 Id=43981 MessageData="shiguredo")",
    },
    {
        "esds",
        new shiguredo::mp4::box::Esds(
            {.descriptors =
                 {
                     std::shared_ptr<shiguredo::mp4::box::Descriptor>(
                         new shiguredo::mp4::box::ESDescriptor(
                             {.sub_descriptors_size = 27,
                              .ESID = 0x1234,
                              .stream_dependence_flag =
                                  false,
                              .url_flag = true,
                              .ocr_stream_flag = false,
                              .stream_priority = 0x03,
                              .depends_on_esid = 0,
                              .url_string = {'h', 't', 't', 'p', ':', '/', '/', 'h', 'o', 'g', 'e'},
                              .OCRESID = 0})),
                     std::
                         shared_ptr<shiguredo::mp4::box::Descriptor>(
                             new shiguredo::mp4::box::DecoderConfigDescriptor({.object_type_indication = 0x12,
                                                                               .stream_type = 0x15,
                                                                               .upstream = true,
                                                                               .reserved = false,
                                                                               .buffer_size_db = 0x123456,
                                                                               .max_bitrate = 0x12345678,
                                                                               .avg_bitrate = 0x23456789})),
                     std::
                         shared_ptr<shiguredo::mp4::box::Descriptor>(new shiguredo::mp4::box::DecSpecificInfo({.data = {0x11, 0x22, 0x33}})),  // NOLINT
                     std::
                         shared_ptr<shiguredo::mp4::box::Descriptor>(new shiguredo::mp4::box::SLConfigDescr({.data = {0x11, 0x22, 0x33, 0x44, 0x55}})),  // NOLINT
                 }}),
        new shiguredo::mp4::box::Esds(),
        {
            0,                 // version
            0x00, 0x00, 0x00,  // flags
            //
            0x03,        // tag
            0x2a,        // size (varint)
            0x12, 0x34,  // esid
            0x43,        // flags & streamPriority
            11,          // urlLength
            'h', 't', 't', 'p', ':', '/', '/', 'h', 'o', 'g',
            'e',                     // urlString
                                     //
            0x04,                    // tag
            0x0d,                    // size (varint)
            0x12,                    // objectTypeIndication
            0x56,                    // streamType & upStream & reserved
            0x12, 0x34, 0x56,        // bufferSizeDB
            0x12, 0x34, 0x56, 0x78,  // maxBitrate
            0x23, 0x45, 0x67, 0x89,  // avgBitrate
            //
            0x05,              // tag
            0x03,              // size (varint)
            0x11, 0x22, 0x33,  // data
            //
            0x06,                          // tag
            0x05,                          // size (varint)
            0x11, 0x22, 0x33, 0x44, 0x55,  // data
        },
        "Version=0 Flags=0x000000 Descriptors=[{Tag=ESDescr "
        "Size=42 "
        "ESID=4660 "
        "StreamDependenceFlag=false UrlFlag=true "
        "OcrStreamFlag=false "
        "StreamPriority=3 URLLength=0xb "
        "URLString=\"http://hoge\"}, "
        "{Tag=DecoderConfigDescr Size=13 "
        "ObjectTypeIndication=0x12 "
        "StreamType=21 UpStream=true Reserved=false "
        "BufferSizeDB=1193046 "
        "MaxBitrate=305419896 AvgBitrate=591751049}, "
        "{Tag=DecSpecificInfo "
        "Size=3 Data=[0x11, 0x22, 0x33]}, {Tag=SLConfigDescr "
        "Size=5 "
        "Data=[0x11, 0x22, 0x33, 0x44, 0x55]}]",
    },
    {
        "free",
        new shiguredo::mp4::box::Free({.data = {0x12, 0x34, 0x56}}),
        new shiguredo::mp4::box::Free(),
        {
            0x12,
            0x34,
            0x56,
        },
        "Data=[0x12, 0x34, 0x56]",
    },
    {
        "skip",
        new shiguredo::mp4::box::Skip({.data = {0x12, 0x34, 0x56}}),
        new shiguredo::mp4::box::Skip(),
        {
            0x12,
            0x34,
            0x56,
        },
        "Data=[0x12, 0x34, 0x56]",
    },
    {
        "ftyp",
        new shiguredo::mp4::box::Ftyp(
            {
                .major_brand = {'a', 'b', 'e', 'm'},
                .minor_version = 0x12345678,
                .compatible_brands =
                    {
                        {'a', 'b', 'c', 'd'},
                        {'e', 'f', 'g', 'h'},
                    },
            }),
        new shiguredo::mp4::box::Ftyp(),
        {
            'a', 'b', 'e', 'm',      // major brand
            0x12, 0x34, 0x56, 0x78,  // minor version
            'a', 'b', 'c', 'd',      // compatible brand
            'e', 'f', 'g', 'h',      // compatible brand
        },
        R"(MajorBrand="abem" MinorVersion=305419896 CompatibleBrands=[{CompatibleBrand="abcd"}, {CompatibleBrand="efgh"}])",
    },
    {
        "hdlr",
        new shiguredo::mp4::box::Hdlr(
            {.pre_defined = 0x12345678, .handler_type = {'a', 'b', 'e', 'm'}, .name = "Abema"}),
        new shiguredo::mp4::box::Hdlr(),
        {
            0,                                 // version
            0x00, 0x00, 0x00,                  // flags
            0x12, 0x34, 0x56, 0x78,            // pre-defined
            'a',  'b',  'e',  'm',             // handler type
            0x00, 0x00, 0x00, 0x00,            // reserved
            0x00, 0x00, 0x00, 0x00,            // reserved
            0x00, 0x00, 0x00, 0x00,            // reserved
            'A',  'b',  'e',  'm',  'a', 0x00  // name
        },
        R"(Version=0 Flags=0x000000 PreDefined=305419896 HandlerType="abem" Name="Abema")",
    },
    {"ilst", new shiguredo::mp4::box::Ilst{}, new shiguredo::mp4::box::Ilst{}, {}, ""},
    {
        "mdat",
        new shiguredo::mp4::box::Mdat({.data = {0x11, 0x22, 0x33}}),
        new shiguredo::mp4::box::Mdat(),
        {
            0x11,
            0x22,
            0x33,
        },
        "Data=[0x11, 0x22, 0x33]",
    },
    {"mdhd: version 0",
     new shiguredo::mp4::box::Mdhd({.creation_time = 0x12345678,
                                    .modification_time = 0x23456789,
                                    .timescale = 0x01020304,
                                    .duration = 0x02030405,
                                    .pad = true,
                                    .language = {'j', 'p', 'n'}}),
     new shiguredo::mp4::box::Mdhd(),
     {
         0,                       // version
         0x00, 0x00, 0x00,        // flags
         0x12, 0x34, 0x56, 0x78,  // creation time
         0x23, 0x45, 0x67, 0x89,  // modification time
         0x01, 0x02, 0x03, 0x04,  // timescale
         0x02, 0x03, 0x04, 0x05,  // duration
         0xaa, 0x0e,              // pad, language (1 01010 10000 01110)
         0x00, 0x00,              // pre defined
     },
     R"(Version=0 Flags=0x000000 CreationTime="1913-09-04T22:51:36Z" ModificationTime="1922-10-01T23:17:29Z" Timescale=16909060 Duration=33752069 Pad=true Language="jpn" PreDefined=0)"},
    {"mdhd: version 1",
     new shiguredo::mp4::box::Mdhd({
         .version = 1,
         .creation_time = 3687994187,
         .modification_time = 3687994187,
         .timescale = 0x01020304,
         .duration = 0x0203040506070809,
         .pad = true,
         .language = {'j', 'p', 'n'},  // 0x0a, 0x10, 0x0e
     }),
     new shiguredo::mp4::box::Mdhd(),
     {
         1,                                               // version
         0x00, 0x00, 0x00,                                // flags
         0x00, 0x00, 0x00, 0x00, 0xdb, 0xd2, 0x53, 0x4b,  // creation time
         0x00, 0x00, 0x00, 0x00, 0xdb, 0xd2, 0x53, 0x4b,  // modification time
         0x01, 0x02, 0x03, 0x04,                          // timescale
         0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
         0x09,        // duration
         0xaa, 0x0e,  // pad, language (1 01010 10000 01110)
         0x00, 0x00,  // pre defined
     },
     R"(Version=1 Flags=0x000000 CreationTime="2020-11-12T02:49:47Z" ModificationTime="2020-11-12T02:49:47Z" Timescale=16909060 Duration=144964032628459529 Pad=true Language="jpn" PreDefined=0)"},
    {
        "mehd: version 0",
        new shiguredo::mp4::box::Mehd({.fragment_duration = 0x01234567}),
        new shiguredo::mp4::box::Mehd{},
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x01, 0x23, 0x45, 0x67,  // frangment duration
        },
        "Version=0 Flags=0x000000 FragmentDuration=19088743",
    },
    {
        "mehd: version 1",
        new shiguredo::mp4::box::Mehd({.version = 1, .fragment_duration = 0x0123456789abcdef}),
        new shiguredo::mp4::box::Mehd{},
        {
            1,                 // version
            0x00, 0x00, 0x00,  // flags
            0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd,
            0xef,  // frangment duration
        },
        "Version=1 Flags=0x000000 "
        "FragmentDuration=81985529216486895",
    },
    {
        "meta",
        new shiguredo::mp4::box::Meta(),
        new shiguredo::mp4::box::Meta(),
        {
            0,                 // version
            0x00, 0x00, 0x00,  // flags
        },
        "Version=0 Flags=0x000000",
    },
    {
        "mfhd",
        new shiguredo::mp4::box::Mfhd({.sequence_number = 0x12345678}),
        new shiguredo::mp4::box::Mfhd(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x12, 0x34, 0x56, 0x78,  // sequence number
        },
        "Version=0 Flags=0x000000 SequenceNumber=305419896",
    },
    {"mfra", new shiguredo::mp4::box::Mfra(), new shiguredo::mp4::box::Mfra(), {}, ""},
    {"mfro",
     new shiguredo::mp4::box::Mfro({.size = 0x12345678}),
     new shiguredo::mp4::box::Mfro(),
     {
         0,                       // version
         0x00, 0x00, 0x00,        // flags
         0x12, 0x34, 0x56, 0x78,  // size
     },
     "Version=0 Flags=0x000000 Size=305419896"},
    {"minf", new shiguredo::mp4::box::Minf(), new shiguredo::mp4::box::Minf(), {}, ""},
    {"moof", new shiguredo::mp4::box::Moof(), new shiguredo::mp4::box::Moof(), {}, ""},
    {"moov", new shiguredo::mp4::box::Moov(), new shiguredo::mp4::box::Moov(), {}, ""},
    {"mvex", new shiguredo::mp4::box::Mvex(), new shiguredo::mp4::box::Mvex(), {}, ""},
    {
        "mvhd: version 0",
        new shiguredo::mp4::box::Mvhd({
            .creation_time = 0x01234567,
            .modification_time = 0x23456789,
            .timescale = 0x456789ab,
            .duration = 0x6789abcd,
            .rate = -0x01234567,
            .volume = 0x0123,
            .matrix = {0, 0, 0, 0, 0, 0, 0, 0, 0},
            .next_track_id = 0xabcdef01,
        }),
        new shiguredo::mp4::box::Mvhd(),
        {
            0,                                               // version
            0x00, 0x00, 0x00,                                // flags
            0x01, 0x23, 0x45, 0x67,                          // creation time
            0x23, 0x45, 0x67, 0x89,                          // modification time
            0x45, 0x67, 0x89, 0xab,                          // timescale
            0x67, 0x89, 0xab, 0xcd,                          // duration
            0xfe, 0xdc, 0xba, 0x99,                          // rate
            0x01, 0x23,                                      // volume
            0x00, 0x00,                                      // reserved
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // matrix
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // pre-defined
            0xab, 0xcd, 0xef, 0x01,                                                  // next track ID
        },
        R"(Version=0 Flags=0x000000 CreationTime="1904-08-08T22:25:43Z" ModificationTime="1922-10-01T23:17:29Z" Timescale=1164413355 )"
        "Duration=1737075661 "
        "Rate=-291.2711 Volume=291 Matrix=[0x0, 0x0, 0x0, 0x0, "
        "0x0, 0x0, 0x0, "
        "0x0, 0x0] PreDefined=[0, 0, 0, 0, 0, 0] "
        "NextTrackID=2882400001",
    },
    {"mvhd: version 1",
     new shiguredo::mp4::box::Mvhd({.version = 1,
                                    .creation_time = 0x0123456789abcdef,
                                    .modification_time = 0x23456789abcdef01,
                                    .timescale = 0x89abcdef,
                                    .duration = 0x456789abcdef0123,
                                    .rate = -0x01234567,
                                    .volume = 0x0123,
                                    .matrix = {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                    .next_track_id = 0xabcdef01}),
     new shiguredo::mp4::box::Mvhd(),
     {
         1,                                               // version
         0x00, 0x00, 0x00,                                // flags
         0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,  // creation time
         0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01,  // modification
         0x89, 0xab, 0xcd, 0xef,                          // timescale
         0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23,  // duration
         0xfe, 0xdc, 0xba, 0x99,                          // rate
         0x01, 0x23,                                      // volume
         0x00, 0x00,                                      // reserved
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // matrix
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // pre-defined
         0xab, 0xcd, 0xef, 0x01,                                                  // next track ID
     },
     R"(Version=1 Flags=0x000000 CreationTime="0000-00-00T00:00:00Z" ModificationTime="0000-00-00T00:00:00Z" Timescale=2309737967 )"
     "Duration=5001117282205630755 Rate=-291.2711 Volume=291 "
     "Matrix=[0x0, "
     "0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0] PreDefined=[0, 0, 0, "
     "0, 0, 0] "
     "NextTrackID=2882400001"},
    {
        "pssh: version 0: no KIDs",
        new shiguredo::mp4::box::Pssh({.system_id = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                                     0x0c, 0x0d, 0x0e, 0x0f, 0x10},
                                       .data = {0x21, 0x22, 0x23, 0x24, 0x25}}),
        new shiguredo::mp4::box::Pssh(),
        {
            0,                 // version
            0x00, 0x00, 0x00,  // flags
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,  // system ID
            0x00, 0x00, 0x00, 0x05,                          // data size
            0x21, 0x22, 0x23, 0x24, 0x25,                    // data
        },
        R"(Version=0 Flags=0x000000 SystemID="0102030405060708090a0b0c0d0e0f10" DataSize=5 Data=[0x21, 0x22, 0x23, 0x24, 0x25])",
    },
    {
        "pssh: version 1: with KIDs",
        new shiguredo::mp4::box::Pssh(
            {.version = 1,
             .system_id = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                           0x10},
             .kids =
                 {
                     shiguredo::mp4::box::PsshKID(
                         {.kid = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                                  0x1f, 0x10}}),
                     shiguredo::mp4::box::PsshKID(
                         {.kid = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e,
                                  0x2f, 0x20}}),
                 },
             .data = {0x21, 0x22, 0x23, 0x24, 0x25}}),
        new shiguredo::mp4::box::Pssh(),
        {
            1,                                                                                               // version
            0x00, 0x00, 0x00,                                                                                // flags
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,  // system ID NOLINT
            0x00, 0x00, 0x00, 0x02,  // KID count
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x10, 0x21,
            0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x20,  // KIDs
            0x00, 0x00, 0x00, 0x05,                                                                    // data size
            0x21, 0x22, 0x23, 0x24, 0x25,                                                              // data
        },
        R"(Version=1 Flags=0x000000 SystemID="0102030405060708090a0b0c0d0e0f10" KIDCount=2 KIDs=["1112131415161718191a1b1c1d1e1f10" "2122232425262728292a2b2c2d2e2f20"] DataSize=5 Data=[0x21, 0x22, 0x23, 0x24, 0x25])",
    },
    {"VisualSampleEntry",
     new shiguredo::mp4::box::VisualSampleEntry({.type = shiguredo::mp4::BoxType("avc1"),
                                                 .data_reference_index = 0x1234,
                                                 .pre_defined = 0x0101,
                                                 .pre_defined2 = {0x01000001, 0x01000002, 0x01000003},
                                                 .width = 0x0102,
                                                 .height = 0x0103,
                                                 .horizresolution = 0x01000004,
                                                 .vertresolution = 0x01000005,
                                                 .frame_count = 0x0104,
                                                 .compressorname = {9, 's', 'h', 'i', 'g', 'u', 'r', 'e', 'd', 'o'},
                                                 .depth = 0x0105,
                                                 .pre_defined3 = 1001}),
     new shiguredo::mp4::box::VisualSampleEntry(),
     {
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                      // reserved
         0x12, 0x34,                                                              // data reference index
         0x01, 0x01,                                                              // PreDefined
         0x00, 0x00,                                                              // Reserved
         0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x03,  // PreDefined2
         0x01, 0x02,                                                              // Width
         0x01, 0x03,                                                              // Height
         0x01, 0x00, 0x00, 0x04,                                                  // Horizresolution
         0x01, 0x00, 0x00, 0x05,                                                  // Vertresolution
         0x00, 0x00, 0x00, 0x00,                                                  // Reserved2
         0x01, 0x04,                                                              // FrameCount
         9,    's',  'h',  'i',  'g',  'u',  'r',  'e',  'd',  'o',  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Compressorname
         0x01, 0x05,                                                                                // Depth
         0x03, 0xe9,                                                                                // PreDefined3
     },
     R"(DataReferenceIndex=4660 PreDefined=257 PreDefined2=[16777217, 16777218, 16777219] Width=258 Height=259 Horizresolution=16777220 Vertresolution=16777221 FrameCount=260 Compressorname="shiguredo" Depth=261 PreDefined3=1001)"},
    {
        "AudioSampleEntry",
        new shiguredo::mp4::box::AudioSampleEntry({.type = shiguredo::mp4::BoxType("enca"),
                                                   .data_reference_index = 0x1234,
                                                   .entry_version = 0x0123,
                                                   .channel_count = 0x2345,
                                                   .sample_size = 0x4567,
                                                   .pre_defined = 0x6789,
                                                   .sample_rate = 0x01234567}),
        new shiguredo::mp4::box::AudioSampleEntry(shiguredo::mp4::BoxType("enca")),
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x12, 0x34,                          // data reference index
            0x01, 0x23,                          // entry version
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x23, 0x45,                          // channel count
            0x45, 0x67,                          // sample size
            0x67, 0x89,                          // pre-defined
            0x00, 0x00,                          // reserved
            0x01, 0x23, 0x45, 0x67,              // sample rate
        },
        "DataReferenceIndex=4660 EntryVersion=291 "
        "ChannelCount=9029 "
        "SampleSize=17767 PreDefined=26505 SampleRate=19088743",
    },
    {
        "AudioSampleEntry (QuickTime v1)",
        new shiguredo::mp4::box::AudioSampleEntry(
            {.type = shiguredo::mp4::BoxType("enca"),
             .data_reference_index = 0x1234,
             .entry_version = 1,
             .channel_count = 0x2345,
             .sample_size = 0x4567,
             .pre_defined = 0x6789,
             .sample_rate = 0x01234567,
             .is_quick_time_compatible = true,
             .quick_time_data = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd,
                                 0xee, 0xff}}),
        new shiguredo::mp4::box::AudioSampleEntry(),
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x12, 0x34,                          // data reference index
            0x00, 0x01,                          // entry version
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x23, 0x45,                          // channel count
            0x45, 0x67,                          // sample size
            0x67, 0x89,                          // pre-defined
            0x00, 0x00,                          // reserved
            0x01, 0x23, 0x45, 0x67,              // sample rate
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        },
        "DataReferenceIndex=4660 EntryVersion=1 ChannelCount=9029 "
        "SampleSize=17767 PreDefined=26505 SampleRate=19088743 "
        "QuickTimeData=[0x0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, "
        "0x77, 0x88, "
        "0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff]",
    },
    {
        "AudioSampleEntry (QuickTime v2)",
        new shiguredo::mp4::box::AudioSampleEntry(
            {.type = shiguredo::mp4::BoxType("enca"),
             .data_reference_index = 0x1234,
             .entry_version = 2,
             .channel_count = 0x2345,
             .sample_size = 0x4567,
             .pre_defined = 0x6789,
             .sample_rate = 0x01234567,
             .is_quick_time_compatible = true,
             .quick_time_data =
                 {
                     0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
                     0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                     0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0x33,
                 }}),
        new shiguredo::mp4::box::AudioSampleEntry(),
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x12, 0x34,                          // data reference index
            0x00, 0x02,                          // entry version
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x23, 0x45,                          // channel count
            0x45, 0x67,                          // sample size
            0x67, 0x89,                          // pre-defined
            0x00, 0x00,                          // reserved
            0x01, 0x23, 0x45, 0x67,              // sample rate
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11,
            0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0x33,
        },
        "DataReferenceIndex=4660 EntryVersion=2 ChannelCount=9029 "
        "SampleSize=17767 PreDefined=26505 SampleRate=19088743 "
        "QuickTimeData=[0x0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, "
        "0x77, 0x88, "
        "0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x0, 0x11, "
        "0x22, 0x33, "
        "0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, "
        "0xdd, 0xee, "
        "0xff, 0x0, 0x11, 0x22, 0x33]",
    },
    {
        "AVCDecoderConfiguration main profile",
        new shiguredo::mp4::box::AVCDecoderConfiguration(
            {
                .configuration_version = 0x12,
                .profile = shiguredo::mp4::box::AVCMainProfile,
                .profile_compatibility = 0x40,
                .level = 0x1f,
                .length_size_minus_one = 0x2,
                .sequence_parameter_sets =
                    {
                        shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0x12, 0x34}}),
                        shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0x12, 0x34, 0x56}}),
                    },
                .picture_parameter_sets =
                    {
                        shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0xab, 0xcd}}),
                        shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0xab, 0xcd, 0xef}}),
                    },
            }),
        new shiguredo::mp4::box::AVCDecoderConfiguration(),
        {
            0x12,              // configuration version
            0x4d,              // profile
            0x40,              // profile compatibility
            0x1f,              // level
            0xfe,              // reserved,  lengthSizeMinusOne
            0xe2,              // reserved, numOfSequenceParameterSets
            0x00, 0x02,        // length
            0x12, 0x34,        // nalUnit
            0x00, 0x03,        // length
            0x12, 0x34, 0x56,  // nalUnit
            0x02,              // reserved, numOfSequenceParameterSets
            0x00, 0x02,        // length
            0xab, 0xcd,        // nalUnit
            0x00, 0x03,        // length
            0xab, 0xcd, 0xef,  // nalUnit
        },
        "ConfigurationVersion=0x12 Profile=0x4d "
        "ProfileCompatibility=0x40 "
        "Level=0x1f LengthSizeMinusOne=0x2 "
        "NumOfSequenceParameterSets=0x2 "
        "SequenceParameterSets=[{Length=2 NALUnit=[0x12, 0x34]}, "
        "{Length=3 "
        "NALUnit=[0x12, 0x34, 0x56]}] "
        "NumOfPictureParameterSets=0x2 "
        "PictureParameterSets=[{Length=2 NALUnit=[0xab, 0xcd]}, "
        "{Length=3 "
        "NALUnit=[0xab, 0xcd, 0xef]}]",
    },
    {
        "AVCDecoderConfiguration high profile old spec",
        new shiguredo::mp4::box::AVCDecoderConfiguration(
            {
                .configuration_version = 0x12,
                .profile = shiguredo::mp4::box::AVCHighProfile,
                .profile_compatibility = 0x00,
                .level = 0x28,
                .length_size_minus_one = 0x2,
                .sequence_parameter_sets =
                    {
                        shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0x12, 0x34}}),
                        shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0x12, 0x34, 0x56}}),
                    },
                .picture_parameter_sets =
                    {
                        shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0xab, 0xcd}}),
                        shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0xab, 0xcd, 0xef}}),
                    },
            }),
        new shiguredo::mp4::box::AVCDecoderConfiguration(),
        {
            0x12,              // configuration version
            0x64,              // profile
            0x00,              // profile compatibility
            0x28,              // level
            0xfe,              // reserved,  lengthSizeMinusOne
            0xe2,              // reserved, numOfSequenceParameterSets
            0x00, 0x02,        // length
            0x12, 0x34,        // nalUnit
            0x00, 0x03,        // length
            0x12, 0x34, 0x56,  // nalUnit
            0x02,              // reserved, numOfSequenceParameterSets
            0x00, 0x02,        // length
            0xab, 0xcd,        // nalUnit
            0x00, 0x03,        // length
            0xab, 0xcd, 0xef,  // nalUnit
        },
        "ConfigurationVersion=0x12 Profile=0x64 "
        "ProfileCompatibility=0x0 "
        "Level=0x28 LengthSizeMinusOne=0x2 "
        "NumOfSequenceParameterSets=0x2 "
        "SequenceParameterSets=[{Length=2 NALUnit=[0x12, 0x34]}, "
        "{Length=3 "
        "NALUnit=[0x12, 0x34, 0x56]}] "
        "NumOfPictureParameterSets=0x2 "
        "PictureParameterSets=[{Length=2 NALUnit=[0xab, 0xcd]}, "
        "{Length=3 "
        "NALUnit=[0xab, 0xcd, 0xef]}]",
    },
    {
        "AVCDecoderConfiguration high profile new spec",
        new shiguredo::mp4::box::AVCDecoderConfiguration(
            {.configuration_version = 0x12,
             .profile = shiguredo::mp4::box::AVCHighProfile,
             .profile_compatibility = 0x00,
             .level = 0x28,
             .length_size_minus_one = 0x2,
             .sequence_parameter_sets =
                 {
                     shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0x12, 0x34}}),
                     shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0x12, 0x34, 0x56}}),
                 },
             .picture_parameter_sets =
                 {
                     shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0xab, 0xcd}}),
                     shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0xab, 0xcd, 0xef}}),
                 },
             .high_profile_fields_enabled = true,
             .chroma_format = 0x1,
             .bit_depth_luma_minus8 = 0x2,
             .bit_depth_chroma_minus8 = 0x3,
             .sequence_parameter_sets_ext =
                 {
                     shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0x12, 0x34}}),
                     shiguredo::mp4::box::AVCParameterSet({.nal_unit = {0x12, 0x34, 0x56}}),
                 }}),
        new shiguredo::mp4::box::AVCDecoderConfiguration(),
        {
            0x12,              // configuration version
            0x64,              // profile
            0x00,              // profile compatibility
            0x28,              // level
            0xfe,              // reserved,  lengthSizeMinusOne
            0xe2,              // reserved, numOfSequenceParameterSets
            0x00, 0x02,        // length
            0x12, 0x34,        // nalUnit
            0x00, 0x03,        // length
            0x12, 0x34, 0x56,  // nalUnit
            0x02,              // numOfSequenceParameterSets
            0x00, 0x02,        // length
            0xab, 0xcd,        // nalUnit
            0x00, 0x03,        // length
            0xab, 0xcd, 0xef,  // nalUnit
            0xfd,              // reserved, chromaFormat
            0xfa,              // reserved, bitDepthLumaMinus8
            0xfb,              // reserved, bitDepthChromaMinus8
            0x02,              // numOfSequenceParameterSets
            0x00, 0x02,        // length
            0x12, 0x34,        // nalUnit
            0x00, 0x03,        // length
            0x12, 0x34, 0x56,  // nalUnit
        },
        "ConfigurationVersion=0x12 Profile=0x64 "
        "ProfileCompatibility=0x0 "
        "Level=0x28 LengthSizeMinusOne=0x2 "
        "NumOfSequenceParameterSets=0x2 "
        "SequenceParameterSets=[{Length=2 NALUnit=[0x12, 0x34]}, "
        "{Length=3 "
        "NALUnit=[0x12, 0x34, 0x56]}] "
        "NumOfPictureParameterSets=0x2 "
        "PictureParameterSets=[{Length=2 NALUnit=[0xab, 0xcd]}, "
        "{Length=3 "
        "NALUnit=[0xab, 0xcd, 0xef]}] ChromaFormat=0x1 "
        "BitDepthLumaMinus8=0x2 "
        "BitDepthChromaMinus8=0x3 NumOfSequenceParameterSetExt=0x2 "
        "SequenceParameterSetsExt=[{Length=2 NALUnit=[0x12, "
        "0x34]}, {Length=3 "
        "NALUnit=[0x12, 0x34, 0x56]}]",
    },
    {
        "PixelAspectRatio",
        new shiguredo::mp4::box::PixelAspectRatio({.h_spacing = 0x01234567, .v_spacing = 0x23456789}),
        new shiguredo::mp4::box::PixelAspectRatio(),
        {
            0x01,
            0x23,
            0x45,
            0x67,
            0x23,
            0x45,
            0x67,
            0x89,
        },
        "HSpacing=19088743 VSpacing=591751049",
    },
    {
        "sbgp: version 0",
        new shiguredo::mp4::box::Sbgp({.grouping_type = shiguredo::mp4::GroupingTypeRoll,
                                       .grouping_type_parameter = 0,
                                       .entries =
                                           {
                                               shiguredo::mp4::box::SbgpEntry(
                                                   {.sample_count = 0x23456789, .group_description_index = 0x3456789a}),
                                               shiguredo::mp4::box::SbgpEntry(
                                                   {.sample_count = 0x456789ab, .group_description_index = 0x56789abc}),
                                           }}),
        new shiguredo::mp4::box::Sbgp(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x72, 0x6f, 0x6c, 0x6c,  // grouping type
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x23, 0x45, 0x67, 0x89,  // sample count
            0x34, 0x56, 0x78, 0x9a,  // group description index
            0x45, 0x67, 0x89, 0xab,  // sample count
            0x56, 0x78, 0x9a, 0xbc,  // group description index
        },
        R"(Version=0 Flags=0x000000 GroupingType="roll" EntryCount=2 )"
        "Entries=[{SampleCount=591751049 "
        "GroupDescriptionIndex=878082202}, "
        "{SampleCount=1164413355 "
        "GroupDescriptionIndex=1450744508}]",
    },
    {
        "sbgp: version 1",
        new shiguredo::mp4::box::Sbgp({.version = 1,
                                       .grouping_type = shiguredo::mp4::GroupingTypeProl,
                                       .grouping_type_parameter = 0x89abcdef,
                                       .entries =
                                           {
                                               shiguredo::mp4::box::SbgpEntry(
                                                   {.sample_count = 0x23456789, .group_description_index = 0x3456789a}),
                                               shiguredo::mp4::box::SbgpEntry(
                                                   {.sample_count = 0x456789ab, .group_description_index = 0x56789abc}),
                                           }}),
        new shiguredo::mp4::box::Sbgp{},
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            0x70, 0x72, 0x6f, 0x6c,  // grouping type
            0x89, 0xab, 0xcd, 0xef,  // grouping type parameter
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x23, 0x45, 0x67, 0x89,  // sample count
            0x34, 0x56, 0x78, 0x9a,  // group description index
            0x45, 0x67, 0x89, 0xab,  // sample count
            0x56, 0x78, 0x9a, 0xbc,  // group description index
        },
        R"(Version=1 Flags=0x000000 GroupingType="prol")"
        " GroupingTypeParameter=2309737967 EntryCount=2 "
        "Entries=[{SampleCount=591751049 "
        "GroupDescriptionIndex=878082202}, "
        "{SampleCount=1164413355 "
        "GroupDescriptionIndex=1450744508}]",
    },
    {"schi", new shiguredo::mp4::box::Schi{}, new shiguredo::mp4::box::Schi{}, {}, ""},
    {
        "sgpd: version 1 roll",
        new shiguredo::mp4::box::Sgpd({
            .grouping_type = {'r', 'o', 'l', 'l'},
            .default_length = 2,
            .default_sample_description_index = 0,
            .roll_distances = {shiguredo::mp4::box::RollDistance({.roll_distance = 0x1111}),
                               shiguredo::mp4::box::RollDistance({.roll_distance = 0x2222})},
        }),
        new shiguredo::mp4::box::Sgpd(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            'r',  'o',  'l',  'l',   // grouping type
            0x00, 0x00, 0x00, 0x02,  // default length
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x11, 0x11, 0x22, 0x22,  // roll distances
        },
        R"(Version=1 Flags=0x000000 GroupingType="roll" DefaultLength=2 EntryCount=2 RollDistances=[4369, 8738])",
    },
    {
        "sgpd: version 1 prol",
        new shiguredo::mp4::box::Sgpd({
            .grouping_type = {'p', 'r', 'o', 'l'},
            .default_length = 2,
            .default_sample_description_index = 0,
            .roll_distances = {shiguredo::mp4::box::RollDistance({.roll_distance = 0x1111}),
                               shiguredo::mp4::box::RollDistance({.roll_distance = 0x2222})},
        }),
        new shiguredo::mp4::box::Sgpd(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            'p',  'r',  'o',  'l',   // grouping type
            0x00, 0x00, 0x00, 0x02,  // default length
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x11, 0x11, 0x22, 0x22,  // roll distances
        },
        R"(Version=1 Flags=0x000000 GroupingType="prol" DefaultLength=2 EntryCount=2 RollDistances=[4369, 8738])",
    },
    {
        "sgpd: version 1 alst no-opts",
        new shiguredo::mp4::box::Sgpd(
            {.grouping_type = {'a', 'l', 's', 't'},
             .default_length = 12,
             .default_sample_description_index = 0,
             .alternative_startup_entries =
                 {
                     shiguredo::mp4::box::AlternativeStartupEntry(
                         {.roll_count = 2, .first_output_sample = 0x0123, .sample_offsets = {0x01234567, 0x89abcdef}}),
                     shiguredo::mp4::box::AlternativeStartupEntry(
                         {.roll_count = 2, .first_output_sample = 0x4567, .sample_offsets = {0x456789ab, 0xcdef0123}}),
                 }}),
        new shiguredo::mp4::box::Sgpd(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            'a',  'l',  's',  't',   // grouping type
            0x00, 0x00, 0x00, 0x0c,  // default length
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x00, 0x02,              // roll count
            0x01, 0x23,              // first output sample
            0x01, 0x23, 0x45, 0x67,  // sample offset
            0x89, 0xab, 0xcd, 0xef,  // sample offset
            0x00, 0x02,              // roll count
            0x45, 0x67,              // first output sample
            0x45, 0x67, 0x89, 0xab,  // sample offset
            0xcd, 0xef, 0x01, 0x23,  // sample offset
        },
        R"(Version=1 Flags=0x000000 GroupingType="alst" DefaultLength=12 EntryCount=2 AlternativeStartupEntries=[{RollCount=2 FirstOutputSample=291 SampleOffset=[19088743, 2309737967] Opts=[]}, {RollCount=2 FirstOutputSample=17767 SampleOffset=[1164413355, 3454992675] Opts=[]}])",
    },
    {
        "sgpd: version 1 alst default-length",
        new shiguredo::mp4::box::Sgpd(
            {.grouping_type = {'a', 'l', 's', 't'},
             .default_length = 20,
             .default_sample_description_index = 0,
             .alternative_startup_entries =
                 {
                     shiguredo::mp4::box::AlternativeStartupEntry(
                         {.roll_count = 2,
                          .first_output_sample = 0x0123,
                          .sample_offsets = {0x01234567, 0x89abcdef},
                          .opts = {shiguredo::mp4::box::AlternativeStartupEntryOpt(
                                       {.num_output_samples = 0x0123, .num_total_samples = 0x4567}),
                                   shiguredo::mp4::box::AlternativeStartupEntryOpt(
                                       {.num_output_samples = 0x89ab, .num_total_samples = 0xcdef})}}),
                     shiguredo::mp4::box::AlternativeStartupEntry(
                         {.roll_count = 2,
                          .first_output_sample = 0x4567,
                          .sample_offsets = {0x456789ab, 0xcdef0123},
                          .opts = {shiguredo::mp4::box::AlternativeStartupEntryOpt(
                                       {.num_output_samples = 0x0123, .num_total_samples = 0x4567}),
                                   shiguredo::mp4::box::AlternativeStartupEntryOpt(
                                       {.num_output_samples = 0x89ab, .num_total_samples = 0xcdef})}}),
                 }}),
        new shiguredo::mp4::box::Sgpd(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            'a',  'l',  's',  't',   // grouping type
            0x00, 0x00, 0x00, 0x14,  // default length
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x00, 0x02,              // roll count
            0x01, 0x23,              // first output sample
            0x01, 0x23, 0x45, 0x67,  // sample offset
            0x89, 0xab, 0xcd, 0xef,  // sample offset
            0x01, 0x23,              // num output samples
            0x45, 0x67,              // num total samples
            0x89, 0xab,              // num output samples
            0xcd, 0xef,              // num total samples
            0x00, 0x02,              // roll count
            0x45, 0x67,              // first output sample
            0x45, 0x67, 0x89, 0xab,  // sample offset
            0xcd, 0xef, 0x01, 0x23,  // sample offset
            0x01, 0x23,              // num output samples
            0x45, 0x67,              // num total samples
            0x89, 0xab,              // num output samples
            0xcd, 0xef,              // num total samples
        },
        R"(Version=1 Flags=0x000000 GroupingType="alst" DefaultLength=20 EntryCount=2 AlternativeStartupEntries=[{RollCount=2 FirstOutputSample=291 SampleOffset=[19088743, 2309737967] Opts=[{NumOutputSamples=291 NumTotalSamples=17767}, {NumOutputSamples=35243 NumTotalSamples=52719}]}, {RollCount=2 FirstOutputSample=17767 SampleOffset=[1164413355, 3454992675] Opts=[{NumOutputSamples=291 NumTotalSamples=17767}, {NumOutputSamples=35243 NumTotalSamples=52719}]}])",
    },
    {
        "sgpd: version 1 alst no-default-length",
        new shiguredo::mp4::
            box::Sgpd({.grouping_type = {'a', 'l', 's', 't'},
                       .default_length = 0,
                       .default_sample_description_index = 0,
                       .alternative_startup_entries =
                           {
                               shiguredo::mp4::
                                   box::AlternativeStartupEntry({.description_length = 16,
                                                                 .roll_count = 2,
                                                                 .first_output_sample = 0x0123,
                                                                 .sample_offsets = {0x01234567, 0x89abcdef},
                                                                 .opts =
                                                                     {
                                                                         shiguredo::
                                                                             mp4::box::AlternativeStartupEntryOpt(
                                                                                 {.num_output_samples = 0x0123, .num_total_samples = 0x4567}),  // NOLINT
                                                                     }}),
                               shiguredo::
                                   mp4::box::AlternativeStartupEntry({.description_length = 20,
                                                                      .roll_count = 2,
                                                                      .first_output_sample = 0x4567,
                                                                      .sample_offsets = {0x456789ab, 0xcdef0123},
                                                                      .opts = {shiguredo::
                                                                                   mp4::box::AlternativeStartupEntryOpt(
                                                                                       {.num_output_samples = 0x0123, .num_total_samples = 0x4567}),  // NOLINT
                                                                               shiguredo::
                                                                                   mp4::box::AlternativeStartupEntryOpt({.num_output_samples =  // NOLINT
                                                                                                                         0x89ab,  // NOLINT
                                                                                                                         .num_total_samples = 0xcdef})}}),  // NOLINT
                           }}),
        new shiguredo::mp4::box::Sgpd(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            'a',  'l',  's',  't',   // grouping type
            0x00, 0x00, 0x00, 0x00,  // default length
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x00, 0x00, 0x00, 0x10,  // description length
            0x00, 0x02,              // roll count
            0x01, 0x23,              // first output sample
            0x01, 0x23, 0x45, 0x67,  // sample offset
            0x89, 0xab, 0xcd, 0xef,  // sample offset
            0x01, 0x23,              // num output samples
            0x45, 0x67,              // num total samples
            0x00, 0x00, 0x00, 0x14,  // description length
            0x00, 0x02,              // roll count
            0x45, 0x67,              // first output sample
            0x45, 0x67, 0x89, 0xab,  // sample offset
            0xcd, 0xef, 0x01, 0x23,  // sample offset
            0x01, 0x23,              // num output samples
            0x45, 0x67,              // num total samples
            0x89, 0xab,              // num output samples
            0xcd, 0xef,              // num total samples
        },
        R"(Version=1 Flags=0x000000 GroupingType="alst" DefaultLength=0 EntryCount=2 AlternativeStartupEntriesL=[{DescriptionLength=16 RollCount=2 FirstOutputSample=291 SampleOffset=[19088743, 2309737967] Opts=[{NumOutputSamples=291 NumTotalSamples=17767}]}, {DescriptionLength=20 RollCount=2 FirstOutputSample=17767 SampleOffset=[1164413355, 3454992675] Opts=[{NumOutputSamples=291 NumTotalSamples=17767}, {NumOutputSamples=35243 NumTotalSamples=52719}]}])",
    },
    {
        "sgpd: version 1 rap",
        new shiguredo::mp4::box::Sgpd({.grouping_type = {'r', 'a', 'p', ' '},
                                       .default_length = 1,
                                       .default_sample_description_index = 2,
                                       .visual_random_access_entries =
                                           {
                                               shiguredo::mp4::box::VisualRandomAccessEntry(
                                                   {.num_leading_samples_known = true, .num_leading_samples = 0x27}),
                                               shiguredo::mp4::box::VisualRandomAccessEntry(
                                                   {.num_leading_samples_known = false, .num_leading_samples = 0x1a}),
                                           }}),
        new shiguredo::mp4::box::Sgpd(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            'r', 'a', 'p', ' ',      // grouping type
            0x00, 0x00, 0x00, 0x01,  // default length
            0x00, 0x00, 0x00, 0x02,  // entry count
            0xa7, 0x1a,              // visual random access entry
        },
        R"(Version=1 Flags=0x000000 GroupingType="rap " DefaultLength=1 EntryCount=2 VisualRandomAccessEntries=[{NumLeadingSamplesKnown=true NumLeadingSamples=0x27}, {NumLeadingSamplesKnown=false NumLeadingSamples=0x1a}])",
    },
    {
        "sgpd: version 1 tele",
        new shiguredo::mp4::box::Sgpd(
            {.grouping_type = {'t', 'e', 'l', 'e'},
             .default_length = 1,
             .temporal_level_entries =
                 {
                     shiguredo::mp4::box::TemporalLevelEntry({.level_independently_decodable = true}),
                     shiguredo::mp4::box::TemporalLevelEntry({.level_independently_decodable = false}),
                 }}),
        new shiguredo::mp4::box::Sgpd(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            't', 'e', 'l', 'e',      // grouping type
            0x00, 0x00, 0x00, 0x01,  // default length
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x80, 0x00,              // temporal level entry
        },
        R"(Version=1 Flags=0x000000 GroupingType="tele" DefaultLength=1 EntryCount=2 TemporalLevelEntries=[{LevelIndependentlyRecodable=true}, {LevelIndependentlyRecodable=false}])",
    },
    {
        "sgpd: version 2 roll",
        new shiguredo::mp4::box::Sgpd({
            .version = 2,
            .grouping_type = {'r', 'o', 'l', 'l'},
            .default_length = 0,
            .default_sample_description_index = 5,
            .roll_distances = {shiguredo::mp4::box::RollDistance({.roll_distance = 0x1111}),
                               shiguredo::mp4::box::RollDistance({.roll_distance = 0x2222})},
        }),
        new shiguredo::mp4::box::Sgpd(),
        {
            2,                 // version
            0x00, 0x00, 0x00,  // flags
            'r',  'o',  'l',
            'l',  // grouping type
            0x00, 0x00, 0x00,
            0x05,  // default sample description index
            0x00, 0x00, 0x00,
            0x02,  // entry count
            0x11, 0x11, 0x22,
            0x22,  // roll distances
        },
        R"(Version=2 Flags=0x000000 GroupingType="roll" DefaultSampleDescriptionIndex=5 EntryCount=2 RollDistances=[4369, 8738])",
    },
    {
        "sidx: version=0",
        new shiguredo::mp4::box::Sidx({
            .reference_id = 0x01234567,
            .timescale = 0x23456789,
            .earliest_presentation_time = 0x456789ab,
            .first_offset = 0x6789abcd,
            .references = {shiguredo::mp4::box::SidxReference({.reference_type = false,
                                                               .reference_size = 0x01234567,
                                                               .subsegument_duration = 0x23456789,
                                                               .starts_with_sap = true,
                                                               .sap_type = 6,
                                                               .sap_delta_time = 0x09abcdef}),
                           shiguredo::mp4::box::SidxReference({.reference_type = true,
                                                               .reference_size = 0x01234567,
                                                               .subsegument_duration = 0x23456789,
                                                               .starts_with_sap = false,
                                                               .sap_type = 5,
                                                               .sap_delta_time = 0x09abcdef})},
        }),
        new shiguredo::mp4::box::Sidx(),
        {
            0,                 // version
            0x00, 0x00, 0x00,  // flags
            0x01, 0x23, 0x45,
            0x67,  // ReferenceID
            0x23, 0x45, 0x67,
            0x89,  // Timescale
            0x45, 0x67, 0x89,
            0xab,  // EarliestPresentationTime
            0x67, 0x89, 0xab,
            0xcd,        // FirstOffset
            0x00, 0x00,  // Reserved
            0x00, 0x2,   // ReferenceCount
            0x01, 0x23, 0x45,
            0x67,  // ReferenceType, ReferencedSize
            0x23, 0x45, 0x67,
            0x89,  // SubsegmentDuration
            0xe9, 0xab, 0xcd,
            0xef,  // StartsWithSAP, SAPType, SAPDeltaTime
            0x81, 0x23, 0x45,
            0x67,  // ReferenceType, ReferencedSize
            0x23, 0x45, 0x67,
            0x89,  // SubsegmentDuration
            0x59, 0xab, 0xcd,
            0xef,  // StartsWithSAP, SAPType, SAPDeltaTime
        },
        "Version=0 Flags=0x000000 ReferenceID=19088743 "
        "Timescale=591751049 EarliestPresentationTime=1164413355 "
        "FirstOffset=1737075661 ReferenceCount=2 "
        "References=[{ReferenceType=false ReferencedSize=19088743 "
        "SubsegmentDuration=591751049 StartsWithSAP=true SAPType=6 "
        "SAPDeltaTime=162254319}, {ReferenceType=true "
        "ReferencedSize=19088743 SubsegmentDuration=591751049 "
        "StartsWithSAP=false SAPType=5 SAPDeltaTime=162254319}]",
    },
    {
        "sidx: version=1",
        new shiguredo::mp4::box::Sidx(
            {.version = 1,
             .reference_id = 0x01234567,
             .timescale = 0x23456789,
             .earliest_presentation_time = 0x0123456789abcdef,
             .first_offset = 0x23456789abcdef01,
             .references = {shiguredo::mp4::box::SidxReference({.reference_type = false,
                                                                .reference_size = 0x01234567,
                                                                .subsegument_duration = 0x23456789,
                                                                .starts_with_sap = true,
                                                                .sap_type = 6,
                                                                .sap_delta_time = 0x09abcdef}),
                            shiguredo::mp4::box::SidxReference({.reference_type = true,
                                                                .reference_size = 0x01234567,
                                                                .subsegument_duration = 0x23456789,
                                                                .starts_with_sap = false,
                                                                .sap_type = 5,
                                                                .sap_delta_time = 0x09abcdef})}}),
        new shiguredo::mp4::box::Sidx(),
        {
            1,                                               // version
            0x00, 0x00, 0x00,                                // flags
            0x01, 0x23, 0x45, 0x67,                          // ReferenceID
            0x23, 0x45, 0x67, 0x89,                          // Timescale
            0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,  // EarliestPresentationTime
            0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01,  // FirstOffset
            0x00, 0x00,                                      // Reserved
            0x00, 0x2,                                       // ReferenceCount
            0x01, 0x23, 0x45, 0x67,                          // ReferenceType, ReferencedSize
            0x23, 0x45, 0x67, 0x89,                          // SubsegmentDuration
            0xe9, 0xab, 0xcd, 0xef,                          // StartsWithSAP, SAPType, SAPDeltaTime
            0x81, 0x23, 0x45, 0x67,                          // ReferenceType, ReferencedSize
            0x23, 0x45, 0x67, 0x89,                          // SubsegmentDuration
            0x59, 0xab, 0xcd, 0xef,                          // StartsWithSAP, SAPType, SAPDeltaTime
        },
        "Version=1 Flags=0x000000 ReferenceID=19088743 "
        "Timescale=591751049 "
        "EarliestPresentationTime=81985529216486895 "
        "FirstOffset=2541551405711093505 ReferenceCount=2 "
        "References=[{ReferenceType=false ReferencedSize=19088743 "
        "SubsegmentDuration=591751049 StartsWithSAP=true SAPType=6 "
        "SAPDeltaTime=162254319}, {ReferenceType=true "
        "ReferencedSize=19088743 SubsegmentDuration=591751049 "
        "StartsWithSAP=false SAPType=5 SAPDeltaTime=162254319}]",
    },
    {
        "sinf",
        new shiguredo::mp4::box::Sinf(),
        new shiguredo::mp4::box::Sinf(),
        {},
        "",
    },
    {
        "smhd",
        new shiguredo::mp4::box::Smhd({.balance = 0x0123}),
        new shiguredo::mp4::box::Smhd(),
        {
            0,                 // version
            0x00, 0x00, 0x00,  // flags
            0x01, 0x23,        // balance
            0x00, 0x00,        // reserved
        },
        "Version=0 Flags=0x000000 Balance=1.14",
    },
    {
        "stbl",
        new shiguredo::mp4::box::Stbl(),
        new shiguredo::mp4::box::Stbl(),
        {},
        "",
    },
    {
        "stco",
        new shiguredo::mp4::box::Stco({.chunk_offsets = {0x01234567, 0x89abcdef}}),
        new shiguredo::mp4::box::Stco(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x01, 0x23, 0x45, 0x67,  // chunk offset
            0x89, 0xab, 0xcd, 0xef,  // chunk offset
        },
        "Version=0 Flags=0x000000 EntryCount=2 ChunkOffsets=[0x1234567, 0x89abcdef]",
    },
    {
        "stsc",
        new shiguredo::mp4::box::Stsc({.entries =
                                           {
                                               shiguredo::mp4::box::StscEntry({.first_chunk = 0x01234567,
                                                                               .samples_per_chunk = 0x23456789,
                                                                               .sample_description_index = 0x456789ab}),
                                               shiguredo::mp4::box::StscEntry({.first_chunk = 0x6789abcd,
                                                                               .samples_per_chunk = 0x89abcdef,
                                                                               .sample_description_index = 0xabcdef01}),
                                           }}),
        new shiguredo::mp4::box::Stsc(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x01, 0x23, 0x45, 0x67,  // first chunk
            0x23, 0x45, 0x67, 0x89,  // sample per chunk
            0x45, 0x67, 0x89, 0xab,  // sample description index
            0x67, 0x89, 0xab, 0xcd,  // first chunk
            0x89, 0xab, 0xcd, 0xef,  // sample per chunk
            0xab, 0xcd, 0xef, 0x01,  // sample description index
        },
        "Version=0 Flags=0x000000 EntryCount=2 Entries=[{FirstChunk=19088743 SamplesPerChunk=591751049 "
        "SampleDescriptionIndex=1164413355}, {FirstChunk=1737075661 SamplesPerChunk=2309737967 "
        "SampleDescriptionIndex=2882400001}]",
    },
    {
        "stsd",
        new shiguredo::mp4::box::Stsd({.entry_count = 0x01234567}),
        new shiguredo::mp4::box::Stsd(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x01, 0x23, 0x45, 0x67,  // entry count
        },
        "Version=0 Flags=0x000000 EntryCount=19088743",
    },
    {
        "stss",
        new shiguredo::mp4::box::Stss({.sample_numbers = {0x01234567, 0x89abcdef}}),
        new shiguredo::mp4::box::Stss(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x01, 0x23, 0x45, 0x67,  // sample number
            0x89, 0xab, 0xcd, 0xef,  // sample number
        },
        "Version=0 Flags=0x000000 EntryCount=2 SampleNumbers=[19088743, 2309737967]",
    },
    {
        "stsz: common sample size",
        new shiguredo::mp4::box::Stsz({.sample_size = 0x01234567, .entry_sizes = {}}),
        new shiguredo::mp4::box::Stsz(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x01, 0x23, 0x45, 0x67,  // sample size
            0x00, 0x00, 0x00, 0x00,  // sample count
        },
        "Version=0 Flags=0x000000 SampleSize=19088743 SampleCount=0 EntrySizes=[]",
    },
    {
        "stsz: sample size array",
        new shiguredo::mp4::box::Stsz({.sample_size = 0, .entry_sizes = {0x01234567, 0x23456789}}),
        new shiguredo::mp4::box::Stsz(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x00, 0x00, 0x00, 0x00,  // sample size
            0x00, 0x00, 0x00, 0x02,  // sample count
            0x01, 0x23, 0x45, 0x67,  // entry size
            0x23, 0x45, 0x67, 0x89,  // entry size
        },
        "Version=0 Flags=0x000000 SampleSize=0 SampleCount=2 EntrySizes=[19088743, 591751049]",
    },
    {
        "stts",
        new shiguredo::mp4::box::Stts(
            {.entries =
                 {
                     shiguredo::mp4::box::SttsEntry({.sample_count = 0x01234567, .sample_duration = 0x23456789}),
                     shiguredo::mp4::box::SttsEntry({.sample_count = 0x456789ab, .sample_duration = 0x6789abcd}),
                 }}),
        new shiguredo::mp4::box::Stts(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x00, 0x00, 0x00, 0x02,  // entry count
            0x01, 0x23, 0x45, 0x67,  // sample count
            0x23, 0x45, 0x67, 0x89,  // sample delta
            0x45, 0x67, 0x89, 0xab,  // sample count
            0x67, 0x89, 0xab, 0xcd,  // sample delta
        },
        "Version=0 Flags=0x000000 EntryCount=2 Entries=[{SampleCount=19088743 SampleDuration=591751049}, "
        "{SampleCount=1164413355 SampleDuration=1737075661}]",
    },
    {
        "styp",
        new shiguredo::mp4::box::Styp({.major_brand = {'a', 'b', 'e', 'm'},
                                       .minor_version = 0x12345678,
                                       .compatible_brands =
                                           {
                                               {'a', 'b', 'c', 'd'},
                                               {'e', 'f', 'g', 'h'},
                                           }}),
        new shiguredo::mp4::box::Styp(),
        {
            'a', 'b', 'e', 'm',      // major brand
            0x12, 0x34, 0x56, 0x78,  // minor version
            'a', 'b', 'c', 'd',      // compatible brand
            'e', 'f', 'g', 'h',      // compatible brand
        },
        R"(MajorBrand="abem" MinorVersion=305419896 CompatibleBrands=[{CompatibleBrand="abcd"}, {CompatibleBrand="efgh"}])",
    },
    {
        "tfdt: version 0",
        new shiguredo::mp4::box::Tfdt({.base_media_decode_time = 0x01234567}),
        new shiguredo::mp4::box::Tfdt(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x01, 0x23, 0x45, 0x67,  // base media decode time
        },
        "Version=0 Flags=0x000000 BaseMediaDecodeTime=19088743",
    },
    {
        "tfdt: version 1",
        new shiguredo::mp4::box::Tfdt({.version = 1, .base_media_decode_time = 0x0123456789abcdef}),
        new shiguredo::mp4::box::Tfdt(),
        {
            1,                                               // version
            0x00, 0x00, 0x00,                                // flags
            0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,  // base media decode time
        },
        "Version=1 Flags=0x000000 BaseMediaDecodeTime=81985529216486895",
    },
    {
        "tfhd: no flags",
        new shiguredo::mp4::box::Tfhd({.track_id = 0x08404649}),
        new shiguredo::mp4::box::Tfhd(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x08, 0x40, 0x46, 0x49,  // track ID
        },
        "Version=0 Flags=0x000000 TrackID=138430025",
    },
    {
        "tfhd: base data offset & default sample duration",
        new shiguredo::mp4::box::Tfhd({
            .flags =
                shiguredo::mp4::box::TfhdBaseDataOffsetPresent | shiguredo::mp4::box::TfhdDefaultSampleDurationPresent,
            .track_id = 0x08404649,
            .base_data_offset = 0x0123456789abcdef,
            .default_sample_duration = 0x23456789,
        }),
        new shiguredo::mp4::box::Tfhd(),
        {
            0,                                               // version
            0x00, 0x00, 0x09,                                // flags (0000 0000 1001)
            0x08, 0x40, 0x46, 0x49,                          // track ID
            0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,  // base data offset
            0x23, 0x45, 0x67, 0x89,                          // default sample duration
        },
        "Version=0 Flags=0x000009 TrackID=138430025 BaseDataOffset=81985529216486895 "
        "DefaultSampleDuration=591751049",
    },
    {
        "tfra: version 0",
        new shiguredo::mp4::box::Tfra({
            .track_id = 0x11111111,
            .length_size_of_traf_num = 0x1,
            .length_size_of_trun_num = 0x2,
            .length_size_of_sample_num = 0x3,
            .entries = {shiguredo::mp4::box::TfraEntry({
                            .time = 0x22222222,
                            .moof_offset = 0x33333333,
                            .traf_number = 0x4444,
                            .trun_number = 0x555555,
                            .sample_number = 0x66666666,
                        }),
                        shiguredo::mp4::box::TfraEntry({
                            .time = 0x77777777,
                            .moof_offset = 0x88888888,
                            .traf_number = 0x9999,
                            .trun_number = 0xaaaaaa,
                            .sample_number = 0xbbbbbbbb,
                        })},
        }),
        new shiguredo::mp4::box::Tfra(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x11, 0x11, 0x11, 0x11,  // trackID
            0x00, 0x00, 0x00, 0x1b,  // reserved lengthSizeOfTrafNum lengthSizeOfTrunNum lengthSizeOfSampleNum
            0x00, 0x00, 0x00, 0x02,  // numberOfEntry
            0x22, 0x22, 0x22, 0x22,  // time
            0x33, 0x33, 0x33, 0x33,  // moofOffset
            0x44, 0x44,              // trafNumber
            0x55, 0x55, 0x55,        // trunNumber
            0x66, 0x66, 0x66, 0x66,  // sampleNumber
            0x77, 0x77, 0x77, 0x77,  // time
            0x88, 0x88, 0x88, 0x88,  // moofOffset
            0x99, 0x99,              // trafNumber
            0xaa, 0xaa, 0xaa,        // trunNumber
            0xbb, 0xbb, 0xbb, 0xbb,  // sampleNumber
        },
        "Version=0 Flags=0x000000 TrackID=286331153 LengthSizeOfTrafNum=0x1 LengthSizeOfTrunNum=0x2 "
        "LengthSizeOfSampleNum=0x3 NumberOfEntry=2 Entries=[{Time=572662306 MoofOffset=858993459 TrafNumber=17476 "
        "TrunNumber=5592405 SampleNumber=1717986918}, {Time=2004318071 MoofOffset=2290649224 TrafNumber=39321 "
        "TrunNumber=11184810 SampleNumber=3149642683}]",
    },
    {
        "tfra: version 1",
        new shiguredo::mp4::box::Tfra(
            {
                .version = 1,
                .track_id = 0x11111111,
                .length_size_of_traf_num = 0x1,
                .length_size_of_trun_num = 0x2,
                .length_size_of_sample_num = 0x3,
                .entries =
                    {
                        shiguredo::mp4::box::TfraEntry(
                            {
                                .time = 0x2222222222222222,
                                .moof_offset = 0x3333333333333333,
                                .traf_number = 0x4444,
                                .trun_number = 0x555555,
                                .sample_number = 0x66666666,
                            }),
                        shiguredo::mp4::box::TfraEntry(
                            {
                                .time = 0x7777777777777777,
                                .moof_offset = 0x8888888888888888,
                                .traf_number = 0x9999,
                                .trun_number = 0xaaaaaa,
                                .sample_number = 0xbbbbbbbb,
                            }),
                    },
            }),
        new shiguredo::mp4::box::Tfra(),
        {
            1,                       // version
            0x00, 0x00, 0x00,        // flags
            0x11, 0x11, 0x11, 0x11,  // trackID
            0x00, 0x00, 0x00, 0x1b,  // reserved lengthSizeOfTrafNum lengthSizeOfTrunNum lengthSizeOfSampleNum
            0x00, 0x00, 0x00, 0x02,  // numberOfEntry
            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,  // time
            0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,  // moofOffset
            0x44, 0x44,                                      // trafNumber
            0x55, 0x55, 0x55,                                // trunNumber
            0x66, 0x66, 0x66, 0x66,                          // sampleNumber
            0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,  // time
            0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,  // moofOffset
            0x99, 0x99,                                      // trafNumber
            0xaa, 0xaa, 0xaa,                                // trunNumber
            0xbb, 0xbb, 0xbb, 0xbb,                          // sampleNumber
        },
        "Version=1 Flags=0x000000 TrackID=286331153 LengthSizeOfTrafNum=0x1 LengthSizeOfTrunNum=0x2 "
        "LengthSizeOfSampleNum=0x3 NumberOfEntry=2 Entries=[{Time=2459565876494606882 "
        "MoofOffset=3689348814741910323 TrafNumber=17476 TrunNumber=5592405 SampleNumber=1717986918}, "
        "{Time=8608480567731124087 MoofOffset=9838263505978427528 TrafNumber=39321 TrunNumber=11184810 "
        "SampleNumber=3149642683}]",
    },
    {
        "tkhd: version 0",
        new shiguredo::mp4::box::Tkhd({.creation_time = 0x01234567,
                                       .modification_time = 0x12345678,
                                       .track_id = 0x23456789,
                                       .duration = 0x456789ab,
                                       .layer = 23456,             // 0x5ba0
                                       .alternate_group = -23456,  // 0xdba0
                                       .volume = 0x0100,
                                       .matrix =
                                           {
                                               0x00010000,
                                               0,
                                               0,
                                               0,
                                               0x00010000,
                                               0,
                                               0,
                                               0,
                                               0x40000000,
                                           },
                                       .width = 125829120,
                                       .height = 70778880}),
        new shiguredo::mp4::box::Tkhd(),
        {
            0,                                               // version
            0x00, 0x00, 0x00,                                // flags
            0x01, 0x23, 0x45, 0x67,                          // creation time
            0x12, 0x34, 0x56, 0x78,                          // modification time
            0x23, 0x45, 0x67, 0x89,                          // track ID
            0x00, 0x00, 0x00, 0x00,                          // reserved
            0x45, 0x67, 0x89, 0xab,                          // duration
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x5b, 0xa0,                                      // layer
            0xa4, 0x60,                                      // alternate group
            0x01, 0x00,                                      // volume
            0x00, 0x00,                                      // reserved
            0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,  // matrix
            0x07, 0x80, 0x00, 0x00,                                                  // width
            0x04, 0x38, 0x00, 0x00,                                                  // height
        },
        R"(Version=0 Flags=0x000000 CreationTime="1904-08-08T22:25:43Z" ModificationTime="1913-09-04T22:51:36Z" TrackID=591751049 )"
        "Duration=1164413355 Layer=23456 AlternateGroup=-23456 Volume=256 Matrix=[0x10000, 0x0, 0x0, 0x0, 0x10000, "
        "0x0, 0x0, 0x0, 0x40000000] Width=1920.0000 Height=1080.0000",
    },
    {
        "tkhd: version 1",
        new shiguredo::mp4::box::Tkhd({.version = 1,
                                       .creation_time = 0x01234567,
                                       .modification_time = 0x12345678,
                                       .track_id = 0x23456789,
                                       .duration = 0x456789ab,
                                       .layer = 23456,             // 0x5ba0
                                       .alternate_group = -23456,  // 0xdba0
                                       .volume = 0x0100,
                                       .matrix =
                                           {
                                               0x00010000,
                                               0,
                                               0,
                                               0,
                                               0x00010000,
                                               0,
                                               0,
                                               0,
                                               0x40000000,
                                           },
                                       .width = 125829120,
                                       .height = 70778880}),
        new shiguredo::mp4::box::Tkhd(),
        {
            1,                                               // version
            0x00, 0x00, 0x00,                                // flags
            0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0x45, 0x67,  // creation time
            0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x56, 0x78,  // modification time
            0x23, 0x45, 0x67, 0x89,                          // track ID
            0x00, 0x00, 0x00, 0x00,                          // reserved
            0x00, 0x00, 0x00, 0x00, 0x45, 0x67, 0x89, 0xab,  // duration
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // reserved
            0x5b, 0xa0,                                      // layer
            0xa4, 0x60,                                      // alternate group
            0x01, 0x00,                                      // volume
            0x00, 0x00,                                      // reserved
            0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,  // matrix
            0x07, 0x80, 0x00, 0x00,                                                  // width
            0x04, 0x38, 0x00, 0x00,                                                  // height
        },
        R"(Version=1 Flags=0x000000 CreationTime="1904-08-08T22:25:43Z" ModificationTime="1913-09-04T22:51:36Z" TrackID=591751049 )"
        "Duration=1164413355 Layer=23456 AlternateGroup=-23456 Volume=256 Matrix=[0x10000, 0x0, 0x0, 0x0, 0x10000, "
        "0x0, 0x0, 0x0, 0x40000000] Width=1920.0000 Height=1080.0000",
    },
    {"traf", new shiguredo::mp4::box::Traf(), new shiguredo::mp4::box::Traf(), {}, ""},
    {"trak", new shiguredo::mp4::box::Trak(), new shiguredo::mp4::box::Trak(), {}, ""},
    {
        "trex",
        new shiguredo::mp4::box::Trex({.track_id = 0x01234567,
                                       .default_sample_description_index = 0x23456789,
                                       .default_sample_duration = 0x456789ab,
                                       .default_sample_size = 0x6789abcd,
                                       .default_sample_flags = 0x89abcdef}),
        new shiguredo::mp4::box::Trex(),
        {
            0,                       // version
            0x00, 0x00, 0x00,        // flags
            0x01, 0x23, 0x45, 0x67,  // track ID
            0x23, 0x45, 0x67, 0x89,  // default sample description index
            0x45, 0x67, 0x89, 0xab,  // default sample duration
            0x67, 0x89, 0xab, 0xcd,  // default sample size
            0x89, 0xab, 0xcd, 0xef,  // default sample flags
        },
        "Version=0 Flags=0x000000 TrackID=19088743 DefaultSampleDescriptionIndex=591751049 "
        "DefaultSampleDuration=1164413355 DefaultSampleSize=1737075661 DefaultSampleFlags=0x89abcdef",
    },
    {
        "trun: version=0 flag=0x101",
        new shiguredo::mp4::box::
            Trun(
                {.flags = 0x101,
                 .data_offset = 50,
                 .entries =
                     {
                         shiguredo::mp4::box::TrunEntry(
                             {
                                 .sample_duration = 100,
                             }),
                         shiguredo::mp4::box::TrunEntry(
                             {
                                 .sample_duration = 101,
                             }),
                         shiguredo::mp4::box::TrunEntry(
                             {
                                 .sample_duration = 102,
                             }),
                     }}),
        new shiguredo::mp4::box::Trun{},
        {
            0,                       // version
            0x00, 0x01, 0x01,        // flags
            0x00, 0x00, 0x00, 0x03,  // sample count
            0x00, 0x00, 0x00, 0x32,  // data offset
            0x00, 0x00, 0x00, 0x64,  // sample duration
            0x00, 0x00, 0x00, 0x65,  // sample duration
            0x00, 0x00, 0x00, 0x66,  // sample duration
        },
        "Version=0 Flags=0x000101 SampleCount=3 DataOffset=50 Entries=[{SampleDuration=100}, {SampleDuration=101}, "
        "{SampleDuration=102}]",
    },
    {
        "trun: version=0 flag=0x204",
        new shiguredo::mp4::box::
            Trun(
                {.flags = 0x204,
                 .first_sample_flags = 0x02468ace,
                 .entries =
                     {
                         shiguredo::mp4::box::TrunEntry(
                             {
                                 .sample_size = 100,
                             }),
                         shiguredo::mp4::box::TrunEntry(
                             {
                                 .sample_size = 101,
                             }),
                         shiguredo::mp4::box::TrunEntry(
                             {
                                 .sample_size = 102,
                             }),
                     }}),
        new shiguredo::mp4::box::Trun{},
        {
            0,                       // version
            0x00, 0x02, 0x04,        // flags
            0x00, 0x00, 0x00, 0x03,  // sample count
            0x02, 0x46, 0x8a, 0xce,  // first sample flags
            0x00, 0x00, 0x00, 0x64,  // sample size
            0x00, 0x00, 0x00, 0x65,  // sample size
            0x00, 0x00, 0x00, 0x66,  // sample size
        },
        "Version=0 Flags=0x000204 SampleCount=3 FirstSampleFlags=0x2468ace Entries=[{SampleSize=100}, "
        "{SampleSize=101}, {SampleSize=102}]",
    },
    {
        "trun: version=0 flag=0xc00",
        new shiguredo::mp4::box::Trun(
            {.flags = 0xc00,
             .entries =
                 {
                     shiguredo::mp4::box::TrunEntry({.sample_flags = 100, .sample_composition_time_offset = 200}),
                     shiguredo::mp4::box::TrunEntry({.sample_flags = 101, .sample_composition_time_offset = 201}),
                     shiguredo::mp4::box::TrunEntry({.sample_flags = 102, .sample_composition_time_offset = 202}),
                 }}),
        new shiguredo::mp4::box::Trun(),
        {
            0,                       // version
            0x00, 0x0c, 0x00,        // flags
            0x00, 0x00, 0x00, 0x03,  // sample count
            0x00, 0x00, 0x00, 0x64,  // sample flags
            0x00, 0x00, 0x00, 0xc8,  // sample composition time offset
            0x00, 0x00, 0x00, 0x65,  // sample flags
            0x00, 0x00, 0x00, 0xc9,  // sample composition time offset
            0x00, 0x00, 0x00, 0x66,  // sample flags
            0x00, 0x00, 0x00, 0xca,  // sample composition time offset
        },
        "Version=0 Flags=0x000c00 SampleCount=3 Entries=[{SampleFlags=0x64 SampleCompositionTimeOffset=200}, "
        "{SampleFlags=0x65 SampleCompositionTimeOffset=201}, {SampleFlags=0x66 SampleCompositionTimeOffset=202}]",
    },
    {"trun: version=1 flag=0x800",
     new shiguredo::mp4::box::Trun({.version = 1,
                                    .flags = 0x800,
                                    .entries =
                                        {
                                            shiguredo::mp4::box::TrunEntry({.sample_composition_time_offset = 200}),
                                            shiguredo::mp4::box::TrunEntry({.sample_composition_time_offset = 201}),
                                            shiguredo::mp4::box::TrunEntry({.sample_composition_time_offset = -202}),
                                        }}),
     new shiguredo::mp4::box::Trun(),
     {
         1,                       // version
         0x00, 0x08, 0x00,        // flags
         0x00, 0x00, 0x00, 0x03,  // sample count
         0x00, 0x00, 0x00, 0xc8,  // sample composition time offset
         0x00, 0x00, 0x00, 0xc9,  // sample composition time offset
         0xff, 0xff, 0xff, 0x36,  // sample composition time offset
     },
     "Version=1 Flags=0x000800 SampleCount=3 Entries=[{SampleCompositionTimeOffset=200}, "
     "{SampleCompositionTimeOffset=201}, {SampleCompositionTimeOffset=-202}]"},
    {"udta", new shiguredo::mp4::box::Udta(), new shiguredo::mp4::box::Udta(), {}, ""},
    {"vmhd",
     new shiguredo::mp4::box::Vmhd({.graphicsmode = 0x0123, .opcolor = {0x2345, 0x4567, 0x6789}}),
     new shiguredo::mp4::box::Vmhd(),
     {
         0,                                   // version
         0x00, 0x00, 0x00,                    // flags
         0x01, 0x23,                          // graphics mode
         0x23, 0x45, 0x45, 0x67, 0x67, 0x89,  // opcolor
     },
     "Version=0 Flags=0x000000 Graphicsmode=291 Opcolor=[9029, 17767, 26505]"},
    {"dOps",
     new shiguredo::mp4::box::DOps({
         .output_channel_count = 2,
         .pre_skip = 312,
         .input_sample_rate = 48000,
         .output_gain = 0,
         .channel_mapping_family = 0,
     }),
     new shiguredo::mp4::box::DOps(),
     {0, 2, 0x1, 0x38, 0x00, 0x00, 0xbb, 0x80, 0x00, 0x00, 0x00},
     "Version=0 OutputChannelCount=2 PreSkip=312 InputSampleRate=48000 OutputGain=0 ChannelMappingFamily=0"},
    {"btrt",
     new shiguredo::mp4::box::Btrt({.decoding_buffer_size = 586, .max_bitrate = 125216, .avg_bitrate = 115336}),
     new shiguredo::mp4::box::Btrt(),
     {0x00, 0x00, 0x02, 0x4a, 0x00, 0x01, 0xe9, 0x20, 0x00, 0x01, 0xc2, 0x88},
     "DecodingBufferSize=586 MaxBitrate=125216 AvgBitrate=115336"},
    {"iods",
     new shiguredo::mp4::box::Iods({.type_tag = 0x10,
                                    .OD_ID = 79,
                                    .OD_profile_level = 0xff,
                                    .scene_profile_level = 0xff,
                                    .audio_profile_level = 0xfe,
                                    .video_profile_level = 0xff,
                                    .graphics_profile_level = 0xff}),
     new shiguredo::mp4::box::Iods(),
     {0x00, 0x00, 0x00, 0x00, 0x10, 0x07, 0x00, 0x4f, 0xff, 0xff, 0xfe, 0xff, 0xff},
     "Version=0 Flags=0x000000 TypeTag=0x10 ODID=79 ODProfileLevel=0xff SceneProfileLevel=0xff "
     "AudioProfileLevel=0xfe "
     "VideoProfileLevel=0xff GraphicsProfileLevel=0xff"},
    {"vpcC",
     new shiguredo::mp4::box::VPCodecConfiguration({.profile = 0,
                                                    .level = 0,
                                                    .bit_depth = 8,
                                                    .chroma_sub_sampling = 0,
                                                    .video_full_range_flag = 0,
                                                    .colour_primaries = 0xff,
                                                    .transfer_characteristics = 0xff,
                                                    .matrix_coefficents = 0}),
     new shiguredo::mp4::box::VPCodecConfiguration(),
     {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x00, 0x00, 0x00},
     "Version=1 Flags=0x000000 Profile=0 Level=0 BitDepth=8 ChromaSubSampling=0 VideoFullRangeFlag=0 "
     "ColourPrimaries=255 TransferCharacteristics=255 MatrixCoefficents=0 CodecInitializationData=[]"},
    {"vpcC version: 0",
     new shiguredo::mp4::box::VPCodecConfiguration({.version = 0,
                                                    .profile = 0,
                                                    .level = 0,
                                                    .bit_depth = 8,
                                                    .chroma_sub_sampling = 0,
                                                    .video_full_range_flag = 0,
                                                    .colour_primaries = 0xff,
                                                    .transfer_characteristics = 0xff,
                                                    .matrix_coefficents = 0}),
     new shiguredo::mp4::box::VPCodecConfiguration(),
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x00},
     "Version=0 Flags=0x000000 Profile=0 Level=0 BitDepth=8 ChromaSubSampling=0 VideoFullRangeFlag=0 "
     "ColourPrimaries=255 TransferCharacteristics=255 MatrixCoefficents=0 CodecInitializationData=[]"},
    {
        "sdtp",
        new shiguredo::mp4::box::Sdtp({.samples =
                                           {
                                               shiguredo::mp4::box::SdtpSample({.is_leading = 0,
                                                                                .sample_depends_on = 0,
                                                                                .sample_is_depended_on = 0,
                                                                                .sample_has_redundancy = 0}),
                                               shiguredo::mp4::box::SdtpSample({.is_leading = 0,
                                                                                .sample_depends_on = 1,
                                                                                .sample_is_depended_on = 2,
                                                                                .sample_has_redundancy = 3}),
                                               shiguredo::mp4::box::SdtpSample({.is_leading = 3,
                                                                                .sample_depends_on = 2,
                                                                                .sample_is_depended_on = 1,
                                                                                .sample_has_redundancy = 0}),
                                           }}),
        new shiguredo::mp4::box::Sdtp(),
        {
            0,                 // version
            0x00, 0x00, 0x00,  // flags
            0x00,              // 0<<6 + 0<<4 + 0<<2 + 0,
            0x1b,              // 0<<6 + 1<<4 + 2<<2 + 3,
            0xe4,              // 3<<6 + 2<<4 + 1<<2 + 0,
        },
        "Version=0 Flags=0x000000 Samples=[{IsLeading=0x0 SampleDependsOn=0x0 SampleIsDependedOn=0x0 "
        "SampleHasRedundancy=0x0}, "
        "{IsLeading=0x0 SampleDependsOn=0x1 SampleIsDependedOn=0x2 SampleHasRedundancy=0x3}, "
        "{IsLeading=0x3 SampleDependsOn=0x2 SampleIsDependedOn=0x1 SampleHasRedundancy=0x0}]",
    },
    {
        "ilst meta",
        new shiguredo::mp4::box::IlstMeta({.type = shiguredo::mp4::BoxType("----")}),
        new shiguredo::mp4::box::IlstMeta(),
        {},
        "",
    },
    {
        "ilst data (binary)",
        new shiguredo::mp4::box::Data(
            {.data_type = shiguredo::mp4::box::DataType::Binary, .data_lang = 0x12345678, .data = {'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x00,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x66, 0x6f, 0x6f,        // data
        },
        "DataType=BINARY DataLang=305419896 Data=[0x66, 0x6f, 0x6f]",
    },
    {
        "ilst data (utf8)",
        new shiguredo::mp4::box::Data(
            {.data_type = shiguredo::mp4::box::DataType::StringUTF8, .data_lang = 0x12345678, .data = {'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x01,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x66, 0x6f, 0x6f,        // data
        },
        R"(DataType=UTF8 DataLang=305419896 Data="foo")",
    },
    {
        "ilst data (utf8 escaped)",
        new shiguredo::mp4::box::Data({.data_type = shiguredo::mp4::box::DataType::StringUTF8,
                                       .data_lang = 0x12345678,
                                       .data = {0x00, 'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x01,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x00, 0x66, 0x6f, 0x6f,  // data
        },
        R"(DataType=UTF8 DataLang=305419896 Data=".foo")",
    },
    {
        "ilst data (utf16)",
        new shiguredo::mp4::box::Data({.data_type = shiguredo::mp4::box::DataType::StringUTF16,
                                       .data_lang = 0x12345678,
                                       .data = {'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x02,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x66, 0x6f, 0x6f,        // data
        },
        "DataType=UTF16 DataLang=305419896 Data=[0x66, 0x6f, 0x6f]",
    },
    {
        "ilst data (utf16)",
        new shiguredo::mp4::box::Data(
            {.data_type = shiguredo::mp4::box::DataType::StringMac, .data_lang = 0x12345678, .data = {'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x03,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x66, 0x6f, 0x6f,        // data
        },
        "DataType=MAC_STR DataLang=305419896 Data=[0x66, 0x6f, 0x6f]",
    },
    {
        "ilst data (jpsg)",
        new shiguredo::mp4::box::Data(
            {.data_type = shiguredo::mp4::box::DataType::JPEG, .data_lang = 0x12345678, .data = {'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x0e,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x66, 0x6f, 0x6f,        // data
        },
        "DataType=JPEG DataLang=305419896 Data=[0x66, 0x6f, 0x6f]",
    },
    {
        "ilst data (int)",
        new shiguredo::mp4::box::Data({.data_type = shiguredo::mp4::box::DataType::SignedIntBigEndian,
                                       .data_lang = 0x12345678,
                                       .data = {'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x15,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x66, 0x6f, 0x6f,        // data
        },
        "DataType=INT DataLang=305419896 Data=[0x66, 0x6f, 0x6f]",
    },
    {
        "ilst data (float32)",
        new shiguredo::mp4::box::Data({.data_type = shiguredo::mp4::box::DataType::Float32BigEndian,
                                       .data_lang = 0x12345678,
                                       .data = {'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x16,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x66, 0x6f, 0x6f,        // data
        },
        "DataType=FLOAT32 DataLang=305419896 Data=[0x66, 0x6f, 0x6f]",
    },
    {
        "ilst data (float64)",
        new shiguredo::mp4::box::Data({.data_type = shiguredo::mp4::box::DataType::Float64BigEndian,
                                       .data_lang = 0x12345678,
                                       .data = {'f', 'o', 'o'}}),
        new shiguredo::mp4::box::Data(),
        {
            0x00, 0x00, 0x00, 0x17,  // data type
            0x12, 0x34, 0x56, 0x78,  // data lang
            0x66, 0x6f, 0x6f,        // data
        },
        "DataType=FLOAT64 DataLang=305419896 Data=[0x66, 0x6f, 0x6f]",
    },
};

BOOST_AUTO_TEST_CASE(box) {
  shiguredo::mp4::BoxMap boxMap;
  shiguredo::mp4::register_box_map(&boxMap);

  for (const auto& tc : box_test_cases) {
    BOOST_TEST_MESSAGE(tc.name);
    BOOST_REQUIRE_EQUAL(tc.str, tc.src->toStringOnlyData());

    std::stringstream ss;
    auto wbits = tc.src->writeData(ss);
    BOOST_REQUIRE_EQUAL(std::size(tc.bin) * 8, wbits);
    BOOST_REQUIRE_EQUAL(std::size(tc.bin), tc.src->getDataSize());

    auto s = ss.str();
    std::vector<std::uint8_t> actual(std::begin(s), std::end(s));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(std::begin(tc.bin), std::end(tc.bin), std::begin(actual), std::end(actual));

    ss.seekg(0, std::ios_base::beg);
    auto rbits = tc.dst->readData(ss);
    BOOST_REQUIRE_EQUAL(std::size(tc.bin) * 8, rbits);
    BOOST_REQUIRE_EQUAL(tc.str, tc.dst->toStringOnlyData());
    BOOST_REQUIRE_EQUAL(std::size(tc.bin), tc.dst->getDataSize());

    auto dst = boxMap.getBoxInstance(tc.src->getType());
    BOOST_REQUIRE(dst != nullptr);

    ss.seekg(0, std::ios_base::beg);
    if (dst) {
      auto rbits2 = dst->readData(ss);
      BOOST_REQUIRE_EQUAL(std::size(tc.bin) * 8, rbits2);
      BOOST_REQUIRE_EQUAL(tc.str, dst->toStringOnlyData());
      BOOST_REQUIRE_EQUAL(std::size(tc.bin), tc.dst->getDataSize());
      delete dst;
    }
    delete tc.src;
    delete tc.dst;
  }
}

BOOST_AUTO_TEST_SUITE_END()
