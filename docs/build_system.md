# 🔨 **UEVR Build System Guide**

## 🎯 **Build System Overview**

UEVR uses a modern, cross-platform build system based on **CMake** with advanced features for cross-engine VR development. This guide covers everything you need to build UEVR from source code.

---

## 🏗️ **Architecture Overview**

### **📁 Project Structure**

```
uevr/
├── CMakeLists.txt              # Main build configuration
├── cmake.toml                  # CMake presets and options
├── src/                        # Source code
│   ├── core/                   # Core UEVR systems
│   ├── vr/                     # VR framework
│   ├── adapters/               # Engine-specific adapters
│   ├── hooks/                  # Engine hooking
│   └── mods/                   # VR modifications
├── include/                    # Header files
├── dependencies/               # Third-party libraries
├── tests/                      # Test suite
├── examples/                   # Example projects
└── docs/                       # Documentation
```

### **🔧 Build Components**

#### **Core Libraries:**
- **uevr_Core** - Core UEVR functionality
- **uevr_context7_database** - AI database system
- **uevr_vr** - Universal VR framework
- **uevr_adapters_common** - Common adapter functionality

#### **Engine Adapters:**
- **uevr_cyberpunk2077_adapter** - REDengine 4 support
- **uevr_re7_adapter** - RE Engine support
- **uevr_mhw_adapter** - MT Framework support

---

## 🚀 **Quick Start Build**

### **📋 Prerequisites**

#### **Required Tools:**
- **CMake 3.20+** - Build system generator
- **C++ Compiler** - MSVC, GCC, or Clang
- **Git** - Version control
- **Python 3.8+** - Build scripts

#### **System Requirements:**
- **Windows:** Visual Studio 2019+ or Build Tools
- **Linux:** GCC 9+ or Clang 10+
- **macOS:** Xcode 12+ or Command Line Tools

### **🔨 Build Commands**

#### **Basic Build:**
```bash
# Clone repository
git clone https://github.com/uevr/uevr.git
cd uevr

# Configure build
cmake -B build

# Build project
cmake --build build --config Release

# Install (optional)
cmake --install build
```

#### **Advanced Build:**
```bash
# Configure with specific options
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DUEVR_BUILD_TESTS=ON \
    -DUEVR_BUILD_EXAMPLES=ON

# Build with multiple jobs
cmake --build build --config Release --parallel 8

# Build specific target
cmake --build build --target uevr_core
```

---

## ⚙️ **Build Configuration**

### **🔧 CMake Options**

#### **Core Build Options:**
```bash
# Build type
-DCMAKE_BUILD_TYPE=Release      # Release, Debug, RelWithDebInfo

# Compiler options
-DCMAKE_CXX_STANDARD=20         # C++ standard (17, 20, 23)
-DCMAKE_CXX_FLAGS="-O3"         # Compiler flags

# Platform options
-DCMAKE_SYSTEM_NAME=Windows     # Target platform
-DCMAKE_SYSTEM_PROCESSOR=x86_64 # Target architecture
```

#### **UEVR-Specific Options:**
```bash
# Feature toggles
-DUEVR_BUILD_CROSS_ENGINE=ON    # Build cross-engine support
-DUEVR_BUILD_CONTEXT7_DATABASE=ON # Build AI database
-DUEVR_ENABLE_AI_ANALYSIS=ON    # Enable AI analysis
-DUEVR_BUILD_TESTS=ON           # Build test suite
-DUEVR_BUILD_EXAMPLES=ON        # Build examples

# Performance options
-DUEVR_ENABLE_20X_PERFORMANCE=ON # Enable 20x performance
-DUEVR_ENABLE_DEEPWIKI=ON       # Enable DeepWiki integration
-DUEVR_ENABLE_GPU_ACCELERATION=ON # Enable GPU acceleration
```

### **📁 Build Presets**

#### **Available Presets:**
```bash
# Development build
cmake --preset=dev

# Release build
cmake --preset=release

# Debug build
cmake --preset=debug

# Minimal build
cmake --preset=minimal

# Full build
cmake --preset=full
```

#### **Custom Preset:**
```bash
# Create custom preset
cmake --preset=custom \
    -DUEVR_BUILD_CROSS_ENGINE=ON \
    -DUEVR_BUILD_TESTS=OFF \
    -DCMAKE_BUILD_TYPE=Release
```

---

## 🎯 **Platform-Specific Builds**

### **🪟 Windows Build**

#### **Visual Studio Build:**
```bash
# Generate Visual Studio solution
cmake -B build -G "Visual Studio 16 2019" -A x64

# Build with Visual Studio
cmake --build build --config Release

# Or open in Visual Studio
start build/UEVR.sln
```

#### **MSBuild Build:**
```bash
# Build with MSBuild
cmake --build build --config Release --parallel 8

# Build specific configuration
cmake --build build --config Debug
```

#### **Windows Dependencies:**
```bash
# Install vcpkg (recommended)
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# Install dependencies
./vcpkg install spdlog:x64-windows
./vcpkg install nlohmann-json:x64-windows
./vcpkg install glm:x64-windows
./vcpkg install zlib:x64-windows
./vcpkg install curl:x64-windows
./vcpkg install sol2:x64-windows

# Configure with vcpkg
cmake -B build -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
```

### **🐧 Linux Build**

#### **Ubuntu/Debian:**
```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake git
sudo apt install libvulkan-dev libgl1-mesa-dev libxrandr-dev
sudo apt install libcurl4-openssl-dev zlib1g-dev

# Build project
cmake -B build
cmake --build build --config Release --parallel $(nproc)
```

#### **CentOS/RHEL:**
```bash
# Install dependencies
sudo yum groupinstall "Development Tools"
sudo yum install cmake3 git
sudo yum install vulkan-devel mesa-libGL-devel

# Build project
cmake3 -B build
cmake3 --build build --config Release --parallel $(nproc)
```

#### **Arch Linux:**
```bash
# Install dependencies
sudo pacman -S base-devel cmake git
sudo pacman -S vulkan-devel mesa libxrandr

# Build project
cmake -B build
cmake --build build --config Release --parallel $(nproc)
```

### **🍎 macOS Build**

#### **Prerequisites:**
```bash
# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git
brew install vulkan-headers
brew install curl zlib

# Install Xcode Command Line Tools
xcode-select --install
```

#### **Build Commands:**
```bash
# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build project
cmake --build build --config Release --parallel $(sysctl -n hw.ncpu)

# Install
cmake --install build
```

---

## 🔧 **Advanced Build Configuration**

### **🏗️ Custom Build Targets**

#### **Core Library Only:**
```bash
# Build only core functionality
cmake -B build -DUEVR_BUILD_CORE_ONLY=ON
cmake --build build --target uevr_core
```

#### **Specific Adapters:**
```bash
# Build specific engine adapters
cmake -B build \
    -DUEVR_BUILD_CYBERPUNK2077_ADAPTER=ON \
    -DUEVR_BUILD_RE7_ADAPTER=ON \
    -DUEVR_BUILD_MHW_ADAPTER=OFF

# Build specific targets
cmake --build build --target uevr_cyberpunk2077_adapter
cmake --build build --target uevr_re7_adapter
```

#### **Context7 Database:**
```bash
# Build AI database system
cmake -B build -DUEVR_BUILD_CONTEXT7_DATABASE=ON
cmake --build build --target uevr_context7_database

# Build with AI features
cmake -B build \
    -DUEVR_BUILD_CONTEXT7_DATABASE=ON \
    -DUEVR_ENABLE_AI_ANALYSIS=ON \
    -DUEVR_ENABLE_20X_PERFORMANCE=ON
```

### **⚡ Performance Optimizations**

#### **Compiler Optimizations:**
```bash
# Maximum optimization
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native" \
    -DCMAKE_C_FLAGS="-O3 -march=native -mtune=native"

# Link-time optimization
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

#### **Parallel Build:**
```bash
# Use all CPU cores
cmake --build build --config Release --parallel $(nproc)

# Windows specific
cmake --build build --config Release --parallel 8

# macOS specific
cmake --build build --config Release --parallel $(sysctl -n hw.ncpu)
```

### **🔍 Debug Builds**

#### **Debug Configuration:**
```bash
# Debug build
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Debug with symbols
cmake -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS_DEBUG="-g -O0 -DDEBUG"

# Debug with sanitizers
cmake -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS_DEBUG="-g -O0 -fsanitize=address,undefined"
```

---

## 📦 **Dependency Management**

### **🔗 Third-Party Libraries**

#### **Required Dependencies:**
```bash
# Core libraries
spdlog          # Logging
MinHook         # API hooking
nlohmann/json   # JSON processing
glm             # Mathematics
zlib            # Compression
curl            # HTTP client
sol2            # Lua binding
```

#### **Optional Dependencies:**
```bash
# AI and ML
ONNX Runtime    # Neural network inference
OpenCV          # Computer vision
TensorRT        # GPU acceleration

# Graphics
Vulkan          # Modern graphics API
DirectX 12      # Windows graphics
OpenGL 4.6      # Cross-platform graphics
```

### **📥 Dependency Installation**

#### **vcpkg (Recommended):**
```bash
# Install vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat  # Windows
./bootstrap-vcpkg.sh   # Linux/macOS

# Install dependencies
./vcpkg install spdlog:x64-windows
./vcpkg install nlohmann-json:x64-windows
./vcpkg install glm:x64-windows
./vcpkg install curl:x64-windows
./vcpkg install sol2:x64-windows

# Configure CMake
cmake -B build -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
```

#### **System Package Managers:**
```bash
# Ubuntu/Debian
sudo apt install libspdlog-dev libnlohmann-json3-dev libglm-dev libcurl4-openssl-dev

# CentOS/RHEL
sudo yum install spdlog-devel nlohmann-json-devel glm-devel libcurl-devel

# macOS
brew install spdlog nlohmann-json glm curl
```

#### **Manual Installation:**
```bash
# Clone and build manually
git clone https://github.com/gabime/spdlog.git
cd spdlog
cmake -B build -DSPDLOG_BUILD_EXAMPLES=OFF
cmake --build build --config Release
cmake --install build
```

---

## 🧪 **Testing & Validation**

### **🔬 Test Suite**

#### **Build Tests:**
```bash
# Enable tests
cmake -B build -DUEVR_BUILD_TESTS=ON

# Build tests
cmake --build build --target uevr_tests

# Run tests
ctest --test-dir build --output-on-failure
```

#### **Test Categories:**
```bash
# Unit tests
ctest --test-dir build -L unit

# Integration tests
ctest --test-dir build -L integration

# Performance tests
ctest --test-dir build -L performance

# VR tests
ctest --test-dir build -L vr
```

### **✅ Build Validation**

#### **Build Verification:**
```bash
# Check build artifacts
ls -la build/bin/
ls -la build/lib/

# Verify dependencies
ldd build/bin/uevr  # Linux
otool -L build/bin/uevr  # macOS
dumpbin /dependents build/bin/uevr.exe  # Windows
```

#### **Installation Test:**
```bash
# Install to system
cmake --install build

# Test installation
uevr --version
uevr --help
```

---

## 🚀 **Continuous Integration**

### **🔄 CI/CD Pipeline**

#### **GitHub Actions:**
```yaml
# .github/workflows/build.yml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [windows-latest, ubuntu-latest, macos-latest]
        build_type: [Debug, Release]

    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Test
      run: ctest --test-dir build --output-on-failure
```

#### **Local CI:**
```bash
# Run local CI
./scripts/local-ci.sh

# Or manually
cmake -B build -DUEVR_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

---

## 🔍 **Troubleshooting**

### **🚨 Common Build Issues**

#### **CMake Errors:**
```bash
# Clear build cache
rm -rf build/
cmake -B build

# Update CMake
# Windows: Download latest from cmake.org
# Linux: sudo apt update && sudo apt install cmake
# macOS: brew upgrade cmake
```

#### **Compiler Errors:**
```bash
# Check C++ standard
cmake -B build -DCMAKE_CXX_STANDARD=20

# Check compiler version
gcc --version
clang --version
```

#### **Dependency Errors:**
```bash
# Install missing dependencies
# Use vcpkg or system package manager

# Check dependency paths
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/dependencies
```

#### **Linker Errors:**
```bash
# Check library paths
cmake -B build -DCMAKE_LIBRARY_PATH=/path/to/libs

# Check for missing symbols
nm -D /usr/lib/libcurl.so | grep curl_easy_init
```

### **📊 Build Diagnostics**

#### **Verbose Output:**
```bash
# Verbose CMake
cmake -B build --debug-output

# Verbose build
cmake --build build --verbose

# Verbose tests
ctest --test-dir build --output-on-failure --verbose
```

#### **Build Logs:**
```bash
# Save build log
cmake --build build --config Release 2>&1 | tee build.log

# Analyze build log
grep -i error build.log
grep -i warning build.log
```

---

## 📚 **Advanced Topics**

### **🔧 Custom Build Scripts**

#### **PowerShell Script (Windows):**
```powershell
# build.ps1
param(
    [string]$BuildType = "Release",
    [string]$Platform = "x64"
)

Write-Host "Building UEVR for $Platform in $BuildType mode"

# Configure
cmake -B build -G "Visual Studio 16 2019" -A $Platform -DCMAKE_BUILD_TYPE=$BuildType

# Build
cmake --build build --config $BuildType --parallel 8

# Test
if ($BuildType -eq "Release") {
    ctest --test-dir build --output-on-failure
}

Write-Host "Build completed successfully!"
```

#### **Bash Script (Linux/macOS):**
```bash
#!/bin/bash
# build.sh

BUILD_TYPE=${1:-Release}
PLATFORM=${2:-$(uname -m)}

echo "Building UEVR for $PLATFORM in $BUILD_TYPE mode"

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build
cmake --build build --config $BUILD_TYPE --parallel $(nproc)

# Test
if [ "$BUILD_TYPE" = "Release" ]; then
    ctest --test-dir build --output-on-failure
fi

echo "Build completed successfully!"
```

### **🏗️ Cross-Compilation**

#### **Windows to Linux:**
```bash
# Install cross-compiler
sudo apt install gcc-mingw-w64 g++-mingw-w64

# Configure cross-compilation
cmake -B build \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
```

#### **Linux to Windows:**
```bash
# Use MXE or similar cross-compilation toolchain
git clone https://github.com/mxe/mxe.git
cd mxe
make MXE_TARGETS=x86_64-w64-mingw32.static

# Configure
cmake -B build \
    -DCMAKE_TOOLCHAIN_FILE=../mxe/usr/x86_64-w64-mingw32.static/share/cmake/mxe-conf.cmake
```

---

## 📞 **Getting Help**

### **🆘 Support Channels**

- **Build Issues:** [GitHub Issues](https://github.com/uevr/uevr/issues)
- **Community Help:** [Discord Server](https://discord.gg/uevr)
- **Documentation:** [Build Guide](build_system.md)
- **Examples:** [Build Examples](examples/build_examples.md)

### **🔍 Troubleshooting Resources**

- **CMake Documentation:** [cmake.org](https://cmake.org/documentation/)
- **vcpkg Guide:** [github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)
- **Cross-Compilation:** [cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)

---

*UEVR Build System Guide - Build Your VR Future*
*Last updated: August 2024*
