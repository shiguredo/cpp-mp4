cmake_minimum_required(VERSION 3.16)

# Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0054 NEW)
# MSVC runtime library flags are selected by an abstraction.
cmake_policy(SET CMP0091 NEW)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

include(cmake/env.cmake)
include(cmake/tools.cmake)

set_cache_string_from_env(BOOST_VERSION 1.83.0 "Boost のバージョン")
set_cache_string_from_env(CLI11_VERSION 2.3.2 "CLI11 のバージョン")
set_cache_string_from_env(FMT_VERSION 9.1.0 "fmt のバージョン")
set_cache_string_from_env(RAPIDCSV_VERSION 8.77 "rapidcsv のバージョン")
set_cache_string_from_env(SPDLOG_VERSION 1.11.0 "spdlog のバージョン")

add_compile_options(
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    )

set(CMAKE_CXX_FLAGS_NATIVE "-O3 -DNDEBUG -march=native -mtune=native")
set(CMAKE_C_FLAGS_NATIVE "-O3 -DNDEBUG -march=native -mtune=native")

set(CMAKE_CXX_FLAGS_PROFILE "-pg -O3 -DNDEBUG -march=native -mtune=native")
set(CMAKE_C_FLAGS_PROFILE "-pg -O3 -DNDEBUG -march=native -mtune=native")

CPMAddPackage(
    NAME boost_assert
    GITHUB_REPOSITORY boostorg/assert
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_bind
    GITHUB_REPOSITORY boostorg/bind
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_config
    GITHUB_REPOSITORY boostorg/config
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_container_hash
    GITHUB_REPOSITORY boostorg/container_hash
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_core
    GITHUB_REPOSITORY boostorg/core
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_describe
    GITHUB_REPOSITORY boostorg/describe
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_detail
    GITHUB_REPOSITORY boostorg/detail
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_function
    GITHUB_REPOSITORY boostorg/function
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_functional
    GITHUB_REPOSITORY boostorg/functional
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_integer
    GITHUB_REPOSITORY boostorg/integer
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_mp11
    GITHUB_REPOSITORY boostorg/mp11
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_preprocessor
    GITHUB_REPOSITORY boostorg/preprocessor
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_static_assert
    GITHUB_REPOSITORY boostorg/static_assert
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_throw_exception
    GITHUB_REPOSITORY boostorg/throw_exception
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_type_index
    GITHUB_REPOSITORY boostorg/type_index
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME boost_type_traits
    GITHUB_REPOSITORY boostorg/type_traits
    GIT_TAG boost-${BOOST_VERSION}
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG ${FMT_VERSION}
    )

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION ${SPDLOG_VERSION}
    )

project(shiguredo::mp4 C CXX)

add_library(shiguredo-mp4)

target_sources(shiguredo-mp4
    PRIVATE
    src/box_info.cpp
    src/bitio/bitio.cpp
    src/bitio/reader.cpp
    src/bitio/writer.cpp
    src/box_types.cpp
    src/endian/endian.cpp
    src/box_header.cpp
    src/box_type.cpp
    src/box_types.cpp
    src/box/av1c.cpp
    src/box/avc.cpp
    src/box/btrt.cpp
    src/box/co64.cpp
    src/box/colr.cpp
    src/box/ctts.cpp
    src/box/data.cpp
    src/box/dinf.cpp
    src/box/dOps.cpp
    src/box/dref.cpp
    src/box/edts.cpp
    src/box/elst.cpp
    src/box/emsg.cpp
    src/box/esds.cpp
    src/box/fiel.cpp
    src/box/free.cpp
    src/box/ftyp.cpp
    src/box/hdlr.cpp
    src/box/ilst.cpp
    src/box/iods.cpp
    src/box/mdat.cpp
    src/box/mdhd.cpp
    src/box/mdia.cpp
    src/box/mehd.cpp
    src/box/meta.cpp
    src/box/mfhd.cpp
    src/box/mfra.cpp
    src/box/mfro.cpp
    src/box/minf.cpp
    src/box/moof.cpp
    src/box/moov.cpp
    src/box/mvex.cpp
    src/box/mvhd.cpp
    src/box/pixel_aspect_ratio.cpp
    src/box/pssh.cpp
    src/box/sample_entry.cpp
    src/box/sbgp.cpp
    src/box/schi.cpp
    src/box/sdtp.cpp
    src/box/sgpd.cpp
    src/box/sidx.cpp
    src/box/sinf.cpp
    src/box/skip.cpp
    src/box/smhd.cpp
    src/box/stbl.cpp
    src/box/stco.cpp
    src/box/stsc.cpp
    src/box/stsd.cpp
    src/box/stss.cpp
    src/box/stsz.cpp
    src/box/stts.cpp
    src/box/styp.cpp
    src/box/tfdt.cpp
    src/box/tfhd.cpp
    src/box/tfra.cpp
    src/box/tkhd.cpp
    src/box/traf.cpp
    src/box/trak.cpp
    src/box/trex.cpp
    src/box/trun.cpp
    src/box/udta.cpp
    src/box/vmhd.cpp
    src/box/url.cpp
    src/box/urn.cpp
    src/box/unsupported.cpp
    src/box/vpc.cpp
    src/box/wave.cpp
    src/box.cpp
    src/box_map.cpp
    src/reader/reader.cpp
    src/stream/stream.cpp
    src/time/time.cpp
    src/track/track.cpp
    src/track/aac.cpp
    src/track/av1.cpp
    src/track/h264.cpp
    src/track/mp3.cpp
    src/track/opus.cpp
    src/track/soun.cpp
    src/track/vide.cpp
    src/track/vpx.cpp
    src/version.cpp
    src/writer/writer.cpp
    src/writer/simple_writer.cpp
    src/writer/faststart_writer.cpp
    )

target_include_directories(shiguredo-mp4
    PRIVATE
    include
    ${boost_assert_SOURCE_DIR}/include
    ${boost_bind_SOURCE_DIR}/include
    ${boost_config_SOURCE_DIR}/include
    ${boost_container_hash_SOURCE_DIR}/include
    ${boost_core_SOURCE_DIR}/include
    ${boost_describe_SOURCE_DIR}/include
    ${boost_detail_SOURCE_DIR}/include
    ${boost_function_SOURCE_DIR}/include
    ${boost_functional_SOURCE_DIR}/include
    ${boost_integer_SOURCE_DIR}/include
    ${boost_mp11_SOURCE_DIR}/include
    ${boost_preprocessor_SOURCE_DIR}/include
    ${boost_static_assert_SOURCE_DIR}/include
    ${boost_throw_exception_SOURCE_DIR}/include
    ${boost_type_index_SOURCE_DIR}/include
    ${boost_type_traits_SOURCE_DIR}/include
    ${fmt_SOURCE_DIR}/include
    ${spdlog_SOURCE_DIR}/include
    )

set_target_properties(shiguredo-mp4 PROPERTIES CXX_STANDARD 20 C_STANDARD 11)

target_link_libraries(shiguredo-mp4
    PRIVATE
    fmt
    spdlog
    )

if(WITH_CLI)

    CPMAddPackage(
        NAME cli11
        URL  https://github.com/CLIUtils/CLI11/archive/v${CLI11_VERSION}.zip
        DOWNLOAD_ONLY YES
        )

    add_executable(mp4-tool)

    target_sources(mp4-tool
        PRIVATE
        src/cli/mp4-tool.cpp
        )

    target_include_directories(mp4-tool
        PRIVATE
        include
        ${boost_assert_SOURCE_DIR}/include
        ${boost_bind_SOURCE_DIR}/include
        ${boost_config_SOURCE_DIR}/include
        ${boost_container_hash_SOURCE_DIR}/include
        ${boost_core_SOURCE_DIR}/include
        ${boost_describe_SOURCE_DIR}/include
        ${boost_detail_SOURCE_DIR}/include
        ${boost_function_SOURCE_DIR}/include
        ${boost_functional_SOURCE_DIR}/include
        ${boost_integer_SOURCE_DIR}/include
        ${boost_mp11_SOURCE_DIR}/include
        ${boost_preprocessor_SOURCE_DIR}/include
        ${boost_static_assert_SOURCE_DIR}/include
        ${boost_throw_exception_SOURCE_DIR}/include
        ${boost_type_index_SOURCE_DIR}/include
        ${boost_type_traits_SOURCE_DIR}/include
        ${cli11_SOURCE_DIR}/include
        ${fmt_SOURCE_DIR}/include
        ${spdlog_SOURCE_DIR}/include
        )

    set_target_properties(mp4-tool PROPERTIES CXX_STANDARD 20 C_STANDARD 11)

    target_link_libraries(mp4-tool
        PRIVATE
        fmt
        spdlog
        shiguredo-mp4
        )
endif()

if(WITH_EXAMPLE)

    CPMAddPackage(
        NAME cli11
        URL  https://github.com/CLIUtils/CLI11/archive/v${CLI11_VERSION}.zip
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME rapidcsv
        GITHUB_REPOSITORY d99kris/rapidcsv
        VERSION ${RAPIDCSV_VERSION}
        DOWNLOAD_ONLY YES
        )

    add_executable(mp4-muxer)

    target_sources(mp4-muxer
        PRIVATE
        example/mp4-muxer.cpp
        example/resource/resource.cpp
        )

    target_include_directories(mp4-muxer
        PRIVATE
        include
        example
        ${boost_assert_SOURCE_DIR}/include
        ${boost_bind_SOURCE_DIR}/include
        ${boost_config_SOURCE_DIR}/include
        ${boost_container_hash_SOURCE_DIR}/include
        ${boost_core_SOURCE_DIR}/include
        ${boost_detail_SOURCE_DIR}/include
        ${boost_function_SOURCE_DIR}/include
        ${boost_functional_SOURCE_DIR}/include
        ${boost_integer_SOURCE_DIR}/include
        ${boost_preprocessor_SOURCE_DIR}/include
        ${boost_static_assert_SOURCE_DIR}/include
        ${boost_throw_exception_SOURCE_DIR}/include
        ${boost_type_index_SOURCE_DIR}/include
        ${boost_type_traits_SOURCE_DIR}/include
        ${cli11_SOURCE_DIR}/include
        ${fmt_SOURCE_DIR}/include
        ${rapidcsv_SOURCE_DIR}/src
        ${spdlog_SOURCE_DIR}/include
        )

    set_target_properties(mp4-muxer PROPERTIES CXX_STANDARD 20 C_STANDARD 11)

    target_link_libraries(mp4-muxer
        PRIVATE
        fmt
        spdlog
        shiguredo-mp4
        )
endif()

if(WITH_TEST)
    enable_testing()

    CPMAddPackage(
        NAME boost_algorithm
        GITHUB_REPOSITORY boostorg/algorithm
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_exception
        GITHUB_REPOSITORY boostorg/exception
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_io
        GITHUB_REPOSITORY boostorg/io
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_iterator
        GITHUB_REPOSITORY boostorg/iterator
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_move
        GITHUB_REPOSITORY boostorg/move
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_mpl
        GITHUB_REPOSITORY boostorg/mpl
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_numeric_conversion
        GITHUB_REPOSITORY boostorg/numeric_conversion
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_range
        GITHUB_REPOSITORY boostorg/range
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_smart_ptr
        GITHUB_REPOSITORY boostorg/smart_ptr
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_test
        GITHUB_REPOSITORY boostorg/test
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )

    CPMAddPackage(
        NAME boost_utility
        GITHUB_REPOSITORY boostorg/utility
        GIT_TAG boost-${BOOST_VERSION}
        DOWNLOAD_ONLY YES
        )


    add_subdirectory(test)
endif()
