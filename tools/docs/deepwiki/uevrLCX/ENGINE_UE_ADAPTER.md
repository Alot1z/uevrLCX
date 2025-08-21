# Unreal Engine Adapter Deep-Dive

## Goals
- Real collision via PhysX/Chaos bridge.
- Correct stereo via camera duplication and per-eye VP.

## Hooks
- `UWorld::Tick`
- `APlayerController::CalcCamera`
- Render thread: RDG/ENQUEUE_RENDER_COMMAND for camera matrices.

## Rendering
- Handle TAA jitter (lock or disable per-eye jitter).
- World-to-meters scaling; per-eye projection.
- HUD/UI migrated to OpenXR quad layers.

## Physics
- Raycast, sweep, overlap; step offset; crouch.
- Map collision channels to VR interaction layers.

## Adapter Overview
- Targets UE4/UE5 (PhysX/Chaos) with per-eye stereo and real collisions.
- Disables any built-in HMD camera management we don’t control; owns per-eye view/projection.
- Modes:
  - Full VR (geometry stereo, collisions, controllers).
  - Compatibility (depth stereo + head tracking) for hard titles.

## Injection & Interop
- Game thread: hook `UWorld::Tick` and `APlayerController::PlayerTick` to update HMD/controller transforms late.
- Camera: intercept `APlayerController::CalcCamera` to apply per-eye view and projection via our runtime.
- Render thread: enqueue per-eye matrices and targets via RDG commands before submission.
- Runtime: drive through `uevr::vr::IStereoRenderer` and schedule with `uevr::vr::IFrameScheduler`.

## Stereo Render Path
1) Lock/disable temporal jitter per-eye to avoid cross-eye mismatch.
2) Compute per-eye `ViewMatrix`/`ProjMatrix` from VR runtime each frame.
3) Render both eyes to eye-specific RTs; submit to VR compositor.
4) UI/HUD: prefer quad layers to avoid double-resolve and shimmer.

## Physics: Real Collision Bridge
- Prefer native physics (PhysX/Chaos). Expose raycast/sweep/overlap; step/slide logic.
- Player: kinematic capsule driven by sweeps; respect step offset/crouch.
- Controllers: sphere/ray queries for interaction; map to collision channels.
- Scale: world-to-meters sync with project settings; override per-title as needed.

## Input Mapping
- HMD pose: apply after game logic before render.
- Controllers: map XR inputs to gameplay (use/gamepad synthesis when necessary).
- Recenter and locomotion modes (seated/standing) exposed via profile.

## Performance & Stability
- TAA/TSR: prefer stable AA or per-eye jitter lock; consider FXAA/SMAA.
- Frame pacing: schedule via `IFrameScheduler`, allow skip-frame when behind.
- Foveation/upscalers: enable via driver or runtime where supported.

## Community Compatibility Modes
- DepthStereoPath with Depth3D/Reshade + head tracking when geometry path not viable.
- External 3D drivers (Vireio/VorpX) interop as last-resort with our input glue.

## Validation & Test Matrix
- UE4 (PhysX) and UE5 (Chaos) samples.
- CharacterController games, vehicle games, UI-heavy games.
- DX11/DX12 rendering paths.

## References
- Runtime: `include/uevr/vr/StereoRenderer.hpp`, `include/uevr/vr/FrameScheduler.hpp`.
- Master plan: `..rewriting/COMPLETE_UEVRLCX_MASTER_PLAN.md`.
- Resources: `extended-resources/List.txt`.

## Troubleshooting
- TAA ghosting: prefer TSR settings; disable temporal where needed.
- FOV clamping: adjust via console vars.
