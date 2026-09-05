#include "lddm/core/error.hpp"
#include <ostream>
#include <sstream>

namespace lddm {

std::string_view to_string(ErrorCategory category) noexcept {
    switch (category) {
        case ErrorCategory::Configuration: return "Configuration";
        case ErrorCategory::Session:       return "Session";
        case ErrorCategory::Process:       return "Process";
        case ErrorCategory::Platform:      return "Platform";
        case ErrorCategory::Compositor:    return "Compositor";
        case ErrorCategory::Desktop:       return "Desktop";
        case ErrorCategory::Resource:      return "Resource";
        case ErrorCategory::Internal:      return "Internal";
    }
    return "Unknown";
}

std::string_view to_string(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::Success:                   return "Success";
        case ErrorCode::ConfigFileNotFound:        return "ConfigFileNotFound";
        case ErrorCode::ConfigFileReadError:       return "ConfigFileReadError";
        case ErrorCode::ConfigParseSyntaxError:    return "ConfigParseSyntaxError";
        case ErrorCode::ConfigValidationFailed:    return "ConfigValidationFailed";
        case ErrorCode::ConfigMissingRequiredKey:  return "ConfigMissingRequiredKey";
        case ErrorCode::ConfigInvalidValue:        return "ConfigInvalidValue";
        case ErrorCode::SessionAlreadyActive:          return "SessionAlreadyActive";
        case ErrorCode::SessionNotFound:               return "SessionNotFound";
        case ErrorCode::SessionInvalidState:           return "SessionInvalidState";
        case ErrorCode::SessionStartupFailed:          return "SessionStartupFailed";
        case ErrorCode::SessionTeardownFailed:         return "SessionTeardownFailed";
        case ErrorCode::SessionEnvironmentError:       return "SessionEnvironmentError";
        case ErrorCode::SessionAlreadyExists:          return "SessionAlreadyExists";
        case ErrorCode::SessionInitializationFailed:   return "SessionInitializationFailed";
        case ErrorCode::SessionRuntimeDirectoryFailed: return "SessionRuntimeDirectoryFailed";
        case ErrorCode::SessionEnvironmentFailed:      return "SessionEnvironmentFailed";
        case ErrorCode::SessionStartFailed:            return "SessionStartFailed";
        case ErrorCode::SessionStopFailed:             return "SessionStopFailed";
        case ErrorCode::SessionCleanupFailed:          return "SessionCleanupFailed";
        case ErrorCode::ProcessSpawnFailed:        return "ProcessSpawnFailed";
        case ErrorCode::ProcessTerminated:         return "ProcessTerminated";
        case ErrorCode::ProcessTimeout:            return "ProcessTimeout";
        case ErrorCode::ProcessSignalFailed:       return "ProcessSignalFailed";
        case ErrorCode::ProcessNotFound:           return "ProcessNotFound";
        case ErrorCode::PlatformSyscallFailed:     return "PlatformSyscallFailed";
        case ErrorCode::PlatformFdError:           return "PlatformFdError";
        case ErrorCode::PlatformPathResolution:    return "PlatformPathResolution";
        case ErrorCode::PlatformSignalError:       return "PlatformSignalError";
        case ErrorCode::PlatformClockError:        return "PlatformClockError";
        case ErrorCode::PlatformPermissionDenied:  return "PlatformPermissionDenied";
        case ErrorCode::CompositorSpawnFailed:     return "CompositorSpawnFailed";
        case ErrorCode::CompositorSocketError:     return "CompositorSocketError";
        case ErrorCode::CompositorCrash:           return "CompositorCrash";
        case ErrorCode::CompositorTimeout:         return "CompositorTimeout";
        case ErrorCode::DesktopSpawnFailed:        return "DesktopSpawnFailed";
        case ErrorCode::DesktopSocketError:        return "DesktopSocketError";
        case ErrorCode::DesktopCrash:              return "DesktopCrash";
        case ErrorCode::DesktopTimeout:            return "DesktopTimeout";
        case ErrorCode::ResourceExhausted:         return "ResourceExhausted";
        case ErrorCode::ResourceAllocationFailed:  return "ResourceAllocationFailed";
        case ErrorCode::ResourceAlreadyInUse:      return "ResourceAlreadyInUse";
        case ErrorCode::ResourceInvalid:           return "ResourceInvalid";
        case ErrorCode::InternalLogicError:        return "InternalLogicError";
        case ErrorCode::InternalNotImplemented:    return "InternalNotImplemented";
        case ErrorCode::InternalInvalidState:      return "InternalInvalidState";
        case ErrorCode::InternalUnknown:           return "InternalUnknown";
    }
    return "UnknownCode";
}

std::ostream& operator<<(std::ostream& os, ErrorCategory category) {
    return os << to_string(category);
}

std::ostream& operator<<(std::ostream& os, ErrorCode code) {
    return os << to_string(code);
}

std::string Error::to_string() const {
    std::ostringstream oss;
    oss << "[" << lddm::to_string(category_) << ":" << lddm::to_string(code_) << " ("
        << static_cast<std::uint32_t>(code_) << ")] " << message_;
    if (!context_.empty()) {
        oss << " (context: " << context_ << ")";
    }
    if (!file_name_.empty()) {
        oss << " at " << file_name_ << ":" << line_;
    }
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Error& error) {
    os << error.to_string();
    return os;
}

} // namespace lddm
