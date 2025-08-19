# Architecture Deep Dive

Status: WIP — tracked in `docs/deepwiki/planning.md` (Phase 2)

- Audience: developers, maintainers
- Scope: components, lifecycles, data flow, hooks, adapters

## Acceptance Criteria
- Architecture overview with diagrams (TBD)
- Cross-links to headers in `include/uevr/` and implementations in `src/`
- Example lifecycles and error paths
- Troubleshooting/limitations note

## Table of Contents
- System overview
- Core framework
- Adapter system
- Hooking architecture
- VR integration
- Advanced topics

## System Overview
- Cross‑engine VR adapter framework with per‑engine adapters

## Core Framework
- Public API: see `include/uevr/` (e.g., `Framework.hpp`, `Plugin.hpp`)

## Adapter System
- Base interfaces in `include/uevr/adapters/common/*`
- Examples: `adapters/REDengine4/*`

## Hooking Architecture
- Graphics hooks: `src/hooks/D3D11Hook.cpp`, `D3D12Hook.cpp`

## VR Integration
- OpenXR bridge: `include/uevr/vr/OpenXRIntegration.hpp`

## Advanced Topics
- Threading, memory management, error handling (see `adapters/common/memory_management.*`)
