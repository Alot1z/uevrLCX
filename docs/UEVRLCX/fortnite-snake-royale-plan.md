---
# Fortnite Snake Royale VR Conversion Plan (UEVRLCX)

description: End-to-end plan to implement a competitive Snake Royale experience with VR-first design using the UEVRLCX architecture (engine-agnostic via adapters)
owner: uevrLCX team
status: draft-initial
links:
  - adapters/: engine integration entrypoints
  - src/vr/: VR systems (renderer, physics, input, comfort)
  - src/ai/: gameplay/bot AI hooks
  - src/neural/: optional neural rendering/post
  - profiles/: per-title runtime profiles and defaults
  - tests/: unit/integration coverage
---

## 1) Vision & Scope
- Create a high-polish, comfort-first VR “Snake Royale” arcade battle mode.
- 10–40 players/snakes, shrinking arena, last snake standing.
- Arcade-fast rounds (3–7 min), high replayability, spectator-friendly cams.
- Engine-agnostic: Unreal/Unity via `adapters/` with minimal per-engine glue.

## 2) Core Game Loop
- Spawn → grow tail by collecting orbs → avoid self/others/walls → survive until final circle → win.
- Risk/reward pickups (speed, shield, cut-tail bomb), limited charges.
- Dynamic ring: soft push → hard wall. Visible, audio-cued.

## 3) VR Experience Design
- Camera modes (switchable at runtime):
  1) Third-person follow (default): behind-and-above snake head; height adapts to speed.
  2) Head-cam: seated/cockpit perspective from snake head with horizon lock.
  3) God-view: tabletop diorama; player scales arena with pinch/drag.
- Comfort & UX:
  - Snap turn (45°/60°), optional smooth turn.
  - Dynamic vignette at high angular velocity.
  - Horizon lock, camera inertia smoothing, acceleration clamps.
  - Diegetic HUD: ring status, tail length, ability cooldowns as world-space widgets.
- Input mapping (VR controllers):
  - Left stick: heading/strafe depending on mode.
  - Right stick: camera orbit/snap turn.
  - Triggers/grips: dash/boost, “bite” pickup, tail-cut utility.
  - A/X: camera mode toggle. B/Y: scoreboard.
  - Haptics: collisions, pickups, near-miss.

Implementation paths:
- `src/vr/EngineAgnosticRenderer.cpp`: camera rigs, world-space UI anchors.
- `src/vr/AdvancedPhysicsIntegration.cpp`: inertial cam smoothing, dash forces.
- `profiles/SnakeRoyale/`: default comfort presets per HMD.

## 4) Gameplay Systems
- Tail representation:
  - Spline or segmented capsule chain with collision proxies.
  - Self and opponent collision resolution with grace frames to avoid unfair taps.
- Pickups/Abilities:
  - Speed orb (stacking decay), shield bubble, tail-cut shock.
  - Telemetry for balancing; cooldown UIs.
- Arena & circle:
  - Ring phase script (time → radius) with visual “wave” and low-frequency rumble.
  - Wall material feedback (heatmap near boundary).

Implementation paths:
- `src/vr/FullPhysicsIntegration.cpp`, `src/vr/FullAestheticCollisionEngine.cpp`.
- `adapters/*`: engine colliders, VFX materials.

## 5) Networking & Sessions
- Modes:
  - Online: server-authoritative movement with client-side smoothing.
  - Local: hot-seat or bot lobby.
- Net sync:
  - Authoritative head position + tail procedural reconstruction.
  - Deterministic tail sim with snapshot + delta.
- Anti-cheat basics: server validates dash, pickup, and collision events.

Implementation paths:
- `adapters/RE-Engine|UEVR|Unity`: hook engine net messages or provide thin custom layer.
- Prediction/smoothing in `src/vr/` camera and movement components.

## 6) AI Bots (fill lobbies, practice)
- Behavior:
  - Hunt orbs, avoid walls/others, opportunistic cutoffs.
  - Difficulty tiers: chill, standard, aggressive.
- Technical:
  - Lightweight utility AI; optional neural heuristic for path scoring.

Implementation paths:
- `src/ai/AIPoweredVROptimizer.cpp`: hooks for heuristic scoring and perf budgets.
- `src/ai/` new files: pathing, threat assessment, pickup prioritization.

## 7) Rendering & Post
- Clear color/shape readability at VR resolution.
- Stylized VFX with low overdraw (GPU-bound arenas are a risk).
- Optional: `src/neural/NeuralRenderingSystem.cpp` for upscaling/denoise when budget allows.

## 8) Audio & Haptics
- Spatialized cues: near collisions, ring closings, ability triggers.
- Minimal voice-comms UI (push-to-talk panel). Optional transcription later.

## 9) Performance Targets
- PCVR baseline:
  - 90 Hz target (72 Hz acceptable fallback), 10–12 ms GPU, 3–5 ms CPU.
  - 20+ snakes, 100+ segments each; LOD tail segments far from cam.
- Budgets:
  - CPU: physics + net + AI < 6 ms.
  - GPU: tail materials/VFX < 6 ms, post < 2 ms.
- Tools: RenderDoc captures under `captures/renderdoc/SnakeRoyale/`.

## 10) Accessibility & Safety
- Comfort presets per profile (`profiles/SnakeRoyale/comfort.json`).
- Colorblind-safe palettes for teams and pickups.
- Seated/standing modes with recenter.

## 11) Engine Adapters
- Base glue in `adapters/` with advanced route via `uevr_advanced_adapters`:
  - Unreal: actor bindings, input injection, spline/tail components.
  - Unity: GameObject bindings, Cinemachine or custom rig, Jobified tail sim.
- Platform abstractions via `${PLATFORM_LIBS}`.

## 12) Data & Community Systems
- Telemetry events: collisions, deaths, pickups, round time, comfort toggles.
- `src/community/CommunityLearningSystem.cpp` to aggregate anonymized balance data.
- In-game community presets import/export to `docs/UEVRLCX/COMMUNITY_PRESETS.md` format.

## 13) Testing & QA
- Unit tests: tail math, collision, ring phase scripts (`tests/unit/`).
- Integration: 8/16/32-player sims, net jitter/fakelag (`tests/integration/`).
- Performance CI: fixed-path camera flythrough captures + budgets gate.

## 14) Build & CI/CD
- Targets: `uevr_core`, `uevr_vr`, `uevr_ai`, `uevr_advanced_adapters`, `uevr_community`.
- Artifacts: configs under `profiles/SnakeRoyale/`, docs to `docs/UEVRLCX/`.
- GitHub Actions: manual/push triggers only, `secrets.GITHUBTOKEN` fallback to `github.token`.

## 15) Milestones (high-level)
- M1 Prototype (wk 1–2):
  - Third-person camera, basic tail sim, pickups, local bots.
- M2 Net Alpha (wk 3–4):
  - Server-auth movement, prediction/smoothing, minimal UI.
- M3 Comfort & Polish (wk 5–6):
  - Full comfort suite, haptics, effects, accessibility.
- M4 Beta (wk 7–8):
  - Perf gates, telemetry, community presets, docs.

## 16) Risks & Mitigations
- Tail physics perf at scale → LOD + GPU instancing + simplified colliders.
- Motion sickness in head-cam → make it optional; default third-person.
- Net fairness in collision events → server-auth with grace windows.
- Overdraw from VFX → profile; prioritize shape readability.

## 17) Task Breakdown (repo-mapped)
- VR Camera/Comfort: `src/vr/*`.
- Tail Sim/Collisions: `src/vr/FullAestheticCollisionEngine.cpp`, `src/vr/AdvancedPhysicsIntegration.cpp`.
- AI: `src/ai/*`.
- Neural Optional: `src/neural/NeuralRenderingSystem.cpp`.
- Adapters: `adapters/*` + `uevr_advanced_adapters`.
- Community/Telemetry: `src/community/CommunityLearningSystem.cpp`.
- Docs: this file + DeepWiki pages under `tools/docs/deepwiki/uevrLCX/`.

## 18) Acceptance Criteria (MVP)
- Smooth, comfort-friendly third-person play at 90 Hz on baseline PCVR.
- 20+ snakes with stable collisions and readable visuals.
- Online round with prediction that feels fair; inputs responsive.
- Accessibility and presets present; tests and captures in repo.

## 19) Next Actions
- Create `profiles/SnakeRoyale/` with comfort defaults and input map.
- Implement third-person camera rig and tail LOD.
- Add unit tests for tail segment math and collision proxies.
- Draft DeepWiki page: `tools/docs/deepwiki/uevrLCX/SNAKE_ROYALE_OVERVIEW.md`.
