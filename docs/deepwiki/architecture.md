# Architecture

- __Core__: cross‑engine framework, adapter factory, registry, plugin system.
- __Adapters__: engine‑specific implementations with shared common layer.
- __Hooks__: graphics API interception (D3D11/12, Vulkan), matrix extraction.
- __VR__: OpenXR/OpenVR runtimes, motion controllers, haptics, UI reprojection.
- __Profiles__: JSON schema (`adapter.schema.json`) and `profiles/<Game>/adapter.json`.
- __Build & QA__: CMake, scripts, analyzers, validation, test suites.

Key files/dirs:
- `include/uevr/` public API
- `src/adapters/` with `common/`, `re-engine/`, `redengine4/`, `mt-framework/`
- `src/hooks/` D3D/Vulkan hooks
- `src/core/`, `src/vr/` where present
- `profiles/` game configs, `_TEMPLATE/`

See also: `README_COMPLETE_SYSTEM.md`, `STRUCTURE_README.md`, and `docs/cross-engine/README.md`.
