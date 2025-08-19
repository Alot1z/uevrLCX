# QA Validation Prompt — Monster Hunter: World (MT Framework)

Goal: ship a bug‑free MT Framework adapter with correct camera conversion, input, and performance.

## Preconditions
- Follow `implement.md` strictly; maintain artifacts.
- Confirm baseline capture at `captures/renderdoc/MonsterHunterWorld/mhw_baseline.rdc`.

## Build & Static Checks
- [ ] Build Release, warnings-as-errors:
  - [ ] `adapters/MT-Framework/mt_framework_adapter.cpp`
  - [ ] `adapters/MT-Framework/mt_framework_bridge.cpp`
- [ ] Run static analysis on `adapters/MT-Framework/*.{cpp,hpp}`

## Profile & Artifacts
- [ ] `profiles/MonsterHunterWorld/adapter.json` present with engine config
- [ ] `profiles/MonsterHunterWorld/uobjecthook/*.json` present and loading
- [ ] `profiles/MonsterHunterWorld/README.md` updated (attachments, validation)

## Runtime Validation
- [ ] Injection success; hooks active in rendering path
- [ ] Third→first person camera conversion correct; no offset drift
- [ ] Stereo rendering stable; culling and clipping correct
- [ ] HUD/UI projection readable and well-aligned

## Input & Interaction
- [ ] Motion controller mappings for combat/utilities validated
- [ ] Weapon on Right, Utility on Left; haptics functional
- [ ] Menu interactions reliable; cursor alignment ok

## Stability & Regression
- [ ] Device reset, resolution, fullscreen/windowed stable
- [ ] 30+ minutes gameplay without crash/hard hitch

## Bug‑Free Gate
- [ ] All validation checkboxes complete
- [ ] Zero critical issues open
- [ ] Docs and artifacts updated

## User Testing with various VR hardware configurations
- [ ] Test across Oculus, WMR, SteamVR/OpenXR
- [ ] Verify bindings/haptics and recenter behavior

## Performance Validation across different system specifications
- [ ] Measure frametimes on low/mid/high PCs and document
- [ ] Validate optimization flags and settings do not break visuals

## Final Artifacts
- [ ] `docs/cross-engine/MonsterHunterWorld.md` updated
- [ ] `profiles/MonsterHunterWorld/README.md` QA results appended
- [ ] `profiles/MonsterHunterWorld/adapter.json` finalized

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
- Stability: [device reset, resolution changes, long run]
- Performance: [avg fps, frametimes]
- Artifacts Updated: [profiles/MonsterHunterWorld/adapter.json, profiles/MonsterHunterWorld/README.md]
- Evidence Links: [captures/renderdoc/MonsterHunterWorld/mhw_baseline.rdc, logs, screenshots]

---