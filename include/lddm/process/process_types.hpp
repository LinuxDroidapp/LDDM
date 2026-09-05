#pragma once

#include "lddm/core/types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <string_view>
#include <iosfwd>

namespace lddm {

enum class ProcessState : std::uint8_t {
    NotStarted = 0,
    Running    = 1,
    Exited     = 2,
    Signaled   = 3,
    Failed     = 4
};

[[nodiscard]] std::string_view to_string(ProcessState state) noexcept;
std::ostream& operator<<(std::ostream& os, ProcessState state);

struct ProcessExitStatus {
    int code{0};
    int signal{0};
    bool core_dumped{false};

    [[nodiscard]] bool success() const noexcept {
        return code == 0 && signal == 0;
    }
};

struct ProcessLaunchOptions {
    std::string executable;
    std::vector<std::string> arguments;
    std::unordered_map<std::string, std::string> environment;
    std::string working_directory;
    std::optional<UserId> run_as_user;
    std::optional<GroupId> run_as_group;
};

} // namespace lddm
