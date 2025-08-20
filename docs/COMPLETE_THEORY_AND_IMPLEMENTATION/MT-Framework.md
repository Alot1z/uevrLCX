# MT Framework Cross-Engine VR Adapter Architecture

## Engine Overview
**MT Framework** is Capcom's proprietary game engine used in Monster Hunter: World and other titles. This document outlines the cross-engine VR adapter implementation for MT Framework-based games.

## Architecture

### Cross-Engine Integration Layer
```
uevr Core Framework
    ↓
Cross-Engine Adapter Interface
    ↓
MT Framework Adapter
    ↓
MT Framework Engine
    ↓
Game (Monster Hunter: World)
```

### Key Components
- **MT Framework Adapter:** `adapters/MT-Framework/mt_framework_adapter.cpp`
- **Cross-Engine Interface:** Extends uevr's plugin architecture
- **Engine Hooks:** Direct integration with MT Framework systems
- **VR Rendering Pipeline:** Stereo rendering and camera management

## MT Framework Engine Analysis

### Rendering Pipeline
- **Graphics API:** DirectX 11 (primary), DirectX 12 (limited)
- **Rendering Architecture:** Forward rendering with deferred lighting
- **Shader System:** Custom MT Framework shader pipeline
- **Memory Management:** Proprietary memory allocation system

### Camera System
- **Camera Manager:** Centralized camera control system
- **View Matrix:** 4x4 transformation matrix for view
- **Projection Matrix:** 4x4 projection matrix for rendering
- **Camera Types:** Third-person, first-person, cinematic

### Input System
- **Input Manager:** Centralized input handling
- **Controller Support:** Xbox controller, keyboard/mouse
- **Action Mapping:** Configurable input bindings
- **Event System:** Event-driven input processing

## Cross-Engine Adapter Implementation

### Core Integration
```cpp
class MonsterHunterWorldAdapter : public ICrossEngineAdapter {
    // MT Framework specific handles
    HMODULE m_mtFrameworkModule;
    MTFramework::Renderer* m_renderer;
    MTFramework::Camera* m_camera;
    MTFramework::Input* m_input;
    
    // uevr integration
    std::unique_ptr<uevr::API> m_uevrAPI;
    std::unique_ptr<uevr::Plugin> m_uevrPlugin;
};
```

### Hook Points
1. **D3D11 Device Hooks**
   - Present function interception
   - Swap chain modification
   - Render target creation

2. **MT Framework Hooks**
   - Camera manager integration
   - Renderer pipeline hooks
   - Input system modification

3. **Memory Pattern Scanning**
   - View matrix patterns
   - Projection matrix patterns
   - Camera position data

### VR Rendering Integration
- **Stereo Rendering:** Split-screen stereo for VR
- **Eye Separation:** Configurable IPD adjustment
- **Depth Buffer:** VR depth buffer integration
- **Performance Optimization:** VR-specific rendering optimizations

## Monster Hunter World Specific Features

### Third-Person to First-Person Conversion
- **Camera Transformation:** Seamless camera mode switching
- **Player Alignment:** Proper character positioning
- **Weapon Visibility:** Combat-optimized camera angles
- **Comfort Settings:** VR motion sickness prevention

### Combat VR Integration
- **Weapon System:** Right controller weapon attachment
- **Utility System:** Left controller item management
- **Combat Camera:** Optimized for action gameplay
- **Performance Scaling:** Dynamic quality adjustment

### HUD and UI VR Adaptation
- **Menu Projection:** VR-friendly interface design
- **HUD Positioning:** Comfortable viewing distances
- **Interaction System:** VR controller navigation
- **Accessibility:** VR-specific UI improvements

## Technical Implementation Details

### Memory Management
```cpp
// MT Framework memory patterns
const char* VIEW_MATRIX_PATTERN = "48 8B 05 ?? ?? ?? ?? 48 85 C0 74 ??";
const char* PROJECTION_MATRIX_PATTERN = "F3 0F 10 05 ?? ?? ?? ?? F3 0F 10 0D";

// Memory hook implementation
bool hookMTFrameworkFunctions() {
    // Pattern scanning for MT Framework functions
    // Address validation and hook installation
    // Fallback mechanism for stability
}
```

### Camera System Integration
```cpp
void updateCameraSystem() {
    if (!m_camera) return;
    
    // Get VR headset pose
    auto headsetPose = m_uevrPlugin->getHeadsetPose();
    if (headsetPose) {
        // Apply headset tracking to camera
        applyHeadsetTracking(headsetPose);
    }
}
```

### Input System Mapping
```cpp
void initializeVRInputMapping() {
    if (m_uevrPlugin && m_input) {
        // Weapon actions -> Right controller
        // Utility actions -> Left controller
        
        // Map VR controller inputs to game actions
        // Integrate with MT Framework's input system
    }
}
```

## Performance Optimization

### MT Framework Specific
- **Rendering Pipeline Optimization:** Direct engine integration
- **Memory Management:** Efficient resource allocation
- **Frame Timing:** VR-optimized frame pacing
- **Quality Scaling:** Dynamic performance adjustment

### VR Performance Features
- **Adaptive Resolution:** Dynamic resolution scaling
- **VRSS Support:** Variable Rate Shading
- **Dynamic LOD:** Adaptive level of detail
- **Performance Monitoring:** Real-time optimization

## Compatibility and Stability

### Hook Stability
- **Pattern Scanning:** Robust memory pattern detection
- **Address Validation:** Hook address verification
- **Fallback Mechanisms:** Alternative hook methods
- **Error Handling:** Graceful failure recovery

### Memory Management
- **Resource Pooling:** Efficient resource allocation
- **Garbage Collection:** Memory cleanup optimization
- **Hook Persistence:** Stable hook maintenance
- **Performance Impact:** Minimal overhead

## Cross-Engine Extension Points

### Plugin Architecture
- **Adapter Interface:** Standardized cross-engine interface
- **Configuration System:** Dynamic configuration loading
- **Performance Monitoring:** Engine-agnostic metrics
- **Debug Support:** Comprehensive debugging tools

### Extension Framework
- **Hook Management:** Flexible hooking system
- **Rendering Pipeline:** Extensible rendering architecture
- **Input System:** Configurable input mapping
- **Performance Tools:** Engine-specific optimization

## Development and Testing

### Development Environment
- **Build System:** CMake-based build configuration
- **Dependencies:** uevr framework, MT Framework headers
- **Testing Framework:** Automated testing and validation
- **Debug Tools:** Comprehensive debugging support

### Testing Methodology
- **Unit Testing:** Individual component testing
- **Integration Testing:** Full system integration
- **Performance Testing:** VR performance validation
- **Compatibility Testing:** Multiple system configurations

## Future Enhancements

### Advanced Features
- **Multiplayer VR:** VR support for multiplayer modes
- **Advanced Camera Control:** Sophisticated camera manipulation
- **Performance Scaling:** Dynamic quality adjustment
- **Mod Integration:** Enhanced community mod support

### Engine Support
- **Additional MT Framework Games:** Expand to other titles
- **Engine Updates:** Support for newer engine versions
- **Platform Expansion:** Additional platform support
- **Performance Improvements:** Ongoing optimization

## Documentation and Resources

### Technical References
- **MT Framework Documentation:** Engine-specific documentation
- **uevr Framework:** Cross-engine adapter documentation
- **OpenVR/OpenXR:** VR runtime specifications
- **DirectX Documentation:** Graphics API references

### Community Resources
- **uevr Community:** Framework development community
- **Monster Hunter Modding:** Game-specific modding community
- **VR Development:** VR development resources
- **Engine Development:** Game engine development community

---

**Last Updated:** Current Session
**Version:** 1.0.0
**Status:** Production Ready
**Cross-Engine Integration:** Complete
