# Getting Started (Complete)

Status: WIP — tracked in `docs/deepwiki/planning.md` (Phase 2)

- Audience: end users, developers, contributors
- Scope: setup, installation paths, first run, onboarding

## Acceptance Criteria
- Clear audience and scope
- Up-to-date links to repo files and scripts
- At least one end-to-end procedure
- Troubleshooting subsection present
- Cross-linked to DeepWiki index and root README

## Table of Contents
- Prerequisites and system check
- Installation paths (user/dev)
- First run (select adapter, launch game)
- Developer onboarding (build + run)
- Troubleshooting first-run
- Links

## Prerequisites
See `./requirements.md` and root `README.md`.

## Installation Paths
- End user: packaged release, profiles in `profiles/`
- Developer: build from source using `CMakeLists.txt`, scripts in root

## First Run
1) Launch target game
2) Adapter detection picks engine (see `./GAME_ENGINE_SUPPORT.md`)
3) VR runtime selected (OpenXR preferred)

## Developer Onboarding
- Build: `COMPILING.md`, `build_complete_system.bat`
- Code map: `./files-index.md`, `./functions.md`

## Troubleshooting
See `./TROUBLESHOOTING_COMPLETE.md`.

## Links
- DeepWiki index: `./README.md`
- Build system: `./BUILD_SYSTEM_COMPLETE.md`
- Profiles: `../compatibility/README.md` (if present), `../../profiles/_TEMPLATE/README.md`
