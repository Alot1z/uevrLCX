---
title: "Error Handling"
slug: "/deepwiki/implant/architecture/error-handling"
summary: "Approach to error surfaces and resilience."
audience: ["dev"]
tags: ["errors", "resilience"]
level: "guide"
status: "active"
owner: "@uevr/docs"
reviewers: ["@uevr/maintainers"]
last_verified: "2025-08-19"
---

- Fail fast in setup, degrade gracefully at runtime.
- Log with actionable context.
- Avoid exceptions across module boundaries.

# Overview
This guide documents how errors propagate and are handled across hooks, plugins, adapters, and the core runtime. It cites real symbols from the codebase to clarify contracts and best practices.

# Principles
- Fail fast during initialization; prefer graceful degradation at runtime (skip frames, rehook, or early-return) rather than crashing the process.
- Return `bool`/error codes and populate error channels (e.g., `setLastError(...)`) instead of throwing exceptions across module boundaries.
- Log actionable context: include identifiers (hook id, plugin name), target module/function, and remediation hints.

# Surfaces and Contracts

## Hooks and Hook Manager
File: `include/uevr/hooks/HookManager.hpp`

- Configuration flags in `HookManagerConfig`:
  - `enable_error_logging`, `max_retry_attempts`, `hook_timeout_ms`, `performance_threshold_ms`, `enable_safety_checks`.
- Error API:
  - `const std::string& getLastError() const`
  - `void clearLastError()`
  - `std::vector<std::string> getErrorHistory() const`
  - `void enableErrorLogging(bool)` / `bool isErrorLoggingEnabled() const`
  - Internal helpers: `setLastError(const std::string&)`, `logError(const std::string&)`
- Stats contain `total_hook_errors` and per-category counts in `HookManagerStats`.

Operational guidance:
- Validation first: `validateHook(...)`, `validateHookInstallation(...)`, `validateHookConfiguration(...)` return `false` on failure and set last error.
- On install/remove failures: set last error, push to history, optionally retry up to `max_retry_attempts` with backoff (implementation-dependent), and leave hook state consistent (`HookState::{FAILED, UNINSTALLED}`).
- Keep callbacks lightweight; if over `performance_threshold_ms`, log a warning and consider disabling the offending hook.

Example error logging pattern:
```cpp
auto& hm = uevr::Hooks::HookManager::getInstance();
if (!hm.installHook("dx11.present")) {
    // capture last error for diagnostics/UI
    auto err = hm.getLastError();
    // Optionally escalate or surface to a higher-level manager
}
```

## Plugins
File: `include/uevr/Plugin.hpp`

- Error channel and state:
  - `setLastError(const std::string&)`, `getLastError()`
  - `hasError()`, `State::ERROR` transition expected on unrecoverable faults
  - Event: `onError(const std::string&)`
- Lifecycle methods should return `false` on failure and set a descriptive error:
```cpp
bool MyPlugin::initialize() {
    if (!checkDependencies()) {
        setLastError("Missing dependency: D3D subsystem");
        return false;
    }
    return true;
}
```

Runtime guidance:
- `update()`/`render()` must be exception-free and fast; catch-local exceptions if used internally and convert to `setLastError()` + state change.
- Prefer typed settings via `getSetting<T>()` and validate inputs; log and fallback to defaults on invalid values.

## Framework and Rendering Hooks
File: `src/core/Framework.cpp`

- Uses `spdlog` extensively for actionable logs (`info`, `debug`, `error`). A default logger is set in the constructor.
- Graceful degrade examples:
  - `Framework::on_post_present_d3d11()` and `on_post_present_d3d12()` early-return if `m_error` is non-empty or initialization gates are not met, while maintaining timing state to keep the hook monitor healthy.
  - `hook_monitor()` detects stalled presents and issues rehook requests (`hook_d3d11()` / `hook_d3d12()`) after cooldowns, avoiding crash loops.
  - Resize/reset paths (`on_reset`) safely deinitialize and recreate device resources.

Logging guidance:
- Include renderer type, hook name, and recent state transitions when logging failures.
- Avoid logging in tight per-frame hot paths unless behind a debug/trace level.

# Exceptions Policy
- Avoid exceptions across module boundaries (hooks, plugins, adapters). Prefer:
  - `bool`/error-string returns
  - explicit state transitions (`State::ERROR`, `HookState::FAILED`)
  - event callbacks (`onError`)
- If exceptions are used internally, catch them at the boundary, convert to an error string, and continue with a safe state.

# Actionable Logging Checklist
- Identify the failing unit: hook id (`HookInfo.id`), plugin name (`Plugin::getName()`), adapter name (`ICrossEngineAdapter::getName()`).
- Record target surface: `target_module`/`target_function` where applicable.
- Provide next steps: disable hook, reduce settings, restart, or check compatibility notes.
- Rate-limit repeated messages; aggregate with `getErrorHistory()` for UI surfaces.

# Quick Reference
- Hook manager: `getLastError()`, `getErrorHistory()`, `enableErrorLogging()`, validation methods.
- Plugin: `setLastError()`, `hasError()`, `State::ERROR`, `onError()`.
- Framework: `spdlog` logging; present/resize early-return gates; rehook logic in `hook_monitor()`.
