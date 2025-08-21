# uevrLCX Architecture

This document defines the unified architecture to convert flat PC games to VR with real collision and universal compatibility. It follows `extended-resources/List.txt` and `..rewriting/COMPLETE_UEVRLCX_MASTER_PLAN.md`.

- Source of truth for resources: `extended-resources/List.txt`
- Community baseline: https://www.reddit.com/r/oculus/comments/am9tyv/best_solution_to_turn_any_game_into_vr_game/

## Core Modules

- HookManager
  - Process injection, renderer API detours (DX9/10/11/12, OpenGL, Vulkan), overlay lifecycle.
- XRRuntime
  - OpenXR-first. Fallbacks: OpenVR/OpenComposite/Revive. Pose, spaces, actions, timing.
- StereoRenderer
  - GeometryStereoPath (preferred) and DepthStereoPath (fallback via Depth3D+ReShade). Hot-switchable per title/profile.
- FrameScheduler
  - HMD-synced pacing; ATW/ASW-friendly prediction.
- PerfOrchestrator
  - Foveated rendering, VRS, dynamic resolution, upscalers; integrates `vrperfkit`.

## Physics and Collision

- CollisionBridge
  - Bridges into game physics when available (UE PhysX/Chaos, Unity PhysX, Bullet, Havok). Exposes raycast/sweep/overlap.
- ControllerColliders
  - Hand colliders with haptics on contact. Interaction layers per game.
- RoomScaleCapsule
  - HMD-synced capsule, locomotion resolution, step offset, crouch.

## Input Mapping

- InputMapper
  - Maps OpenXR/SteamVR actions to game input (keyboard/mouse/XInput). Profiles + OpenVR Input Emulator support.
- HeadAimBridge
  - Optional OpenTrack path for head-look titles with smoothing/deadzone.

## Rendering Paths

- GeometryStereoPath
  - Engine-hooked camera duplication; per-eye VP matrices; correct IPD, near/far, world scale; VR UI layers.
- DepthStereoPath
  - Depth3D + ReShade for stereopsis when geometry path is unavailable. Visual fallback, not a collision source.

## Adapters (src/adapters/)

- UEAdapter (UE4/UE5)
  - Hook `UWorld::Tick`, `APlayerController::CalcCamera`, physics queries; handle TAA jitter; world-to-meters.
- UnityAdapter
  - Hook `Camera.Render`, transforms, PhysX queries; XR override.
- Existing engines (RE/MT-Framework/REDengine4)
  - Extend code in `adapters/` with known render/physics entry-points.
- LegacyAdapter (DX9/10/OpenGL)
  - Minimal geometry signals; prefer depth fallback; optional signatures for physics symbols.

## Profiles and Mods

- `profiles/<GameName>/config.json`
  - Camera offsets, FOV, world scale, input bindings, physics scale, post FX toggles, depth linearization hints.
- `lua-api/` hooks for title-specific patches.

## Tiered Compatibility Matrix

- Tier A: Engine-hook + geometry stereo + real collision (target path).
- Tier B: Partial physics + depth stereo.
- Tier C: Depth-only + head tracking (max compatibility).

## Performance Stack

- Foveation (fixed/eye-tracked), VRS (DX12/VK), upscalers (FSR/NIS/DLSS), dynamic res, occlusion culling; per-HMD presets.

## Context7 + DeepWiki

- Context7: structured API and integration pages, cross-referenced indices.
- DeepWiki: deep-dive hooking patterns per engine, physics symbol maps, rendering quirks.

## File/Dir References

- `extended-resources/tools/`: VRGIN, Depth3D, ReShade, physics, optimization, ai-ml, neural-rendering.
- `scripts/generate_context7_database.py`: index docs.
- `tools/docs/deepwiki/`: DeepWiki content root.
