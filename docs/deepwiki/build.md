# Build System

- CMake presets via `CMakeLists*.txt` and `cmake.toml`
- Visual Studio 2022 generator recommended
- Typical flow:
  1. `cmake -B build -S . -DCMAKE_BUILD_TYPE=Release`
  2. `cmake --build build --config Release`

Advanced
- See `docs/build_system/ultra_advanced_build_system_guide.md`
- Helper scripts in repo root: `build_restructured.ps1`, `build_complete_system.bat`
- Dependencies fetched via `FetchContent` per `cmake.toml`
