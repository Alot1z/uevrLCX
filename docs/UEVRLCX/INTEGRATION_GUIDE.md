# uevrLCX Integration Guide

How to select and enable the best path per game while ensuring real collision whenever possible.

## 1. Choose the Rendering Path

- Prefer GeometryStereoPath when the engine adapter is available or symbols are resolvable.
- Use DepthStereoPath (Depth3D+ReShade) only as a fallback for visuals when geometry path isn’t feasible.

## 2. Enable Physics Bridge (Real Collision)

- UE: PhysX/Chaos via UE symbols; enable sweep/raycast/overlap in `CollisionBridge`.
- Unity: PhysX via IL2CPP/Mono bridges; inject colliders on hand objects.
- Others: attempt pattern-based symbol resolution; if not possible, enable minimal internal physics for haptics-only.

## 3. Configure Input

- Map OpenXR/SteamVR actions to game controls via `InputMapper`.
- For head-look titles, enable OpenTrack bridging with configurable deadzones.

## 4. Profiles

- Copy `profiles/template/config.json` to `profiles/<GameName>/config.json` and customize.
- Key fields: rendererMode, cameraOffset, fovDegrees, ipdScale, physicsScale, inputBindings, depthHints.

## 5. Performance Setup

- Start with `vrperfkit` defaults. Enable fixed foveated rendering; enable VRS on DX12/Vulkan; use FSR/NIS and DLSS where available.
- Use `FrameScheduler` targets matching HMD refresh; test 72/80/90/120 Hz.

## 6. Reddit/Community Presets

- Reference links in `extended-resources/List.txt` (3D Vision SBS, ReShade presets, VRGIN tips).
- Import community profiles as starting points; convert into uevrLCX profile JSON.

## 7. Troubleshooting

- TAA jitter: lock history or force camera jitter off per engine; adjust projection for per-eye stability.
- UI too close/flat: migrate HUD to OpenXR quad layers; scale and position.
- Physics mismatch: verify world scale and near clipping; recalibrate `physicsScale` and controller collider sizes.

## 8. Context7 + DeepWiki

- Add integration notes as Context7 topics; add deep-dive pages in `tools/docs/deepwiki/`.
- Run `scripts/generate_context7_database.py` to refresh the index.
