#pragma once

#include "lddm/core/result.hpp"
#include <string>

namespace lddm {

class ICompositorInstance {
public:
    virtual ~ICompositorInstance() = default;

    [[nodiscard]] virtual const std::string& name() const noexcept = 0;
    [[nodiscard]] virtual bool is_running() const noexcept = 0;
    [[nodiscard]] virtual const std::string& socket_path() const noexcept = 0;

    virtual Result<void> start() = 0;
    virtual Result<void> stop() = 0;
};

class IDesktopEnvironmentInstance {
public:
    virtual ~IDesktopEnvironmentInstance() = default;

    [[nodiscard]] virtual const std::string& name() const noexcept = 0;
    [[nodiscard]] virtual bool is_running() const noexcept = 0;

    virtual Result<void> start() = 0;
    virtual Result<void> stop() = 0;
};

} // namespace lddm

