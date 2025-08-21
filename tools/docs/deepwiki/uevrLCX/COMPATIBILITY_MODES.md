# Compatibility Modes (uevrLCX)

Defines fallback and interop modes to maximize compatibility across engines and titles, based on the consolidated resource list and community recommendations.

## Modes
- Full VR (GeometryStereo)
  - Per-eye rendering via `IStereoRenderer` with true collisions and controller interactions.
  - Requires adapter hooks and access to engine camera/physics.
- DepthStereo (Depth3D + ReShade)
  - Stereo from depth buffer; head tracking via OpenXR/SteamVR/OpenTrack.
  - No true world collision; input via gamepad/mouse synthesis or minimal bindings.
- VRGIN/VRGIN.NET Integration
  - Use existing VRGIN profiles to drive camera/input where available.
  - We provide compositor submission + physics bridge if possible.
- External 3D Driver Interop (Vireio/VorpX)
  - External tool provides stereo; we maintain pose/input glue.
  - Last-resort for hard titles.

## Selection Heuristics
- Prefer Full VR if:
  - Engine adapter (UE/Unity/custom) hooks are established (camera, render events, physics queries).
  - TAA/temporal options are controllable or can be per-eye stabilized.
- Switch to DepthStereo if:
  - IL2CPP/obfuscation prevents camera/physics control, or SRP/TAA cannot be disabled.
  - Render API hook exists but engine integration is blocked.
- Use VRGIN Mode if:
  - A known VRGIN profile exists for the title, offering camera/input control.
- Use External 3D Driver if:
  - Neither Full VR nor DepthStereo yields stable results, or user requests a known VorpX/Vireio profile.

## Operational Notes
- Head tracking: Always enabled via runtime (OpenXR/OpenVR). OpenTrack can supply pose when needed.
- Controllers: In fallback modes, prefer virtual gamepad/mouse synthesis; expose user bindings.
- Collisions: Only Full VR mode enables world collision. Others may provide haptics-only contact.
- UI: Convert to worldspace or compositor quad layers where possible; otherwise pin a HUD plane.

## References
- Resource index: `extended-resources/List.txt`
- Reddit thread: best-conversion discussion integrated into heuristics.
- Adapters: `ENGINE_UNITY_ADAPTER.md`, `ENGINE_UE_ADAPTER.md`
- Rendering: `RENDERING_PATHS.md`
