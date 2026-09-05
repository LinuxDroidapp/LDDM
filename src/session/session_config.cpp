#include "lddm/session/session_config.hpp"
#include <ostream>

namespace lddm {

std::string_view to_string(SessionType type) noexcept {
    switch (type) {
        case SessionType::Wayland:  return "Wayland";
        case SessionType::X11:      return "X11";
        case SessionType::Headless: return "Headless";
    }
    return "Unknown";
}

std::ostream& operator<<(std::ostream& os, SessionType type) {
    return os << to_string(type);
}

} // namespace lddm
