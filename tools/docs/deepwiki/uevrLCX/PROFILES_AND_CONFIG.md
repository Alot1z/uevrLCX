# Per-Title Profiles & Config (uevrLCX)

Defines the configuration keys used under `profiles/<Title>/config.json` to tailor adapters and runtime behavior per game.

## File Location
- Per-title folder: `profiles/<Title>/`
- Main file: `profiles/<Title>/config.json`

## Core Keys
- runtime.stereoMode: "GEOMETRY_STEREO" | "DEPTH_STEREO"
- runtime.graphicsAPI: "DX11" | "DX12" | "VULKAN" | "OPENGL"
- runtime.targetFramerate: number (e.g., 90)
- runtime.worldToMeters: number (default 1.0)
- runtime.ipdOverrideMeters: number | null
- runtime.foveation.enabled: boolean
- runtime.foveation.profile: string (driver/runtime specific)

## Rendering
- rendering.disableTAA: boolean
- rendering.lockPerEyeJitter: boolean
- rendering.forceStableAA: boolean (FXAA/SMAA)
- rendering.reverseZ: boolean
- rendering.depthLinearizationHint: string (for DepthStereo)
- rendering.ui.mode: "WORLDSPACE" | "QUAD_LAYER" | "SCREEN_PLANE"
- rendering.ui.distanceMeters: number (default 1.6)

## Physics
- physics.enabled: boolean
- physics.scale: number (alias of worldToMeters if set)
- physics.player.capsule.radius: number
- physics.player.capsule.height: number
- physics.player.stepOffset: number
- physics.player.crouchHeight: number
- physics.layers.masks: { "Interactable": number, "World": number, ... }

## Input
- input.mode: "OPENXR" | "STEAMVR" | "VIRTUAL_GAMEPAD"
- input.gamepadSynthesis.enabled: boolean
- input.bindings: { "Use": "Trigger", "Jump": "A", ... }
- input.headTracking: "RUNTIME" | "OPENTRACK"
- input.recenter.key: string
- input.locomotion: "SMOOTH" | "TELEPORT" | "ROOMSCALE"

## Unity Adapter
- unity.pipeline: "BUILTIN" | "URP" | "HDRP"
- unity.il2cpp.patternSet: string (pattern set ID if needed)
- unity.srp.hooks.enabled: boolean

## Unreal Adapter
- ue.versionHint: "UE4" | "UE5"
- ue.physics: "PHYSX" | "CHAOS"
- ue.taa.tsr.tuning: string (preset name)

## Compatibility
- compat.mode: "FULL" | "DEPTH3D" | "VRGIN" | "EXTERNAL_3D_DRIVER"
- compat.external.driver: "VIREIO" | "VORPX" | null

## Diagnostics
- debug.overlay: boolean
- debug.logs.level: "INFO" | "DEBUG" | "TRACE"
- debug.dump.symbols: boolean

## Notes
- Keys map onto `uevr::vr::IStereoRenderer` and `uevr::vr::IFrameScheduler` where applicable.
- Enum strings must align with `StereoMode` and `GraphicsAPI` used by the core runtime.
- Titles may supply partial configs; unspecified values use sensible defaults.

## References
- Runtime: `include/uevr/vr/StereoRenderer.hpp`, `include/uevr/vr/FrameScheduler.hpp`
- Rendering: `RENDERING_PATHS.md`
- Physics: `PHYSICS_BRIDGE.md`
- Adapters: `ENGINE_UNITY_ADAPTER.md`, `ENGINE_UE_ADAPTER.md`
