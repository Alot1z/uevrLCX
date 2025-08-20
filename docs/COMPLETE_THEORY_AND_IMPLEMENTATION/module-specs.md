# Cross-Engine Adapter Module Specs

Purpose: define planned functions per subsystem with acceptance criteria. Keep aligned with `function-registry.md` and per-game docs.

## 1) Initialization
- initAdapter(ctx)
  - Reads `profiles/<Game>/adapter.json`
  - Detects engine (RE-Engine | REDengine4 | MT-Framework)
  - Initializes logging and diagnostics
- Acceptance:
  - Logs engine detection and config values
  - Fails gracefully with actionable errors

## 2) Render Hook Installation
- installRenderHooks(api)
  - D3D11: hook Present/ResizeBuffers
  - D3D12: hook ExecuteCommandLists/Present path
- Acceptance:
  - Stable hook across scene loads; no deadlocks; safe unhook on exit

## 3) Matrix Discovery & Access
- locateMatrixSources(capture|heuristics)
- decodeMatrix(buffer, offset, layout, transpose)
- Acceptance:
  - View/Proj offsets validated against RenderDoc for each game
  - Handles row_major/col_major and transpose correctly

## 4) Stereo Rendering
- updateEyeTransforms(hmdState)
- applyStereoRendering(cmd)
- buildStereoProjections(fovPolicy, near, far)
- Acceptance:
  - Correct IPD separation; no eye inversion
  - Proper near/far and FOV policy applied

## 5) UI Projection
- projectUI(policy)
  - billboard | depthAwareUI | raycastUI
- Acceptance:
  - HUD readable at comfortable scale and distance
  - No excessive parallax for screen-space HUD

## 6) Input Integration
- mapInput(bindings)
  - Aim method alignment: right/left
- Acceptance:
  - Trigger → Fire, Grip → Interact (as configured)
  - Latency within headset frame budget

## 7) Attachments & Persistence
- applyAttachment(path, controller, offsets, rotation, permanent)
- persistAttachment(statePath)
- Acceptance:
  - Matches `uobjecthook/*_mc_state.json` semantics
  - Survives relaunch; Permanent Change semantics documented

## 8) Performance Controls
- setResolutionScale(value)
- uncapFramerate(enable)
- Acceptance:
  - Stable frame pacing; no stutter when toggled

## 9) Diagnostics & Logging
- debug overlays, capture triggers, sanity checks
- Acceptance:
  - One-click capture and dump of current CB slots/offsets

## 10) Config & Schema
- Enforce `adapter.json` schema from `adapter_json_schema.md`
- Acceptance:
  - Validation errors list exact bad paths

Keep this file updated as functions are implemented or changed.
