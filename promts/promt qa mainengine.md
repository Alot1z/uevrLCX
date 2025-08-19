# QA Validation Prompt — Main Cross-Engine (RE Engine, REDengine 4, MT Framework)

Goal: ensure all cross-engine adapters are bug-free, fully validated, and all artifacts are present and correct.

## Scope
- Applies to adapters under `adapters/RE-Engine/`, `adapters/REDengine4/`, `adapters/MT-Framework/`.
- Targets games: RE7 (`re7.exe`), Cyberpunk 2077 (`Cyberpunk2077.exe`), Monster Hunter: World (`MonsterHunterWorld.exe`).

## Preconditions
- Read and follow `implement.md` (EVERY section, EVERY line).
- Keep artifacts up-to-date in `docs/`, `profiles/<Game>/`, and `captures/renderdoc/`.
- Use Context7 for docs and Deepwiki for repo-specific guidance when needed.

## Build & Static Analysis
- [ ] Configure and build all adapters in Release with warnings-as-errors.
  - [ ] `adapters/RE-Engine/engine_hooks.cpp`, `re7_adapter.cpp`
  - [ ] `adapters/REDengine4/redengine4_adapter.cpp`, `redengine_adapter.cpp`
  - [ ] `adapters/MT-Framework/mt_framework_adapter.cpp`, `mt_framework_bridge.cpp`
- [ ] No unresolved symbols, no deprecated APIs, zero new warnings.
- [ ] Run static analysis (MSVC/clang-tidy) on adapters and headers.

## Profiles & Artifacts Presence
- [ ] `profiles/re7/adapter.json` with camera matrices mapping and hooks
- [ ] `profiles/Cyberpunk2077/adapter.json` engine settings
- [ ] `profiles/MonsterHunterWorld/adapter.json` engine settings
- [ ] `profiles/<Game>/uobjecthook/*.json` persistent state exists and loads
- [ ] RenderDoc baselines:
  - [ ] `captures/renderdoc/RE7/re7_baseline.rdc`
  - [ ] `captures/renderdoc/Cyberpunk2077/cp2077_baseline.rdc`
  - [ ] `captures/renderdoc/MonsterHunterWorld/mhw_baseline.rdc`
- [ ] Per-game `README.md` updated with attachments and validation results

## Runtime QA — Injection & Hooks
- [ ] UEVRInjector attaches cleanly to each game (no crashes, no deadlocks)
- [ ] Swapchain hooks active (D3D11 for RE, D3D12 for REDengine4 if applicable, MT Framework path verified)
- [ ] Stereoscopic rendering correct in both eyes; no eye inversion or culling issues
- [ ] Camera tracking and head pose applied correctly; no matrix jitter
- [ ] Input mapping functional (motion controllers, UI interactions)

## Matrices & Rendering Validation
- [ ] RE7: constant buffer readout aligns with `profiles/re7/adapter.json` (cb_slot, offset_bytes)
- [ ] CP2077: projection/view matrices produce correct IPD scale and FOV
- [ ] MHW: third-to-first person transformation stable; near/far planes correct
- [ ] HUD/UI projected to VR space without stretching; cursor alignment correct

## UObjectHook Attachments
- [ ] Weapon → Right controller; Utility → Left controller (all games)
- [ ] Attachments persist (Permanent Change) and serialize to `uobjecthook/*.json`
- [ ] All configured hooks resolve to valid paths at runtime; log any misses

## Regression & Stability
- [ ] 30 min playtest per game without crash, memory leak, or severe hitching
- [ ] Device reset and windowed/fullscreen toggles do not break rendering
- [ ] Save/Load flows unaffected by the adapter

## Bug‑Free Gate (must be 100% before ship)
- [ ] All checkboxes above complete
- [ ] All tests green across three games
- [ ] No critical/major issues open in tracker

## User Testing with various VR hardware configurations
- [ ] Test with WMR, Oculus, SteamVR/OpenXR devices
- [ ] Verify controller bindings and haptics per device
- [ ] Confirm roomscale vs seated play works; recenter function reliable

## Performance Validation across different system specifications
- [ ] Collect frame-time traces on low/mid/high PCs
- [ ] Ensure GPU/CPU frametime within comfort targets (e.g., 11.1ms/13.3ms/16.6ms as appropriate)
- [ ] Validate FSR/DLSS/XeSS interactions and no ghosting artifacts
- [ ] Record tunings in `docs/reports/` and per-game `README.md`

## Docs Sync & Traceability
- [ ] Update `docs/cross-engine/*.md` with: hook points, matrices mapping, input bindings, and known quirks.
- [ ] Append QA results to `docs/reports/*` per game with date/time, build, and headset used.
- [ ] Update `profiles/<Game>/README.md` with attachment paths, overrides, and validation outcomes.
- [ ] Ensure `profiles/<Game>/adapter.json` reflects final matrices and input mappings; commit with message referencing QA.
- [ ] Link RenderDoc captures (`captures/renderdoc/.../*.rdc`) and any probe logs from adapters in the report.

## Defect Tracking & Reporting
- [ ] Log defects with: title, severity, repro steps, expected vs actual, environment, and evidence links.
- [ ] Track fixes and retests in the same report section; mark status (open/fixed/verified).
- [ ] If any item remains open, block the Bug‑Free Gate and note blocker details and ETA.

## Final Artifacts to Update
- [ ] `docs/cross-engine/*.md` (engine and game pages)
- [ ] `docs/reports/*` with validation results per game
- [ ] `profiles/<Game>/README.md` with attachments, overrides, and checklist results
- [ ] Confirm `profiles/<Game>/adapter.json` contains final matrices/input mappings

## Documentation Requirements
- [ ] All progress must be documented continuously throughout QA process
- [ ] Update implementation notes in real-time during validation steps
- [ ] Maintain traceability between test results and configuration changes
- [ ] Record all discovered issues and their resolutions in artifact files
- [ ] Ensure comprehensive coverage of all validation checkpoints in documentation

## QA Results — Pasteback Summary (All Games)
- Date/Build/Headset: [YYYY-MM-DD] / [hash/tag] / [device]
- Summary: [1–3 lines]
- Build & Static: [pass/fail + notes per adapter]
- Runtime Hooks: [RE7: DX11 Present/ResizeBuffers; CP2077: Present/Present1, ExecuteCommandLists; MHW: Present/ResizeBuffers]
- Matrices:
  - RE7: [cb_slot=.., view_offset=.., proj_offset=.., layout, handedness, near/far]
  - CP2077: [root_param/CBV idx=.., view_offset=.., proj_offset=.., layout, handedness, near/far]
  - MHW: [cb_slot=.., view_offset=.., proj_offset=.., layout, handedness, near/far]
- Input/UI: [status/notes per game]
- Stability: [alt-tab, device reset, long run per game]
- Performance: [avg fps, frametimes per game]
- Artifacts Updated: [profiles/<Game>/adapter.json, profiles/<Game>/README.md]
- Evidence Links: [captures/renderdoc/.../*.rdc, logs, screenshots]

---
