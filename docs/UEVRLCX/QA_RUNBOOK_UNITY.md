# QA Runbook — Unity Adapter (uevrLCX)

- Smoke
  - Disable Unity XR; verify compositor receives stereo frames.
- Rendering
  - Eye separation/culling match; no cross-eye TAA ghosts; UI readable.
- Physics
  - No head-through-walls; stair step height and slope limits OK; controller collisions register.
- Performance
  - Meet target FPS; adjust dynamic resolution/foveation; validate reprojection stability.
- Regression
  - Toggle TAA on/off; auto-switch to SMAA/FXAA works; Depth3D fallback path functional.

Matrix
- Pipelines: Built-in, URP, HDRP
- APIs: DX11, DX12, Vulkan, OpenGL
- Versions: 2019.4, 2020.x, 2021.x, 2022.x
