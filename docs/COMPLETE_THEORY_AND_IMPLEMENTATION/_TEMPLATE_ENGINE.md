# <Engine> Integration Template

## Overview
- Version(s) targeted
- Rendering API(s): D3D11/D3D12

## Hook Insertion Points
- Swapchain/Present hook(s)
- Command submission hooks (if needed)
- Resource creation interception

## Matrix Extraction
- Primary CB slot(s) and offsets
- Layout: row_major/col_major, transpose rules
- FOV/near/far source

## Stereo Strategy
- sequential | multiview | instanced
- Eye transforms application point

## UI Projection
- Screen-space vs world-space
- Chosen technique and rationale

## Input Integration
- Aim method mapping
- Motion controller events

## Performance
- Resolution scaling, frame pacing options
- Known bottlenecks and mitigations

## Risks / Edge Cases
- TAA reproject
- Post-process override constraints

## References
- Context7 links
- Deepwiki repos
