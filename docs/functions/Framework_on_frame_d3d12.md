# Framework on_frame_d3d12() Function Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::on_frame_d3d12()`  
**File**: `src/Framework.cpp`  
**Purpose**: Handle DirectX 12 frame rendering for VR integration  
**Category**: DirectX 12 Frame Processing  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
void on_frame_d3d12()
```

### **Parameters**
- None

### **Return Value**
- `void`

---

## 🔧 **IMPLEMENTATION DETAILS**

### **D3D12 Frame Processing Pipeline**
1. **Command List Management**: Handle D3D12 command lists
2. **Resource Barriers**: Manage resource state transitions
3. **VR Render Target Setup**: Configure VR-specific render targets
4. **Multi-GPU Coordination**: Handle SLI/CrossFire setups
5. **Descriptor Heap Management**: Manage D3D12 descriptor heaps
6. **Synchronization**: Handle D3D12 fence synchronization

### **Key D3D12 Specific Operations**

#### **Command List Processing**
```cpp
void Framework::on_frame_d3d12() {
    if (!is_ready() || !is_dx12()) {
        return;
    }
    
    // D3D12 command list preparation
    auto command_list = get_current_command_list();
    if (!command_list) return;
    
    // VR rendering commands
    if (vr_mod && vr_mod->is_enabled()) {
        vr_mod->on_frame_d3d12(command_list);
    }
    
    // Execute D3D12 mods
    for (auto& mod : m_mods) {
        if (mod->supports_d3d12() && mod->is_enabled()) {
            mod->on_frame_d3d12(command_list);
        }
    }
}
```

#### **Resource Management**
- **Descriptor Heaps**: Manage CBV/SRV/UAV descriptor heaps
- **Resource Barriers**: Handle resource state transitions
- **Memory Management**: Efficient GPU memory allocation
- **Multi-Frame Resources**: Handle multiple frames in flight

#### **VR-Specific D3D12 Features**
- **Multi-Res Shading**: Variable rate shading for VR
- **GPU Scheduling**: Advanced GPU work scheduling
- **Memory Bandwidth**: Optimized memory access patterns
- **Async Compute**: Parallel compute and graphics queues

---

## 📚 **USAGE EXAMPLES**

### **D3D12 Hook Integration**
```cpp
// Called from D3D12 Present hook
HRESULT WINAPI D3D12Present_Hook(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags) {
    if (g_framework && g_framework->is_valid()) {
        g_framework->on_frame_d3d12();
    }
    
    return original_present(swap_chain, sync_interval, flags);
}
```

### **VR D3D12 Mod Implementation**
```cpp
class VRD3D12Mod : public Mod {
public:
    void on_frame_d3d12(ID3D12GraphicsCommandList* command_list) override {
        if (!is_vr_enabled()) return;
        
        // D3D12 VR-specific operations
        setup_vr_render_targets(command_list);
        configure_stereo_projection(command_list);
        render_vr_overlay(command_list);
    }
    
private:
    void setup_vr_render_targets(ID3D12GraphicsCommandList* cmd_list) {
        // Configure VR render targets
        D3D12_RESOURCE_BARRIER barriers[] = {
            CD3DX12_RESOURCE_BARRIER::Transition(
                left_eye_target.Get(),
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET
            ),
            CD3DX12_RESOURCE_BARRIER::Transition(
                right_eye_target.Get(),
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET
            )
        };
        
        cmd_list->ResourceBarrier(2, barriers);
    }
};
```

### **Multi-GPU D3D12 Support**
```cpp
class MultiGPUD3D12Mod : public Mod {
public:
    void on_frame_d3d12(ID3D12GraphicsCommandList* command_list) override {
        // Multi-GPU rendering
        if (is_multi_gpu_enabled()) {
            distribute_work_across_gpus(command_list);
        }
        
        // Standard processing
        process_frame_d3d12(command_list);
    }
    
private:
    void distribute_work_across_gpus(ID3D12GraphicsCommandList* cmd_list) {
        // GPU 0: Left eye rendering
        // GPU 1: Right eye rendering
        // Synchronize results
    }
};
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::on_frame_d3d11()` - DirectX 11 frame processing
- `Framework::on_post_present_d3d12()` - Post-present D3D12 processing
- `D3D12Component::on_frame()` - D3D12 VR component processing
- `Framework::is_dx12()` - D3D12 detection

---

## ⚠️ **IMPORTANT NOTES**
- More complex than D3D11 due to explicit resource management
- Requires careful command list and resource state management
- Performance critical for VR applications
- Must handle multiple frames in flight
- Synchronization is crucial for stability

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement D3D12 frame processing:

1. **Command List Management**: Properly handle D3D12 command lists
2. **Resource State Tracking**: Track resource states carefully
3. **Descriptor Management**: Efficiently manage descriptor heaps
4. **Synchronization**: Implement proper fence synchronization

```cpp
class MyD3D12Framework {
private:
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12CommandQueue> command_queue;
    ComPtr<ID3D12GraphicsCommandList> command_list;
    ComPtr<ID3D12Fence> fence;
    UINT64 fence_value = 0;
    
public:
    void on_frame_d3d12() {
        try {
            if (!is_ready() || !device) return;
            
            // Reset command list
            command_list->Reset(command_allocator.Get(), nullptr);
            
            // VR frame processing
            if (vr_enabled) {
                process_vr_frame_d3d12();
            }
            
            // Execute mods
            for (auto& mod : d3d12_mods) {
                mod->on_frame_d3d12(command_list.Get());
            }
            
            // Close and execute command list
            command_list->Close();
            ID3D12CommandList* cmd_lists[] = { command_list.Get() };
            command_queue->ExecuteCommandLists(1, cmd_lists);
            
            // Signal fence
            command_queue->Signal(fence.Get(), ++fence_value);
            
        } catch (const std::exception& e) {
            spdlog::error("D3D12 frame processing error: {}", e.what());
        }
    }
    
private:
    void process_vr_frame_d3d12() {
        // VR-specific D3D12 processing
        setup_vr_descriptor_heaps();
        configure_vr_render_targets();
        apply_vr_transformations();
    }
    
    void setup_vr_descriptor_heaps() {
        // Configure descriptor heaps for VR rendering
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
        heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heap_desc.NumDescriptors = VR_DESCRIPTOR_COUNT;
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        
        device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&vr_descriptor_heap));
    }
};
```

---

## 🛡️ **D3D12 SPECIFIC CONSIDERATIONS**
- **Explicit Resource Management**: Must manually manage all resources
- **Command List Lifecycle**: Proper reset/close/execute cycle
- **Multi-Threading**: D3D12 supports multi-threaded command list recording
- **Memory Management**: Efficient GPU memory allocation strategies
- **Debugging**: Use D3D12 debug layer for development
- **Performance**: Leverage D3D12's low-level control for optimization
