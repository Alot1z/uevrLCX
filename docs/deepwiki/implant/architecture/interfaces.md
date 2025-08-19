---
title: "Interfaces"
slug: "/deepwiki/implant/architecture/interfaces"
summary: "Key boundaries and contracts between modules."
audience: ["dev"]
tags: ["interfaces", "contracts"]
level: "reference"
status: "active"
owner: "@uevr/docs"
reviewers: ["@uevr/maintainers"]
last_verified: "2025-08-19"
---

- Adapter ↔ Runtime
- Hooks ↔ Engine
- Input ↔ Bindings
- Plugins ↔ Framework

# Overview
This page documents the core interface boundaries in the uevr framework and the contracts each side must honor. All sections cite real symbols from the codebase.

- Adapter ↔ Runtime
- Hooks ↔ Engine
- Input ↔ Bindings
- Plugins ↔ Framework

# Hooks ↔ Engine
Primary types live in `include/uevr/hooks/HookManager.hpp` and hook implementations like DirectX live under `src/hooks/`.

- Interface types:
  - `uevr::Hooks::HookInfo` — declarative hook description and runtime stats.
  - `uevr::Hooks::HookManager` — lifecycle, validation, dependency/conflict handling, stats, logging.
  - Enums: `HookPriority`, `HookCategory`, `HookState`.
- Engine-specific hooks:
  - `D3D11Hook` in `src/hooks/D3D11Hook.hpp` — exposes `hook()`, `unhook()`, `on_present(...)`, `on_resize_buffers(...)`, `on_post_present(...)`, `is_hooked()`, `is_inside_present()`.

Contract highlights:
- Hooks are registered as data via `HookInfo` (id, category, priority, target module/function, callbacks, dependencies/conflicts).
- Managers enforce safety: dependency resolution, conflict detection, validation, performance thresholds, and thread-safety via internal mutexes.
- Engine hooks must avoid heavy work on the Present thread; render thread work goes through ImGui/command contexts.

Minimal example (conceptual):
```cpp
#include <uevr/hooks/HookManager.hpp>
using namespace uevr::Hooks;

HookInfo presentHook{
    .id = "dx11.present",
    .name = "DX11 Present Hook",
    .description = "Observes IDXGISwapChain::Present",
    .category = HookCategory::GRAPHICS,
    .priority = HookPriority::HIGH,
    .state = HookState::UNINSTALLED,
    .target_function = "Present",
    .target_module = "dxgi.dll",
    .callback = [](const void* ctx, void*) {/* lightweight */},
    .cleanup_callback = [](){},
    .enabled = true,
    .critical = false,
};

auto& hm = HookManager::getInstance();
hm.registerHook(presentHook);
hm.installHook(presentHook.id);
```

# Plugins ↔ Framework
Defined in `include/uevr/Plugin.hpp`.

- Configuration and identity:
  - `Plugin::Config { name, version, description, author, website, enabled, auto_load, dependencies, settings }`
- Lifecycle surface (virtual):
  - `initialize() -> bool`, `start() -> bool`, `stop()`, `pause()`, `resume()`, `cleanup()`
  - Per-frame: `update(float delta_seconds)`, `render()`
  - Messaging: `handleMessage(uint32_t message_id, const void* data) -> bool`
- State and priority:
  - `Plugin::State { UNLOADED, LOADING, LOADED, INITIALIZING, RUNNING, PAUSED, ERROR, UNLOADING }`
  - `Plugin::Priority { LOW, NORMAL, HIGH, CRITICAL }`
- Error channel and events:
  - `getLastError()`, `setLastError(const std::string&)`, `onError(const std::string&)`
  - Hooks: `onLoad/onUnload/onStart/onStop/onPause/onResume`
- Management:
  - `PluginManager` loads/enables/orders plugins and executes lifecycle and frame calls.

Contract highlights:
- `initialize()` and `start()` must return `false` on failure and set a descriptive error via `setLastError()`; transitions to `State::ERROR` are expected for unrecoverable faults.
- `update()` and `render()` must be exception-free and fast. Use config and dependency checks (`checkDependencies()`).
- Use `getSetting<T>(key, default)`/`setSetting<T>(key, value)` for typed settings.

Minimal plugin skeleton:
```cpp
class MyPlugin : public uevr::Plugin {
public:
    MyPlugin() : Plugin(Config{ .name = "MyPlugin", .version = "1.0.0" }) {}
    bool initialize() override {
        if (!checkDependencies()) { setLastError("Missing deps"); return false; }
        return true;
    }
    bool start() override { return true; }
    void update(float dt) override {/* game logic */}
};
```

# Cross-Engine Adapters ↔ Runtime
Defined in `include/uevr/ICrossEngineAdapter.hpp`.

- Adapter interface: `ICrossEngineAdapter`
  - `initialize() -> bool`, `cleanup()`
  - `isVREnabled() const -> bool`
  - `update()`, `render()`
  - Metadata: `getName()`, `getEngineType()`, `getGameExecutable()`, `getVersion()`
- Factory: `ICrossEngineAdapterFactory`
  - `createAdapter(gameExe) -> shared_ptr<ICrossEngineAdapter>`
  - Capability query: `canCreateAdapter(gameExe)`, engine type declaration
- Registry: `CrossEngineAdapterRegistry`
  - `registerFactory(...)`, `unregisterFactory(...)`
  - `createAdapter(gameExe)`, `getRegisteredFactories()`, `clearFactories()`

Contract highlights:
- Factories must be thread-safe for registration; registry uses an internal mutex.
- `createAdapter()` may return `nullptr` on unsupported titles; callers must handle gracefully.
- Adapters must keep `initialize()` side-effect free on failure (idempotent `cleanup()`).

# Input ↔ Bindings
Primary sources: `src/core/mods/VR.hpp` (action constants and APIs) and `src/core/mods/vr/Bindings.cpp` (OpenVR action manifest and per-controller default bindings).

- Action naming (OpenVR):
  - Inputs are under `/actions/default/in/...` and outputs under `/actions/default/out/...`.
  - Canonical constants in `VR.hpp`:
    - Poses: `VR::s_action_pose = "/actions/default/in/Pose"`, `VR::s_action_grip_pose = "/actions/default/in/GripPose"`
    - Buttons/axes: `VR::s_action_trigger`, `VR::s_action_grip`, `VR::s_action_joystick`, `VR::s_action_joystick_click`
    - AB buttons (per-hand): `VR::s_action_a_button_left/right`, `VR::s_action_b_button_left/right`
    - Touch surfaces: `VR::s_action_a_button_touch_left/right`, `VR::s_action_b_button_touch_left/right`, `VR::s_action_thumbrest_touch_left/right`
    - DPad emulation: `VR::s_action_dpad_up/right/down/left`
    - System: `VR::s_action_system_button`
  - Additional actions present in `Bindings.cpp` manifest (may not have dedicated constants/handles): `HeadsetOnHead`, `SkeletonLeftHand/RightHand`, `Touchpad`, `TouchpadClick`, `Squeeze`, `Teleport`, `ThumbRestTouchLeft/Right`.

- Handles and lookup:
  - `VR::m_action_handles` maps string paths to `vr::VRActionHandle_t` references. Example entries include the constants above and `"/actions/default/out/Haptic"` for `m_action_haptic`.
  - `vr::VRActionHandle_t VR::get_action_handle(std::string_view action_path)` returns a handle or `vr::k_ulInvalidActionHandle` if not present.

- Per-frame state and queries:
  - `void VR::update_action_states()` — updates OpenVR action states each frame.
  - `bool VR::is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle) const`
  - `bool VR::is_action_active_any_joystick(vr::VRActionHandle_t action) const`
  - Joystick axes:
    - `Vector2f VR::get_joystick_axis(vr::VRInputValueHandle_t handle) const`
    - `Vector2f VR::get_left_stick_axis() const`, `Vector2f VR::get_right_stick_axis() const`

- Input sources and controller swap:
  - Input value handles: `VR::m_left_joystick`, `VR::m_right_joystick`.
  - Swap-aware accessors: `VR::get_left_joystick()`, `VR::get_right_joystick()` respect `m_swap_controllers`.
  - Controller detection: `VR::detect_controllers()`, usage gating: `VR::is_using_controllers()`.

- Haptics:
  - Output action path: `"/actions/default/out/Haptic"` bound in `m_action_handles`.
  - API: `void VR::trigger_haptic_vibration(float seconds_from_now, float duration, float frequency, float amplitude, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle)`.

- Default bindings (OpenVR) in `Bindings.cpp`:
  - `actions_json` defines actions and `default_bindings` for controller types: `vive`, `vive_controller`, `oculus_touch`, `knuckles`, `rift`.
  - Per-controller JSONs: `bindings_oculus_touch_json`, `bindings_vive_controller`, `bindings_knuckles`, etc., route device-specific inputs (e.g., `thumbstick`, `trigger`, `system`) to the action paths above.

Minimal example (conceptual):
```cpp
// src reference: src/core/mods/VR.hpp
auto& vr = VR::get();

// Update actions once per frame
vr->update_action_states();

// Query a boolean action
auto trigger = vr->get_action_handle(VR::s_action_trigger);
if (trigger != vr::k_ulInvalidActionHandle && vr->is_action_active(trigger)) {
    // Handle trigger press (any source)
}

// Joystick axes (swap-aware)
Vector2f leftAxis  = vr->get_left_stick_axis();
Vector2f rightAxis = vr->get_right_stick_axis();

// Haptics on left controller
vr->trigger_haptic_vibration(0.0f, 0.10f, 150.0f, 0.5f, vr->get_left_joystick());
```

Notes:
- Naming consistency: `Bindings.cpp` uses `"ThumbRestTouchLeft/Right"` while `VR.hpp` defines `"ThumbrestTouchLeft/Right"`. Consider normalizing casing to avoid confusion between manifests and code. See `VR::s_action_thumbrest_touch_left/right` and `VR::actions_json` entries.

# DirectX Hook Surfaces
`src/core/Framework.cpp` demonstrates how the framework drives D3D11/12 hooks and message hooks.

- DX11: `Framework::hook_d3d11()` wires `D3D11Hook::on_present`, `on_post_present`, `on_resize_buffers` to `Framework` methods (`on_frame_d3d11`, `on_post_present_d3d11`, `on_reset`).
- DX12: analogous wiring via `D3D12Hook`.
- Present-thread contract: callbacks must be fast; heavy work is deferred to command threads or ImGui frame building.

# Thread Safety and Ownership
- `HookManager` uses `std::shared_mutex` for hooks, dependencies, conflicts, and stats maps.
- `Framework` uses `std::jthread`, `std::scoped_lock` around critical UI/hook sections.
- Caller responsibilities:
  - Do not mutate plugin state from hook callbacks without synchronization.
  - Treat callback lifetimes as tied to hook installation state (ensure `cleanup_callback`).

# Validation and Safety
- Prefer `validateHook(...)`, `checkHookIntegrity(...)`, and `performSafetyChecks(...)` in `HookManager` when developing new hooks.
- Keep `HookManagerConfig` thresholds conservative: `performance_threshold_ms`, `max_retry_attempts`, `hook_timeout_ms`.

# See Also
- `include/uevr/API.hpp` for utility and subsystem helpers used across interfaces (profiling, threading, memory, utils).
- `src/core/Framework.cpp` for concrete engine wiring and lifecycle flow.

- Hooks ↔ Engine
- Input ↔ Bindings
