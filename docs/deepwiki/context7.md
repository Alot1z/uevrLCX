# Context7 References

This page lists authoritative Context7-compatible library IDs for key dependencies used by the uevr cross‑engine VR project. These IDs enable deep-linking and traceable references across the Deep Wiki.

- Format: `/org/project` or `/org/project/version`
- Use: cite the ID directly in docs, or fetch docs via Context7 using the ID

## Resolved IDs (authoritative)
- OpenXR SDK: `/khronosgroup/openxr-sdk`
- OpenVR (legacy runtime): `/valvesoftware/openvr`
- nlohmann/json (JSON for Modern C++): `/nlohmann/json`
- spdlog (logging): `/gabime/spdlog`
- DirectX Graphics Samples: `/microsoft/directx-graphics-samples`
- DirectX Tool Kit (D3D12): `/microsoft/directxtk12`

## Pending resolution (to be confirmed)
- RenderDoc (frame capture)
- GoogleTest (testing)
- MinHook (x86/x64 API hooking)
- cmkr (CMake TOML generator)

If you need an additional library resolved, open an issue or request it in the documentation tasks.

## Usage Guidance
- In docs, reference the ID inline, e.g. “see `/khronosgroup/openxr-sdk` for loader details.”
- For curated links, prefer pointing to the Context7 ID and the upstream repository URL together.
- When a version matters for a frozen dependency, use the `/org/project/version` form.
