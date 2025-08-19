# Cross-Engine Adapter Function Registry (Proposed)

Goal: enumerate the key functions/modules we need across engines. Use as a checklist for implementation and documentation. Names are conceptual; adjust to actual implementation.

## Core Lifecycle
- initAdapter(ctx): initialize runtime, detect engine, read adapter.json
- installRenderHooks(api): hook Present/Execute, set up stereo path
- locateMatrixSources(capture|heuristics): resolve CB slots/offsets for view/proj
- updateEyeTransforms(hmdState): compute per-eye view/proj, handle world_scale
- applyStereoRendering(cmd): submit left/right with correct constants
- projectUI(policy): apply selected UI projection technique
- mapInput(bindings): align aim method and controller bindings
- tick(frameTime): per-frame updates and validation hooks
- shutdown(): remove hooks and cleanup

## D3D11/D3D12 Integration
- d3d11HookPresent(swap, sync, flags)
- d3d12HookExecute(cmdQueue, lists, count)
- cbReadWriteHelper(device, context): read/override constants safely

## Matrix Utilities
- decodeMatrix(buffer, offset, layout, transpose)
- buildStereoProjections(fovPolicy, near, far)

## UI
- overlayBillboard(params)
- depthAwareUI(params)
- raycastUI(params)

## Attachments
- applyAttachment(path, controller, offsets, rotation, permanent)
- persistAttachment(statePath)

## Performance
- setResolutionScale(value)
- uncapFramerate(enable)

Keep this aligned with `docs/cross-engine/adapter_json_schema.md` and per-game findings. Update per engine/game pages as functions are implemented.
