#include "lddm/process/process_types.hpp"
#include <ostream>

namespace lddm {

std::string_view to_string(ProcessState state) noexcept {
    switch (state) {
        case ProcessState::NotStarted: return "NotStarted";
        case ProcessState::Running:    return "Running";
        case ProcessState::Exited:     return "Exited";
        case ProcessState::Signaled:   return "Signaled";
        case ProcessState::Failed:     return "Failed";
    }
    return "Unknown";
}

std::ostream& operator<<(std::ostream& os, ProcessState state) {
    return os << to_string(state);
}

} // namespace lddm
