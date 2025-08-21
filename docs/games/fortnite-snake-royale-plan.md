# Fortnite: Snake Royale — VR Conversion Plan (uevrLCX)

## 1) Overview
- Purpose: Define a safe, non-invasive plan to enable a VR play experience for the Fortnite “Snake Royale” mode while aligning to uevrLCX’s modular architecture.
- Engine: Unreal Engine (Fortnite/UEFN). Adapter reference: `docs/adapters/unreal-engine.md`.
- Cross-engine parity: Provide a Unity parity checklist so the Unity adapter documentation (`docs/engines/unity-engine.md`) stays consistent with the same gameplay blueprint.
- Scope: PC VR via injection. No mod file redistribution. Docs-only plan; no build breakage.

Assumptions (explicit):
- “Snake Royale” is an arena-style experience with a head/camera, body growth, pickups, obstacles, and a score/placement UI.
- We do not rely on unpublished engine internals. We focus on hook patterns and interface mappings.

## 2) Architecture Alignment (uevrLCX)
- Rendering: `include/uevr/vr/StereoRenderer.hpp` — stereo modes, IPD, near/far plane, per-eye begin/end.
- Input: `include/uevr/vr/InputMapper.hpp` — bind actions (turn-left/right, boost, confirm, menu), pose queries, haptics.
- Physics: `include/uevr/vr/PhysicsBridge.hpp` — collisions (head/pickups/obstacles), raycasts, rigid body params.
- Frame pacing: `include/uevr/vr/FrameScheduler.hpp` — VR frame pacing separate from game tick; frame skip policy.
- Performance: `include/uevr/vr/PerfOrchestrator.hpp` — targets (FPS/latency), dynamic resolution, toggles (VRS/foveation/upscalers).

## 3) Gameplay-to-VR Feature Map
- Camera & Presence
  - Attach VR head pose to player camera rig (third-person or top-down comfort modes).
  - World scale validation; comfort vignette and snap/smooth turn options.
- UI/HUD
  - Convert core HUD to world-space: score, length, leaderboard cues, health/energy.
  - Menus at fixed depth (1.5–2.0m), large font scale, high-contrast.
- Controls
  - Map thumbstick: turn-left/right, forward impulse (if relevant), snap turn (comfort).
  - Bind click/trigger: boost, interact, confirm.
  - Recenter pose and quick-menu binding.
- Physics & Collisions
  - Head vs pickup overlap events -> score/length growth.
  - Head vs obstacle/body -> penalty/KO logic. Collision layers with predictable masks.
  - Optional raycast feelers for obstacle anticipation (haptics cue).
- Performance & Rendering
  - Stereo mode selection (multiview/instanced/double-pass per backend).
  - Foveated rendering (hardware/runtime), dynamic res targets, lens-matched shading where available.
  - Asynchronous reprojection compatibility checks per runtime.

## 4) Unreal (Fortnite/UEFN) Adapter Notes
Reference: `docs/adapters/unreal-engine.md`
- Detection & Hooks
  - Use adapter’s process/module/pattern checks to confirm Unreal runtime and rendering API (DX11/12/Vulkan).
  - Install present/swap hooks via the adapter’s render hook surfaces for stereo begin/end.
- Camera Binding
  - Intercept active camera transform each frame; apply HMD pose with clamped pitch/roll if comfort mode requires.
  - Provide menu toggle between “top-down comfort” and “chase/over-shoulder.”
- Input Mapping
  - Use adapter input hook to map VR controller axes/buttons to gameplay turn/boost/menu. Keep native KB/M inputs intact.
  - Haptics when pickup is collected or near-collision (short pulse).
- UI Strategy
  - Identify HUD widgets; render a duplicate in world-space at fixed depth layer for clarity in VR.
  - Cursor ray + reticle for menu interactions.
- Physics/Collisions
  - Read collision events via adapter-safe interfaces; avoid invasive replacement. Route to `IPhysicsBridge` events.
  - Scale and gravity sanity checks for motion sickness reduction.
- Frame Pacing & Perf
  - Use `IFrameScheduler` to decouple tick pacing from display refresh. Coordinate with `IPerfOrchestrator` for dynamic res.

## 5) Unity Parity Checklist (keep engines aligned)
Use as a mirror for `docs/engines/unity-engine.md`:
- Rendering
  - XR: `UnityXR_Initialize`, `UnityXR_Update`, `UnityXR_Render`, `UnityXR_SubmitFrame`.
  - Stereo begin/end per eye with correct near/far and IPD from profile.
- Input
  - Unity Input System or XR Input: map turn/boost/menu; expose bindings in adapter config.
  - Haptic pulses on pickups/collisions.
- UI
  - Convert Canvas to World Space; set 1.5–2.0m depth, scale fonts/icons for legibility.
  - Laser pointer or direct touch (XR Interaction Toolkit).
- Physics
  - Use layer-based collisions and trigger volumes; head overlap -> pickup; obstacle/body collision -> KO.
  - Optional forward raycasts for early warning.
- Frame Pacing & Perf
  - OpenXR/SteamVR reprojection compatibility, dynamic resolution scaler, foveated (if platform supports), GPU frame time budget.

## 6) Phased Delivery (Docs-First)
- Phase A — Hook & Render Scaffolding
  - Verify engine detection and stereo render path reachable; show test grid in VR.
- Phase B — Input + Camera Modes
  - Bind controls; implement top-down comfort + chase camera; recenter.
- Phase C — UI/HUD VRization
  - World-space HUD and menus; laser pointer interaction.
- Phase D — Physics & Feedback
  - Stable collision events; growth/score updates; haptics cues.
- Phase E — Performance Pass
  - Targets via `IPerfOrchestrator`; dynamic res; foveated; QA on typical PCVR rigs.

## 7) QA Validation Checklist
- Presence
  - Head tracking stable; no unintended camera roll; comfort modes switch correctly.
- Interaction
  - Turn, boost, menu, recenter all functional; bindings configurable.
- UI
  - HUD readable, correctly anchored, not clipping; menus usable with ray.
- Physics
  - Pickups increment score/length; obstacle/body collisions end round as expected.
- Performance
  - 90 Hz target feasible on recommended GPU; reprojection fallback smooth; no major spikes.

## 8) Deliverables
- This plan: `docs/games/fortnite-snake-royale-plan.md`.
- Unity parity integration: link from `docs/engines/unity-engine.md`.
- Optional profiles and presets (future): `profiles/Fortnite/` with input bindings and comfort defaults.

## 9) References
- Unreal Adapter: `docs/adapters/unreal-engine.md`
- Unity Engine: `docs/engines/unity-engine.md`
- Core Interfaces:
  - `include/uevr/vr/StereoRenderer.hpp`
  - `include/uevr/vr/InputMapper.hpp`
  - `include/uevr/vr/PhysicsBridge.hpp`
  - `include/uevr/vr/FrameScheduler.hpp`
  - `include/uevr/vr/PerfOrchestrator.hpp`
