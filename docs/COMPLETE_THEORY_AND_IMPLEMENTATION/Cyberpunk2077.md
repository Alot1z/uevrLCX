# Cyberpunk 2077 (REDengine 4) — Game Notes

Status: In progress — baseline capture planned at `captures/renderdoc/Cyberpunk2077/cp2077_baseline.rdc`.

## Integration
- Approach: Cyber Engine Tweaks (ASI) + MinHook
- Hook candidates:
  - IDXGISwapChain3::Present / Present1
  - ID3D12CommandQueue::ExecuteCommandLists
  - Optional: DXGI::ResizeBuffers
- Baseline capture: `captures/renderdoc/cp2077_baseline.rdc`
- Notes: Use CET ASI bootstrap; detour selected D3D12/IDXGI functions via MinHook for telemetry and matrix extraction

## Matrices
- TODO: Root parameter/CBV indices and byte offsets for view/proj
- After capture, record D3D12 root parameter indices / descriptor table ranges and byte offsets for view/proj; note layout (row/col major), handedness, near/far.
- Update `profiles/Cyberpunk2077/adapter.json` → `render.matrix_sources` and `matrices.*` accordingly.

## Attachments
- Right = weapon, Left = utility
- Persistent state JSON under `profiles/Cyberpunk2077/uobjecthook/` once component paths are identified in-game.

## Validation
- implement.md checks and performance notes

## QA Results
- Last run: [YYYY-MM-DD], Build: [hash/tag], Headset: [device]
- Summary: [short summary of current QA status]
- Matrices: [root_param=.. / CBV idx=.., view_offset=.., proj_offset=.., layout=row/col, handedness=LH/RH, near/far]
- Input & Interaction: [status/notes]
- Stability: [status/notes]
- Performance: [fps/frametime/notes]
- Artifacts updated: [profiles/Cyberpunk2077/adapter.json, profiles/Cyberpunk2077/README.md]
- Links: report `docs/reports/Cyberpunk2077.md`, prompt `promts/promt qa cp.md`

## Artifacts
- Adapter: `profiles/Cyberpunk2077/adapter.json`
- Captures: `captures/renderdoc/Cyberpunk2077/cp2077_baseline.rdc`

## Bootstrap and Load Order
- Loader: Cyber Engine Tweaks (ASI) — ensure ASI is placed so CET loads it before first frame.
- Timing: Install MinHook detours before the first `Present`/`ExecuteCommandLists` to capture device/swapchain early.
- Resets: Rebuild cached state on `ResizeBuffers` or device removal.

## Render Targets
- Prefer the LDR backbuffer for HUD reprojection; verify HDR/TAA resolve stages if present.
- Validate stereo scaling and eye buffer mapping on the `Present1` path.

## Citations
- MinHook API reference (Context7): https://github.com/TsudaKageyu/minhook
- Microsoft Docs — DXGI Present/ResizeBuffers (Context7): https://learn.microsoft.com/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-present
- Microsoft Docs — D3D12 ExecuteCommandLists (Context7): https://learn.microsoft.com/windows/win32/api/d3d12/nf-d3d12-id3d12commandqueue-executecommandlists
- RenderDoc User Guide (Context7): https://renderdoc.org/docs
- Cyber Engine Tweaks (Deepwiki): https://github.com/yamashi/CyberEngineTweaks
- Context7: /baldurk/renderdoc — constant buffer inspection workflow and D3D12 pipeline overview
- Context7: /microsoft/DirectX-Graphics-Samples — DXGI/D3D12 usage patterns (Present, ExecuteCommandLists)
- Deepwiki: CET repo indexing pending; add CET ASI loading docs once indexed
