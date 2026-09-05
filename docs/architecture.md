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

5. **`lddm::session`**:
   - Session contract: `Session`, `ICompositorInstance`, `IDesktopEnvironmentInstance`.
   - Preparation, activation, pausing, teardown.

