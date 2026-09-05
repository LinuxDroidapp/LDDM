# LDDM — LinuxDroid Display Manager

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-brightgreen.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

**LDDM (LinuxDroid Display Manager)** is the display and graphical session manager for the LinuxDroid project. It is responsible for the graphical session lifecycle, display server orchestration, process supervision, and desktop environment integration.

---

## Architectural Role

LDDM sits between the LinuxDroid runtime and the graphical Linux desktop stack:

```text
LinuxDroid Android App
        ↓
LinuxDroid Runtime
        ↓
Guest Init
        ↓
LDDM (LinuxDroid Display Manager)
        ↓
Weston (Wayland Compositor)
        ↓
LDDE (LinuxDroid Desktop Environment)
        ↓
Linux Applications
```

LDDM provides a pure Linux-native session manager with zero Android API couplings, ensuring clean portability within LinuxDroid PRoot/container environments.

---

## Features (Phase L0 Foundation)

* **Modern C++20 Architecture**: Strict RAII, deterministic lifetimes, move semantics, and strong types.
* **Unified Error Model**: Categorized errors (`Configuration`, `Session`, `Process`, `Platform`, `Compositor`, `Desktop`, `Resource`, `Internal`) with stable codes, source location tracking, and type-safe `Result<T>` propagation.
* **Centralized Structured Logging**: Thread-safe multi-sink logging (`StreamSink`, `FileSink`, `MemorySink`) with severity levels (`TRACE` to `FATAL`), subsystem filtering, and ANSI terminal colorization.
* **Linux-Native Configuration**: INI-style configuration parser with default fallback, schema validation, and typed structures.
* **Lifecycle State Machine**: Explicit, observable state transitions (`CREATED` → `INITIALIZING` → `READY` → `STARTING` → `RUNNING` → `STOPPING` → `STOPPED`).
* **Platform Abstraction Layer**: Safe RAII Linux primitives including `UniqueFd`, signal handling via self-pipe trick, high-resolution monotonic clocks, and XDG directory resolution.
* **Session Contract**: Abstract session model decoupling display management from compositor (`Weston`) and desktop (`LDDE`) implementations.
* **Zero External Dependency Test Harness**: Complete unit and integration test suite running seamlessly under `CTest`.

---

## Project Structure

```text
LDDM/
├── CMakeLists.txt              # Root build configuration
├── README.md                   # Project overview and quickstart
├── include/lddm/               # Public API headers
│   ├── version.hpp.in          # Semantic version template
│   ├── core/                   # Error, Result, Lifecycle, Types
│   ├── logging/                # Logger, Sinks, Levels, Messages
│   ├── config/                 # Config types, Parser, Validator, Manager
│   ├── platform/               # UniqueFd, Clock, Environment, Paths, Signals
│   ├── process/                # Process types and launch options
│   └── session/                # Session, Contracts, States, Config
├── src/                        # Implementation sources
│   ├── main.cpp                # LDDM daemon CLI entry point
│   ├── core/
│   ├── logging/
│   ├── config/
│   ├── platform/
│   ├── process/
│   └── session/
├── tests/                      # Test suite
│   ├── CMakeLists.txt
│   ├── test_framework.hpp      # Lightweight test framework
│   ├── unit/                   # Unit tests (version, error, config, lifecycle, etc.)
│   └── integration/            # Full lifecycle integration tests
├── config/                     # Configuration files
│   ├── lddm.conf.example       # Production example config
│   └── lddm.conf.defaults      # Built-in defaults reference
├── packaging/                  # Systemd service units and packaging files
│   └── lddm.service.in
└── docs/                       # Developer documentation
    ├── architecture.md
    ├── configuration.md
    ├── lifecycle.md
    ├── logging.md
    └── development.md
```

---

## Building and Testing

### Prerequisites

* GCC 13+ or Clang 18+ (C++20 compliant)
* CMake 3.20+
* GNU Make or Ninja

### Build Instructions

```bash
# 1. Configure Release build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DLDDM_BUILD_TESTS=ON -DLDDM_WARNINGS_AS_ERRORS=ON

# 2. Build binaries and libraries
cmake --build build -j$(nproc)

# 3. Run test suite
ctest --test-dir build --output-on-failure

# 4. Install
sudo cmake --install build
```

---

## CLI Usage

```text
Usage: lddm [OPTIONS]

Options:
  -h, --help                  Show this help message and exit
  -v, --version               Display version information and exit
  -c, --config <file>         Specify path to configuration file
      --validate-config <file> Validate configuration file and exit
      --log-level <level>      Set logging level (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
      --dry-run               Initialize and validate environment then exit
```

Validate a configuration file:
```bash
./build/lddm --validate-config config/lddm.conf.example
```

Run in dry-run mode:
```bash
./build/lddm --dry-run
```

---

## Phase Roadmap

* [x] **Phase L0: Production Foundation** (Current)
* [ ] **Phase L1: Session Model** (Multi-session support, seat management, environment preparation)
* [ ] **Phase L2: Process Supervisor** (Child process lifecycle, signalfd event loops, watchdog)
* [ ] **Phase L3: Weston Manager** (Compositor spawning, socket verification, crash detection)
* [ ] **Phase L4: LDDE Session Integration** (Desktop environment launcher, desktop components)
* [ ] **Phase L5: Session Recovery** (Crash recovery, compositor restart without killing apps)
* [ ] **Phase L6: Packaging & Distribution** (Debian/RPM packages, systemd daemonization)
