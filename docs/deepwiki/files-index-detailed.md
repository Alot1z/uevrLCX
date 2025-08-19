# Detailed Files Index

Granular index of key directories and files in the uevr cross‑engine VR system. Use this as a starting point when exploring the codebase.

- Tip: See `docs/deepwiki/functions.md` for a function/module view
- Tip: See `docs/deepwiki/architecture.md` for system-level context

## `src/hooks/`
- CMakeLists.txt
- D3D11Hook.hpp / D3D11Hook.cpp
- D3D12Hook.hpp / D3D12Hook.cpp
- DInputHook.hpp / DInputHook.cpp
- DirectXHooks.cpp
- EngineHooks.cpp
- GameHooks.h
- HookManager.cpp
- OpenGLHooks.cpp
- VulkanHooks.cpp
- WindowsMessageHook.hpp / WindowsMessageHook.cpp
- XInputHook.hpp / XInputHook.cpp

Focus: Graphics, input, window message hooks. DX11/DX12/Vulkan/OpenGL entrypoints.

## `src/core/`
- CMakeLists.txt
- ExceptionHandler.hpp / ExceptionHandler.cpp
- Framework.hpp / Framework.cpp
- LicenseStrings.hpp
- Main.cpp
- Mod.hpp / Mod.cpp
- Mods.hpp / Mods.cpp
- WindowFilter.hpp / WindowFilter.cpp
- adapter_loader.h / adapter_loader.cpp
- engine_detection.h / engine_detection.cpp
- Subdirs: `hooks/`, `mods/`, `uevr-imgui/`, `utility/`

Focus: Bootstrap, adapter loading, lifecycle, exception handling, window filtering.

## `src/mods/`
- CMakeLists.txt
- FrameworkConfig.hpp / FrameworkConfig.cpp
- ImGuiThemeHelpers.hpp / ImGuiThemeHelpers.cpp
- LuaLoader.hpp / LuaLoader.cpp
- PluginLoader.hpp / PluginLoader.cpp
- UObjectHook.hpp / UObjectHook.cpp
- VR.hpp / VR.cpp
- Subdirs: `bindings/`, `pluginloader/`, `uobjecthook/`, `vr/`

Focus: Mod/plugin system, Lua, UE object hooking, VR runtime integration.

## `include/uevr/`
- API.h / API.hpp
- CrossEngineAdapterRegistry.cpp
- Framework.hpp
- ICrossEngineAdapter.hpp
- Plugin.hpp
- cross_engine_validation_status_finder.cpp
- Subdirs: `adapters/`, `hooks/`, `vr/`

Focus: Public API and adapter interfaces for cross-engine integration.

## `adapters/common/`
- matrix_extraction.hpp
- memory_management.hpp / memory_management.cpp
- motion_controllers.hpp / motion_controllers.cpp
- performance_optimizer.hpp / performance_optimizer.cpp
- testing_framework.hpp / testing_framework.cpp
- ui_projection.hpp / ui_projection.cpp
- vr_integration.hpp

Focus: Shared adapter utilities (matrix math, memory, controllers, perf, UI reprojection, test scaffolding).

## `adapters/REDengine4/`
- redengine_adapter.hpp
- redengine4_adapter.cpp
- temp_arrow.txt

Focus: REDengine 4 game-specific adapter entrypoints and glue.

## `tests/`
- CMakeLists.txt
- Subdirs:
  - `unit/`
  - `integration/`
  - `fixtures/`

Focus: Unit and integration test suites; fixtures for harnesses.

## `profiles/`
- Per-game folders (examples):
  - `AbioticFactor-Win64-Shipping/`
  - `Cyberpunk2077/`
  - `MonsterHunterWorld/`
  - `_TEMPLATE/`, `_TEMPLATES/`
  - `re7/`, `re7-Win64-Shipping/`, `re8-Win64-Shipping/`

Focus: Adapter configuration, camera definitions, per-title overrides.

---

If a path is missing or outdated, please open an issue. This page mirrors the repository state and will evolve with the codebase.
