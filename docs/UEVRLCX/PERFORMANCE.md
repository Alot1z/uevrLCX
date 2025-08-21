# uevrLCX Performance

Targets and recipes for smooth VR.

## Targets
- Min: 72 FPS (Quest 2), <20 ms motion-to-photon.
- Optimal: 90–120 FPS, <11 ms.

## Techniques
- Foveation
  - Fixed foveation via `vrperfkit`.
  - Eye-tracked foveation when OpenXR gaze is available.
- VRS
  - DX12/Vulkan backends; tier-select VRS shading rates.
- Upscalers
  - FSR/NIS/DLSS; dynamic resolution scaling bound to GPU headroom.
- Frame pacing
  - Align with HMD refresh; ATW-friendly scheduling; prediction from OpenXR.
- Post FX
  - Disable TAA or use stable history; prefer SMAA/FXAA; ensure per-eye consistency.

## Per-Backend Notes
- DX11: no native VRS; rely on resolution scaling and upscalers.
- DX12/Vulkan: enable VRS and foveation in tandem.
- OpenGL: limited; prefer DepthStereoPath fallback with careful shader choices.
