# adapter.json Schema (Cross-Engine Adapters)

Purpose: canonical structure for `profiles/<GameExeName>/adapter.json` capturing engine integration, matrices, input, UI, and performance policies.

## Example
```json
{
  "engine": "RE-Engine",
  "version": 1,
  "render": {
    "api": "d3d11",
    "swapchain_hook": "Present|Present1|ExecuteCommandLists",
    "matrix_sources": {
      "view": { "cb_slot": 2, "offset": 0, "rows": 4, "cols": 4, "layout": "row_major", "transpose": false },
      "projection": { "cb_slot": 2, "offset": 64, "rows": 4, "cols": 4, "layout": "row_major", "transpose": false },
      "viewproj": { "cb_slot": 2, "offset": 128, "rows": 4, "cols": 4, "layout": "row_major", "transpose": false }
    },
    "fov_policy": "dynamic|fixed",
    "stereo_mode": "sequential|multiview|instanced",
    "depth_handling": "native|override"
  },
  "camera": {
    "world_scale": 1.0,
    "hmd_offsets": { "x": 0.0, "y": 0.0, "z": 0.0 },
    "decoupled_pitch": true
  },
  "ui": {
    "projection": { "technique": "billboard|depth|raycast", "scale": 1.0 },
    "overrides": [
      { "name": "HUD_Main", "mode": "world|screen", "scale": 0.75 }
    ]
  },
  "input": {
    "aim_method": "right|left",
    "bindings": { "trigger": "Fire", "grip": "Interact" }
  },
  "attachments": [
    { "path": "Acknowledged Pawn>Components>FP_Gun", "controller": "right", "permanent": true,
      "rotation_quat": {"w": 1, "x": 0, "y": 0, "z": 0}, "location_offset": {"x": 0, "y": 0, "z": 0} }
  ],
  "performance": { "resolution_scale": 0.9, "uncap_framerate": true },
  "notes": []
}
```

## Fields
- engine: RE-Engine | REDengine4 | MT-Framework
- version: integer schema version
- render.api: d3d11 | d3d12
- render.swapchain_hook: primary hook point(s)
- render.matrix_sources: CB slot and offset mapping for view/projection matrices
- render.fov_policy: dynamic (from game) or fixed (override)
- render.stereo_mode: approach for stereo
- render.depth_handling: how depth is sourced or overridden
- camera.*: VR camera policies
- ui.*: UI projection policy and overrides
- input.*: aim method and bindings
- attachments[]: persistent controller attachments akin to UObjectHook
- performance.*: perf-related tuning
- notes[]: free-form annotations

Keep this in sync with per-game docs and RenderDoc findings.
