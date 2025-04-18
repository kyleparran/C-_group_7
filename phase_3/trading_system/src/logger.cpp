#include "logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <stdexcept>
#include <vector>

spdlog::level::level_enum to_spdlog_level(const std::string& level_str) {
    if (level_str == "trace") return spdlog::level::trace;
    if (level_str == "debug") return spdlog::level::debug;
    if (level_str == "info") return spdlog::level::info;
    if (level_str == "warn") return spdlog::level::warn;
    if (level_str == "err") return spdlog::level::err;
    if (level_str == "critical") return spdlog::level::critical;
    if (level_str == "off") return spdlog::level::off;
    throw std::invalid_argument("Invalid log level: " + level_str);
}

void initLogger(const LoggerConfig& loggerCfg) {
    std::vector<spdlog::sink_ptr> sinks;

    if (loggerCfg.use_file) {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(loggerCfg.file_name, true);
        file_sink->set_pattern(loggerCfg.pattern);
        file_sink->set_level(to_spdlog_level(loggerCfg.file_level));
        sinks.push_back(file_sink);
    }

    if (loggerCfg.use_console) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern(loggerCfg.pattern);
        console_sink->set_level(to_spdlog_level(loggerCfg.console_level));
        sinks.push_back(console_sink);
    }

    auto marketLogger = std::make_shared<spdlog::logger>("market", sinks.begin(), sinks.end());
    marketLogger->set_level(to_spdlog_level(loggerCfg.level));
    spdlog::register_logger(marketLogger);

    auto mainLogger = std::make_shared<spdlog::logger>(loggerCfg.name, sinks.begin(), sinks.end());
    mainLogger->set_level(to_spdlog_level(loggerCfg.level));
    spdlog::set_default_logger(mainLogger);
}
