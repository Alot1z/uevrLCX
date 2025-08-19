# uevr New File Structure & Build System

## Overview

uevr has been reorganized with a new file structure that follows modern development practices and provides better organization for the growing codebase. This document explains the new structure and how to use the updated build system.

## New File Structure

```
uevr/
├── src/                           # Main source code directory
│   ├── adapters/                  # Game engine adapters
│   │   ├── redengine4/           # REDengine 4 (Cyberpunk 2077)
│   │   ├── mt-framework/         # MT Framework (Monster Hunter World)
│   │   └── re-engine/            # RE Engine (Resident Evil 7)
│   ├── mods/                      # Core uevr modules
│   ├── hooks/                     # DirectX and system hooks
│   ├── examples/                  # Example plugins
│   ├── lua-api/                  # Lua scripting API
│   └── CMakeLists.txt            # Main build configuration
├── build_system/                  # Ultra-Advanced Build & Analysis System
├── docs/                          # Documentation
├── tools/                         # Development tools
└── include/                       # Public headers
```

## Key Changes

### 1. Adapters Moved to `src/adapters/`

All game engine adapters are now located in `src/adapters/` instead of the root `adapters/` directory:

- **Before**: `adapters/REDengine4/`
- **After**: `src/adapters/redengine4/`

### 2. Main Framework in `src/`

The core uevr framework is now in `src/` with proper CMake organization.

### 3. Build System in `build_system/`

The Ultra-Advanced Build & Analysis System is located in `build_system/` for easy access.

## Updated Build Scripts

### Complete Structure Build

- **Batch**: `complete_structure_build.bat`
- **PowerShell**: `complete_structure_build.ps1`

These scripts build the entire uevr project following the new structure:

1. **uevr Core Framework** - Main backend using CMake
2. **Game Engine Adapters** - REDengine4, MT-Framework, RE-Engine
3. **Plugins & Extensions** - Example plugin, Lua API
4. **Ultra-Advanced Build System** - QA, Security, Rendering, Deployment

### Validation Executor

- **File**: `updated_validation_executor.bat`

Validates that the new file structure is properly set up and all components are in place.

## Building with New Structure

### Prerequisites

- Visual Studio 2022 with C++23 support
- CMake 3.15 or newer
- Git with submodules

### Quick Start

1. **Clone with submodules**:
   ```bash
   git clone --recursive https://github.com/praydog/uevr.git
   cd uevr
   ```

2. **Run complete build**:
   ```bash
   cd build_system
   complete_structure_build.bat
   ```

   Or with PowerShell:
   ```powershell
   cd build_system
   .\complete_structure_build.ps1
   ```

3. **Validate structure**:
   ```bash
   updated_validation_executor.bat
   ```

## Build Outputs

### Main Components

| Component | Output | Location |
|-----------|--------|----------|
| uevr Backend | uevrBackend.dll | `src/build/` |
| REDengine4 Adapter | redengine_adapter.dll | `src/adapters/redengine4/` |
| MT-Framework Adapter | mt_framework_adapter.dll | `src/adapters/mt-framework/` |
| RE-Engine Adapter | re_engine_adapter.dll | `src/adapters/re-engine/` |
| Example Plugin | example_plugin.dll | `src/examples/example_plugin/build/` |
| Lua API | LuaVR.dll | `src/lua-api/build/` |

### Build System Components

| Component | Output | Purpose |
|-----------|--------|---------|
| QA System | qa_system.exe | Quality assurance and testing |
| Security System | security_system.exe | Enterprise security features |
| Rendering System | rendering_system.exe | Advanced VR rendering |
| Deployment System | deployment_system.exe | Container and orchestration |
| Master Executor | master_executor.exe | Orchestrates all systems |

## Directory Structure Details

### `src/adapters/`

Each adapter directory contains:

- **Source files** (`.cpp`, `.hpp`)
- **Build scripts** (`build.bat`, `CMakeLists.txt`)
- **Documentation** (`README.md`)
- **Validation tools** (validation status finders)

### `src/mods/`

Core uevr modules:

- **VR.cpp/hpp** - Main VR functionality
- **FrameworkConfig.cpp/hpp** - Configuration management
- **PluginLoader.cpp/hpp** - Plugin system
- **LuaLoader.cpp/hpp** - Lua scripting

### `src/hooks/`

System integration hooks:

- **D3D11Hook.cpp/hpp** - DirectX 11 hooks
- **D3D12Hook.cpp/hpp** - DirectX 12 hooks
- **XInputHook.cpp/hpp** - Input system hooks

## Migration from Old Structure

If you have existing projects using the old structure:

1. **Update include paths** from `adapters/` to `src/adapters/`
2. **Update build scripts** to use new paths
3. **Update documentation** references
4. **Test builds** with new structure

## Benefits of New Structure

### 1. **Better Organization**
- Clear separation of concerns
- Logical grouping of related components
- Easier to navigate and understand

### 2. **Improved Build System**
- Centralized CMake configuration
- Better dependency management
- Cleaner build outputs

### 3. **Enhanced Development Experience**
- Easier to add new adapters
- Better plugin development workflow
- Improved documentation structure

### 4. **Scalability**
- Ready for additional game engines
- Better support for community contributions
- Easier maintenance and updates

## Troubleshooting

### Common Issues

1. **Build fails with path errors**
   - Ensure you're using the updated build scripts
   - Check that all files are in the new structure

2. **CMake configuration fails**
   - Verify Visual Studio 2022 is installed
   - Check CMake version (3.15+ required)

3. **Adapter builds fail**
   - Ensure adapter directories exist in `src/adapters/`
   - Check that build scripts are present

### Getting Help

- Check the validation executor output
- Review build logs for specific errors
- Ensure all submodules are properly cloned
- Verify file paths match the new structure

## Future Development

The new structure is designed to support:

- **Additional game engines** - Easy to add new adapters
- **Enhanced plugin system** - Better plugin development workflow
- **Community contributions** - Clearer contribution guidelines
- **Automated testing** - Better CI/CD integration
- **Documentation** - Improved developer experience

## Conclusion

The new uevr file structure provides a solid foundation for continued development and community contributions. By following the updated build scripts and understanding the new organization, developers can easily work with uevr and contribute to its growth.

For questions or issues with the new structure, please refer to the validation executor output and ensure all components are properly placed according to the new organization.
