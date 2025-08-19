# Report — Monster Hunter: World

## Status
- In progress — scaffolding complete; awaiting first D3D11 RenderDoc baseline capture.

## Integration
- MinHook + DLL proxy (dinput8.dll or d3d11.dll)
- Hook candidates: `IDXGISwapChain::Present`, `IDXGISwapChain::ResizeBuffers` (device reset)
- Discovery helpers: optionally hook `ID3D11DeviceContext::VSSetConstantBuffers/PSSetConstantBuffers` during matrix discovery.

## Capture
- Baseline: `captures/renderdoc/MonsterHunterWorld/mhw_baseline.rdc`
- Goal: Identify D3D11 constant buffer slot(s) and byte offsets for camera view/projection; record layout, handedness, near/far.

## Matrices
- Pending until baseline capture; update `profiles/MonsterHunterWorld/adapter.json` → `render.matrix_sources` and `matrices.*` after discovery.

## QA Results
- Last run: [YYYY-MM-DD], Build: [hash/tag], Headset: [device]
- Summary: [short summary of current QA status]
- Build/Static: [results/notes]
- Runtime: [injection/hooks/camera/stereo status]
- Matrices: [cb_slot=X, view_offset=.., proj_offset=.., layout=row/col, handedness=LH/RH, near/far]
- Input & Interaction: [status/notes]
- Stability: [status/notes]
- Performance: [fps/frametime/notes]
- Artifacts updated: [profiles/MonsterHunterWorld/adapter.json, profiles/MonsterHunterWorld/README.md]
- Evidence: [links to captures/renderdoc/MonsterHunterWorld/*.rdc, logs, screenshots]

## Attachments
- Convention: Right = weapon, Left = utility.
- Persistent state JSON to be stored under `profiles/MonsterHunterWorld/uobjecthook/` after component paths are identified.

## Next Steps
- Perform baseline capture in a representative scene (Astera/main hunt); document CB slot indices and offsets.
- Implement matrix extraction; validate stereo FOV and scaling on Present path.
- Start UI reprojection on main backbuffer; verify post-process chain.

## Links
- Cross-engine notes: `docs/cross-engine/MT-Framework.md`, `docs/cross-engine/MonsterHunterWorld.md`
- Adapter profile: `profiles/MonsterHunterWorld/adapter.json`

## Validation
- Pending: aiming, locomotion, UI, performance, audio, comfort.
