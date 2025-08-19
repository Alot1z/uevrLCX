# QA Validation Prompt — Cyberpunk 2077 (REDengine 4)

Goal: deliver a bug‑free REDengine 4 adapter with complete profile, stable rendering, and correct input.

## Preconditions
- Follow `implement.md` strictly; update artifacts continuously.
- Confirm baseline capture at `captures/renderdoc/Cyberpunk2077/cp2077_baseline.rdc`.

## Build & Static Checks
- [ ] Build Release, warnings-as-errors:
  - [ ] `adapters/REDengine4/redengine4_adapter.cpp`
  - [ ] `adapters/REDengine4/redengine_adapter.cpp`
- [ ] Run static analysis on `adapters/REDengine4/*.{cpp,hpp}`

## Profile & Artifacts
- [ ] `profiles/Cyberpunk2077/adapter.json` present with engine parameters
- [ ] `profiles/Cyberpunk2077/uobjecthook/*.json` present and loading
- [ ] `profiles/Cyberpunk2077/README.md` updated (attachments, validation)

## Runtime Validation
- [ ] Injection attaches; appropriate D3D12/graphics hooks active
- [ ] Stereo rendering correct; eye parity and depth consistent
- [ ] Head pose and FOV behave correctly (no warping/judder)
- [ ] HUD/UI projected to VR space; menus usable

## Camera & Rendering
- [ ] First-person camera transform correct; weapon alignment verified
- [ ] IPD scale and near/far planes validated across scenes
- [ ] Temporal upscalers (FSR/DLSS/XeSS) do not introduce stereo artifacts

## Input & Interaction
- [ ] Motion controller mappings functional
- [ ] Weapon/inventory interactions responsive
- [ ] Haptics and recenter work as expected

## Stability & Regression
- [ ] Graphics settings changes safe; device resets stable
- [ ] 30+ minutes gameplay stable; no memory growth/leaks

## Bug‑Free Gate
- [ ] All validation checkboxes complete
- [ ] Zero critical issues open
- [ ] Artifacts and docs updated

## User Testing with various VR hardware configurations
- [ ] Test across Oculus, WMR, and SteamVR/OpenXR devices
- [ ] Validate bindings/haptics, seated vs roomscale

## Performance Validation across different system specifications
- [ ] Collect frametimes on low/mid/high PCs; document bottlenecks
- [ ] Validate performance features do not break VR comfort

## Final Artifacts
- [ ] `docs/cross-engine/Cyberpunk2077.md` updated
- [ ] `profiles/Cyberpunk2077/README.md` QA results appended
- [ ] `profiles/Cyberpunk2077/adapter.json` finalized

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
- Runtime Hooks: [Present/Present1, ExecuteCommandLists, ResizeBuffers]
- Matrices: [root_param=.. / CBV idx=.., view_offset=.., proj_offset=.., layout=row/col, handedness=LH/RH, near/far]
- Input/UI: [status/notes]
- Stability: [graphics settings, device reset, long run]
- Performance: [avg fps, frametimes]
- Artifacts Updated: [profiles/Cyberpunk2077/adapter.json, profiles/Cyberpunk2077/README.md]
- Evidence Links: [captures/renderdoc/Cyberpunk2077/cp2077_baseline.rdc, logs, screenshots]

---