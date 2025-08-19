# Report — Cyberpunk 2077

## Status
- In progress — scaffolding complete; awaiting first D3D12 RenderDoc baseline capture.

## Integration
- Cyber Engine Tweaks (ASI) + MinHook
- Hook candidates: `IDXGISwapChain3::Present/Present1`, `ID3D12CommandQueue::ExecuteCommandLists`, optional `ResizeBuffers`.

## Capture
- Baseline: `captures/renderdoc/Cyberpunk2077/cp2077_baseline.rdc`
- Goal: Identify root parameter indices/descriptor tables for camera view/projection CBVs; record byte offsets, layout, handedness, near/far.

## Matrices
- Pending until baseline capture; update `profiles/Cyberpunk2077/adapter.json` → `render.matrix_sources` and `matrices.*` after discovery.

## QA Results
- Last run: [YYYY-MM-DD], Build: [hash/tag], Headset: [device]
- Summary: [short summary of current QA status]
- Build/Static: [results/notes]
- Runtime: [injection/hooks/camera/stereo status]
- Matrices: [root_param=.. / CBV idx=.., view_offset=.., proj_offset=.., layout=row/col, handedness=LH/RH, near/far]
- Input & Interaction: [status/notes]
- Stability: [status/notes]
- Performance: [fps/frametime/notes]
- Artifacts updated: [profiles/Cyberpunk2077/adapter.json, profiles/Cyberpunk2077/README.md]
- Evidence: [links to captures/renderdoc/Cyberpunk2077/*.rdc, logs, screenshots]

## Attachments
- Convention: Right = weapon, Left = utility.
- Persistent state JSON to be stored under `profiles/Cyberpunk2077/uobjecthook/` after component paths are identified.

## Next Steps
- Perform baseline capture in a representative scene; document CBV bindings and offsets.
- Implement matrix extraction and validate stereo FOV and scaling on `Present1` path.
- Begin UI reprojection on main backbuffer; verify post-process chain.

## Links
- Cross-engine notes: `docs/cross-engine/REDengine4.md`, `docs/cross-engine/Cyberpunk2077.md`
- Adapter profile: `profiles/Cyberpunk2077/adapter.json`

## Validation
- Pending: aiming, locomotion, UI, performance, audio, comfort.
