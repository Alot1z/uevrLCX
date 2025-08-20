# <Game Title> (Executable: <GameExeName>.exe)

## Summary
- Engine: <RE-Engine|REDengine4|MT-Framework>
- Rendering API: D3D11/D3D12

## Evidence
- RenderDoc: `captures/renderdoc/<game>/<timestamp>.rdc`
- Notes/screenshots

## Matrices
- View: cb_slot=<n>, offset=<bytes>, layout=row_major/col_major, transpose=<bool>
- Projection: cb_slot=<n>, offset=<bytes>, layout=row_major/col_major, transpose=<bool>
- ViewProj (if used): cb_slot=<n>, offset=<bytes>

## UI
- Projection technique: billboard|depth|raycast
- Overrides: list affected widgets/elements

## Attachments
- Weapon → right, path: <...>
- Utility/UI → left, path: <...>
- Permanent Change: true/false; offsets/quats

## Adapter
- Profile: `profiles/<GameExeName>/adapter.json`
- Camera: world_scale, decoupled_pitch, offsets
- Input: aim_method, key bindings

## Validation
- Aiming/Locomotion/UI/Performance results

## Known Quirks
- TAA, post-processing, interaction quirks

## Last Updated
- Date, uevr build
