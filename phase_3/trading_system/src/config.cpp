#include "config.h"
#include <yaml-cpp/yaml.h>
#include <stdexcept>
#include <cctype>

template <typename T>
T getSafe(const YAML::Node& node, const std::string& key, const T& default_value) {
    return node[key] ? node[key].as<T>() : default_value;
}

AppConfig loadAppConfig(const std::string& yaml_path) {
    YAML::Node root = YAML::LoadFile(yaml_path);
    if (!root) {throw std::runtime_error("Failed to load config file: " + yaml_path);}
    if (!root["logger"]) {throw std::runtime_error("Missing 'logger' section in config file: " + yaml_path);}

    AppConfig config(
        { // LoggerConfig
            getSafe<std::string>(root["logger"], "name", "main"),
            getSafe<std::string>(root["logger"], "level", "info"),
            getSafe<std::string>(root["logger"], "pattern", "[%Y-%m-%d %H:%M:%S.%e] [%l] %v"),
            getSafe<bool>(root["logger"], "use_file", true),
            getSafe<std::string>(root["logger"], "file_name", "logs/main_[%Y-%m-%d_%H-%M-%S].log"),
            getSafe<std::string>(root["logger"], "file_level", "info"),
            getSafe<bool>(root["logger"], "use_console", true),
            getSafe<std::string>(root["logger"], "console_level", "info"),
        }
    );

    return config;
}
