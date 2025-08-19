# Performance Optimization

Status: WIP — tracked in `docs/deepwiki/planning.md` (Phase 2)

- Audience: developers, advanced users
- Scope: profiling, per‑engine tips, VR specifics

## Acceptance Criteria
- Profiling workflow
- Per‑engine guidance
- Validation checklist

## Profiling
- Use RenderDoc, frame analyzers; logging via `tracy` (if integrated)

## Engine Tips
- See `./GAME_ENGINE_SUPPORT.md` for engine specifics

## VR Specifics
- Maintain 90+ FPS, reduce latency (foveation/timewarp where applicable)

## Checklist
- Verify GPU/CPU frametime balance; validate with tests in `tests/`
