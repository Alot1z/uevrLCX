# Framework on_frame_d3d11() Function Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::on_frame_d3d11()`  
**File**: `src/Framework.cpp`  
**Purpose**: Handle DirectX 11 frame rendering for VR integration  
**Category**: DirectX 11 Frame Processing  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
void on_frame_d3d11()
```

### **Parameters**
- None

### **Return Value**
- `void`

---

## 🔧 **IMPLEMENTATION DETAILS**

### **Frame Processing Pipeline**
1. **VR Frame Setup**: Prepare VR rendering context
2. **Stereo Rendering**: Configure left/right eye rendering
3. **Matrix Extraction**: Extract view and projection matrices
4. **Mod Processing**: Execute VR modifications
5. **UI Rendering**: Render ImGui overlay
6. **Performance Monitoring**: Track frame timing

### **Key Operations**

#### **VR Integration**
```cpp
void Framework::on_frame_d3d11() {
    if (!is_ready() || !is_dx11()) {
        return;
    }
    
    // VR frame preparation
    if (vr_mod && vr_mod->is_enabled()) {
        vr_mod->on_frame_d3d11();
    }
    
    // Execute mods
    for (auto& mod : m_mods) {
        if (mod->is_enabled()) {
            mod->on_frame();
        }
    }
}
```

#### **Stereo Rendering Setup**
- Configure viewport for VR rendering
- Set up stereo projection matrices
- Handle eye-specific rendering passes
- Manage render target switching

#### **Performance Optimization**
- Frame timing analysis
- GPU utilization monitoring
- Memory usage tracking
- VR-specific performance metrics

---

## 📚 **USAGE EXAMPLES**

### **Hook Integration**
```cpp
// Called from D3D11 Present hook
HRESULT WINAPI D3D11Present_Hook(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    if (g_framework && g_framework->is_valid()) {
        g_framework->on_frame_d3d11();
    }
    
    return original_present(swap_chain, sync_interval, flags);
}
```

### **VR Mod Integration**
```cpp
class VRMod : public Mod {
public:
    void on_frame_d3d11() override {
        if (!is_vr_enabled()) return;
        
        // VR-specific frame processing
        update_hmd_pose();
        render_controllers();
        apply_stereo_projection();
    }
};
```

### **Custom Mod Example**
```cpp
class CustomMod : public Mod {
public:
    void on_frame() override {
        // Custom frame processing
        update_custom_ui();
        process_input();
        apply_modifications();
    }
};
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::on_frame_d3d12()` - DirectX 12 frame processing
- `Framework::on_post_present_d3d11()` - Post-present processing
- `Framework::run_imgui_frame()` - UI rendering
- `Framework::is_ready()` - Readiness check

---

## ⚠️ **IMPORTANT NOTES**
- Called every frame during rendering
- Performance critical - must be optimized
- Should not block the rendering thread
- Must handle exceptions gracefully
- VR timing requirements are strict

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement D3D11 frame processing:

1. **Hook Integration**: Install D3D11 Present hook
2. **Performance Focus**: Minimize frame processing time
3. **Error Handling**: Graceful error recovery
4. **VR Timing**: Respect VR frame timing requirements

```cpp
class MyD3D11Framework {
public:
    void on_frame_d3d11() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            // Quick validity check
            if (!is_ready()) return;
            
            // VR frame processing
            if (vr_enabled) {
                process_vr_frame();
            }
            
            // Execute frame mods
            for (auto& mod : active_mods) {
                mod->on_frame();
            }
            
            // UI rendering
            render_ui_overlay();
            
        } catch (const std::exception& e) {
            spdlog::error("Frame processing error: {}", e.what());
        }
        
        // Performance monitoring
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = end_time - start_time;
        
        if (duration > std::chrono::milliseconds(1)) {
            spdlog::warn("Frame processing took {}ms", 
                std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
        }
    }
    
private:
    void process_vr_frame() {
        // VR-specific processing
        update_head_tracking();
        setup_stereo_rendering();
        render_controller_models();
    }
};
```

---

## 🛡️ **PERFORMANCE CONSIDERATIONS**
- Keep processing under 1ms for 90 FPS VR
- Use frame-based processing for heavy operations
- Cache frequently used data
- Minimize memory allocations
- Use efficient algorithms for matrix operations
- Profile regularly to identify bottlenecks
