#include "test_framework.hpp"
#include "lddm/session/session.hpp"

class MockCompositor : public lddm::ICompositorInstance {
public:
    explicit MockCompositor(std::string name = "MockWeston")
        : name_(std::move(name)) {}

    [[nodiscard]] const std::string& name() const noexcept override { return name_; }
    [[nodiscard]] bool is_running() const noexcept override { return running_; }
    [[nodiscard]] const std::string& socket_path() const noexcept override { return socket_; }

    lddm::Result<void> start() override {
        running_ = true;
        return lddm::Result<void>::success();
    }

    lddm::Result<void> stop() override {
        running_ = false;
        return lddm::Result<void>::success();
    }

private:
    std::string name_;
    std::string socket_{"/run/lddm/wayland-0"};
    bool running_{false};
};

class MockDesktop : public lddm::IDesktopEnvironmentInstance {
public:
    explicit MockDesktop(std::string name = "MockLDDE")
        : name_(std::move(name)) {}

    [[nodiscard]] const std::string& name() const noexcept override { return name_; }
    [[nodiscard]] bool is_running() const noexcept override { return running_; }

    lddm::Result<void> start() override {
        running_ = true;
        return lddm::Result<void>::success();
    }

    lddm::Result<void> stop() override {
        running_ = false;
        return lddm::Result<void>::success();
    }

private:
    std::string name_;
    bool running_{false};
};

TEST_CASE(Session_CreationAndEnvironment) {
    lddm::SessionConfig cfg{
        .id = lddm::SessionId("session-test-01"),
        .type = lddm::SessionType::Wayland,
        .user = "droiduser",
        .wayland_display = "wayland-9",
        .runtime_dir = "/run/user/1000"
    };

    lddm::Session session(cfg);
    EXPECT_EQ(session.id().str(), "session-test-01");
    EXPECT_EQ(session.state(), lddm::SessionLifecycleState::Idle);

    const auto& env = session.environment();
    auto it_user = env.find("USER");
    EXPECT_TRUE(it_user != env.end());
    if (it_user != env.end()) {
        EXPECT_EQ(it_user->second, "droiduser");
    }

    auto it_disp = env.find("WAYLAND_DISPLAY");
    EXPECT_TRUE(it_disp != env.end());
    if (it_disp != env.end()) {
        EXPECT_EQ(it_disp->second, "wayland-9");
    }
}

TEST_CASE(Session_LifecycleAndComponentOrchestration) {
    lddm::SessionConfig cfg{
        .id = lddm::SessionId("session-orchestration"),
        .type = lddm::SessionType::Wayland,
        .user = "root"
    };

    lddm::Session session(cfg);
    auto comp = std::make_unique<MockCompositor>();
    auto* comp_ptr = comp.get();
    session.attach_compositor(std::move(comp));

    auto desk = std::make_unique<MockDesktop>();
    auto* desk_ptr = desk.get();
    session.attach_desktop(std::move(desk));

    EXPECT_FALSE(comp_ptr->is_running());
    EXPECT_FALSE(desk_ptr->is_running());

    // Activate session
    auto act_res = session.activate();
    EXPECT_TRUE(act_res.has_value());
    EXPECT_EQ(session.state(), lddm::SessionLifecycleState::Active);
    EXPECT_TRUE(comp_ptr->is_running());
    EXPECT_TRUE(desk_ptr->is_running());

    // Terminate session
    auto term_res = session.terminate();
    EXPECT_TRUE(term_res.has_value());
    EXPECT_EQ(session.state(), lddm::SessionLifecycleState::Terminated);
    EXPECT_FALSE(comp_ptr->is_running());
    EXPECT_FALSE(desk_ptr->is_running());
}

TEST_CASE(Session_InvalidTransitionRejected) {
    lddm::SessionConfig cfg{.id = lddm::SessionId("session-invalid-trans")};
    lddm::Session session(cfg);

    // Idle directly to Terminated is illegal
    auto res = session.transition_to(lddm::SessionLifecycleState::Terminated);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(res.error().code(), lddm::ErrorCode::SessionInvalidState);
    EXPECT_EQ(session.state(), lddm::SessionLifecycleState::Idle);
}

TEST_MAIN()

