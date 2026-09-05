#pragma once

#include "lddm/session/session_state.hpp"
#include "lddm/session/session_config.hpp"
#include "lddm/session/session_contract.hpp"
#include "lddm/core/result.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>

namespace lddm {

class Session {
public:
    explicit Session(SessionConfig config);
    ~Session();

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&) noexcept;
    Session& operator=(Session&&) noexcept;

    [[nodiscard]] const SessionId& id() const noexcept { return config_.id; }
    [[nodiscard]] const SessionConfig& config() const noexcept { return config_; }
    [[nodiscard]] SessionLifecycleState state() const noexcept;

    [[nodiscard]] const std::unordered_map<std::string, std::string>& environment() const noexcept {
        return environment_;
    }

    void set_environment_variable(std::string key, std::string value);

    void attach_compositor(std::unique_ptr<ICompositorInstance> compositor);
    void attach_desktop(std::unique_ptr<IDesktopEnvironmentInstance> desktop);

    [[nodiscard]] ICompositorInstance* compositor() const noexcept { return compositor_.get(); }
    [[nodiscard]] IDesktopEnvironmentInstance* desktop() const noexcept { return desktop_.get(); }

    Result<void> transition_to(SessionLifecycleState target, std::string reason = "");

    Result<void> prepare();
    Result<void> activate();
    Result<void> terminate();

private:
    SessionConfig config_;
    SessionLifecycleState state_{SessionLifecycleState::Idle};
    std::unordered_map<std::string, std::string> environment_;
    std::unique_ptr<ICompositorInstance> compositor_;
    std::unique_ptr<IDesktopEnvironmentInstance> desktop_;
    mutable std::mutex mutex_;
};

} // namespace lddm

