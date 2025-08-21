# Unity Adapter (uevrLCX)

This adapter integrates Unity titles with uevrLCX for full VR conversion: true collisions, stereo rendering, input, UI overlays, and performance tuning.

- Rendering hooks:
  - Built-in: Camera.onPreCull/onPostRender
  - SRP (URP/HDRP): RenderPipelineManager.beginCameraRendering/endCameraRendering
- Core APIs:
  - include/uevr/vr/StereoRenderer.hpp (IStereoRenderer)
  - include/uevr/vr/FrameScheduler.hpp (IFrameScheduler)
- Submission: OpenXR primary, OpenVR fallback
- PhysX bridge: real collisions via ray/sweep/overlap; managed Physics fallback
- Compatibility: Depth3D/Reshade, VRGIN interop, legacy SBS as last resort

Quick plan:
1) Disable Unity XR; detect pipeline and choose hook path.
2) Per-frame: FrameScheduler.beginFrame → per-eye render → submit → endFrame.
3) Drive PhysX queries for HMD/capsule and hands. Maintain meter scale.
4) Promote Canvas to quad layers when needed; capture screen-space UI if required.
5) Apply foveation/DRR; prefer FXAA/SMAA when TAA breaks stereo.

See also:
- tools/docs/deepwiki/uevrLCX/ENGINE_UNITY_ADAPTER.md
- ..rewriting/COMPLETE_UEVRLCX_MASTER_PLAN.md
- extended-resources/List.txt
