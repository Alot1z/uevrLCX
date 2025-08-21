# Rendering Paths

## GeometryStereoPath (Preferred)
- Duplicate camera; compute per-eye VP; correct IPD and near/far.
- Disable/adjust TAA jitter; handle motion vectors per-eye.
- UI/HUD moved to OpenXR quad layers.

## DepthStereoPath (Fallback)
- Depth3D + ReShade; depth linearization and reverse-Z hints from profile.
- No collision; visual stereopsis only.

## Choosing a Path
- Adapter available or symbols resolvable -> GeometryStereoPath.
- Otherwise -> DepthStereoPath with OpenTrack for head-look and `InputMapper` for controls.

## Enum Mapping
- `uevr::vr::StereoMode::GEOMETRY_STEREO` ↔ GeometryStereoPath.
- `uevr::vr::StereoMode::DEPTH_STEREO` ↔ DepthStereoPath.

## Runtime Submission
- The compositor submission is handled by the runtime layer; renderers pass per-eye textures to the VR runtime after `beginFrame/endFrame`.

## Pitfalls
- Temporal artifacts with TAA/TSR: switch to stable AA.
- Post FX causing eye mismatch: disable per-eye inconsistent effects.
