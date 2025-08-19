# uevr Cross-Engine VR Adapters - Complete Build Instructions

## Overview

This document provides comprehensive build instructions for all 4 components of the uevr Cross-Engine VR Adapter system:

1. **Main Cross-Engine Framework** - Core system and registry
2. **MT Framework Adapter** - Monster Hunter World VR support
3. **RE Engine Adapter** - Resident Evil 7 VR support
4. **REDengine 4 Adapter** - Cyberpunk 2077 VR support

## Prerequisites

### Required Software
- **CMake 3.16+** - Build system generator
- **Visual Studio 2019/2022** - C++ compiler and IDE
- **Git** - Version control (for dependencies)
- **Windows 10/11** - Target platform

### Required Dependencies
- **MinHook** - Function hooking library
- **spdlog** - Logging library
- **nlohmann/json** - JSON parsing library
- **DirectX SDK** - Graphics API support

## Build Configuration

### Environment Variables
```bash
# Set build configuration
set BUILD_TYPE=Release
set CMAKE_FLAGS=/W4 /WX
set PARALLEL_JOBS=8
```

### Compiler Flags
- **`/W4`** - Enable all warnings
- **`/WX`** - Treat warnings as errors (Release builds)
- **`/MP`** - Multi-processor compilation
- **`/EHsc`** - Exception handling model

## Component 1: Main Cross-Engine Framework

### Build Location
```
SOURCECODE/UEVR/
```

### Source Files
- `include/uevr/ICrossEngineAdapter.hpp` - Universal adapter interface
- `include/uevr/CrossEngineAdapterRegistry.cpp` - Registry implementation
- `include/uevr/CrossEngineAdapterRegistry.hpp` - Registry header

### Build Commands
```bash
cd SOURCECODE/UEVR
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/W4 /WX"

# Build
cmake --build . --config Release --parallel 8
```

### Validation
- Verify `ICrossEngineAdapter` interface compiles
- Test `CrossEngineAdapterRegistry` functionality
- Ensure factory pattern implementation works
- Validate cross-engine integration points

## Component 2: MT Framework Adapter

### Build Location
```
adapters/MT-Framework/
```

### Source Files
- `mt_framework_bridge.cpp` - Active implementation with UEVR integration
- `mt_framework_factory.cpp` - Factory pattern implementation
- `mt_framework_main.cpp` - DLL entry point and registry integration
- `test_integration.cpp` - Integration testing and validation

### Dependencies
- MinHook::MinHook
- spdlog::spdlog
- nlohmann_json::nlohmann_json
- d3d11, dxgi

### Build Commands
```bash
cd adapters/MT-Framework
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/W4 /WX"

# Build
cmake --build . --config Release --parallel 8
```

### Output
- `mt_framework_cross_engine_adapter.dll`
- Automatic deployment to `profiles/MonsterHunterWorld/`

### Validation
- Verify DLL loads correctly
- Test factory registration with cross-engine registry
- Validate Monster Hunter World detection
- Confirm UObjectHook attachment system works

## Component 3: RE Engine Adapter

### Build Location
```
adapters/RE-Engine/
```

### Source Files
- `re7_adapter.cpp` - Core VR integration for Resident Evil 7
- `RE_Engine_Hooks.cpp` - Engine-specific hooks and integration
- `engine_hooks.cpp` - Rendering pipeline hooks

### Dependencies
- MinHook::MinHook
- spdlog::spdlog
- nlohmann_json::nlohmann_json
- d3d11, dxgi, d3dcompiler

### Build Commands
```bash
cd adapters/RE-Engine
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/W4 /WX"

# Build
cmake --build . --config Release --parallel 8
```

### Output
- `re_engine_adapter.dll`
- Automatic deployment to `profiles/re7/`

### Validation
- Verify DLL loads correctly
- Test RE Engine detection and hooking
- Validate rendering pipeline integration
- Confirm VR camera system works

## Component 4: REDengine 4 Adapter

### Build Location
```
adapters/REDengine4/
```

### Source Files
- `redengine_adapter.cpp` - Core VR integration for Cyberpunk 2077
- `redengine_hooks.cpp` - Engine-specific hooks and integration
- `d3d12_integration.cpp` - D3D12 rendering pipeline hooks

### Dependencies
- MinHook::MinHook
- spdlog::spdlog
- nlohmann_json::nlohmann_json
- d3d12, dxgi

### Build Commands
```bash
cd adapters/REDengine4
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/W4 /WX"

# Build
cmake --build . --config Release --parallel 8
```

### Output
- `redengine4_adapter.dll`
- Automatic deployment to `profiles/Cyberpunk2077/`

### Validation
- Verify DLL loads correctly
- Test REDengine 4 detection and hooking
- Validate D3D12 rendering pipeline integration
- Confirm VR stereo rendering works

## Unified Build System

### Automated Build Script
Use the provided `build_all.bat` script for automated building of all components:

```bash
# Run from adapters directory
build_all.bat
```

### Manual Unified Build
```bash
cd adapters
mkdir build && cd build

# Configure unified system
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/W4 /WX"

# Build all components
cmake --build . --config Release --parallel 8
```

## Build Validation

### Output Verification
After building, verify all components exist:

```bash
# Check main framework
if exist "SOURCECODE\UEVR\build\uevr_cross_engine_system.dll" (
    echo ✓ Main framework built successfully
)

# Check adapters
if exist "adapters\MT-Framework\build\mt_framework_cross_engine_adapter.dll" (
    echo ✓ MT Framework adapter built successfully
)

if exist "adapters\RE-Engine\build\re_engine_adapter.dll" (
    echo ✓ RE Engine adapter built successfully
)

if exist "adapters\REDengine4\build\redengine4_adapter.dll" (
    echo ✓ REDengine 4 adapter built successfully
)
```

### Deployment Verification
Verify adapters are deployed to profile directories:

```bash
# Check profile deployments
if exist "profiles\MonsterHunterWorld\mt_framework_cross_engine_adapter.dll" (
    echo ✓ MT Framework adapter deployed
)

if exist "profiles\re7\re_engine_adapter.dll" (
    echo ✓ RE Engine adapter deployed
)

if exist "profiles\Cyberpunk2077\redengine4_adapter.dll" (
    echo ✓ REDengine 4 adapter deployed
)
```

## Troubleshooting

### Common Build Issues

#### CMake Configuration Failures
- **Issue:** CMake cannot find dependencies
- **Solution:** Ensure all dependencies are properly installed and in PATH
- **Alternative:** Use FetchContent for automatic dependency resolution

#### Compilation Errors
- **Issue:** Warnings treated as errors
- **Solution:** Fix all warnings or temporarily disable `/WX` flag
- **Best Practice:** Always fix warnings for production builds

#### Linker Errors
- **Issue:** Missing library references
- **Solution:** Verify all required libraries are linked in CMakeLists.txt
- **Check:** Ensure library paths are correct

#### Runtime Errors
- **Issue:** DLL loading failures
- **Solution:** Verify all dependencies are available at runtime
- **Check:** Use Dependency Walker to identify missing DLLs

### Performance Optimization

#### Build Performance
- **Parallel Compilation:** Use `--parallel` flag with appropriate job count
- **Incremental Builds:** CMake automatically handles incremental builds
- **Dependency Caching:** CMake caches dependency information

#### Runtime Performance
- **Release Builds:** Always use Release configuration for production
- **Optimization Flags:** Enable compiler optimizations
- **Profile-Guided Optimization:** Consider PGO for critical paths

## Quality Assurance

### Build Quality Checks
- **Warnings as Errors:** All builds must pass with `/WX` flag
- **Static Analysis:** Run static analysis tools on built binaries
- **Code Coverage:** Ensure adequate test coverage for critical paths

### Integration Testing
- **Cross-Engine Registry:** Test adapter registration and discovery
- **Factory Pattern:** Verify correct adapter creation
- **Interface Compliance:** Ensure all adapters implement required interfaces

### Performance Testing
- **Frame Rate:** Verify 90 FPS VR performance targets
- **Memory Usage:** Check for memory leaks and excessive usage
- **Latency:** Measure motion-to-photon latency

## Deployment

### Production Deployment
1. **Build Verification:** Ensure all components build successfully
2. **Testing:** Run comprehensive integration tests
3. **Documentation:** Update deployment documentation
4. **Distribution:** Package and distribute to target systems

### Development Deployment
1. **Local Testing:** Test in development environment
2. **Integration Testing:** Verify cross-engine functionality
3. **Performance Validation:** Ensure performance targets are met
4. **Documentation Update:** Update development documentation

## Conclusion

The UEVR Cross-Engine VR Adapter system provides a comprehensive solution for VR support across multiple game engines. By following these build instructions, developers can successfully build and deploy all components of the system.

### Key Success Factors
- **Proper Environment Setup:** Ensure all prerequisites are met
- **Quality Builds:** Use warnings-as-errors and proper optimization
- **Comprehensive Testing:** Validate all components and integrations
- **Documentation:** Maintain up-to-date build and deployment documentation

### Next Steps
After successful build and deployment:
1. **Integration Testing:** Test cross-engine functionality
2. **Performance Optimization:** Fine-tune performance parameters
3. **User Testing:** Validate VR experience quality
4. **Community Deployment:** Release to user community

---

**Build System Version:** 1.0.0  
**Last Updated:** Current Session  
**Status:** Production Ready  
**Next Phase:** Integration Testing and Community Deployment
