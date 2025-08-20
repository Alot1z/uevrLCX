# UEVRLCX Master Plan: OpenXR Delegation, Context7/DeepWiki, and Engine Adapters

This plan coordinates:
- VRSystem → OpenXRIntegration delegation (keep coordinator light; no duplication).
- Context7 and DeepWiki alignment for traceable references.
- First-pass adapters for three target games (engines): Cyberpunk 2077 (REDengine 4), Resident Evil 3 (RE Engine), Monster Hunter: World (MT Framework).
- Weekly roadmap for engine plugins (Unreal, Unity, Frostbite, Source).
- A master Cursor-Agent prompt to execute the plan.

## Source-of-truth files
- VR Coordinator: `src/vr/VRSystem.cpp`
- OpenXR Integration API: `include/uevr/vr/OpenXRIntegration.hpp` (+ `.cpp`)
- D3D11 pipeline: `src/hooks/D3D11Hook.cpp`
- Build system: root `CMakeLists.txt`, `src/hooks/CMakeLists.txt`, per-module CMake
- Docs:
  - DeepWiki VR: `docs/deepwiki/vr-integration.md`
  - Context7: `docs/context7/README.md`, `docs/deepwiki/context7-integration.md`
  - Plans index: `docs/deployment/Plans/index.md` (if used)

## Context7 references (authoritative)
- OpenXR Specification: `/khronosgroup/openxr-docs`
- OpenXR SDK (headers/loader): `/khronosgroup/openxr-sdk`
- OpenVR Runtime API: `/valvesoftware/openvr`

Use these IDs in docs and code comments for traceability.

## Phase 0 — OpenXR Delegation (immediate)
- Include `uevr/vr/OpenXRIntegration.hpp` in `VRSystem.cpp`.
- Extend state with `std::unique_ptr<uevr::OpenXRIntegration>`.
- `initializeOpenXR()` delegates:
  `initialize()` → `createSession()` → `initializeInput()` → `initializeHaptics()` → `initializeRendering()` → `beginSession()`.
- `initializeVRStereoRendering()` ensures rendering readiness; sets `stereo_rendering_active`.
- Frame loop:
  - `submitVRFrame()` → `beginFrame()`
  - `presentVRFrame()` → `submitFrame()` → `endFrame()`
- Cleanup: `openxr->shutdown()` then reset pointer.
- If `submitFrame()` signature differs in `.cpp`, add a no-arg overload or pass prepared layer list.

Status
- Completed: precise patch for `src/vr/VRSystem.cpp` and DeepWiki doc patch.
- Pending: `src/hooks/D3D11Hook.cpp` stereo swapchain resolve; CMake flags/guards; optional `OpenXRIntegration` header/impl sync.

## Phase 1 — Game-first adapters (3 targets)
Targets:
- CP2077 (REDengine 4) — `src/adapters/cyberpunk2077/`
- RE3 (RE Engine) — `src/adapters/re_engine/`
- MHW (MT Framework) — `src/adapters/mt_framework/`

Per-target checklist:
- Camera extraction: consistent head pose, FOV, near/far planes; runtime overrides via config.
- Stereo swapchains: route per-eye resolves in `D3D11Hook.cpp` to OpenXR swapchains from integration.
- Input mapping: map controller actions via OpenXR action sets; fallback mapping if runtime ≠ OpenXR.
- Haptics: basic pulse feedback wired to OpenXR haptics if available.
- UI reprojection: depth-aware if possible; otherwise HUD plane reprojection.
- Timing: align draw/resolve with `beginFrame()` / `endFrame()` window; avoid missed frames.
- Logging: `spdlog` with clear subsystem prefixes; bubble up `getLastError()` from integration.
- Config: minimal `profiles/<Game>/adapter.json` entries for toggles (stereo, FOV scale, HUD mode).

Deliverables:
- Minimal working 6DOF stereo with correct timings per game.
- Docs: short “Integration Notes” under `docs/adapters/` and DeepWiki links.

## Phase 2 — Engine plugin roadmap
- Week 1–2: Unreal Engine plugin (highest priority)
- Week 2–3: Unity Engine plugin (second priority)
- Week 3–4: Frostbite plugin (high priority)
- Week 4–5: Source plugin (medium priority)

Common plugin requirements:
- OpenXR session hookup (or fallback runtime).
- Stereo swapchains + per-eye resolve hook-in.
- Input action layer configuration and mapping.
- Haptics pass-through.
- Optional: foveation/perf controls, HUD reprojection helpers.

## Rendering hook tasks (pending)
- Implement “stereo swapchain rendering” path in `src/hooks/D3D11Hook.cpp`:
  - Acquire per-eye `ID3D11Texture2D` from OpenXR swapchains via integration.
  - Resolve render targets into swapchain images at the right time.
  - Submit layers (color + optional depth if supported).
  - Respect frame pacing; handle resizing / lost session gracefully.

## Build system tasks (pending)
- Add `UEVR_ENABLE_OPENXR` CMake option; link OpenXR loader when ON.
- Guard code paths and compile units accordingly.
- Keep OpenVR optional as fallback.

## API consistency (optional)
- Ensure `OpenXRIntegration.hpp` and `.cpp` signatures align for `submitFrame()` and `endFrame()`.
- If needed, add no-arg overloads that internally compose layers.

## DeepWiki updates
- Update `docs/deepwiki/vr-integration.md` with the delegation model and Context7 references.
- Cross-link to this plan under “Deployment/Plans”.

## Acceptance criteria
- CP2077, RE3, MHW launch with OpenXR runtime active: session begins, stereo active, frame loop stable, no critical errors.
- Logs show `beginFrame()`/`submitFrame()`/`endFrame()` lifecycle each frame without failures.
- Docs: DeepWiki page updated; plan published; Context7 references present.

---

## Cursor-Agent Master Prompt (copy this into Cursor)

You are the Cursor Agent for the UEVRLCX project. Work only within this repo. Follow these steps:

1) Read the plan and references
   - Plans:
     - `docs/deployment/Plans/UEVRLCX_Cursor_Agent_MasterPlan.md`
     - `docs/deployment/Plans/OpenXR_Delegation_Integration_Plan.md` (if present)
   - DeepWiki:
     - `docs/deepwiki/vr-integration.md`
     - `docs/deepwiki/context7-integration.md`
   - Source targets:
     - `src/vr/VRSystem.cpp`
     - `include/uevr/vr/OpenXRIntegration.hpp` (+ `.cpp`)
     - `src/hooks/D3D11Hook.cpp`
     - Root and hooks `CMakeLists.txt`

2) Apply the OpenXR delegation to VRSystem
   - Ensure inclusion of `uevr/vr/OpenXRIntegration.hpp`.
   - Extend VR state with `std::unique_ptr<uevr::OpenXRIntegration> openxr`.
   - Implement lifecycle delegation:
     - `initialize()` → `createSession()` → `initializeInput()` → `initializeHaptics()` → `initializeRendering()` → `beginSession()`.
     - Frame loop mapping: `submitVRFrame()` → `beginFrame()`, `presentVRFrame()` → `submitFrame()` → `endFrame()`.
   - Cleanup calls `openxr->shutdown()` and resets pointer.
   - If `submitFrame()` signature mismatch: add a no-arg overload or plumb the required layer list.

3) Implement stereo swapchain rendering in D3D11 hook (pending task)
   - Acquire OpenXR swapchain images; resolve per-eye render targets into them at the correct time.
   - Submit layers; end frame.
   - Handle resize/lost-session edge cases.

4) Add CMake option and linking
   - Add `UEVR_ENABLE_OPENXR` (default ON if appropriate).
   - When enabled, link OpenXR loader; guard OpenXR-dependent code.
   - Keep OpenVR optional as fallback.

5) Game-first adapters (CP2077, RE3, MHW)
   - Create or update per-engine adapter files under:
     - `src/adapters/cyberpunk2077/`
     - `src/adapters/re_engine/`
     - `src/adapters/mt_framework/`
   - For each: implement camera extraction, input mapping via OpenXR, haptics pass-through, and per-eye resolves via the hook.
   - Add minimal configs in `profiles/<Game>/adapter.json` (stereo toggle, FOV scale, HUD reprojection mode).

6) Documentation updates
   - Update `docs/deepwiki/vr-integration.md` with the delegation details (Context7 IDs: `/khronosgroup/openxr-docs`, `/khronosgroup/openxr-sdk`, `/valvesoftware/openvr`).
   - Ensure this plan is referenced from DeepWiki and Plans index.

7) Validation & logging
   - Build with OpenXR enabled; run with OpenXR runtime.
   - Verify frame lifecycle and no errors in logs.
   - Confirm docs and configs are present.

Constraints & Style:
- Do not remove existing runtimes; only delegate OpenXR specifics to the integration.
- Maintain robust `spdlog` logging; use `getLastError()` where available.
- Small, focused commits per step. Write concise, cited comments (file/function references).

Deliverables:
- Updated `VRSystem.cpp`, `D3D11Hook.cpp`, CMake, and per-game adapters (CP2077/RE3/MHW).
- Updated docs: DeepWiki VR page and this plan cross-links.
- Buildable project with OpenXR on; frame loop operational.
