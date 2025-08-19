# ========================================
# uevr Project Structure Documentation
# ========================================
# 
# This document describes the new, improved project structure for uevr.
# The restructuring aims to create a professional, scalable, and maintainable
# codebase that follows modern C++ development best practices.
# 
# Table of Contents:
# 1. Overview and Goals
# 2. New Directory Structure
# 3. Key Improvements
# 4. Migration Guide
# 5. Build Configuration
# 6. Development Workflow
# 7. Adding New Components
# 8. Testing Strategy
# 9. Troubleshooting
# 10. Future Roadmap

## 1. Overview and Goals

### Purpose
The uevr project restructuring transforms a monolithic codebase into a modular, professional-grade framework that:

- **Improves Code Organization**: Clear separation of concerns and logical grouping
- **Enhances Maintainability**: Easier to understand, modify, and extend
- **Enables Scalability**: Simple to add new adapters, engines, and features
- **Facilitates Testing**: Comprehensive test coverage with clear organization
- **Streamlines Development**: Better developer experience and workflow
- **Ensures Quality**: Professional build system with proper dependency management

### Design Principles
- **Modularity**: Each component is self-contained with clear interfaces
- **Dependency Management**: Explicit dependencies with proper isolation
- **Consistency**: Uniform patterns across all components
- **Extensibility**: Easy to add new features without modifying existing code
- **Maintainability**: Clear structure that new developers can understand quickly

## 2. New Directory Structure

```
uevr/
├── 📁 src/                          # Main source code
│   ├── 📁 core/                     # Core uevr functionality
│   │   ├── 📁 utility/              # Utility functions and helpers
│   │   │   ├── Logger.cpp           # Logging system
│   │   │   ├── ConfigManager.cpp    # Configuration management
│   │   │   └── ExceptionHandler.cpp # Exception handling
│   │   ├── 📁 sdk/                  # SDK components
│   │   │   ├── 📁 threading/        # Threading utilities
│   │   │   ├── 📁 math/             # Mathematical utilities
│   │   │   └── 📁 memory/           # Memory management
│   │   ├── Framework.cpp            # Main framework implementation
│   │   ├── Framework.hpp            # Main framework interface
│   │   └── Plugin.cpp               # Plugin system
│   │
│   ├── 📁 adapters/                 # Engine-specific adapters
│   │   ├── 📁 common/               # Shared adapter code
│   │   │   ├── CrossEngineAdapter.hpp    # Base adapter interface
│   │   │   ├── AdapterFactory.hpp        # Factory pattern
│   │   │   └── GameProfile.hpp           # Game configuration
│   │   ├── 📁 mt-framework/         # MT Framework adapters
│   │   │   ├── MTFrameworkAdapter.cpp    # MT Framework implementation
│   │   │   ├── MonsterHunterWorld.cpp    # MHW specific logic
│   │   │   └── CMakeLists.txt            # Build configuration
│   │   ├── 📁 re-engine/            # RE Engine adapters
│   │   │   ├── REEngineAdapter.cpp       # RE Engine implementation
│   │   │   ├── ResidentEvil7.cpp        # RE7 specific logic
│   │   │   └── CMakeLists.txt            # Build configuration
│   │   └── 📁 redengine4/           # REDengine 4 adapters
│   │       ├── REDengine4Adapter.cpp     # REDengine 4 implementation
│   │       ├── Cyberpunk2077.cpp        # CP2077 specific logic
│   │       └── CMakeLists.txt            # Build configuration
│   │
│   ├── 📁 hooks/                    # Engine hooking system
│   │   ├── 📁 d3d11/                # DirectX 11 hooks
│   │   ├── 📁 d3d12/                # DirectX 12 hooks
│   │   ├── 📁 opengl/                # OpenGL hooks
│   │   ├── 📁 vulkan/                # Vulkan hooks
│   │   ├── HookManager.cpp           # Central hook management
│   │   └── CMakeLists.txt            # Build configuration
│   │
│   ├── 📁 mods/                     # VR modification system
│   │   ├── 📁 vr/                    # VR-specific modifications
│   │   │   ├── 📁 d3d11/             # D3D11 VR mods
│   │   │   ├── 📁 d3d12/             # D3D12 VR mods
│   │   │   └── 📁 runtimes/           # VR runtime support
│   │   ├── 📁 pluginloader/          # Plugin loading system
│   │   ├── 📁 uobjecthook/           # Unreal Engine object hooks
│   │   ├── VRModManager.cpp          # VR mod management
│   │   └── CMakeLists.txt            # Build configuration
│   │
│   ├── 📁 utils/                     # Additional utilities
│   │   ├── 📁 rendering/             # Rendering utilities
│   │   ├── 📁 scripting/             # Scripting support
│   │   └── CMakeLists.txt            # Build configuration
│   │
│   ├── main.cpp                      # Application entry point
│   └── Plugin.cpp                    # Main plugin implementation
│
├── 📁 include/                       # Public API headers
│   └── 📁 uevr/                      # uevr API namespace
│       ├── Framework.hpp              # Main framework interface
│       ├── Plugin.hpp                 # Plugin interface
│       ├── 📁 adapters/               # Adapter interfaces
│       │   └── 📁 common/             # Common adapter interfaces
│       └── 📁 hooks/                  # Hook interfaces
│
├── 📁 tests/                         # Comprehensive test suite
│   ├── 📁 unit/                      # Unit tests
│   │   ├── core_tests.cpp            # Core functionality tests
│   │   ├── adapter_tests.cpp         # Adapter tests
│   │   └── hook_tests.cpp            # Hook system tests
│   ├── 📁 integration/               # Integration tests
│   │   ├── adapter_integration.cpp   # Adapter integration tests
│   │   └── system_integration.cpp    # System integration tests
│   ├── 📁 fixtures/                  # Test data and fixtures
│   ├── 📁 performance/               # Performance tests
│   └── CMakeLists.txt                # Test build configuration
│
├── 📁 examples/                      # Example implementations
│   ├── 📁 basic_plugin/              # Basic plugin example
│   ├── 📁 custom_adapter/            # Custom adapter example
│   ├── 📁 lua_scripting/             # Lua scripting example
│   ├── 📁 vr_mod/                    # VR modification example
│   └── CMakeLists.txt                # Examples build configuration
│
├── 📁 docs/                          # Comprehensive documentation
│   ├── 📁 api/                       # API documentation
│   ├── 📁 guides/                    # User and developer guides
│   ├── 📁 examples/                  # Example documentation
│   └── 📁 architecture/              # System architecture docs
│
├── 📁 tools/                         # Build and utility tools
│   ├── build_restructured.bat        # Windows build script
│   ├── build_restructured.ps1        # PowerShell build script
│   ├── migrate_to_new_structure.ps1  # Migration script
│   └── update_include_paths.ps1      # Include path updater
│
├── 📁 profiles/                      # Game-specific profiles
│   ├── 📁 MonsterHunterWorld/        # MHW configuration
│   ├── 📁 ResidentEvil7/             # RE7 configuration
│   ├── 📁 Cyberpunk2077/             # CP2077 configuration
│   └── 📁 _TEMPLATE/                 # Template for new games
│
├── 📁 cmake/                         # CMake configuration files
│   ├── uevrConfig.cmake.in           # Package configuration template
│   └── FindDependencies.cmake        # Dependency finders
│
├── 📁 scripts/                       # Build and deployment scripts
├── 📁 .github/                       # GitHub workflows and templates
├── CMakeLists.txt                    # Main build configuration
├── README.md                         # Project overview
└── build/                            # Build outputs (gitignored)
```

## 3. Key Improvements

### 3.1 Modular Build System
- **Component Isolation**: Each component has its own CMakeLists.txt
- **Dependency Management**: Clear dependency definitions with proper linking
- **Build Options**: Easy to enable/disable specific components
- **Cross-Platform**: Consistent build experience across Windows, Linux, and macOS

### 3.2 Clear Separation of Concerns
- **Core Framework**: Independent of specific game engines
- **Adapter Layer**: Clean interface between uevr and game engines
- **Hook System**: Isolated engine hooking functionality
- **Mod System**: Separate VR modification logic

### 3.3 Better Include Paths
- **Consistent Structure**: Uniform include patterns across all components
- **Public API**: Clear distinction between public and private headers
- **No Relative Paths**: Eliminates include path issues
- **Namespace Organization**: Logical grouping of related functionality

### 3.4 Improved Testing
- **Comprehensive Coverage**: Unit, integration, and performance tests
- **Test Organization**: Clear separation by test type and component
- **Test Fixtures**: Reusable test data and setup
- **Continuous Integration**: Automated testing in CI/CD pipeline

### 3.5 Professional Development Workflow
- **Modern CMake**: Uses latest CMake features and best practices
- **Dependency Management**: Proper handling of external libraries
- **Code Quality**: Integration with static analysis tools
- **Documentation**: Comprehensive and up-to-date documentation

## 4. Migration Guide

### 4.1 Prerequisites
- CMake 3.16 or higher
- Visual Studio 2019/2022 (Windows) or GCC/Clang (Linux/macOS)
- PowerShell 5.1+ (for migration scripts)

### 4.2 Step-by-Step Migration

#### Step 1: Backup Current Project
```powershell
# Create backup before migration
Copy-Item -Path "." -Destination "../uevr_backup_$(Get-Date -Format 'yyyyMMdd_HHmmss')" -Recurse
```

#### Step 2: Create New Structure
```powershell
# Run the migration script
.\migrate_to_new_structure.ps1 -Backup
```

#### Step 3: Move Source Files
The migration script automatically moves files to their new locations:

- **Core Files**: `SOURCECODE/uevr/src/` → `src/core/`
- **Adapter Files**: `adapters/` → `src/adapters/`
- **Hook Files**: `SOURCECODE/uevr/src/hooks/` → `src/hooks/`
- **Mod Files**: `SOURCECODE/uevr/src/mods/` → `src/mods/`

#### Step 4: Update Include Paths
```powershell
# Automatically update all include paths
.\update_include_paths.ps1 -Backup -Validate
```

#### Step 5: Test Build
```powershell
# Test the new structure
.\build_restructured.ps1
```

#### Step 6: Verify Functionality
```powershell
# Run tests to ensure everything works
cd build
cmake --build . --target test
```

### 4.3 Include Path Examples

**Before (Old Structure):**
```cpp
#include "../../include/uevr/CrossEngineAdapterRegistry.hpp"
#include "../common/BaseCrossEngineAdapter.hpp"
#include "../../SOURCECODE/uevr/src/Framework.hpp"
```

**After (New Structure):**
```cpp
#include "uevr/CrossEngineAdapterRegistry.hpp"
#include "uevr/adapters/common/CrossEngineAdapter.hpp"
#include "uevr/Framework.hpp"
```

## 5. Build Configuration

### 5.1 Main Build Options
```cmake
# Core options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build example implementations" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Feature options
option(uevr_ENABLE_DEBUG "Enable debug features" OFF)
option(uevr_ENABLE_PROFILING "Enable performance profiling" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
```

### 5.2 Adapter-Specific Options
```cmake
# Adapter options
option(BUILD_MT_FRAMEWORK_ADAPTER "MT Framework support" ON)
option(BUILD_RE_ENGINE_ADAPTER "RE Engine support" ON)
option(BUILD_REDENGINE4_ADAPTER "REDengine 4 support" ON)
```

### 5.3 Build Commands
```bash
# Windows (PowerShell)
.\build_restructured.ps1

# Windows (Command Prompt)
.\build_restructured.bat

# Linux/macOS
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 6. Development Workflow

### 6.1 Adding New Adapters
1. **Create Directory Structure**:
   ```bash
   mkdir -p src/adapters/new-engine
   mkdir -p include/uevr/adapters/new-engine
   ```

2. **Implement Adapter Interface**:
   ```cpp
   // src/adapters/new-engine/NewEngineAdapter.hpp
   #include "uevr/adapters/common/CrossEngineAdapter.hpp"
   
   namespace uevr::Adapters {
       class NewEngineAdapter : public CrossEngineAdapter {
           // Implementation
       };
   }
   ```

3. **Add Build Configuration**:
   ```cmake
   # src/adapters/new-engine/CMakeLists.txt
   add_library(new_engine_adapter SHARED
       NewEngineAdapter.cpp
       NewEngineAdapter.hpp
   )
   
   target_link_libraries(new_engine_adapter
       uevr_adapters_common
       uevr_core
   )
   ```

4. **Register in Main System**:
   ```cpp
   // Add to adapter registry
   registry->registerAdapter<NewEngineAdapter>();
   ```

### 6.2 Adding New Hooks
1. **Create Hook Class**:
   ```cpp
   // src/hooks/NewHook.hpp
   #include "uevr/hooks/EngineHooks.hpp"
   
   namespace uevr::Hooks {
       class NewHook : public EngineHooks {
           // Implementation
       };
   }
   ```

2. **Register with Hook Manager**:
   ```cpp
   // Add to hook manager
   hookManager->addHook<NewHook>();
   ```

### 6.3 Testing New Components
1. **Create Unit Tests**:
   ```cpp
   // tests/unit/new_component_tests.cpp
   #include <gtest/gtest.h>
   #include "uevr/NewComponent.hpp"
   
   TEST(NewComponentTest, BasicFunctionality) {
       // Test implementation
   }
   ```

2. **Run Tests**:
   ```bash
   cd build
   cmake --build . --target test
   ```

## 7. Testing Strategy

### 7.1 Test Organization
- **Unit Tests**: Test individual components in isolation
- **Integration Tests**: Test component interactions
- **Performance Tests**: Measure performance characteristics
- **Test Fixtures**: Reusable test data and setup

### 7.2 Test Coverage
- **Core Framework**: 100% coverage of public API
- **Adapters**: Comprehensive testing of each adapter
- **Hooks**: Test all hook installation/removal scenarios
- **Mods**: Test VR modification functionality

### 7.3 Continuous Integration
- **Automated Testing**: Tests run on every commit
- **Multiple Platforms**: Windows, Linux, and macOS testing
- **Code Quality**: Static analysis and linting
- **Performance Monitoring**: Track performance regressions

## 8. Troubleshooting

### 8.1 Common Issues

#### Build Failures
```bash
# Check CMake configuration
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Verify dependencies
cmake --find-package -DNAME=MinHook -DCOMPILER_ID=MSVC
```

#### Include Path Errors
```bash
# Update include paths
.\update_include_paths.ps1 -Validate

# Check include directory structure
tree include/
```

#### Linker Errors
```bash
# Verify library linking
cmake --build . --verbose

# Check library dependencies
ldd build/lib/libuevr.so  # Linux
dumpbin /dependents build/lib/uevr.dll  # Windows
```

### 8.2 Getting Help
1. **Check Documentation**: Review this document and API docs
2. **Run Validation**: Use migration and validation scripts
3. **Review Build Output**: Check CMake and compiler output
4. **Check Dependencies**: Verify all required libraries are available

## 9. Future Roadmap

### 9.1 Short Term (Next 3 Months)
- [ ] Complete migration of all existing code
- [ ] Comprehensive testing suite
- [ ] Performance optimization
- [ ] Documentation completion

### 9.2 Medium Term (3-6 Months)
- [ ] Additional game engine support
- [ ] Enhanced VR features
- [ ] Plugin marketplace
- [ ] Advanced debugging tools

### 9.3 Long Term (6+ Months)
- [ ] Cross-platform VR support
- [ ] Cloud-based configuration
- [ ] Machine learning integration
- [ ] Professional support and training

## 10. Contributing

### 10.1 Development Setup
1. Fork the repository
2. Create a feature branch
3. Follow the coding standards
4. Add comprehensive tests
5. Submit a pull request

### 10.2 Coding Standards
- **C++17**: Use modern C++ features
- **Naming**: Follow consistent naming conventions
- **Documentation**: Document all public APIs
- **Testing**: Maintain high test coverage

### 10.3 Code Review Process
1. **Automated Checks**: CI/CD pipeline validation
2. **Peer Review**: Code review by maintainers
3. **Testing**: All tests must pass
4. **Documentation**: Update relevant documentation

---

## Conclusion

The new uevr project structure represents a significant improvement in code organization, maintainability, and developer experience. By following this guide, developers can:

- **Understand the codebase** more quickly
- **Add new features** with confidence
- **Maintain existing code** more easily
- **Collaborate effectively** with other developers
- **Scale the project** to support more games and features

For questions or support, please refer to the project documentation or create an issue in the repository.

---

*Last updated: $(Get-Date -Format 'yyyy-MM-dd')*
*uevr Version: 1.0.0*
