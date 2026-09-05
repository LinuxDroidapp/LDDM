#pragma once

#include "lddm/session/session_id.hpp"
#include "lddm/session/session_config.hpp"
#include "lddm/session/session_paths.hpp"
#include "lddm/session/session_environment.hpp"
#include "lddm/session/session_state.hpp"

namespace lddm {

struct SessionContext {
    const SessionIdentity& identity;
    const SessionConfig& config;
    const SessionPaths& paths;
    const SessionEnvironment& environment;
    SessionState state;
};

} // namespace lddm

