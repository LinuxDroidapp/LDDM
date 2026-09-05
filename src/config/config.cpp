#include "lddm/config/config.hpp"
#include "lddm/platform/paths.hpp"
#include "lddm/logging/logger.hpp"
#include <filesystem>

namespace lddm {

ConfigManager::ConfigManager()
    : config_(LddmConfig::create_default()) {}

ConfigManager::ConfigManager(LddmConfig initial_config)
    : config_(std::move(initial_config)) {}

Result<void> ConfigManager::load_file(const std::string& filepath) {
    auto parse_res = parser_.parse_file(filepath, config_);
    if (!parse_res.has_value()) {
        return Result<void>::failure(parse_res.error());
    }

    auto val_res = ConfigValidator::validate(parse_res.value());
    if (!val_res.has_value()) {
        return val_res;
    }

    config_ = std::move(parse_res.value());
    return Result<void>::success();
}

Result<void> ConfigManager::load_defaults() {
    config_ = LddmConfig::create_default();
    return Result<void>::success();
}

Result<void> ConfigManager::load_standard(const std::optional<std::string>& override_path) {
    if (override_path && !override_path->empty()) {
        LDDM_LOG_INFO(LogSubsystem::CONFIG, "Loading configuration from override: {}", *override_path);
        return load_file(*override_path);
    }

    std::string user_cfg = default_user_config_path();
    if (!user_cfg.empty() && std::filesystem::exists(user_cfg)) {
        LDDM_LOG_INFO(LogSubsystem::CONFIG, "Loading user configuration: {}", user_cfg);
        return load_file(user_cfg);
    }

    std::string sys_cfg = default_system_config_path();
    if (!sys_cfg.empty() && std::filesystem::exists(sys_cfg)) {
        LDDM_LOG_INFO(LogSubsystem::CONFIG, "Loading system configuration: {}", sys_cfg);
        return load_file(sys_cfg);
    }

    LDDM_LOG_INFO(LogSubsystem::CONFIG, "No configuration file found; using built-in defaults");
    return load_defaults();
}

std::string ConfigManager::default_system_config_path() {
    return "/etc/lddm/lddm.conf";
}

std::string ConfigManager::default_user_config_path() {
    std::string cfg_dir = Paths::config_dir();
    if (cfg_dir.empty()) {
        return "";
    }
    return cfg_dir + "/lddm.conf";
}

} // namespace lddm

