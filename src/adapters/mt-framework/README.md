# MT Framework Cross-Engine VR Adapter

**Status:** Architecture defined — awaiting D3D11 hook implementation and RenderDoc captures

## Overview

MT Framework adapter for uevr, targeting Monster Hunter: World (MonsterHunterWorld.exe) and other Capcom titles using this engine.

## Engine Architecture

- **Graphics API:** DirectX 11
- **Rendering Pipeline:** Forward rendering with MT Framework shaders
- **Camera System:** Third-person with dynamic camera positioning
- **Memory Layout:** Component-based with MT Framework patterns

## Integration Strategy

### Primary Approach: Native D3D11 Hooks
- **Bootstrap:** DLL proxy or global injector
- **Hooks:** Direct D3D11 swapchain and device hooks
- **Implementation:** MinHook-based detours with safety checks

### Fallback: External Injection
- **Process Injection:** DLL injection via external loader
- **Hook Installation:** Runtime hook installation after injection
- **Error Handling:** Graceful degradation on hook failures

## VR Integration Points

### Camera System
- **View Matrix:** Extracted from constant buffer slot TBD
- **Projection Matrix:** Extracted from constant buffer slot TBD
- **FOV Policy:** Dynamic (from game engine)
- **Stereo Mode:** Sequential rendering

### Rendering Pipeline
- **Hook Point:** `IDXGISwapChain::Present`
- **Stereo Rendering:** Left/right eye sequential rendering
- **Depth Handling:** Native depth buffer integration
- **UI Projection:** Billboard-based HUD projection

### Input System
- **Motion Controllers:** Weapon (right) + Utility (left)
- **Aim Method:** Right controller (weapon attachment)
- **Movement:** Roomscale with snap turning support

## Implementation Details

### Plugin Structure
```cpp
class MTFrameworkAdapter : public uevr::Plugin {
public:
    void on_initialize() override;
    void on_present() override;
    void on_device_reset() override;
    
private:
    void setup_d3d11_hooks();
    void extract_camera_matrices();
    void render_stereo_frame();
    void handle_third_person_camera();
};
```

### Hook Registration
- **SwapChain Present:** Main rendering loop hook
- **Device Reset:** Handle device recreation
- **Constant Buffer Updates:** Camera matrix extraction
- **Vertex Shader Setup:** View/projection matrix binding

### Memory Management
- **Safe Hooking:** Validate pointers before dereferencing
- **Resource Tracking:** Monitor D3D11 resource lifecycle
- **Error Recovery:** Graceful fallback on hook failures

## Configuration

### adapter.json Schema
```json
{
  "engine": "MT Framework",
  "executable": "MonsterHunterWorld.exe",
  "graphics_api": "D3D11",
  "render": {
    "api": "d3d11",
    "swapchain_hook": "Present|ResizeBuffers",
    "matrix_sources": {
      "view": { "cb_slot": null, "offset": null },
      "projection": { "cb_slot": null, "offset": null }
    }
  }
}
```

### Profile Integration
- **Config:** `profiles/MonsterHunterWorld/config.txt`
- **Cameras:** `profiles/MonsterHunterWorld/cameras.txt`
- **Attachments:** `profiles/MonsterHunterWorld/adapter.json`

## Development Workflow

1. **Setup injection** method for Monster Hunter: World
2. **Implement D3D11 hooks** for swapchain present
3. **Extract camera matrices** from constant buffers
4. **Implement sequential stereo rendering** pipeline
5. **Add motion controller** support for third-person
6. **Validate performance** and comfort settings

## Testing

### RenderDoc Captures
- **Baseline:** `captures/renderdoc/MonsterHunterWorld/mhw_baseline.rdc`
- **Focus:** D3D11 constant buffers, camera setup
- **Validation:** Sequential rendering correctness

### Performance Metrics
- **Frame Rate:** Target 90/120 FPS in VR
- **Latency:** Motion-to-photon < 20ms
- **Comfort:** Third-person to VR transition

## Known Issues

- **Third-Person Camera:** Complex camera system adaptation
- **MT Framework Shaders:** Custom shader pipeline integration
- **Memory Layout:** Constant buffer indexing TBD

## Next Steps

1. Implement D3D11 swapchain hooks
2. Extract camera matrices from constant buffers
3. Implement sequential stereo rendering pipeline
4. Add motion controller input mapping
5. Validate with RenderDoc captures

## Citations

- **MinHook API:** https://github.com/TsudaKageyu/minhook
- **uevr Plugin API:** `SOURCECODE/uevr/include/uevr/Plugin.hpp`
- **D3D11 Hooking:** IDXGISwapChain::Present + MinHook
- **MT Framework Analysis:** Capcom engine documentation
