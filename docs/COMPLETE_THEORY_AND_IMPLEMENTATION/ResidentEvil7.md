# Resident Evil 7 (RE Engine) — Game Notes

Status: Hooks identified; baseline RenderDoc capture path set. Pending first capture execution and matrix mapping.

## Integration
- Approach: REFramework plugin via `dinput8.dll` (preferred)
- Fallback: Native D3D11 swapchain hook (MinHook)
  - Hook points: `IDXGISwapChain::Present`, `IDXGISwapChain::ResizeBuffers`
  - Prefer REFramework built-in DirectX hook APIs; fall back only if needed

## Hooks
- Primary: REFramework plugin via `dinput8.dll`; use built-in DirectX/render callbacks exposed by the plugin API
- Fallback: Native D3D11 swapchain hook (MinHook)
  - `IDXGISwapChain::Present`, `IDXGISwapChain::ResizeBuffers`
- Notes: Prefer REFramework built-ins before adding external hooks
- Citations: Deepwiki — REFramework 3.1 DirectX Hooking; 3.2 Function Hooking System; 5.1 VR Integration

## Matrices
- Baseline capture: `captures/renderdoc/RE7/re7_baseline.rdc`
- TODO: After capture, record CB slot(s), offsets (view/proj), layout (row/col major, handedness, near/far)

## Attachments
- Right: weapon
- Left: utility
- Always: After Adjust/Permanent Change, Save State (persist). Document paths in `profiles/re7/README.md`.

## Validation
- implement.md Sections 1–2 first; document results in `profiles/re7/README.md`

## QA Results
- Last run: [YYYY-MM-DD], Build: [hash/tag], Headset: [device]
- Summary: [short summary of current QA status]
- Matrices: [cb_slot=X, view_offset=.., proj_offset=.., layout=row/col, handedness=LH/RH, near/far]
- Input & Interaction: [status/notes]
- Stability: [status/notes]
- Performance: [fps/frametime/notes]
- Artifacts updated: [profiles/re7/adapter.json, profiles/re7/README.md]
- Links: report `docs/reports/RE7.md`, prompt `promts/promt qa re7.md`

## Artifacts
- Adapter: `profiles/re7/adapter.json`
- Captures: `captures/renderdoc/RE7/re7_baseline.rdc`
- Profile: `profiles/re7/`

## Citations
- Deepwiki: praydog/REFramework — 3.1 DirectX Hooking; 3.2 Function Hooking System; 5.1 VR Integration
- Context7: RenderDoc user guide (constant buffer inspection); MinHook API reference; OpenXR runtime overview (to be added post-capture)
