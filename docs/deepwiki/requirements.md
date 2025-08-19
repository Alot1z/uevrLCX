# System Requirements

- __OS__: Windows 10/11 64-bit
- __Compiler__: Visual Studio 2022 (MSVC, C++20/23)
- __Build__: CMake 3.26+ and Ninja/MSBuild
- __SDKs__: Windows SDK 10/11, DirectX 11/12, optional Vulkan SDK
- __Runtimes__: OpenXR (preferred), OpenVR (legacy)
- __GPU__: Recent NVIDIA/AMD drivers; VR-ready GPU
- __Tools__: RenderDoc, Git with submodule access

Repo prerequisites reflected in `cmake.toml` and `CMakeLists*.txt`:
- `nlohmann/json`, `spdlog`, `OpenXR-SDK`, `DirectXTK`, `DirectXTK12`, `asmjit`, `tracy`
- Private access for `UESDK` (Unreal fork) via GitHub + SSH

Checklist
- [ ] VS2022 installed (+ C++ workloads)
- [ ] CMake in PATH
- [ ] OpenXR runtime installed
- [ ] Vulkan SDK (if building Vulkan hooks)
- [ ] GitHub SSH configured for `EpicGames` access (for `UESDK`)
