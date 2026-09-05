#pragma once

#include "lddm/core/types.hpp"
#include <string>
#include <unordered_map>
#include <string_view>
#include <iosfwd>

namespace lddm {

enum class SessionType : std::uint8_t {
    Wayland,
    X11,
    Headless
};

[[nodiscard]] std::string_view to_string(SessionType type) noexcept;
std::ostream& operator<<(std::ostream& os, SessionType type);

struct SessionConfig {
    SessionId id{SessionId("session-0")};
    SessionType type{SessionType::Wayland};
    std::string user{"root"};
    std::string wayland_display{"wayland-0"};
    std::string x11_display{":0"};
    std::string runtime_dir{"/run/lddm"};
    std::unordered_map<std::string, std::string> environment_overrides{};
};

} // namespace lddm
