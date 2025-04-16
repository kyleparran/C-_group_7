#pragma once
#include <string>

struct LoggerConfig {
    std::string name;
    std::string level;
    std::string pattern;
    bool use_file;
    std::string file_name;
    std::string file_level;
    bool use_console;
    std::string console_level;
};

// High level config
struct AppConfig {
    LoggerConfig logger;
};

AppConfig loadAppConfig(const std::string& yaml_path);

