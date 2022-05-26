#include <cpparg/cpparg.h>
#include <dbmanager.h>
#include <fmt/chrono.h>
#include <hwstats.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <unwinder.h>


#include <chrono>
#include <compare>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace profiler {

Options ParseOptions(int argc, const char* argv[]) {
    Options options;

    cpparg::parser parser("profiler");

    parser.title("Clickhouse-profiler");

    parser.add('p', "pid")
        .description("Process to trace")
        .required()
        .store(options.Pid);

    parser.add('T', "thread-names")
        .description("Show thread names")
        .flag(options.ThreadNames);

    parser.add('F', "freq")
        .description("Collect samples at this frequency")
        .default_value(0)
        .store(options.Frequency);

    parser.add('l', "lines")
        .description("Show line numbers in output")
        .flag(options.LineNumbers);

    parser.add('d', "debug-info")
        .description("Use separate debug info")
        .optional()
        .handle<std::string>([&options](std::string arg) {
            options.DebugInfo = std::filesystem::path{std::move(arg)};
        });

    parser.add_help('h', "help");

    parser.parse(argc, argv);

    return options;
}

db::dbOptions ParseDBoptions(
    const std::string& config_file_path = "config.json") {
    std::ifstream config_file(config_file_path);

    if (!config_file.is_open()) {
        spdlog::error("Error opening config file!");
        exit(EXIT_FAILURE);
    }

    nlohmann::json config;

    try {
        config_file >> config;
    } catch (const std::exception& err) {
        spdlog::error("Error parsing json: {}", err.what());
        exit(EXIT_FAILURE);
    }

    if (config.find("host") == config.end() ||
        config.find("database") == config.end() ||
        config.find("table") == config.end() ||
        config.find("machine_id") == config.end()) {
        spdlog::error("No required options!");
        exit(EXIT_FAILURE);
    }

    size_t maxLoad = 100;
    if (config.find("maxLoad") != config.end()) {
        maxLoad = config["maxLoad"];
    }

    return {.host = config["host"],
            .database = config["database"],
            .table = config["table"],
            .machine_id = config["machine_id"],
            .maxLoad = maxLoad};
}

std::string getTimestamp() {
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::ctime(&time);

    return ss.str();
}

int Main(int argc, const char* argv[]) {
    if (const char* env = std::getenv("PROFILER_LOG_LEVEL")) {
        spdlog::set_level(spdlog::level::from_str(env));
    } else {
        spdlog::set_level(spdlog::level::info);
    }
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr"));
    spdlog::set_pattern("%Y-%m-%dT%H:%M:%S.%f [%^%l%$] %v");

    util::HandleSigInt(3);

    Options options = ParseOptions(argc, argv);

    db::dbOptions db_options = ParseDBoptions();

    spdlog::info("Going to trace process {}", options.Pid);

    profiler::dw::Unwinder unwinder{options};
    db::Manager db(db_options);
    hwstats::Collector collector(options.Pid);

    collector.startCounters();

    auto sleep_delta = options.Frequency
                           ? std::chrono::seconds{1} / options.Frequency
                           : std::chrono::seconds{0};
    for (; ;) {
        if (util::WasInterrupted()) {
            spdlog::info("Stopped by SIGINT");
            break;
        }

        auto start = std::chrono::high_resolution_clock::now();
        auto timestamp = getTimestamp();

        unwinder.Unwind();

        auto trace = unwinder.DumpTraces();
        auto stats = collector.collect();

        db.Store(stats.INSTRUCTIONS, stats.CACHE_REFERENCES, stats.CACHE_MISSES,
                 stats.BRANCH_INSTRUCTIONS, stats.BRANCH_MISSES, trace, timestamp);

        std::this_thread::sleep_until(start + sleep_delta);
    }

    return 0;
}

}  // namespace profiler

int main(int argc, const char* argv[]) { return profiler::Main(argc, argv); }
