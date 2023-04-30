#include <fmt/core.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include "shiguredo/mp4/track/aac.hpp"
#include "shiguredo/mp4/track/av1.hpp"
#include "shiguredo/mp4/track/mp3.hpp"
#include "shiguredo/mp4/track/opus.hpp"
#include "shiguredo/mp4/track/vpx.hpp"
#include "shiguredo/mp4/writer/faststart_writer.hpp"
#include "shiguredo/mp4/writer/simple_writer.hpp"

#include "resource/resource.hpp"

namespace shiguredo::mp4::track {

class Track;

}

int main(int argc, char** argv) {
  CLI::App app{"mp4-muxer"};

  spdlog::level::level_enum log_level = spdlog::level::info;
  std::vector<std::pair<std::string, spdlog::level::level_enum>> log_level_assoc{
      {"trace", spdlog::level::trace}, {"debug", spdlog::level::debug}, {"info", spdlog::level::info},
      {"warn", spdlog::level::warn},   {"error", spdlog::level::err},   {"critical", spdlog::level::critical},
      {"off", spdlog::level::off},
  };
  app.add_option("-l,--log-level", log_level, "Log level (trace/debug/info/warn/error/critical/off) default: info")
      ->transform(CLI::CheckedTransformer(log_level_assoc, CLI::ignore_case));

  app.require_subcommand(1);

  std::string filename;
  std::string opus_filename;
  std::string vp9_filename;
  std::string vp8_filename;
  std::string av1_filename;
  std::string mp3_filename;
  std::string aac_filename;

  auto csv = app.add_subcommand("csv");
  csv->add_option("-f,--file", filename, "filename");

  auto opus = app.add_subcommand("opus");
  opus->add_option("-f,--file", filename, "filename");
  opus->add_option("--opus", opus_filename, "opus resource filename");

  auto opusvp9 = app.add_subcommand("opusvp9");
  opusvp9->add_option("-f,--file", filename, "filename");
  opusvp9->add_option("--opus", opus_filename, "opus resource filename");
  opusvp9->add_option("--vp9", vp9_filename, "vp9 resource filename");

  auto opusvp9_faststart = app.add_subcommand("opusvp9_faststart");
  opusvp9_faststart->add_option("-f,--file", filename, "filename");
  opusvp9_faststart->add_option("--opus", opus_filename, "opus resource filename");
  opusvp9_faststart->add_option("--vp9", vp9_filename, "vp9 resource filename");

  auto opusav1 = app.add_subcommand("opusav1");
  opusav1->add_option("-f,--file", filename, "filename");
  opusav1->add_option("--opus", opus_filename, "opus resource filename");
  opusav1->add_option("--av1", av1_filename, "av1 resource filename");

  auto mp3vp8 = app.add_subcommand("mp3vp8");
  mp3vp8->add_option("-f,--file", filename, "filename");
  mp3vp8->add_option("--mp3", mp3_filename, "mp3 resource filename");
  mp3vp8->add_option("--vp8", vp8_filename, "vp8 resource filename");

  auto aacvp9 = app.add_subcommand("aacvp9");
  aacvp9->add_option("-f,--file", filename, "filename");
  aacvp9->add_option("--aac", aac_filename, "aac resource filename");
  aacvp9->add_option("--vp9", vp9_filename, "vp9 resource filename");

  auto aacvp9_faststart = app.add_subcommand("aacvp9_faststart");
  aacvp9_faststart->add_option("-f,--file", filename, "filename");
  aacvp9_faststart->add_option("--aac", aac_filename, "aac resource filename");
  aacvp9_faststart->add_option("--vp9", vp9_filename, "vp9 resource filename");

  CLI11_PARSE(app, argc, argv);

  spdlog::set_level(log_level);
  spdlog::set_default_logger(spdlog::stderr_color_mt("mp4-muxer"));

  const auto subcommands = app.get_subcommands();

  if (subcommands[0] == csv) {
    std::vector<Resource> resources;
    load_resources_from_csv(&resources, filename);
    for (const auto& r : resources) {
      std::cout << fmt::format("{} {} {:x} {}", r.timestamp, std::size(r.data), fmt::join(r.data, ""), r.is_key)
                << std::endl;
    }
  } else if (subcommands[0] == opus) {
    std::vector<Resource> resources;
    load_resources_from_csv(&resources, opus_filename);
    std::ofstream ofs(filename, std::ios_base::binary);
    const float duration = 16.0f;
    shiguredo::mp4::writer::SimpleWriter writer(ofs, {.mvhd_timescale = 1000, .duration = duration});
    writer.writeFtypBox();
    shiguredo::mp4::track::OpusTrack opus_trak(
        {.pre_skip = 312, .duration = duration, .track_id = writer.getAndUpdateNextTrackID(), .writer = &writer});
    for (std::size_t i = 0; i < 800; ++i) {
      opus_trak.addData(resources[i].timestamp, resources[i].data, resources[i].is_key);
    }
    writer.appendTrakAndUdtaBoxInfo({&opus_trak});
    writer.writeFreeBoxAndMdatHeader();
    writer.writeMoovBox();
  } else if (subcommands[0] == opusvp9) {
    std::vector<Resource> opus_resources;
    load_resources_from_csv(&opus_resources, opus_filename);
    std::vector<Resource> vp9_resources;
    load_resources_from_csv(&vp9_resources, vp9_filename);
    std::ofstream ofs(filename, std::ios_base::binary);
    const float duration = 16.0f;
    shiguredo::mp4::writer::SimpleWriter writer(ofs, {.mvhd_timescale = 1000, .duration = duration});
    writer.writeFtypBox();
    shiguredo::mp4::track::OpusTrack opus_trak(
        {.pre_skip = 312, .duration = duration, .track_id = writer.getAndUpdateNextTrackID(), .writer = &writer});
    shiguredo::mp4::track::VPXTrack vpx_trak({.timescale = 16000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .width = 640,
                                              .height = 240,
                                              .max_bitrate = 250000,
                                              .avg_bitrate = 250000,
                                              .writer = &writer});
    for (std::size_t s = 0; s < 16; ++s) {
      // chunk length: 1000ms
      for (std::size_t j = 0; j < 50; ++j) {
        const auto i = s * 50 + j;
        opus_trak.addData(opus_resources[i].timestamp, opus_resources[i].data, opus_resources[i].is_key);
      }
      opus_trak.terminateCurrentChunk();
      for (std::size_t j = 0; j < 25; ++j) {
        const auto i = s * 25 + j;
        vpx_trak.addData(vp9_resources[i].timestamp, vp9_resources[i].data, vp9_resources[i].is_key);
      }
      vpx_trak.terminateCurrentChunk();
    }
    writer.appendTrakAndUdtaBoxInfo({&opus_trak, &vpx_trak});
    writer.writeFreeBoxAndMdatHeader();
    writer.writeMoovBox();
  } else if (subcommands[0] == opusav1) {
    std::vector<Resource> opus_resources;
    load_resources_from_csv(&opus_resources, opus_filename);
    std::vector<Resource> av1_resources;
    load_resources_from_csv(&av1_resources, av1_filename);
    std::ofstream ofs(filename, std::ios_base::binary);
    const float duration = 4.0f;
    shiguredo::mp4::writer::SimpleWriter writer(ofs, {.mvhd_timescale = 1000, .duration = duration});
    writer.writeFtypBox();
    shiguredo::mp4::track::OpusTrack opus_trak(
        {.pre_skip = 312, .duration = duration, .track_id = writer.getAndUpdateNextTrackID(), .writer = &writer});
    shiguredo::mp4::track::AV1Track av1_trak({.timescale = 16000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .width = 240,
                                              .height = 160,
                                              .writer = &writer});
    for (std::size_t s = 0; s < 4; ++s) {
      // chunk length: 1000ms
      for (std::size_t j = 0; j < 50; ++j) {
        const auto i = s * 50 + j;
        opus_trak.addData(opus_resources[i].timestamp, opus_resources[i].data, opus_resources[i].is_key);
      }
      opus_trak.terminateCurrentChunk();
      for (std::size_t j = 0; j < 25; ++j) {
        const auto i = s * 25 + j;
        av1_trak.addData(av1_resources[i].timestamp, av1_resources[i].data, av1_resources[i].is_key);
      }
      av1_trak.terminateCurrentChunk();
    }
    writer.appendTrakAndUdtaBoxInfo({&opus_trak, &av1_trak});
    writer.writeFreeBoxAndMdatHeader();
    writer.writeMoovBox();
  } else if (subcommands[0] == mp3vp8) {
    std::vector<Resource> mp3_resources;
    load_resources_from_csv(&mp3_resources, mp3_filename);
    std::vector<Resource> vp8_resources;
    load_resources_from_csv(&vp8_resources, vp8_filename);
    std::ofstream ofs(filename, std::ios_base::binary);
    const float duration = 15.36f;
    shiguredo::mp4::writer::SimpleWriter writer(ofs, {.mvhd_timescale = 1000, .duration = duration});
    writer.writeFtypBox();
    shiguredo::mp4::track::MP3Track mp3_trak({.timescale = 48000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .buffer_size_db = 768,
                                              .max_bitrate = 256000,
                                              .avg_bitrate = 128000,
                                              .writer = &writer});
    shiguredo::mp4::track::VPXTrack vpx_trak({.codec = shiguredo::mp4::track::VPXCodec::VP8,
                                              .timescale = 16000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .width = 640,
                                              .height = 240,
                                              .max_bitrate = 250000,
                                              .avg_bitrate = 250000,
                                              .writer = &writer});
    for (std::size_t s = 0; s < 16; ++s) {
      // chunk length: 960ms
      for (std::size_t j = 0; j < 40; ++j) {
        const auto i = s * 40 + j;
        mp3_trak.addData(mp3_resources[i].timestamp, mp3_resources[i].data, mp3_resources[i].is_key);
      }
      mp3_trak.terminateCurrentChunk();
      for (std::size_t j = 0; j < 24; ++j) {
        const auto i = s * 24 + j;
        vpx_trak.addData(vp8_resources[i].timestamp, vp8_resources[i].data, vp8_resources[i].is_key);
      }
      vpx_trak.terminateCurrentChunk();
    }
    writer.appendTrakAndUdtaBoxInfo({&mp3_trak, &vpx_trak});
    writer.writeFreeBoxAndMdatHeader();
    writer.writeMoovBox();
  } else if (subcommands[0] == aacvp9) {
    std::vector<Resource> aac_resources;
    load_resources_from_csv(&aac_resources, aac_filename);
    std::vector<Resource> vp9_resources;
    load_resources_from_csv(&vp9_resources, vp9_filename);
    std::ofstream ofs(filename, std::ios_base::binary);
    const float duration = 15.36f;
    shiguredo::mp4::writer::SimpleWriter writer(ofs, {.mvhd_timescale = 1000, .duration = duration});
    writer.writeFtypBox();
    shiguredo::mp4::track::AACTrack aac_trak({.timescale = 48000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .buffer_size_db = 0,
                                              .max_bitrate = 64000,
                                              .avg_bitrate = 64000,
                                              .writer = &writer});
    shiguredo::mp4::track::VPXTrack vpx_trak({.timescale = 16000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .width = 640,
                                              .height = 240,
                                              .max_bitrate = 250000,
                                              .avg_bitrate = 250000,
                                              .writer = &writer});
    for (std::size_t s = 0; s < 16; ++s) {
      // chunk length: 960ms
      for (std::size_t j = 0; j < 45; ++j) {
        const auto i = s * 45 + j;
        aac_trak.addData(aac_resources[i].timestamp, aac_resources[i].data, aac_resources[i].is_key);
      }
      aac_trak.terminateCurrentChunk();
      for (std::size_t j = 0; j < 24; ++j) {
        const auto i = s * 24 + j;
        vpx_trak.addData(vp9_resources[i].timestamp, vp9_resources[i].data, vp9_resources[i].is_key);
      }
      vpx_trak.terminateCurrentChunk();
    }
    writer.appendTrakAndUdtaBoxInfo({&aac_trak, &vpx_trak});
    writer.writeFreeBoxAndMdatHeader();
    writer.writeMoovBox();
  } else if (subcommands[0] == opusvp9_faststart) {
    std::vector<Resource> opus_resources;
    load_resources_from_csv(&opus_resources, opus_filename);
    std::vector<Resource> vp9_resources;
    load_resources_from_csv(&vp9_resources, vp9_filename);
    std::ofstream ofs(filename, std::ios_base::binary);
    const float duration = 16.0f;
    shiguredo::mp4::writer::FaststartWriter writer(ofs, {.mvhd_timescale = 1000, .duration = duration});
    writer.writeFtypBox();
    shiguredo::mp4::track::OpusTrack opus_trak(
        {.pre_skip = 312, .duration = duration, .track_id = writer.getAndUpdateNextTrackID(), .writer = &writer});
    shiguredo::mp4::track::VPXTrack vpx_trak({.timescale = 16000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .width = 640,
                                              .height = 240,
                                              .max_bitrate = 250000,
                                              .avg_bitrate = 250000,
                                              .writer = &writer});
    for (std::size_t s = 0; s < 16; ++s) {
      // chunk length: 1000ms
      for (std::size_t j = 0; j < 50; ++j) {
        const auto i = s * 50 + j;
        opus_trak.addData(opus_resources[i].timestamp, opus_resources[i].data, opus_resources[i].is_key);
      }
      opus_trak.terminateCurrentChunk();
      for (std::size_t j = 0; j < 25; ++j) {
        const auto i = s * 25 + j;
        vpx_trak.addData(vp9_resources[i].timestamp, vp9_resources[i].data, vp9_resources[i].is_key);
      }
      vpx_trak.terminateCurrentChunk();
    }

    writer.appendTrakAndUdtaBoxInfo({&opus_trak, &vpx_trak});
    writer.writeMoovBox();
    writer.writeMdatHeader();
    writer.copyMdatData();
  } else if (subcommands[0] == aacvp9_faststart) {
    std::vector<Resource> aac_resources;
    load_resources_from_csv(&aac_resources, aac_filename);
    std::vector<Resource> vp9_resources;
    load_resources_from_csv(&vp9_resources, vp9_filename);
    std::ofstream ofs(filename, std::ios_base::binary);
    const float duration = 15.36f;
    shiguredo::mp4::writer::FaststartWriter writer(ofs, {.mvhd_timescale = 1000, .duration = duration});
    writer.writeFtypBox();
    shiguredo::mp4::track::AACTrack aac_trak({.timescale = 48000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .buffer_size_db = 0,
                                              .max_bitrate = 64000,
                                              .avg_bitrate = 64000,
                                              .writer = &writer});
    shiguredo::mp4::track::VPXTrack vpx_trak({.timescale = 16000,
                                              .duration = duration,
                                              .track_id = writer.getAndUpdateNextTrackID(),
                                              .width = 640,
                                              .height = 240,
                                              .max_bitrate = 250000,
                                              .avg_bitrate = 250000,
                                              .writer = &writer});
    for (std::size_t s = 0; s < 16; ++s) {
      // chunk length: 960 ms
      for (std::size_t j = 0; j < 45; ++j) {
        const auto i = s * 45 + j;
        aac_trak.addData(aac_resources[i].timestamp, aac_resources[i].data, aac_resources[i].is_key);
      }
      aac_trak.terminateCurrentChunk();
      for (std::size_t j = 0; j < 24; ++j) {
        const auto i = s * 24 + j;
        vpx_trak.addData(vp9_resources[i].timestamp, vp9_resources[i].data, vp9_resources[i].is_key);
      }
      vpx_trak.terminateCurrentChunk();
    }
    writer.appendTrakAndUdtaBoxInfo({&aac_trak, &vpx_trak});
    writer.writeMoovBox();
    writer.writeMdatHeader();
    writer.copyMdatData();
  }

  return 0;
}
