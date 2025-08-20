# RE Engine Cross-Engine VR Adapter Architecture

## Overview
The RE Engine Cross-Engine VR Adapter extends uevr's plugin architecture to provide VR support for Capcom's RE Engine games, specifically Resident Evil 7. This adapter leverages uevr's proven injection and rendering frameworks while implementing engine-specific hooks for RE Engine's D3D11 rendering pipeline.

## Architecture Components

### 1. Plugin Integration Layer
- **Base Class**: Extends `uevr::Plugin` from uevr's plugin architecture
- **Initialization**: Implements `on_initialize()` for RE Engine detection and setup
- **Rendering Hooks**: Implements `on_post_render_vr_framework_dx11()` for VR frame injection
- **Device Management**: Implements `on_device_reset()` for resource cleanup

### 2. RE Engine Detection System
- **Process Scanning**: Detects RE Engine by scanning for `re7.exe` executable
- **Memory Pattern Recognition**: Identifies RE Engine specific memory signatures
- **Hook Point Discovery**: Locates critical rendering and camera system addresses

### 3. D3D11 Rendering Pipeline Integration
- **Device Hook**: Intercepts RE Engine's D3D11 device creation
- **Render Target Management**: Captures and modifies render targets for VR
- **Stereo Rendering**: Implements split-screen stereo rendering for VR headsets

### 4. OpenXR VR Runtime Integration
- **Session Management**: Creates and manages OpenXR sessions
- **Space Management**: Establishes view and local reference spaces
- **Frame Rendering**: Implements VR frame rendering loop with proper timing

## Hook Points and Integration

### Primary Hook Points

#### 1. D3D11 Device Creation Hook
```cpp
// Hook into D3D11CreateDeviceAndSwapChain
typedef HRESULT(WINAPI* D3D11CreateDeviceAndSwapChain_t)(
    IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
    const D3D_FEATURE_LEVEL*, UINT, UINT,
    const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**,
    ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
```

**Purpose**: Capture D3D11 device and context for VR rendering
**Integration**: Modify swap chain creation for VR compatibility

#### 2. RE Engine Rendering Pipeline Hook
```cpp
// Hook into RE Engine's main rendering function
// Located in re7.exe memory space
// Intercepts Present() calls for VR frame injection
```

**Purpose**: Inject VR rendering at the right point in RE Engine's pipeline
**Integration**: Modify viewport and render target setup for stereo rendering

#### 3. Camera System Hook
```cpp
// Hook into RE Engine's camera update functions
// Intercepts view and projection matrix calculations
// Modifies matrices for VR head tracking
```

**Purpose**: Transform RE Engine's camera system for VR
**Integration**: Convert flat screen camera to VR head-tracked camera

### Secondary Hook Points

#### 4. Scene Graph Hook
```cpp
// Hook into RE Engine's scene traversal
// Optimize rendering for VR performance
// Handle VR-specific culling and LOD
```

#### 5. Input System Hook
```cpp
// Hook into RE Engine's input handling
// Map VR controller inputs to game actions
// Implement VR-specific input methods
```

## Memory Layout and Address Discovery

### RE Engine Memory Structure
- **Base Address**: `re7.exe` module base address
- **Rendering Pipeline**: Located at offset from base address
- **Camera System**: Camera matrices stored in specific memory regions
- **Scene Data**: Scene graph and object data in heap memory

### Pattern Scanning Strategy
```cpp
// Example pattern for RE Engine camera system
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
D3D11_VIEWPORT viewport;
viewport.TopLeftX = eyeIndex * (1920 / 2);
viewport.Width = 1920 / 2;
viewport.Height = 1080;
```

## Performance Optimization

### 1. Frame Timing
- **VSync Integration**: Sync with RE Engine's vsync for smooth rendering
- **Frame Pacing**: Maintain consistent frame timing for VR comfort
- **Performance Monitoring**: Track frame times and adjust quality settings

### 2. Memory Management
- **Resource Pooling**: Reuse VR rendering resources across frames
- **Garbage Collection**: Proper cleanup of temporary VR resources
- **Memory Mapping**: Efficient mapping of RE Engine memory regions

### 3. Rendering Optimization
- **Frustum Culling**: VR-specific culling for performance
- **LOD Management**: Adjust level of detail based on VR requirements
- **Shader Optimization**: Optimize shaders for VR rendering

## Compatibility and Limitations

### Supported RE Engine Versions
- **Resident Evil 7**: Primary target with full VR support
- **RE Engine 1.0**: Base engine version compatibility
- **Future Titles**: Extensible architecture for upcoming RE Engine games

### Known Limitations
- **Third-Party Mods**: May conflict with VR adapter hooks
- **Performance**: VR rendering adds overhead to base game performance
- **Input Mapping**: Some complex input combinations may require manual mapping

### Workarounds and Solutions
- **Mod Compatibility**: Hook priority management for mod compatibility
- **Performance Tuning**: Configurable quality settings for different hardware
- **Input Customization**: User-configurable input mapping system

## Configuration and Profiles

### Adapter Configuration
```json
{
  "re_engine": {
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
- **Per-Game Profiles**: Individual configuration for each RE Engine title
- **User Profiles**: Customizable settings for different users
- **Performance Profiles**: Pre-configured settings for different hardware tiers

## Development and Testing

### Development Environment
- **Visual Studio**: Primary development environment
- **uevr SDK**: Integration with uevr's plugin development tools
- **RE Engine Analysis**: Tools for analyzing RE Engine memory and behavior

### Testing Methodology
- **Memory Hook Testing**: Verify hook stability and performance
- **VR Rendering Testing**: Test stereo rendering and head tracking
- **Performance Testing**: Benchmark VR performance impact
- **Compatibility Testing**: Test with different RE Engine versions

### Debugging Tools
- **RenderDoc Integration**: Graphics debugging and analysis
- **Memory Debugging**: Tools for analyzing memory hooks and modifications
- **Performance Profiling**: Tools for identifying performance bottlenecks

## Future Enhancements

### Planned Features
- **Advanced Camera Control**: More sophisticated camera manipulation
- **Performance Scaling**: Dynamic quality adjustment based on performance
- **Mod Support**: Better integration with RE Engine mods
- **Multi-Player VR**: VR support for multiplayer modes

### Architecture Extensions
- **Plugin System**: Extensible plugin architecture for custom features
- **Scripting Support**: Lua scripting for advanced customization
- **Community Tools**: Tools for community development and sharing

## References and Resources

### Technical Documentation
- [OpenXR Specification](https://www.khronos.org/openxr/)
- [DirectX 11 Programming Guide](https://docs.microsoft.com/en-us/windows/win32/direct3d11/)
- [uevr Plugin Development](https://github.com/praydog/uevr)

### Community Resources
- [RE Engine Modding Community](https://github.com/topics/re-engine)
- [VR Development Forums](https://community.vrchat.com/)
- [Game Modding Discord](https://discord.gg/modding)

### Related Projects
- [REFramework](https://github.com/praydog/REFramework) - RE Engine modding framework
- [OpenXR Samples](https://github.com/KhronosGroup/OpenXR-Samples) - OpenXR implementation examples
- [DirectX Graphics Samples](https://github.com/microsoft/DirectX-Graphics-Samples) - D3D11/12 examples
