# Unity Adapter Dev Plan (SRP + IL2CPP)

## SRP ScriptableRenderPass (Per-Eye)
- URP: Create a per-camera ScriptableRendererFeature inserting a ScriptableRenderPass before post-processing.
- For each eye:
  - Set view/projection with CommandBuffer.SetViewProjectionMatrices
  - Render opaque + transparent queues to eye RT
- Submit via OpenXR; validate TAA off per-camera.

Deliverables
- URP sample project and patch notes
- HDRP variant notes (custom pass before post)

## IL2CPP Resolver Hardening
- Build pattern DB per Unity minor version (method thunks for Camera.Render, RenderPipelineManager events).
- Prefer il2cpp-api metadata registration; fall back to vtable pattern scan.
- Add logging and safe-fail to compatibility mode (Depth3D) if unresolved.

## Utilities
- Canvas → quad layer promotion helper with DPI-aware scaling.
- Batched PhysX Overlap* queries with layer caches.

## Milestones
- M1: Built-in pipeline stereo + submit
- M2: URP SRP pass per-eye
- M3: PhysX bridge + capsule controller
- M4: IL2CPP pattern DB and resolver
- M5: UI promotion + QA pass
