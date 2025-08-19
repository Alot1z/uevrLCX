# QA Validation Prompt — Resident Evil 7 (RE Engine)

Goal: ship a bug‑free RE7 adapter with verified matrices, hooks, and persistent state.

## Preconditions
- Follow `implement.md` strictly; update artifacts as you go.
- Confirm baseline capture at `captures/renderdoc/RE7/re7_baseline.rdc`.

## Build & Static Checks
- [ ] Build Release, warnings-as-errors:
  - [ ] `adapters/RE-Engine/engine_hooks.cpp`
  - [ ] `adapters/RE-Engine/re7_adapter.cpp`
- [ ] Run clang-tidy/MSVC analyzers on `adapters/RE-Engine/*.{cpp,hpp}`

## Profile & Artifacts
- [ ] `profiles/re7/adapter.json` present with `camera_system.hook_points` (cb_slot, offset_bytes)
- [ ] `profiles/re7/uobjecthook/*.json` present and loading
- [ ] `profiles/re7/README.md` updated (attachments, validation, quirks)

## Runtime Validation
- [ ] Injection succeeds; IDXGISwapChain::Present/ResizeBuffers hooks working
- [ ] REFramework plugin path (dinput8.dll) integration verified if used
- [ ] Stereoscopic rendering parity between eyes; culling and clipping correct
- [ ] Head pose applied; no drift or shaking

## Constant Buffer (CB) & Matrices Mapping
- [ ] Enable probe logs in `engine_hooks.cpp` for first ~120 frames
- [ ] Verify logged VS/PS CB sizes and sample rows match camera matrices
- [ ] Update `profiles/re7/adapter.json` (cb_slot/offset_bytes) to match findings
- [ ] Validate final projection/view matrices by comparing against RenderDoc capture

## Input & Interaction
- [ ] Motion controllers mapped via uevr input abstraction
- [ ] Weapon/utility mapping: Right = weapon, Left = utility
- [ ] UI/hud projection tested; menus readable; cursor aligned

## Stability & Regression
- [ ] Alt-tab, device reset, resolution changes are stable
- [ ] 30+ minutes gameplay without crash/hard hitch
- [ ] Save/Load unaffected by hooks

## Bug‑Free Gate
- [ ] All validation checkboxes complete
- [ ] Zero critical issues open
- [ ] Matrices mapping locked in and documented

## User Testing with various VR hardware configurations
- [ ] Test with multiple headsets (Oculus, WMR, SteamVR)
- [ ] Validate controller bindings and haptics
- [ ] Recenter and guardian/boundary behavior correct

## Performance Validation across different system specifications
- [ ] Measure frametimes on low/mid/high PCs; record in `docs/reports/ResidentEvil7.md`
- [ ] Verify no unusual CPU-GPU sync, stalls, or spikes
- [ ] Confirm FSR/DLSS/XeSS and dynamic resolution do not break stereo

## Final Artifacts
- [ ] `docs/cross-engine/ResidentEvil7.md` updated with CB mapping
- [ ] `profiles/re7/README.md` QA results appended
- [ ] `profiles/re7/adapter.json` final values committed

## Documentation Requirements
- [ ] All progress must be documented continuously throughout QA process
- [ ] Update implementation notes in real-time during validation steps
  - [ ] Maintain traceability between test results and configuration changes
  - [ ] Record all discovered issues and their resolutions in artifact files
  - [ ] Ensure comprehensive coverage of all validation checkpoints in documentation

## QA Results — Pasteback Summary
- Date/Build/Headset: [YYYY-MM-DD] / [hash/tag] / [device]
- Summary: [1–3 lines]
- Build & Static: [pass/fail + notes]
- Runtime Hooks: [Present/ResizeBuffers status, injection]
- Matrices: [cb_slot=X, view_offset=.., proj_offset=.., layout=row/col, handedness=LH/RH, near/far]
- Input/UI: [status/notes]
- Stability: [alt-tab, device reset, long run]
- Performance: [avg fps, frametimes]
- Artifacts Updated: [profiles/re7/adapter.json, profiles/re7/README.md]
- Evidence Links: [captures/renderdoc/RE7/re7_baseline.rdc, logs, screenshots]

---