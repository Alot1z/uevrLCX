# Unity Adapter Deep-Dive

## Goals
- Real collision via PhysX bridge.
- Correct stereo via per-eye VP and camera duplication.

## Hooks
- `Camera.Render` (or SRP callbacks) for matrices and submission.
- Transform updates for HMD and controllers.
- IL2CPP/Mono symbol mapping; domain attach for managed interop.

## Rendering
- Disable Unity XR when injecting; enforce our stereo path.
- Handle TAA/Temporal AA: prefer SMAA/FXAA; ensure per-eye stability.
- UI: move worldspace canvas to OpenXR quad layers when possible.

## Physics
- PhysX raycast/sweep/overlap via exported symbols (IL2CPP) or `UnityEngine.Physics*` (Mono).
- Player capsule, hand/controller colliders, ground snapping; world-scale sync from HMD IPD.
- Attach character controller to camera rig; resolve penetrations with depenetration sweeps.

## Input & Tracking
- HMD pose from OpenXR/OpenVR -> apply to camera rig root each frame.
- Motion controllers -> Unity transforms for hands; optional input passthrough to game UI.
- Game input mapping: emulate mouse/KB/pad when native bindings absent.

## Community-Proven Conversion Modes
- Reshade + Depth3D (SBS/TAB) for wide compatibility; feed into `StereoRenderer` for per-eye projection.
- Legacy 3D Vision SBS path for DX9/DX11 titles where native depth is inaccessible.
- VRGIN/VRGIN.NET injection for Unity titles to bootstrap VR rigs quickly.
- Our path: true 6DoF + collision via PhysX bridge; avoids “fake depth only”.

## Integration Steps (Unity)
1) Disable UnityXR: force our stereo render; ensure single-camera path in URP/HDRP/Built-in.
2) Hook render:
   - Built-in: `Camera.onPreCull/onPreRender/onPostRender`.
   - SRP: `RenderPipelineManager.begin/endCameraRendering`.
3) Per-eye matrices: compute from HMD poses; set view/proj, call into `uevrLCX` `StereoRenderer`.
4) Depth: prefer native depth; fallback to Reshade Depth3D for SBS when blocked.
5) UI: convert worldspace Canvas to quad layers; screen-space -> curved quad in world.
6) Physics: PhysX sweeps/overlaps for headset/hands; character controller with climb/step.
7) IL2CPP/Mono: domain attach; resolve `UnityEngine` types; pattern sets for stripped symbols.
8) Performance: integrate `vrperfkit` and foveated/multi-res presets; turn off TAA, prefer SMAA/FXAA.

## Performance & Stability
- Use `FrameScheduler` to maintain HMD cadence; decouple render and simulation where possible.
- Foveated rendering (fixed/eye-tracked) plus resolution scaling per eye.
- Async reprojection compatibility: avoid excessive late latching; limit CPU spikes with jobified physics queries.

## Testing
- URP/HDRP/Built-in sample scenes: verify stereo proj, culling masks, and post FX per eye.
- IL2CPP builds with obfuscation: validate pattern-matching and metadata registration path.
- Physics harness: head/hand penetration, stair step height, slope limits, and scale accuracy.

## TODO (Unity Adapter)
- [ ] Implement SRP hook shims and built-in camera hooks.
- [ ] Matrix path -> `StereoRenderer` integration.
- [ ] IL2CPP resolver, domain attach, and metadata registration.
- [ ] PhysX bridge: ray/sweep/overlap; character controller.
- [ ] UI quad-layer conversion utilities.
- [ ] Reshade/Depth3D fallback wiring for SBS/TAB.
- [ ] vrperfkit + foveated presets; disable TAA automatically.
- [ ] Test suite across Unity 2019.4–2022.x; URP/HDRP variants.

## Adapter Overview
- Works across Built-in, URP, HDRP and both Mono and IL2CPP builds.
- Disables any in-game XR to avoid double-stereo and drives a custom per-eye render using compositor APIs (OpenXR/OpenVR).
- Provides two modes:
  - "Full VR" (true collisions, real world-scale, VR controllers).
  - "Compatibility VR" (Depth3D/Reshade stereo + head tracking; no world physics) for hard titles.

## Injection & Interop
- Mono builds:
  - Attach via `mono_get_root_domain` → `mono_thread_attach`.
  - Resolve `UnityEngine` classes by name; optionally patch with Harmony where available.
- IL2CPP builds:
  - Use `il2cpp_domain_get`, `il2cpp_thread_attach`, `il2cpp_assembly_get_image`,
    `il2cpp_class_from_name`, `il2cpp_class_get_method_from_name` to locate managed API entrypoints.
  - Pattern-set fallback for stripped symbols; prefer metadata registration table.
- Frame entrypoints:
  - Built-in: hook `Camera.onPreCull`/`onPostRender` or detour `Camera.Render`.
  - SRP: hook `UnityEngine.Rendering.RenderPipelineManager.beginCameraRendering`/`endCameraRendering`.

## Stereo Render Path
1) Detect/disable Unity XR: set `XRSettings.enabled = false` (when accessible) or block XR init calls.
2) Construct per-eye poses/projections from runtime (OpenXR/OpenVR) each frame.
3) For each eye:
   - Clone the active camera settings to a hidden eye camera.
   - Set `camera.projectionMatrix` and `camera.worldToCameraMatrix` for that eye.
   - Render to eye-specific RT(s); avoid TAA (use FXAA/SMAA if needed).
4) Submit to VR compositor:
   - OpenXR: composition layer (color; optional depth where supported by API layer).
   - OpenVR: `VRCompositor()->Submit` per-eye.

Notes:
- For HDRP, prefer SRP hooks; for URP, disable per-camera TAA (`cameraData.antialiasing = None`).
- If the title enforces TAA, switch to compatibility mode (below) to mitigate shimmer.

## UI Handling
- WorldSpace Canvas: keep; anchor to HMD or world as designed; scale to meters.
- ScreenSpace Overlay: convert to WorldSpace at runtime and pin in front of camera; as a fallback, capture to texture and display on an OpenXR quad layer at a fixed depth.
- Cursor and ray interactions: attach XR ray to dominant controller for UI.

## Physics: Real Collision Bridge
- Player: spawn `VRPlayer` GameObject with `CapsuleCollider` + kinematic `Rigidbody`.
  - Advance using `Physics.CapsuleCast` sweeps to resolve movement without tunneling.
- Controllers: small `SphereCollider` triggers for proximity/grab tests; use `Physics.Raycast`/`SphereCast` for interaction.
- Query layer masks from active scene; maintain world-scale (meters) from HMD IPD or known 1m = 1 unit mapping.
- IL2CPP: call into `UnityEngine.Physics` via resolved il2cpp invocations when C# access is stripped.

Limitations:
- Some games gate logic behind in-engine character controllers; prefer pass-through (move original controller) when detected, else use our kinematic body.

## Input Mapping (OpenXR/SteamVR → Unity)
- HMD pose → set main camera transform late (after game logic, before render). Use SRP `beginCameraRendering` timing.
- Controllers → drive `VRRig` transforms (L/R hands). Provide bindings for common actions (grab/use/teleport) and fallback to gamepad/mouse synthesis for gameplay.
- Recentering → zero out yaw on demand; maintain seated and standing modes.

## Performance & Stability
- Foveated/FSR/NIS: support driver-level or `vrperfkit` style upscaling where compatible.
- Multi-resolution or fixed foveation: optional via runtime where supported (vendor extensions).
- TAA: disable where possible; prefer per-eye stable AA (FXAA/SMAA). Ensure motion vectors aren’t reused cross-eye.
- Frame pacing: late-latch HMD pose; prefer async reprojection from runtime.

## Community Compatibility Modes (per Reddit + List.txt)
- Depth3D + Reshade Mode: stereo from depth buffer + head tracking (OpenTrack/SteamVR). No true collisions; best-effort camera stabilization.
- VRGIN/VRGIN.NET Mode: when a VRGIN profile exists for the title, delegate camera and input via VRGIN hooks.
- Vireio/VorpX Interop: allow external 3D driver to provide stereo while we keep controller/head tracking glue; last-resort path.

## Validation & Test Matrix
- Unity 2019 LTS (Built-in), 2021+ URP, 2021+ HDRP.
- Mono vs IL2CPP builds; Windows x64.
- Known-good samples to verify: simple FPS (CharacterController), vehicle controller, UI-heavy menu.

## Unity ↔ uevrLCX API Coupling
- Rendering → `uevr::vr::IStereoRenderer` (`include/uevr/vr/StereoRenderer.hpp`):
  - Init once: `initialize(GraphicsAPI::AUTO, StereoMode::MULTI_PASS)`.
  - Per frame: `beginFrame(frameIndex)` → per eye: `renderEye(eyeIndex, sceneHandle, renderTarget, EyeViewport)` → `endFrame()`.
  - Set near/far and IPD from Unity camera/HMD at startup: `setNearFar`, `setIPD`.
- Frame pacing → `uevr::vr::IFrameScheduler` (`include/uevr/vr/FrameScheduler.hpp`):
  - Init with `initialize(targetFps)`; each tick: `beginFrame()` (skip if false) → `endFrame()`.
  - Use `getLastTimings()` to drive adaptive resolution/foveation when CPU/GPU over budget.
- Hook timing:
  - Built-in: call `beginFrame` in `onPreCull` of main camera; submit in `onPostRender`.
  - SRP: call `beginFrame` in `beginCameraRendering` for the main camera; submit after last eye in `endCameraRendering`.

## Submission and Profiles
- Primary submission: OpenXR through core runtime; fallback: OpenVR compositor.
- User profiles (directory: `profiles/Unity/`):
  - `rendering.json`: stereo mode, AA policy, foveation, dynamic resolution bounds.
  - `input.json`: action bindings, deadzones, snap/continuous turn, recenter.
  - `compat.json`: pipeline overrides (URP/HDRP), depth fallback enable, UI layer rules.

## Compatibility Decision Flow
1) Detect Unity version and SRP (URP/HDRP/Built-in) → pick hook path.
2) Probe XR providers → disable if enabled to prevent double stereo.
3) Attempt native-depth stereo. If TAA/motion vectors break per-eye stability → switch AA to SMAA/FXAA.
4) If depth blocked or camera locked: enable Depth3D + head tracking compatibility mode.
5) If still unstable: enable legacy SBS (3D Vision-style) as last resort.

## Open Items / Missing Pieces
- IL2CPP metadata resolver hardening for obfuscated builds (pattern database per Unity minor).
- SRP `ScriptableRenderPass` eye-splitter sample implementation (URP/HDRP variants).
- Canvas → OpenXR quad layer promotion utility with DPI-aware scaling.
- PhysX bridge perf path: batched `Overlap*` queries and layer-masked caches.
- vrperfkit-style foveation integration toggles and runtime sliders.

## QA Runbook
- Smoke: start title, ensure Unity XR disabled, VR compositor shows stereo.
- Render: eye separation correct, no culling mismatch, UI readable (quad layer if needed).
- Physics: no head-through-walls; stair step works; controllers collide with props.
- Performance: hold target FPS from `IFrameScheduler`; adjust DRR/foveation until green.
- Regression: toggle TAA on/off; verify auto-switch to FXAA/SMAA; Depth3D fallback path functional.

## References
- Master plan: `..rewriting/COMPLETE_UEVRLCX_MASTER_PLAN.md`.
- Resource index: `extended-resources/List.txt`.

## Troubleshooting
- IL2CPP obfuscation: use pattern sets and metadata registration.
- SRP differences (URP/HDRP): hook appropriate camera events.
