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

- Runtime (End Users):
  - Windows 10/11 (x64)
  - VR Runtime: OpenXR or OpenVR (SteamVR)
  - GPU/Drivers: DX11/DX12-capable GPU with up-to-date drivers
  - Storage: ≥ 1 GB free for binaries and profiles

- Development (Contributors):
  - Visual Studio 2019/2022 with C++ workload
  - CMake 3.16+ and Git
  - PowerShell 5.0+ (for helper scripts)
  - See root `README.md` → Quick Start and Build System

## Installation Paths

- End users (Releases):
  - Download the latest release archive from GitHub Releases
  - Extract to a writable directory (e.g., `C:\Games\uevrLCX`)
  - Profiles: use `profiles/` (start from `profiles/_TEMPLATE/` and per-game folders)
  - Optional: copy a game-specific profile into the game’s config location if instructed by the profile README

- Developers (Source):
  - Clone the repo and initialize submodules:
    - `git submodule update --init --recursive`
  - Build using scripts under `tools/` or root batch files:
    - `adapters/build_all.bat` (adapters)
    - `tools/build_main_cross_engine/build_main_framework.bat`
    - `tools/build_mt_framework/build_mt_framework.bat`
    - `tools/build_re_engine/build_re_engine.bat`
    - `tools/build_redengine4/build_redengine4.bat`
  - See root `COMPILING.md` and `README.md` for details

## First Run

1) Launch your target game normally
2) Start the UEVR injector/front-end and select the target process
3) Choose VR runtime:
   - OpenXR (preferred when the headset has a native OpenXR runtime)
   - OpenVR (via SteamVR) for widest compatibility
4) Inject; the framework will detect the engine via signatures in `data/engine_signatures.json` and load the appropriate adapter
5) Configure per-game settings via in-game menu and/or profile files under `profiles/<GameName>/`

Notes:
- See `docs/uevr-docs/src/usage/overview.md` for in-depth usage, menu shortcuts, CVars, and depth buffer integration tips
- For controller mappings and haptics, see `docs/uevr-docs/src/input/mapping.md` and `docs/uevr-docs/src/input/haptics.md`

## Developer Onboarding

1) Clone and prepare:
   - `git clone https://github.com/Alot1z/uevrLCX.git`
   - `git submodule update --init --recursive`

2) Build options:
   - Adapters (all): `adapters/build_all.bat`
   - Main framework: `tools/build_main_cross_engine/build_main_framework.bat`
   - Individual adapters:
     - MT Framework: `tools/build_mt_framework/build_mt_framework.bat`
     - RE Engine: `tools/build_re_engine/build_re_engine.bat`
     - REDengine 4: `tools/build_redengine4/build_redengine4.bat`
   - Advanced builds: see root `build_complete_system.bat` (supports flags like `--debug`, `--release`, `--clean`)

3) Run & debug:
   - Launch target game
   - Start injector/front-end, select process, choose runtime, inject
   - Use the in-game menu (Insert / controller combo) to tweak settings

4) References:
   - Build system: `docs/uevr-docs/src/build/*.md`
   - Architecture: `docs/uevr-docs/src/architecture/*.md`
   - Runtimes: `docs/uevr-docs/src/runtimes/*.md`
   - Adapters: `docs/uevr-docs/src/adapters/*.md`

## Troubleshooting

- Adapter not detected: verify target game is supported and check `data/engine_signatures.json`
- VR runtime not found: ensure OpenXR (active runtime) or SteamVR is installed and running
- Injection blocked: some anti-cheat systems prevent injection; do not use in online/multiplayer modes unless permitted
- Rendering issues: try switching runtime (OpenXR/OpenVR) or rendering method; see `docs/uevr-docs/src/rendering/*`
- Profiles: start from `profiles/_TEMPLATE/` and copy to a per-game folder; confirm `profiles/<GameName>/adapter.json` exists when required
- Logs/captures: see `captures/` and RenderDoc guides

More: `./TROUBLESHOOTING_COMPLETE.md` and root `README.md` → Troubleshooting

## Links
- DeepWiki index: `./README.md`
- Build system: `./BUILD_SYSTEM_COMPLETE.md`
- Profiles: `../compatibility/README.md` (if present), `../../profiles/_TEMPLATE/README.md`
