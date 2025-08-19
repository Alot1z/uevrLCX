# RenderDoc Capture Template (What to Extract)

- Capture path: `captures/renderdoc/<game>/<timestamp>.rdc`
- Scene: main gameplay scene with weapon/UI visible
- API: D3D11|D3D12, swapchain backbuffer index

## Identify Main Draw(s)
- Draw call(s) rendering the scene to backbuffer
- VS/PS bound shaders, input layout, viewport/scissor
- Depth/stencil state, depth buffer resource

## Constant Buffers (CBs)
- For each VS/PS CB used by main draw:
  - Slot index (b0..bN)
  - Size (bytes) and variable layout
  - Offsets for: View, Projection, ViewProj matrices
  - Matrix layout heuristic: row_major/col_major, transpose requirement
  - Near/Far/FOV constants if present

## Root Signature / Descriptor Heuristics (D3D12)
- Root parameters bound to CBVs (slot, register)
- Descriptor tables for CBVs and ranges

## Matrix Validation
- Compare extracted matrices against camera transform
- Verify left/right-eye offsets are applied in stereo path

## UI Pass
- Identify UI pass draw(s); note projection behavior (screen-space vs world)
- Decide projection technique: billboard | depth | raycast

## Notes / Pitfalls
- Post-processing pass that reprojects image
- Temporal AA interactions
- HUD elements tied to view matrix

After filling this, update corresponding `docs/cross-engine/<Game>.md` and `profiles/<Game>/adapter.json`.
