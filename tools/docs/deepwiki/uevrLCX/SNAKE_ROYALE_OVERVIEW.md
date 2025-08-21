# Snake Royale (UEVRLCX) — Overview

This page summarizes the VR-first Snake Royale mode designed to be engine-agnostic via `adapters/` and built on `src/vr/`, `src/ai/`, and optional `src/neural/` systems.

## Core Loop
- Collect orbs, grow tail, avoid collisions, survive shrinking ring.
- Abilities: dash, shield, tail-cut utility (limited charges).

## Camera Modes
- Third-person follow (default)
- Head-cam (cockpit)
- God-view (tabletop diorama)

## Comfort & UX
- Snap turn default, optional smooth turn.
- Dynamic vignette, horizon lock, camera smoothing.
- Diegetic HUD with world-space widgets.

## Networking
- Server-authoritative head; procedural tail reconstruction on clients.
- Prediction + smoothing; fairness windows for collisions.

## AI
- Utility-based bots with pathing, threat assessment, pickups.
- Optional neural heuristics via `src/ai/AIPoweredVROptimizer.cpp`.

## Rendering
- Readable stylized visuals; low overdraw materials.
- Optional neural upscaling/denoise in `src/neural/NeuralRenderingSystem.cpp`.

## Files and Profiles
- Plan: `docs/UEVRLCX/fortnite-snake-royale-plan.md`
- Comfort: `profiles/SnakeRoyale/comfort.json`
- Input map: `profiles/SnakeRoyale/input_map.json`

## Milestones
- Prototype → Net Alpha → Comfort Polish → Beta

## Next Steps
- Implement third-person camera rig and tail LOD in `src/vr/*`.
- Add unit tests under `tests/unit/` for tail math and collisions.
- Capture perf traces in `captures/renderdoc/SnakeRoyale/`.
