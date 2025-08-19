# Development Setup Guide

This guide provides comprehensive instructions for setting up a UEVR development environment, including prerequisites, tools, configuration, and troubleshooting.

## Prerequisites

### System Requirements
- **Operating System**: Windows 10/11, Ubuntu 20.04+, macOS 12+
- **CPU**: Intel i5/AMD Ryzen 5 or better (64-bit)
- **RAM**: 16GB minimum, 32GB recommended
- **Storage**: 50GB free space for development environment
- **Graphics**: DirectX 11/12 or Vulkan compatible GPU

### Required Software
- **Git**: Version 2.30+ for source control
- **CMake**: Version 3.20+ for build system
- **C++ Compiler**: Visual Studio 2019/2022, GCC 9+, or Clang 12+
- **Python**: Version 3.8+ for build scripts and tools
- **Node.js**: Version 16+ for development tools (optional)

## Development Environment Setup

### Windows Setup

#### 1. Install Visual Studio
```batch
# Download and install Visual Studio 2022 Community Edition
# Include the following workloads:
# - Desktop development with C++
# - Game development with C++
# - Python development
# - CMake tools for Visual Studio
```

#### 2. Install vcpkg
```batch
# Clone vcpkg to a permanent location
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Add to system PATH
setx PATH "%PATH%;C:\vcpkg"

# Install required packages
.\vcpkg install openvr:x64-windows
.\vcpkg install openxr:x64-windows
.\vcpkg install vulkan:x64-windows
.\vcpkg install boost:x64-windows
.\vcpkg install lua:x64-windows
```

#### 3. Install Additional Tools
```batch
# Install Python
winget install Python.Python.3.11

# Install Git
winget install Git.Git

# Install CMake (if not included with Visual Studio)
winget install Kitware.CMake
```

### Linux Setup

#### 1. Install System Dependencies
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-pip \
    libopenvr-dev \
    libopenxr-dev \
    libvulkan-dev \
    libboost-all-dev \
    liblua5.4-dev \
    pkg-config

# CentOS/RHEL/Fedora
sudo dnf install -y \
    gcc-c++ \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-pip \
    openvr-devel \
    openxr-devel \
    vulkan-devel \
    boost-devel \
    lua-devel \
    pkgconfig
```

#### 2. Install vcpkg
```bash
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg

# Bootstrap vcpkg
./bootstrap-vcpkg.sh

# Add to shell profile
echo 'export PATH="$HOME/vcpkg:$PATH"' >> ~/.bashrc
source ~/.bashrc

# Install packages
./vcpkg install openvr openxr vulkan boost lua
```

### macOS Setup

#### 1. Install Homebrew
```bash
# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Add Homebrew to PATH
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
```

#### 2. Install Dependencies
```bash
# Install required packages
brew install cmake ninja git python3

# Install C++ libraries
brew install openvr openxr vulkan-headers boost lua

# Install Xcode Command Line Tools
xcode-select --install
```

## Project Setup

### 1. Clone Repository
```bash
# Clone UEVR repository
git clone https://github.com/your-repo/uevr.git
cd uevr

# Initialize submodules
git submodule update --init --recursive
```

### 2. Create Build Directory
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DBUILD_DOCS=ON \
    -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 3. Build Project
```bash
# Build project
cmake --build . --config Debug

# Run tests
ctest --output-on-failure
```

## IDE Configuration

### Visual Studio Code

#### 1. Install Extensions
```json
// .vscode/extensions.json
{
    "recommendations": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "ms-vscode.cpptools-extension-pack",
        "ms-vscode.python",
        "ms-vscode.git",
        "ms-vscode.hexeditor",
        "ms-vscode.cmake-tools"
    ]
}
```

#### 2. Configure C++ IntelliSense
```json
// .vscode/c_cpp_properties.json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/src",
                "${workspaceFolder}/include",
                "C:/vcpkg/installed/x64-windows/include"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "windowsSdkVersion": "10.0.19041.0",
            "compilerPath": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe",
            "cStandard": "c17",
            "cppStandard": "c++20",
            "intelliSenseMode": "windows-msvc-x64"
        }
    ],
    "version": 4
}
```

#### 3. Configure CMake Tools
```json
// .vscode/settings.json
{
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Ninja",
    "cmake.debugConfig": {
        "stopAtEntry": true,
        "cwd": "${workspaceFolder}"
    }
}
```

### Visual Studio

#### 1. Open Project
```batch
# Open UEVR.sln in Visual Studio
# Or open the CMakeLists.txt file directly
```

#### 2. Configure CMake Settings
```json
// CMakeSettings.json
{
    "configurations": [
        {
            "name": "x64-Debug",
            "generator": "Ninja",
            "configurationType": "Debug",
            "inheritEnvironments": [ "msvc_x64_x64" ],
            "buildRoot": "${projectDir}\\out\\build\\${name}",
            "installRoot": "${projectDir}\\out\\install\\${name}",
            "cmakeCommandArgs": "",
            "buildCommandArgs": "",
            "ctestCommandArgs": "",
            "variables": [
                {
                    "name": "CMAKE_TOOLCHAIN_FILE",
                    "value": "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
                }
            ]
        }
    ]
}
```

### CLion

#### 1. Open Project
```bash
# Open the project directory in CLion
# CLion will automatically detect CMake project
```

#### 2. Configure Toolchains
```bash
# In CLion Settings:
# - Build, Execution, Deployment > Toolchains
# - Add new toolchain with your compiler
# - Set CMake executable path
```

## Development Tools

### 1. Code Formatting

#### Clang Format
```bash
# Install clang-format
# Windows: Included with Visual Studio
# Linux: sudo apt-get install clang-format
# macOS: brew install clang-format

# Create .clang-format file
cat > .clang-format << EOF
BasedOnStyle: Google
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 120
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
EOF

# Format code
clang-format -i src/**/*.cpp include/**/*.h
```

#### Pre-commit Hooks
```bash
# Install pre-commit
pip install pre-commit

# Create .pre-commit-config.yaml
cat > .pre-commit-config.yaml << EOF
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: trailing-whitespace
      - id: end-of-file-fixer
      - id: check-yaml
      - id: check-added-large-files
  
  - repo: https://github.com/psf/black
    rev: 23.3.0
    hooks:
      - id: black
  
  - repo: https://github.com/pre-commit/mirrors-clang-format
    rev: v16.0.6
    hooks:
      - id: clang-format
        args: [--style=file]
EOF

# Install hooks
pre-commit install
```

### 2. Static Analysis

#### Clang-Tidy
```bash
# Install clang-tidy
# Windows: Included with Visual Studio
# Linux: sudo apt-get install clang-tidy
# macOS: brew install llvm

# Run clang-tidy
clang-tidy src/**/*.cpp -checks=*,-fuchsia-*,-google-*,-zircon-*,-abseil-duration-*,-abseil-time-*,-modernize-use-trailing-return-type
```

#### Cppcheck
```bash
# Install cppcheck
# Windows: Download from http://cppcheck.sourceforge.net/
# Linux: sudo apt-get install cppcheck
# macOS: brew install cppcheck

# Run cppcheck
cppcheck --enable=all --std=c++20 src/ include/
```

### 3. Debugging Tools

#### GDB/LLDB
```bash
# Linux: GDB included with build-essential
# macOS: LLDB included with Xcode
# Windows: Use Visual Studio debugger

# Debug with GDB
gdb ./build/bin/Debug/UEVR

# Debug with LLDB
lldb ./build/bin/Debug/UEVR
```

#### Valgrind (Linux)
```bash
# Install Valgrind
sudo apt-get install valgrind

# Run memory check
valgrind --tool=memcheck --leak-check=full ./build/bin/Debug/UEVR
```

## Build Configuration

### 1. CMake Configuration

#### Debug Configuration
```cmake
# Debug build with full symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")

# Enable debug features
option(ENABLE_DEBUG_LOGGING "Enable debug logging" ON)
option(ENABLE_ASSERTIONS "Enable assertions" ON)
option(ENABLE_SANITIZERS "Enable sanitizers" ON)
```

#### Release Configuration
```cmake
# Release build with optimizations
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Disable debug features
option(ENABLE_DEBUG_LOGGING "Enable debug logging" OFF)
option(ENABLE_ASSERTIONS "Enable assertions" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)
```

### 2. Compiler Flags

#### Windows (MSVC)
```cmake
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /utf-8 /MP")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")
endif()
```

#### Linux/macOS (GCC/Clang)
```cmake
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()
```

### 3. Feature Flags
```cmake
# Core features
option(ENABLE_VULKAN "Enable Vulkan support" ON)
option(ENABLE_DX12 "Enable DirectX 12 support" ON)
option(ENABLE_OPENXR "Enable OpenXR support" ON)
option(ENABLE_OPENVR "Enable OpenVR support" ON)

# Development features
option(ENABLE_LUA "Enable Lua scripting" ON)
option(ENABLE_PYTHON "Enable Python scripting" OFF)
option(ENABLE_PROFILING "Enable profiling" OFF)
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
option(ENABLE_INTEGRATION_TESTS "Enable integration tests" ON)
```

## Testing Setup

### 1. Unit Testing
```bash
# Build with tests enabled
cmake .. -DBUILD_TESTS=ON
cmake --build . --config Debug

# Run unit tests
ctest --output-on-failure

# Run specific test suite
ctest -R "unit" --output-on-failure

# Run tests with verbose output
ctest --output-on-failure --verbose
```

### 2. Integration Testing
```bash
# Run integration tests
ctest -R "integration" --output-on-failure

# Run performance tests
ctest -R "performance" --output-on-failure

# Run compatibility tests
ctest -R "compatibility" --output-on-failure
```

### 3. Test Coverage
```bash
# Install lcov (Linux)
sudo apt-get install lcov

# Configure with coverage
cmake .. -DENABLE_COVERAGE=ON
cmake --build . --config Debug

# Run tests with coverage
ctest --output-on-failure

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## Documentation Setup

### 1. Doxygen
```bash
# Install Doxygen
# Windows: Download from http://www.doxygen.nl/
# Linux: sudo apt-get install doxygen
# macOS: brew install doxygen

# Generate documentation
doxygen Doxyfile

# Open documentation
# Open html/index.html in your browser
```

### 2. Sphinx
```bash
# Install Sphinx
pip install sphinx sphinx-rtd-theme

# Initialize Sphinx
sphinx-quickstart

# Build documentation
make html

# Open documentation
# Open _build/html/index.html in your browser
```

## Continuous Integration

### 1. GitHub Actions
```yaml
# .github/workflows/ci.yml
name: CI

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
    
    - name: Setup build environment
      run: |
        # Platform-specific setup commands
        
    - name: Configure CMake
      run: |
        cmake -B build \
          -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
          -DBUILD_TESTS=ON
    
    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Test
      run: ctest --test-dir build --output-on-failure
```

### 2. Local CI
```bash
# Create local CI script
cat > ci.sh << 'EOF'
#!/bin/bash
set -e

echo "Running local CI..."

# Clean build
rm -rf build
mkdir build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON

# Build
cmake --build . --config Release

# Test
ctest --output-on-failure

# Format check
cd ..
clang-format --dry-run --Werror src/**/*.cpp include/**/*.h

echo "Local CI completed successfully!"
EOF

chmod +x ci.sh
./ci.sh
```

## Troubleshooting

### Common Issues

#### 1. CMake Configuration Errors
```bash
# Problem: Package not found
# Solution: Check vcpkg installation and package names
vcpkg list
vcpkg install <package-name>:<triplet>

# Problem: Compiler not found
# Solution: Check compiler installation and PATH
which gcc
which clang
which cl
```

#### 2. Build Errors
```bash
# Problem: Missing dependencies
# Solution: Install missing packages
vcpkg install <missing-package>

# Problem: Compiler errors
# Solution: Check C++ standard and compiler version
gcc --version
gcc -std=c++20 -c test.cpp
```

#### 3. Test Failures
```bash
# Problem: Test environment issues
# Solution: Check test dependencies and environment
ctest --output-on-failure --verbose

# Problem: Performance test failures
# Solution: Check system resources and test thresholds
htop
free -h
```

### Getting Help

#### 1. Documentation
- **UEVR Documentation**: Check the docs/ directory
- **CMake Documentation**: https://cmake.org/documentation/
- **vcpkg Documentation**: https://github.com/microsoft/vcpkg

#### 2. Community Support
- **GitHub Issues**: Report bugs and request features
- **Discord Server**: Get real-time help from the community
- **Community Wiki**: Community-maintained knowledge base

#### 3. Development Resources
- **C++ Reference**: https://en.cppreference.com/
- **Modern C++**: https://isocpp.github.io/CppCoreGuidelines/
- **CMake Best Practices**: https://github.com/Akagi201/learning-cmake

## Best Practices

### 1. Development Workflow
1. **Branch Strategy**: Use feature branches for development
2. **Code Review**: Require code review for all changes
3. **Testing**: Write tests for new features
4. **Documentation**: Update documentation with code changes

### 2. Code Quality
1. **Style Guide**: Follow established coding standards
2. **Static Analysis**: Use tools to catch issues early
3. **Code Coverage**: Maintain high test coverage
4. **Performance**: Profile and optimize critical paths

### 3. Build Management
1. **Incremental Builds**: Use Ninja for faster builds
2. **Dependency Management**: Keep dependencies up to date
3. **Build Variants**: Support multiple build configurations
4. **CI/CD**: Automate build and test processes

## Resources

- [UEVR Architecture Overview](../architecture/system-overview.md)
- [Build System Documentation](../build-system/overview.md)
- [Testing Framework](../testing/overview.md)
- [Performance Optimization](../performance/overview.md)
- [Contributing Guidelines](../../CONTRIBUTING.md)

---

*For development setup support, check the GitHub Issues or join the community Discord.*
