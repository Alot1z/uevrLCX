# RE Engine Cross-Engine VR Adapter

**Engine:** RE Engine (Capcom)  
**Target Games:** Resident Evil 7 BIOHAZARD, Resident Evil 2, Resident Evil 3, etc.  
**Graphics API:** DirectX 11  
**VR Support:** Full uevr Integration  

## Overview

The RE Engine Cross-Engine VR Adapter extends uevr's plugin architecture to support Capcom's RE Engine, enabling comprehensive VR support for RE Engine titles through uevr's cross-engine extension system.

## Architecture

### Core Components

1. **RE_Engine_Adapter.cpp** - Main adapter class extending uevr's plugin system
2. **RE_Engine_Hooks.cpp** - D3D11 rendering pipeline hooks
3. **engine_hooks.cpp** - RE Engine-specific engine hooks
4. **RE_Engine_Adapter.hpp** - Header with data structures and declarations
5. **CMakeLists.txt** - Build configuration

### Integration Strategy

- **Primary:** REFramework integration with uevr plugin system
- **Fallback:** Native D3D11 hooks via MinHook
- **VR Pipeline:** Complete uevr stereo rendering integration
- **Performance:** Integrated with uevr optimization systems

## Features

### VR Integration
- **Stereo Rendering:** Full uevr stereo rendering pipeline integration
- **Motion Controllers:** Complete motion controller support via uevr abstraction
- **Head Tracking:** Full VR head tracking integration
- **Performance:** 90 FPS target with adaptive quality scaling

### RE Engine Specific
- **Engine Detection:** Automatic RE Engine detection and validation
- **Hook System:** Comprehensive engine hooking for rendering, camera, and input
- **PBR Optimization:** VR-optimized PBR lighting and shadow systems
- **Horror Game Features:** Atmospheric optimization for VR immersion

### Input Mapping
- **Weapon → Right Controller:** Full weapon handling support
- **Utility → Left Controller:** Inventory and movement support
- **Haptic Feedback:** Complete haptic feedback integration
- **Comfort Settings:** VR-optimized comfort features

## Technical Implementation

### Plugin Architecture
```cpp
class REEngineAdapter : public uevr::Plugin
{
    bool Initialize(uevr::API* api) override;
    void Cleanup() override;
    void OnFrame() override;
    void EnableVRMode(bool enable) override;
};
```

### Hook System
- **D3D11 Hooks:** SwapChain Present, Device functions, Context operations
- **Engine Hooks:** Rendering, camera updates, input processing
- **VR Hooks:** Post-render, post-camera, post-input VR processing

### Data Structures
```cpp
struct RE_Engine_Camera_Data {
    float world_scale;
    bool decoupled_pitch;
    float camera_offsets[3];
};

struct RE_Engine_Render_State {
    bool stereo_rendering;
    bool vr_overlay_mode;
    bool performance_optimization;
};

struct RE_Engine_Matrix_Data {
    XMMATRIX view_matrix;
    XMMATRIX projection_matrix;
    XMMATRIX world_matrix;
};
```

## Installation

### Build Requirements
- Visual Studio 2019/2022 with C++17 support
- DirectX 11 SDK
- MinHook library
- spdlog library
- uevr development headers

### Build Process
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Installation
1. Copy the built `RE_Engine_Adapter.dll` to uevr's plugins directory
2. Ensure uevr is properly configured
3. Launch RE Engine games via uevrInjector.exe
4. The adapter will automatically detect and integrate

## Configuration

### VR Settings
- **Target Framerate:** 90 FPS (VR requirement)
- **Resolution Scale:** Adaptive (0.8 - 1.0)
- **Quality Scaling:** Dynamic based on GPU performance
- **Comfort Mode:** Enabled with configurable settings

### RE Engine Settings
- **Engine Detection:** Automatic RE Engine identification
- **Hook Priority:** High priority for critical engine functions
- **Fallback Support:** Multiple hook strategies for reliability
- **Performance Monitoring:** Real-time performance tracking

## Usage

### Automatic Integration
The adapter automatically:
1. Detects RE Engine games
2. Installs necessary hooks
3. Integrates with uevr's VR systems
4. Provides motion controller support
5. Optimizes performance for VR

### Manual Configuration
Advanced users can:
1. Modify VR comfort settings
2. Adjust performance parameters
3. Customize input mappings
4. Tune rendering optimizations

## Performance

### Optimization Features
- **Dynamic LOD:** Adaptive level-of-detail based on VR performance
- **Quality Scaling:** Automatic quality adjustment for stable frame rates
- **Memory Management:** Efficient resource handling and cleanup
- **Hook Overhead:** Minimized impact on rendering pipeline

### Performance Targets
- **Framerate:** 90 FPS (VR requirement)
- **Latency:** < 11ms (VR requirement)
- **Memory:** Efficient resource usage
- **Scalability:** Support for different VR hardware

## Troubleshooting

### Common Issues
1. **Hooks Not Installing:** Verify RE Engine detection
2. **Performance Issues:** Check VR optimization settings
3. **Controller Problems:** Verify motion controller setup
4. **Comfort Issues:** Adjust VR comfort settings

### Debug Information
- **Log Files:** Check uevr logs for adapter status
- **Performance Metrics:** Monitor frame timing and GPU usage
- **Hook Status:** Verify RE Engine hook installation
- **VR Integration:** Check stereo rendering status

## Development

### Extending the Adapter
The adapter is designed for extensibility:
- **New Games:** Add game-specific detection patterns
- **New Features:** Extend VR functionality through uevr APIs
- **Performance:** Add engine-specific optimizations
- **Compatibility:** Support new RE Engine versions

### Contributing
1. Follow uevr's plugin development guidelines
2. Maintain compatibility with existing functionality
3. Add comprehensive testing for new features
4. Document all changes and additions

## Compatibility

### Supported Games
- **Resident Evil 7 BIOHAZARD** - Primary target (fully tested)
- **Resident Evil 2** - Compatible (testing required)
- **Resident Evil 3** - Compatible (testing required)
- **Other RE Engine titles** - Compatible (testing required)

### System Requirements
- **OS:** Windows 10/11 (64-bit)
- **Graphics:** DirectX 11 compatible GPU
- **VR:** OpenXR compatible headset
- **Memory:** 8GB RAM minimum, 16GB recommended

## Future Enhancements

### Planned Features
- **Advanced VR Features:** Eye tracking, foveated rendering
- **Performance Scaling:** Dynamic quality adjustment
- **Mod Integration:** Support for RE Engine mods
- **Multiplayer VR:** Future multiplayer VR support

### Community Contributions
- **Open Source:** Adapter code available for community enhancement
- **Mod Support:** Community mod integration capabilities
- **Performance Tuning:** Community performance optimization
- **Feature Requests:** Community-driven feature development

## Documentation References

### uevr Integration
- **Plugin API:** `docs/uevr-docs/src/plugins/`
- **VR Controller Bindings:** `docs/uevr-docs/src/usage/vr_controller_bindings.md`
- **6DOF Support:** `docs/uevr-docs/src/usage/adding_6dof.md`

### Cross-Engine Development
- **Adapter Architecture:** `docs/cross-engine/adapter_development.md`
- **RE Engine Specific:** `docs/cross-engine/RE-Engine.md`
- **Implementation Guide:** `docs/cross-engine/adapter_development.md`

### Technical Resources
- **DirectX 11:** Microsoft DirectX documentation
- **RE Engine:** Capcom engine technical specifications
- **VR Development:** OpenXR and VR development guides

---

**Adapter Maintainer:** uevr Cross-Engine Development Team  
**Last Updated:** December 19, 2024  
**Version:** 1.0.0  
**Status:** Production Ready - Full VR Support Active
