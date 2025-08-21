# Validation Checklist (uevrLCX)

This checklist verifies a title delivers realistic VR: correct stereo, true collision, stable input, usable UI, and target performance.

## 1) Smoke Tests
- Launch title with adapter enabled (Unity/UE/custom).
- HMD pose tracks correctly; no double-stereo or fisheye.
- Controllers tracked and visible rig in debug overlay.

## 2) Rendering Correctness
- Stereo path: GeometryStereo preferred. DepthStereo only if required.
- TAA/TSR disabled or per-eye jitter locked; no cross-eye ghosting.
- Per-eye projection matrices match runtime FOV/IPD.
- UI/HUD rendered via worldspace or compositor quad layers (no duplicate overlays).

## 3) Physics & Collision (Realistic Presence)
- Player capsule sweeps correctly resolve steps, slopes, and crouch.
- Controller ray/sphere casts hit expected layers; grabs/proximity triggers fire.
- World scale is 1m ≈ 1 unit (or per-title override) verified by meter stick test.

## 4) Input Mapping
- HMD pose applied late (before render) without rubber-banding.
- Controller bindings mapped to gameplay (use/interact/teleport or gamepad synthesis).
- Recenter works; seated and standing modes switch cleanly.

## 5) UI & Menus
- ScreenSpace Overlay converted to WorldSpace or quad layer at ~1.5–2.0m.
- Cursor/ray UI interaction reliable; no undersampling shimmer in menus.

## 6) Performance & Stability
- Frame pacing stable at target FPS (72/90/120 depending on device).
- Reprojection minimal; no continuous frame drops.
- vrperfkit/driver upscaler or foveation applied where beneficial.

## 7) Engine-Specific Passes
- Unity (Built-in/URP/HDRP): SRP hooks in place; TAA handled; IL2CPP/Mono interop verified.
- Unreal (UE4/UE5 PhysX/Chaos): TAA/TSR tuned; W2M scaling; RDG submission path validated.

## References
- Unity: `ENGINE_UNITY_ADAPTER.md`
- Unreal: `ENGINE_UE_ADAPTER.md`
- Physics: `PHYSICS_BRIDGE.md`
- Rendering: `RENDERING_PATHS.md`
