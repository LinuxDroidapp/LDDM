#include "lddm/session/session.hpp"
#include "lddm/logging/logger.hpp"

namespace lddm {

Session::Session(SessionConfig config)
    : config_(std::move(config))
    , state_(SessionLifecycleState::Idle) {
    // Populate base environment
    environment_["USER"] = config_.user;
    environment_["LOGNAME"] = config_.user;
    environment_["XDG_RUNTIME_DIR"] = config_.runtime_dir;

    if (config_.type == SessionType::Wayland) {
        environment_["WAYLAND_DISPLAY"] = config_.wayland_display;
        environment_["XDG_SESSION_TYPE"] = "wayland";
    } else if (config_.type == SessionType::X11) {
        environment_["DISPLAY"] = config_.x11_display;
        environment_["XDG_SESSION_TYPE"] = "x11";
    } else {
        environment_["XDG_SESSION_TYPE"] = "headless";
    }

    for (const auto& [k, v] : config_.environment_overrides) {
        environment_[k] = v;
    }

    LDDM_LOG_INFO(LogSubsystem::SESSION, "Session '{}' created for user '{}' (type: {})",
                  config_.id.str(), config_.user, to_string(config_.type));
}

Session::~Session() {
    if (state_ == SessionLifecycleState::Active ||
        state_ == SessionLifecycleState::Preparing ||
        state_ == SessionLifecycleState::Paused) {
        (void)terminate();
    }
}

Session::Session(Session&& other) noexcept {
    std::lock_guard<std::mutex> lock(other.mutex_);
    config_ = std::move(other.config_);
    state_ = other.state_;
    environment_ = std::move(other.environment_);
    compositor_ = std::move(other.compositor_);
    desktop_ = std::move(other.desktop_);
    other.state_ = SessionLifecycleState::Terminated;
}

Session& Session::operator=(Session&& other) noexcept {
    if (this != &other) {
        std::scoped_lock lock(mutex_, other.mutex_);
        if (state_ == SessionLifecycleState::Active ||
            state_ == SessionLifecycleState::Preparing) {
            (void)terminate();
        }
        config_ = std::move(other.config_);
        state_ = other.state_;
        environment_ = std::move(other.environment_);
        compositor_ = std::move(other.compositor_);
        desktop_ = std::move(other.desktop_);
        other.state_ = SessionLifecycleState::Terminated;
    }
    return *this;
}

SessionLifecycleState Session::state() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

void Session::set_environment_variable(std::string key, std::string value) {
    std::lock_guard<std::mutex> lock(mutex_);
    environment_[std::move(key)] = std::move(value);
}

void Session::attach_compositor(std::unique_ptr<ICompositorInstance> compositor) {
    std::lock_guard<std::mutex> lock(mutex_);
    compositor_ = std::move(compositor);
    if (compositor_) {
        LDDM_LOG_INFO(LogSubsystem::SESSION, "Attached compositor '{}' to session '{}'",
                      compositor_->name(), config_.id.str());
    }
}

void Session::attach_desktop(std::unique_ptr<IDesktopEnvironmentInstance> desktop) {
    std::lock_guard<std::mutex> lock(mutex_);
    desktop_ = std::move(desktop);
    if (desktop_) {
        LDDM_LOG_INFO(LogSubsystem::SESSION, "Attached desktop environment '{}' to session '{}'",
                      desktop_->name(), config_.id.str());
    }
}

Result<void> Session::transition_to(SessionLifecycleState target, std::string reason) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ == target) {
        return Result<void>::success();
    }

    if (!is_valid_session_transition(state_, target)) {
        std::string err_msg = "Invalid session state transition from ";
        err_msg += to_string(state_);
        err_msg += " to ";
        err_msg += to_string(target);
        if (!reason.empty()) {
            err_msg += " (reason: ";
            err_msg += reason;
            err_msg += ")";
        }
        LDDM_LOG_ERROR(LogSubsystem::SESSION, "[Session '{}'] {}", config_.id.str(), err_msg);
        return Result<void>::failure(Error(ErrorCategory::Session,
                                          ErrorCode::SessionInvalidState,
                                          std::move(err_msg)));
    }

    auto old_state = state_;
    state_ = target;
    LDDM_LOG_INFO(LogSubsystem::SESSION, "[Session '{}'] Transition: {} -> {}{}",
                  config_.id.str(),
                  to_string(old_state),
                  to_string(target),
                  reason.empty() ? "" : (" (reason: " + reason + ")"));

    return Result<void>::success();
}

Result<void> Session::prepare() {
    auto res = transition_to(SessionLifecycleState::Preparing, "Session initialization");
    if (!res.has_value()) {
        return res;
    }
    return Result<void>::success();
}

Result<void> Session::activate() {
    if (state_ != SessionLifecycleState::Preparing) {
        auto prep_res = prepare();
        if (!prep_res.has_value()) {
            return prep_res;
        }
    }

    if (compositor_) {
        auto comp_res = compositor_->start();
        if (!comp_res.has_value()) {
            (void)transition_to(SessionLifecycleState::Failed, "Compositor start failed");
            return comp_res;
        }
    }

    if (desktop_) {
        auto desk_res = desktop_->start();
        if (!desk_res.has_value()) {
            if (compositor_) {
                (void)compositor_->stop();
            }
            (void)transition_to(SessionLifecycleState::Failed, "Desktop environment start failed");
            return desk_res;
        }
    }

    return transition_to(SessionLifecycleState::Active, "All components started");
}

Result<void> Session::terminate() {
    auto term_res = transition_to(SessionLifecycleState::Terminating, "Session termination requested");
    if (!term_res.has_value() && state_ != SessionLifecycleState::Terminating) {
        return term_res;
    }

    if (desktop_) {
        (void)desktop_->stop();
    }

    if (compositor_) {
        (void)compositor_->stop();
    }

    return transition_to(SessionLifecycleState::Terminated, "Teardown complete");
}

} // namespace lddm

