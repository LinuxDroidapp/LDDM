#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <chrono>
#include <compare>

namespace lddm {

using ProcessId = std::int32_t;
using UserId = std::uint32_t;
using GroupId = std::uint32_t;

class SessionId {
public:
    SessionId() = default;
    explicit SessionId(std::string id) : id_(std::move(id)) {}

    [[nodiscard]] const std::string& str() const noexcept { return id_; }
    [[nodiscard]] std::string_view view() const noexcept { return id_; }
    [[nodiscard]] bool empty() const noexcept { return id_.empty(); }

    auto operator<=>(const SessionId& other) const = default;
    bool operator==(const SessionId& other) const = default;

private:
    std::string id_{};
};

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using SystemClock = std::chrono::system_clock;
using SystemTimePoint = SystemClock::time_point;

} // namespace lddm

