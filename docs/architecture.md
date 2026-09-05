# LDDM Architecture

## 1. System Role & Topology

**LDDM (LinuxDroid Display Manager)** is the display and graphical session manager within the LinuxDroid project. It bridges the low-level LinuxDroid runtime and the high-level graphical desktop environment.

```text
+---------------------------------------------------------+
|                LinuxDroid Android App                   |
|  - Android UI & Lifecycle                               |
|  - PRoot / Container Sandboxing                         |
|  - Hardware bridge & Termux-X11/Wayland Surface View    |
+---------------------------------------------------------+
                            |
                            v
+---------------------------------------------------------+
|                  LinuxDroid Runtime                     |
|  - Rootfs orchestration                                 |
|  - Guest environment mounting & initialization          |
+---------------------------------------------------------+
                            |
                            v
+---------------------------------------------------------+
|                      Guest Init                         |
|  - System init scripts / systemd / custom supervisor    |
+---------------------------------------------------------+
                            |
                            v
+---------------------------------------------------------+
|                        LDDM                             |
|            LinuxDroid Display Manager                   |
|  - Session orchestration & configuration                |
|  - Lifecycle state machine                              |
|  - Compositor & Desktop process supervision             |
+---------------------------------------------------------+
                            |
                            v
+---------------------------------------------------------+
|                       Weston                            |
|                 Wayland Compositor                      |
|  - Headless / Wayland backend rendering                 |
|  - Wayland protocol endpoints (/run/lddm/wayland-0)     |
+---------------------------------------------------------+
                            |
                            v
+---------------------------------------------------------+
|                        LDDE                             |
|          LinuxDroid Desktop Environment                 |
|  - Desktop panels, session services, window manager     |
+---------------------------------------------------------+
                            |
                            v
+---------------------------------------------------------+
|                  Linux Applications                     |
|  - Terminal, Browsers, Productivity tools               |
+---------------------------------------------------------+
```

---

## 2. Component Boundaries & Responsibilities

| Component | Owned Responsibilities | Explicit Non-Responsibilities |
| :--- | :--- | :--- |
| **Android App** | Android UI, PRoot bindings, storage access permissions, Android surface rendering. | Linux graphical session management, Wayland socket lifecycles. |
| **LinuxDroid Runtime / Guest Init** | Container filesystem setup, user namespace mapping, init invocation. | Display configuration, compositor argument tuning, session state recovery. |
| **LDDM** | **Graphical session lifecycle**, config parsing, process supervision, Wayland environment setup, graceful recovery. | Android JNI calls, rendering pixels directly, window management. |
| **Weston** | Surface composition, Wayland client protocol handling, input event delivery. | Session authentication, desktop panel startup, desktop lifecycle supervision. |
| **LDDE** | User desktop experience, system tray, app launcher, notification daemon. | Low-level compositor process spawning or display server initialization. |

---

## 3. Subsystem Breakdown

1. **`lddm::core`**:
   - `LifecycleStateMachine`: Deterministic transition state machine (`CREATED` -> `STOPPED`/`FAILED`).
   - `Error` & `Result<T>`: Strict, unified error taxonomy without raw errno leaks.
   - `types`: Strongly typed IDs (`SessionId`, `UserId`, `ProcessId`).

2. **`lddm::logging`**:
   - Centralized, thread-safe logger with structured log records (`LogMessage`).
   - Sinks: `StreamSink` (ANSI colored terminal), `FileSink`, `MemorySink` (for unit testing).
   - Subsystems: `LDDM`, `SESSION`, `PROCESS`, `CONFIG`, `PLATFORM`, `WESTON`, `LDDE`.

3. **`lddm::config`**:
   - Centralized INI configuration parser supporting sections, types, validation, and defaults.
   - System path: `/etc/lddm/lddm.conf`, user path: `~/.config/lddm/lddm.conf`.

4. **`lddm::platform`**:
   - RAII wrappers: `UniqueFd`.
   - OS abstractions: high-resolution monotonic clock, environment management, XDG directory resolution, signal handling (`sigaction` + self-pipe).

5. **`lddm::session`** (Phase L1):
   - `SessionIdentity`: Unique `SessionId` (monotonic sequence + timestamp), user credentials, session type.
   - `SessionStateMachine`: State machine with transition validation, history recording, and observer callbacks.
   - `SessionPaths`: Per-session directory isolation (`/run/lddm/<session-id>`) with `0700` permission enforcement.
   - `SessionEnvironment`: Deterministic multi-layer environment resolution (base -> session -> desktop -> overrides) with redacted diagnostic summaries.
   - `SessionResourceTracker`: RAII tracking of open file descriptors and temporary files ensuring zero leaks on stop or failure.
   - `SessionDiagnostics`: Full transition history and failure recording for inspectability.
   - `SessionManager`: Multi-session registry, active session designation, and controlled teardown.
   - Contracts: `ISessionComponent`, `ICompositorInstance`, `IDesktopEnvironmentInstance`.

6. **`lddm::process`** (Phase L2):
   - `Process`: Encapsulates a single supervised OS process lifecycle with state transitions (`CREATED` -> `STARTING` -> `RUNNING` -> `STOPPING` -> `EXITED`/`FAILED`).
   - `ProcessSpec`: Executable path, argument vector, environment map, working directory, and stream policies.
   - `StreamPolicy`: Configurable stream routing (`Inherit`, `Null`, `Close`, `File`, `Pipe`).
   - `ProcessRegistry`: Thread-safe registry indexable by PID, handle, and component name.
   - `ProcessSupervisor`: High-level process lifecycle orchestrator with non-blocking reaping (`waitpid(WNOHANG)`), process group isolation (`setpgid`), graceful termination (`SIGTERM`), and timeout escalation to `SIGKILL`.
   - `ProcessDiagnostics`: Process runtime metrics, exit info recording, and lifecycle history audit logs.
   - `ProcessEventDispatcher`: Observer event stream (`Started`, `Exited`, `Failed`, `Signaled`).
   - Integration with `Session`: Owned directly by `Session`, initialized in `session.initialize()`, environment synchronized in `session.start()`, and stopped gracefully in `session.stop()`.

---

## 4. Future Phase Roadmap

- **Phase L3 — Weston Compositor Supervision**:
  - Implement `ICompositorInstance` backed by `ProcessSupervisor`.
  - Supervise Weston Wayland compositor process, arguments (`--backend`, `--socket`, `--idle-time`), socket readiness detection, and recovery.
- **Phase L4 — Desktop Environment & Services**:
  - Implement `IDesktopEnvironmentInstance` backed by `ProcessSupervisor`.
  - Supervise LDDE desktop environment, panel, notification service, and application lifetime.


