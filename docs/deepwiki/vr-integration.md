# VR Integration

- __OpenXR__: runtime selection, session lifecycle, action sets, stereo rendering.
- __OpenVR__: legacy runtime support when needed.
- __Controllers__: input mapping, haptics, gestures.
- __UI Reprojection__: depth or HUD reprojection path, comfort settings.
- __Performance__: foveation options, async timewarp interop where possible.

## Architecture: VRSystem → OpenXRIntegration

This project keeps `VRSystem.cpp` focused on orchestration while delegating OpenXR specifics to `uevr::OpenXRIntegration`:

- __Coordinator__: `src/vr/VRSystem.cpp`
  - Runtime selection and high-level state (`vr_mode_active`, viewport, stereo flags).
  - Calling into integration for OpenXR lifecycle and per-frame work.
- __Integration__: `include/uevr/vr/OpenXRIntegration.hpp` (+ corresponding .cpp)
  - Instance/session creation, input action sets, haptics, swapchains, frame loop.
  - Error reporting via `getLastError()`.

### Lifecycle Mapping

- `initializeOpenXR()` → `OpenXRIntegration::initialize()` → `createSession()` → `initializeInput()` → `initializeHaptics()` → `initializeRendering()` → `beginSession()`
- `initializeVRStereoRendering()` → Ensure `initializeRendering()` is ready and mark stereo active.
- Frame loop:
  - `submitVRFrame()` → `OpenXRIntegration::beginFrame()`
  - `presentVRFrame()` → `OpenXRIntegration::submitFrame()` → `endFrame()`

### Error Handling

- All integration calls log `getLastError()` on failure and return `false` to let the caller react.
- Cleanup calls `OpenXRIntegration::shutdown()` before resetting the unique_ptr.

### Header/Implementation Consistency

If `OpenXRIntegration.cpp` requires layer arguments for `submitFrame()`, either:
- Provide a no-arg overload that composes layers internally; or
- Adapt `presentVRFrame()` to pass the prepared layer list from the render path.

## Context7 References (Authoritative)

- OpenXR Specification: `/khronosgroup/openxr-docs`
- OpenXR SDK (headers/loader): `/khronosgroup/openxr-sdk`
- OpenVR Runtime API: `/valvesoftware/openvr`

These Context7 IDs are used across docs to create deep, traceable links for implementation and validation.

## Developer Checklist

- __Runtime__: Prefer OpenXR when available; fallback to OpenVR as needed.
- __Initialization__: Defer instance/session/swapchain creation to `OpenXRIntegration`.
- __Signatures__: Ensure `OpenXRIntegration.hpp` and `.cpp` match for `submitFrame()` and `endFrame()`.
- __Stereo__: Mark `stereo_rendering_active` only when swapchains are ready.
- __Hooks__: Route per-eye resolves in `src/hooks/D3D11Hook.cpp` into XR swapchains.
- __CMake__: Guard optional OpenXR build/link if not universally required.

## Traceability Matrix (DeepWiki ↔ Source)

- `src/vr/VRSystem.cpp`: coordinator for initialization and frame loop.
- `include/uevr/vr/OpenXRIntegration.hpp`: OpenXR lifecycle, input, haptics, rendering, frames.
- `src/hooks/D3D11Hook.cpp`: rendering path integration for XR swapchains (pending patch).

## Cross-References

- See `docs/deepwiki/context7-integration.md` for cross-engine Context7 strategy and deep linking.
- Deployment plans live under `docs/deployment/Plans/` (see plan below for step-by-step).

## Quick Notes

- Minimal duplication: `VRSystem.cpp` calls into integration, avoiding vendor/runtime-specific code.
- Logging consistency: use `spdlog` and integration `getLastError()` for diagnostics.
- Robustness: defensive re-checks around `initializeRendering()` to ensure swapchains exist.
