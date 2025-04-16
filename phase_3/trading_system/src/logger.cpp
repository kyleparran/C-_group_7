#include "logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <chrono>
#include <sstream>
#include <filesystem>

std::string get_stamped_file_name(const std::string& pattern) {
    try {
        // parse pattern and fill in chars like: [%Y-%m-%d %H:%M:%S.%e]
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&in_time_t), pattern.c_str()) << '.' << std::setfill('0') << std::setw(3) << ms.count();
        std::string stamped_file_name = oss.str();

        // Ensure directory exists
        std::filesystem::path file_path(stamped_file_name);
        if (!std::filesystem::exists(file_path.parent_path())) {
            std::filesystem::create_directories(file_path.parent_path());
        }
        return stamped_file_name;
    } 
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error generating stamped file name: ") + e.what());
    }
}

spdlog::level::level_enum to_spdlog_level(const std::string& level_str) {
    if (level_str == "trace")    return spdlog::level::trace;
    if (level_str == "debug")    return spdlog::level::debug;
    if (level_str == "info")     return spdlog::level::info;
    if (level_str == "warn")     return spdlog::level::warn;
    if (level_str == "err")      return spdlog::level::err;
    if (level_str == "critical") return spdlog::level::critical;
    if (level_str == "off")      return spdlog::level::off;
    throw std::invalid_argument("Invalid log level: " + level_str);
}

void initLogger(const LoggerConfig& loggerConfig) {
    std::vector<spdlog::sink_ptr> sinks;

    if (loggerConfig.use_file) {
        std::string stamped_file_name = get_stamped_file_name(loggerConfig.file_name);
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(stamped_file_name, true);
        file_sink->set_pattern(loggerConfig.pattern);
        file_sink->set_level(to_spdlog_level(loggerConfig.file_level));
        sinks.push_back(file_sink);
    }

    if (loggerConfig.use_console) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern(loggerConfig.pattern);
        console_sink->set_level(to_spdlog_level(loggerConfig.console_level));
        sinks.push_back(console_sink);
    }

    auto logger = std::make_shared<spdlog::logger>(loggerConfig.name, sinks.begin(), sinks.end());

    logger->set_level(to_spdlog_level(loggerConfig.level));
    spdlog::set_default_logger(logger);
}