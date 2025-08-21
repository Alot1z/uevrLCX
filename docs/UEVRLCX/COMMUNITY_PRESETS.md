# Community Presets and Guides

This page centralizes community-sourced methods (Reddit, forums) for maximum compatibility.

## Sources (from `extended-resources/List.txt`)
- Reddit thread: https://www.reddit.com/r/oculus/comments/am9tyv/best_solution_to_turn_any_game_into_vr_game/
- 3D Vision SBS tweaks and HelixMod links
- Depth3D and ReShade presets

## Using Presets
1. Export community settings to a ReShade preset and/or VRGIN config.
2. Convert into `profiles/<GameName>/config.json` fields (camera, depthHints, inputBindings, postfx).
3. Select rendererMode:
   - `geometry` if adapter available.
   - `depth` if fallback needed.

## Notes
- Depth-based approaches are visual only; pair with head tracking and controller mapping.
- Prefer Tier A (geometry + physics) when feasible.
