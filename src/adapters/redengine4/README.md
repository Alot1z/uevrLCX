# REDengine 4 Cross-Engine VR Adapter for uevr

## Overview
The REDengine 4 Cross-Engine VR Adapter extends uevr's plugin architecture to provide VR support for CD Projekt Red's REDengine 4 games, specifically **Cyberpunk 2077**. This adapter leverages uevr's proven injection and rendering frameworks while implementing engine-specific hooks for REDengine 4's D3D12 rendering pipeline.

## Features
- **D3D12 Rendering Integration**: Full integration with REDengine 4's DirectX 12 rendering pipeline
- **VR Stereo Rendering**: Split-screen stereo rendering for VR headsets
- **UObjectHook System**: Complete object attachment system for weapons and utilities
- **Camera Matrix Extraction**: Automatic extraction and transformation of REDengine 4 camera matrices
- **Performance Optimization**: VR-specific optimizations for open-world rendering
- **State Persistence**: Automatic saving and loading of VR configuration and attachments

## Architecture

### Core Components
1. **REDengine4Adapter**: Main plugin class extending uevr's Plugin system
2. **UObjectHookManager**: Handles object attachments and transformations
3. **D3D12 Hooks**: Intercepts REDengine 4's rendering pipeline
4. **VR Rendering Pipeline**: Implements stereo rendering for VR headsets

### Integration Points
- **uevr Plugin System**: Extends `uevr::Plugin` base class
- **D3D12 Pipeline**: Hooks into swapchain and command list functions
- **Memory Management**: Safe memory reading/writing for REDengine 4
- **Object System**: Integrates with REDengine 4's object hierarchy

## Building the Adapter

### Prerequisites
- Visual Studio 2019/2022 with C++17 support
- CMake 3.16 or later
- Windows 10/11 SDK
- DirectX 12 SDK

### Build Steps
1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd uevr/adapters/REDengine4
   ```

2. **Build using the provided script**:
   ```bash
   build.bat
   ```

3. **Manual build with CMake**:
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --target redengine4_adapter --config Release
   ```

### Build Output
The build process creates:
- `redengine4_adapter.dll` - Main adapter plugin
- `redengine_adapter.cpp/hpp` - Core adapter implementation
- `uobject_hook.cpp/hpp` - Object attachment system

## Installation

### Automatic Installation
The build script automatically copies files to:
```
profiles/adapters/redengine4/
├── redengine4_adapter.dll
├── redengine_adapter.cpp
├── redengine_adapter.hpp
├── uobject_hook.cpp
└── uobject_hook.hpp
```

### Manual Installation
1. Copy `redengine4_adapter.dll` to your uevr plugins directory
2. Ensure the adapter is loaded by uevr
3. Launch Cyberpunk 2077 with uevr

## Usage

### Basic VR Setup
1. **Launch Cyberpunk 2077** through uevr
2. **Enable VR Mode**: The adapter automatically detects and enables VR
3. **Configure VR Settings**: Adjust resolution, FOV, and comfort settings
4. **Test VR Rendering**: Verify stereo rendering is working correctly

### UObjectHook Attachments

#### Weapon Attachments (Right Controller)
```cpp
// Attach primary weapon to right controller
adapter->attach_weapon_to_right_controller("Primary_Weapon");

// Adjust weapon position and rotation
DirectX::XMFLOAT3 location = {0.0f, 0.0f, 0.0f};
DirectX::XMFLOAT4 rotation = {1.0f, 0.0f, 0.0f, 0.0f};
adapter->adjust_attachment("Primary_Weapon", location, rotation);

// Make attachment permanent
adapter->make_attachment_permanent("Primary_Weapon");
```

#### Utility Attachments (Left Controller)
```cpp
// Attach utility item to left controller
adapter->attach_utility_to_left_controller("Utility_Item");

// Save attachment state
adapter->save_uobject_hook_state();
```

#### Object Discovery
```cpp
// Get list of discovered objects
std::vector<std::string> objects = adapter->get_discovered_objects();

// Get components for a specific object
std::vector<std::string> components = uobject_hook_manager->discover_components("Weapon");
```

### Configuration

#### VR Settings
The adapter automatically configures:
- **Stereo Rendering**: Split-screen stereo for VR
- **Camera Transformation**: VR head tracking integration
- **Performance Optimization**: VR-specific rendering optimizations
- **Comfort Settings**: Decoupled pitch and smooth turning

#### Performance Tuning
- **Resolution Scale**: Adjustable for performance vs quality
- **Frame Rate**: Uncapped for VR comfort
- **Memory Management**: Efficient VR resource handling

## Technical Details

### Hook Points
1. **IDXGISwapChain3::Present1**: Frame presentation hook
2. **IDXGISwapChain3::ResizeBuffers**: Resolution change handling
3. **ID3D12CommandQueue::ExecuteCommandLists**: Command list analysis

### Memory Management
- **Safe Memory Access**: Protected reading/writing of REDengine 4 memory
- **Pattern Scanning**: Automatic detection of game objects and components
- **State Persistence**: JSON-based configuration storage

### VR Rendering Pipeline
1. **Frame Synchronization**: Sync with OpenXR/OpenVR
2. **Stereo View Generation**: Left and right eye rendering
3. **Viewport Management**: Split-screen stereo setup
4. **Matrix Transformation**: VR camera matrix calculation

## Troubleshooting

### Common Issues

#### Build Failures
- **CMake Errors**: Ensure Visual Studio and Windows SDK are installed
- **Dependency Issues**: Check MinHook and nlohmann/json availability
- **Compiler Errors**: Verify C++17 support is enabled

#### Runtime Issues
- **Hook Failures**: Restart game and verify uevr integration
- **Performance Problems**: Adjust resolution scale and quality settings
- **Attachment Issues**: Re-scan objects and reattach items

#### VR Problems
- **Stereo Rendering**: Check VR runtime configuration
- **Controller Issues**: Verify VR controller setup and calibration
- **Comfort Issues**: Adjust comfort settings and turning preferences

### Debug Information
The adapter provides detailed logging for:
- Hook installation and status
- Object discovery and attachment
- VR rendering pipeline status
- Performance metrics and optimization

## Development

### Extending the Adapter
1. **Add New Hook Points**: Implement additional D3D12 function hooks
2. **Extend Object System**: Add support for new object types
3. **Custom VR Features**: Implement game-specific VR functionality
4. **Performance Optimization**: Add new optimization techniques

### Testing
1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Verify uevr integration
3. **Game Testing**: Test with actual Cyberpunk 2077 gameplay
4. **Performance Testing**: Benchmark VR performance impact

## License
This adapter is licensed under the MIT License, separate from the main uevr codebase.

## Support
- **Documentation**: See `docs/cross-engine/REDengine4.md`
- **Issues**: Report problems through the project's issue tracker
- **Community**: Join uevr community discussions

## Version History
- **v1.0.0**: Initial release with basic VR support
- **Future**: Advanced features and optimizations planned

---

**Note**: This adapter is specifically designed for Cyberpunk 2077 and REDengine 4. For other engines, see the corresponding cross-engine adapters in the uevr project.
