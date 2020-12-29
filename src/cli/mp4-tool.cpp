#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include "shiguredo/mp4//reader/reader.hpp"

int main(int argc, char** argv) {
  CLI::App app{"mp4-tool"};

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

  auto dump = app.add_subcommand("dump");
  dump->add_option("-f,--file", filename, "filename");

  CLI11_PARSE(app, argc, argv);

  spdlog::set_level(log_level);
  spdlog::set_default_logger(spdlog::stderr_color_mt("mp4-tool"));

  auto subcommands = app.get_subcommands();

  if (subcommands[0] == dump) {
    std::ifstream ifs(filename, std::ios_base::binary);
    shiguredo::mp4::reader::SimpleReader reader(ifs);
    reader.read();
  }

  return 0;
}
