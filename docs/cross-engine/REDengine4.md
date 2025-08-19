# REDengine 4 Cross-Engine VR Adapter Architecture

## Overview
The REDengine 4 Cross-Engine VR Adapter extends uevr's plugin architecture to provide VR support for CD Projekt Red's REDengine 4 games, specifically Cyberpunk 2077. This adapter leverages uevr's proven injection and rendering frameworks while implementing engine-specific hooks for REDengine 4's D3D12 rendering pipeline.

## Architecture Components

### 1. Plugin Integration Layer
- **Base Class**: Extends `uevr::Plugin` from uevr's plugin architecture
- **Initialization**: Implements `on_initialize()` for REDengine 4 detection and setup
- **Rendering Hooks**: Implements `on_post_render_vr_framework_dx12()` for VR frame injection
- **Device Management**: Implements `on_device_reset()` for resource cleanup

### 2. REDengine 4 Detection System
- **Process Scanning**: Detects REDengine 4 by scanning for `Cyberpunk2077.exe` executable
- **Memory Pattern Recognition**: Identifies REDengine 4 specific memory signatures
- **Hook Point Discovery**: Locates critical rendering and camera system addresses

### 3. D3D12 Rendering Pipeline Integration
- **Device Hook**: Intercepts REDengine 4's D3D12 device creation
- **Command List Hooks**: Captures and modifies D3D12 command lists for VR
- **Render Target Management**: Manages D3D12 render targets for VR compatibility
- **Stereo Rendering**: Implements split-screen stereo rendering for VR headsets

### 4. OpenXR VR Runtime Integration
- **Session Management**: Creates and manages OpenXR sessions
- **Space Management**: Establishes view and local reference spaces
- **Frame Rendering**: Implements VR frame rendering loop with proper timing

## Hook Points and Integration

### Primary Hook Points

#### 1. D3D12 Device Creation Hook
```cpp
// Hook into D3D12CreateDevice
typedef HRESULT(WINAPI* D3D12CreateDevice_t)(
    IUnknown* pAdapter,
    D3D_FEATURE_LEVEL MinimumFeatureLevel,
    REFIID riid,
    void** ppDevice);

// Hook into D3D12CreateCommandList
typedef HRESULT(WINAPI* D3D12CreateCommandList_t)(
    ID3D12Device* pDevice,
    D3D12_COMMAND_LIST_TYPE type,
    ID3D12CommandAllocator* pCommandAllocator,
    ID3D12PipelineState* pInitialState,
    REFIID riid,
    void** ppCommandList);
```

**Purpose**: Capture D3D12 device and command list for VR rendering
**Integration**: Modify command list creation for VR compatibility

#### 2. REDengine 4 Rendering Pipeline Hook
```cpp
// Hook into REDengine 4's main rendering function
// Located in Cyberpunk2077.exe memory space
// Intercepts ExecuteCommandLists calls for VR frame injection
```

**Purpose**: Inject VR rendering at the right point in REDengine 4's pipeline
**Integration**: Modify command list execution for stereo rendering

#### 3. Camera System Hook
```cpp
// Hook into REDengine 4's camera update functions
// Intercepts view and projection matrix calculations
// Modifies matrices for VR head tracking
```

**Purpose**: Transform REDengine 4's camera system for VR
**Integration**: Convert flat screen camera to VR head-tracked camera

### Secondary Hook Points

#### 4. Scene Graph Hook
```cpp
// Hook into REDengine 4's scene traversal
// Optimize rendering for VR performance
// Handle VR-specific culling and LOD
```

#### 5. Input System Hook
```cpp
// Hook into REDengine 4's input handling
// Map VR controller inputs to game actions
// Implement VR-specific input methods
```

## Memory Layout and Address Discovery

### REDengine 4 Memory Structure
- **Base Address**: `Cyberpunk2077.exe` module base address
- **Rendering Pipeline**: Located at offset from base address
- **Camera System**: Camera matrices stored in specific memory regions
- **Scene Data**: Scene graph and object data in heap memory

### Pattern Scanning Strategy
```cpp
// Example pattern for REDengine 4 camera system
const char* cameraPattern = "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x00";
const char* cameraMask = "xxx????xxx?x";

// Scan memory for camera system signatures
uintptr_t findCameraSystem() {
    return scanMemoryForPattern(cameraPattern, cameraMask);
}
```

## VR Rendering Pipeline

### 1. Frame Synchronization
```cpp
// Wait for OpenXR frame
XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
XrFrameState frameState{XR_TYPE_FRAME_STATE};
xrWaitFrame(m_xrSession, &frameWaitInfo, &frameState);
```

### 2. Stereo View Rendering
```cpp
// Render left and right eye views
for (uint32_t i = 0; i < viewCountOutput; i++) {
    renderEye(i, views[i]);
}
```

### 3. Viewport Management
```cpp
// Split screen for stereo rendering
D3D12_VIEWPORT viewport;
viewport.TopLeftX = eyeIndex * (1920 / 2);
viewport.Width = 1920 / 2;
viewport.Height = 1080;
```

## Performance Optimization

### 1. Frame Timing
- **VSync Integration**: Sync with REDengine 4's vsync for smooth rendering
- **Frame Pacing**: Maintain consistent frame timing for VR comfort
- **Performance Monitoring**: Track frame times and adjust quality settings

### 2. Memory Management
- **Resource Pooling**: Reuse VR rendering resources across frames
- **Garbage Collection**: Proper cleanup of temporary VR resources
- **Memory Mapping**: Efficient mapping of REDengine 4 memory regions

### 3. Rendering Optimization
- **Frustum Culling**: VR-specific culling for performance
- **LOD Management**: Adjust level of detail based on VR requirements
- **Shader Optimization**: Optimize shaders for VR rendering
- **Dynamic Quality Scaling**: Adjust quality based on performance

## Compatibility and Limitations

### Supported REDengine 4 Versions
- **Cyberpunk 2077**: Primary target with full VR support
- **REDengine 4.0**: Base engine version compatibility
- **Future Titles**: Extensible architecture for upcoming REDengine 4 games

### Known Limitations
- **Third-Party Mods**: May conflict with VR adapter hooks
- **Performance**: VR rendering adds overhead to open-world performance
- **Input Mapping**: Some complex input combinations may require manual mapping

### Workarounds and Solutions
- **Mod Compatibility**: Hook priority management for mod compatibility
- **Performance Tuning**: Configurable quality settings for different hardware
- **Input Customization**: User-configurable input mapping system

## Configuration and Profiles

### Adapter Configuration
```json
{
  "redengine4": {
    "vr_enabled": true,
    "stereo_rendering": true,
    "performance_mode": "balanced",
    "camera_offset": {
      "x": 0.0,
      "y": 0.0,
      "z": 0.0
    }
  }
}
```

### Profile Management
- **Per-Game Profiles**: Individual configuration for each REDengine 4 title
- **User Profiles**: Customizable settings for different users
- **Performance Profiles**: Pre-configured settings for different hardware tiers

## Development and Testing

### Development Environment
- **Visual Studio**: Primary development environment
- **uevr SDK**: Integration with uevr's plugin development tools
- **REDengine 4 Analysis**: Tools for analyzing REDengine 4 memory and behavior

### Testing Methodology
- **Memory Hook Testing**: Verify hook stability and performance
- **VR Rendering Testing**: Test stereo rendering and head tracking
- **Performance Testing**: Benchmark VR performance impact
- **Compatibility Testing**: Test with different REDengine 4 versions

### Debugging Tools
- **RenderDoc Integration**: Graphics debugging and analysis
- **Memory Debugging**: Tools for analyzing memory hooks and modifications
- **Performance Profiling**: Tools for identifying performance bottlenecks

## Future Enhancements

### Planned Features
- **Advanced Camera Control**: More sophisticated camera manipulation
- **Performance Scaling**: Dynamic quality adjustment based on performance
- **Mod Support**: Better integration with REDengine 4 mods
- **Multi-Player VR**: VR support for multiplayer modes
- **Ray Tracing VR**: VR support for ray tracing features

### Architecture Extensions
- **Plugin System**: Extensible plugin architecture for custom features
- **Scripting Support**: Lua scripting for advanced customization
- **Community Tools**: Tools for community development and sharing

## References and Resources

### Technical Documentation
- [OpenXR Specification](https://www.khronos.org/openxr/)
- [DirectX 12 Programming Guide](https://docs.microsoft.com/en-us/windows/win32/direct3d12/)
- [uevr Plugin Development](https://github.com/praydog/uevr)

### Community Resources
- [REDengine Modding Community](https://www.nexusmods.com/cyberpunk2077/mods/redmod)
- [VR Development Forums](https://community.vrchat.com/)
- [Game Modding Discord](https://discord.gg/modding)

### Related Projects
- [REDmod](https://www.nexusmods.com/cyberpunk2077/mods/redmod) - REDengine 4 modding framework
- [OpenXR Samples](https://github.com/KhronosGroup/OpenXR-Samples) - OpenXR implementation examples
- [DirectX Graphics Samples](https://github.com/microsoft/DirectX-Graphics-Samples) - D3D12 examples
