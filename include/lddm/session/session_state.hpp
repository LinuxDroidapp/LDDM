#pragma once

#include <cstdint>
#include <string_view>

#include <iosfwd>

namespace lddm {

enum class SessionLifecycleState : std::uint8_t {
    Idle        = 0,
    Preparing   = 1,
    Active      = 2,
    Paused      = 3,
    Terminating = 4,
    Terminated  = 5,
    Failed      = 6
};

[[nodiscard]] std::string_view to_string(SessionLifecycleState state) noexcept;
[[nodiscard]] bool is_valid_session_transition(SessionLifecycleState from, SessionLifecycleState to) noexcept;
std::ostream& operator<<(std::ostream& os, SessionLifecycleState state);

} // namespace lddm
