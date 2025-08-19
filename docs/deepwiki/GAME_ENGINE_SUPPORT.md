# Game Engine Support

Status: WIP — tracked in `docs/deepwiki/planning.md` (Phase 2)

- Audience: users, developers
- Scope: engines supported, methods, limits, per‑engine notes

## Acceptance Criteria
- Engine list with detection summary
- Links to adapters and profiles
- Known limitations and workarounds

## Table of Contents
- Supported engines
- Detection and compatibility
- VR features per engine
- Known issues
- Adding new engines

## Supported Engines
- MT Framework — see `docs/cross-engine/MT-Framework.md`
- RE Engine — see `docs/cross-engine/RE-Engine.md`
- REDengine 4 — see `docs/cross-engine/REDengine4.md`
- Unreal/Unity — via `dependencies/submodules/UESDK/` (where applicable)

## Detection & Compatibility
- Engine signatures: `data/engine_signatures.json`

## VR Features
- Runtime: `./vr-integration.md`

## Known Issues
- See `./troubleshooting.md` and `./TROUBLESHOOTING_COMPLETE.md`

## Adding New Engines
- Start from `include/uevr/adapters/common/`
- Create adapter skeleton under `adapters/<engine>/`
