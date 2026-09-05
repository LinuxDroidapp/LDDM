#include "lddm/session/session_state.hpp"
#include <ostream>

namespace lddm {

std::string_view to_string(SessionLifecycleState state) noexcept {
    switch (state) {
        case SessionLifecycleState::Idle:        return "Idle";
        case SessionLifecycleState::Preparing:   return "Preparing";
        case SessionLifecycleState::Active:      return "Active";
        case SessionLifecycleState::Paused:      return "Paused";
        case SessionLifecycleState::Terminating: return "Terminating";
        case SessionLifecycleState::Terminated:  return "Terminated";
        case SessionLifecycleState::Failed:      return "Failed";
    }
    return "Unknown";
}

std::ostream& operator<<(std::ostream& os, SessionLifecycleState state) {
    return os << to_string(state);
}

bool is_valid_session_transition(SessionLifecycleState from, SessionLifecycleState to) noexcept {
    if (from == to) {
        return true;
    }

    switch (from) {
        case SessionLifecycleState::Idle:
            return to == SessionLifecycleState::Preparing;

        case SessionLifecycleState::Preparing:
            return to == SessionLifecycleState::Active ||
                   to == SessionLifecycleState::Terminating ||
                   to == SessionLifecycleState::Failed;

        case SessionLifecycleState::Active:
            return to == SessionLifecycleState::Paused ||
                   to == SessionLifecycleState::Terminating ||
                   to == SessionLifecycleState::Failed;

        case SessionLifecycleState::Paused:
            return to == SessionLifecycleState::Active ||
                   to == SessionLifecycleState::Terminating ||
                   to == SessionLifecycleState::Failed;

        case SessionLifecycleState::Terminating:
            return to == SessionLifecycleState::Terminated ||
                   to == SessionLifecycleState::Failed;

        case SessionLifecycleState::Terminated:
            return to == SessionLifecycleState::Idle;

        case SessionLifecycleState::Failed:
            return to == SessionLifecycleState::Idle;
    }

    return false;
}

} // namespace lddm
