# uevr Cross-Engine VR Adapters - Build Guide

## Prerequisites

### Required Software
- **Windows 10/11** (64-bit)
- **Visual Studio 2019** or later (Community Edition is fine)
- **CMake 3.16** or later
- **Git** for cloning dependencies

### Required SDKs
- **Windows 10 SDK** (latest version)
- **DirectX 11/12 SDK** (included with Windows 10 SDK)
- **C++ CMake tools for Visual Studio** (install via Visual Studio Installer)

### System Requirements
- **RAM**: 8GB minimum, 16GB recommended
- **Storage**: 2GB free space for build artifacts
- **CPU**: Multi-core processor (Intel i5/AMD Ryzen 5 or better)

## Environment Setup

### 1. Install Visual Studio
1. Download Visual Studio Community from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/)
2. During installation, ensure these workloads are selected:
   - **Desktop development with C++**
   - **Game development with C++**
   - **CMake tools for Visual Studio**

### 2. Install CMake
1. Download from [cmake.org](https://cmake.org/download/)
2. Add CMake to system PATH during installation
3. Verify installation: `cmake --version`

### 3. Verify DirectX SDK
The DirectX SDK is included with Windows 10 SDK. Verify installation:
```cmd
dir "C:\Program Files (x86)\Windows Kits\10\Include\*\um\d3d11.h"
dir "C:\Program Files (x86)\Windows Kits\10\Include\*\um\d3d12.h"
```

## Project Setup

### 1. Clone Repository
```cmd
git clone https://github.com/your-username/uevr.git
cd uevr
```

### 2. Navigate to Adapters
```cmd
cd src/adapters
```

### 3. Create Build Directory
```cmd
mkdir build
cd build
```

## Build Configuration

### 1. Configure with CMake
```cmd
# Basic configuration
cmake .. -DCMAKE_BUILD_TYPE=Release

# With specific options
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON

# For Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### 2. Available CMake Options

| Option | Description | Default | Notes |
|--------|-------------|---------|-------|
| `CMAKE_BUILD_TYPE` | Build type | Release | Release, Debug, RelWithDebInfo |
| `BUILD_RE_ENGINE_ADAPTER` | Build RE Engine adapter | ON | Set to OFF to skip |
| `BUILD_REDENGINE4_ADAPTER` | Build REDengine 4 adapter | ON | Set to OFF to skip |
| `BUILD_MT_FRAMEWORK_ADAPTER` | Build MT Framework adapter | ON | Set to OFF to skip |
| `BUILD_TESTS` | Build test suite | OFF | Requires test dependencies |
| `BUILD_EXAMPLES` | Build examples | OFF | Requires example dependencies |

### 3. Advanced Configuration
```cmd
# Specify Visual Studio version
cmake .. -G "Visual Studio 16 2019" -A x64

# Specify compiler flags
cmake .. -DCMAKE_CXX_FLAGS="/W4 /WX /EHsc"

# Specify install prefix
cmake .. -DCMAKE_INSTALL_PREFIX="C:\uevr_Adapters"
```

## Building

### 1. Build All Adapters
```cmd
# Build with default settings
cmake --build . --config Release

# Build with parallel compilation
cmake --build . --config Release --parallel 8

# Build specific target
cmake --build . --config Release --target uevr_re_engine_adapter
```

### 2. Build Individual Components
```cmd
# Build common library only
cmake --build . --config Release --target uevr_common_adapters

# Build specific adapter
cmake --build . --config Release --target uevr_redengine4_adapter

# Build test suite
cmake --build . --config Release --target CrossEngineAdapterTests
```

### 3. Build Verification
After successful build, verify outputs:
```cmd
# Check generated files
dir *.dll
dir *.lib
dir tests\*.exe

# Expected outputs:
# - uevr_re_engine_adapter.dll
# - uevr_redengine4_adapter.dll  
# - uevr_mt_framework_adapter.dll
# - uevr_common_adapters.lib
# - tests/CrossEngineAdapterTests.exe (if tests enabled)
```

## Troubleshooting

### Common Build Errors

#### 1. CMake Configuration Errors
```
CMake Error: Could not find Visual Studio
```
**Solution**: Install Visual Studio with C++ workload, or specify generator:
```cmd
cmake .. -G "Visual Studio 16 2019" -A x64
```

#### 2. Compiler Errors
```
error C2039: 'vector': is not a member of 'std'
```
**Solution**: Ensure C++17 standard is enabled:
```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

#### 3. DirectX Header Errors
```
fatal error C1083: Cannot open include file: 'd3d11.h'
```
**Solution**: Verify Windows 10 SDK installation and include paths:
```cmd
dir "C:\Program Files (x86)\Windows Kits\10\Include\*\um\d3d11.h"
```

#### 4. Linker Errors
```
error LNK2019: unresolved external symbol
```
**Solution**: Check library linking in CMakeLists.txt and ensure all dependencies are found.

### Build Environment Issues

#### 1. PATH Issues
```cmd
# Verify CMake in PATH
where cmake

# Verify Visual Studio tools in PATH
where cl
where link
```

#### 2. Permission Issues
```cmd
# Run as Administrator if needed
# Or change build directory to user-writable location
mkdir %USERPROFILE%\uevr_Build
cd %USERPROFILE%\uevr_Build
```

#### 3. Antivirus Interference
- Temporarily disable real-time protection during build
- Add build directory to antivirus exclusions
- Use Windows Defender exclusions if needed

## Testing

### 1. Run Test Suite
```cmd
# Navigate to test executable
cd tests
CrossEngineAdapterTests.exe

# Or run from build directory
.\tests\CrossEngineAdapterTests.exe
```

### 2. Test Individual Components
```cmd
# Test specific adapter functionality
# (Tests are integrated into main test suite)
```

### 3. Performance Testing
```cmd
# Run performance benchmarks
# (Built into test suite)
```

## Installation

### 1. Install to System
```cmd
# Install all components
cmake --build . --config Release --target install

# Install specific components
cmake --build . --config Release --target install --target uevr_re_engine_adapter
```

### 2. Custom Installation
```cmd
# Specify custom install location
cmake .. -DCMAKE_INSTALL_PREFIX="C:\Custom\uevr_Adapters"
cmake --build . --config Release --target install
```

### 3. Portable Installation
```cmd
# Copy DLLs to target directory
copy *.dll C:\Target\Game\Directory\
```

## Development Workflow

### 1. Debug Build
```cmd
# Configure for debug
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build debug version
cmake --build . --config Debug

# Run with debugger
devenv CrossEngineAdapterTests.sln
```

### 2. Continuous Integration
```cmd
# Clean build
cmake --build . --config Release --target clean

# Rebuild everything
cmake --build . --config Release
```

### 3. Code Analysis
```cmd
# Enable static analysis
cmake .. -DCMAKE_CXX_CLANG_TIDY=clang-tidy

# Build with analysis
cmake --build . --config Release
```

## Performance Optimization

### 1. Build Optimization
```cmd
# Enable optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/O2 /GL"

# Enable link-time optimization
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/O2 /GL" -DCMAKE_EXE_LINKER_FLAGS="/LTCG"
```

### 2. Runtime Optimization
- Enable adaptive quality in adapter configuration
- Use performance monitoring tools
- Profile with RenderDoc or similar tools

## Deployment

### 1. Release Build
```cmd
# Create release build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/O2 /GL"
cmake --build . --config Release

# Verify release artifacts
dir *.dll
dir *.lib
```

### 2. Package for Distribution
```cmd
# Create distribution package
cmake --build . --config Release --target package

# Or manually package
mkdir uevr_Adapters_Release
copy *.dll uevr_Adapters_Release\
copy *.lib uevr_Adapters_Release\
copy README.md uevr_Adapters_Release\
```

### 3. Version Management
```cmd
# Tag release
git tag v1.0.0
git push origin v1.0.0

# Update version in CMakeLists.txt
set(PROJECT_VERSION "1.0.0")
```

## Support and Resources

### 1. Documentation
- [CMake Documentation](https://cmake.org/documentation/)
- [Visual Studio Documentation](https://docs.microsoft.com/visualstudio/)
- [DirectX Documentation](https://docs.microsoft.com/directx/)

### 2. Community
- GitHub Issues for bug reports
- GitHub Discussions for questions
- Discord server for real-time help

### 3. Tools
- **RenderDoc**: Graphics debugging and profiling
- **Visual Studio Profiler**: Performance analysis
- **CMake GUI**: Alternative to command-line configuration

---

**Note**: This build guide assumes Windows development environment. For other platforms, additional configuration may be required.
