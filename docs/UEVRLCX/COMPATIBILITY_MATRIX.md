# uevrLCX Compatibility Matrix

Initial high-level view; refine with per-title profiles in `profiles/`.

| Engine / API | Rendering Path | Collision | Notes |
|---|---|---|---|
| UE4/UE5 | GeometryStereoPath | PhysX/Chaos via bridge | TAA handling, world scale |
| Unity (2019+) | GeometryStereoPath | PhysX bridge | XR override, IL2CPP symbols |
| REDengine4 | Mixed | Partial physics | Extend existing adapter |
| MT-Framework | Mixed | Partial physics | Adapter exists; add physics where possible |
| Custom/Legacy DX9/10 | DepthStereoPath | Minimal (haptics only) | Attempt symbol resolution; fallback depth |
