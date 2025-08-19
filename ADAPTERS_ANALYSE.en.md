# 🔌 UEVR Adapters Files Analysis
## Complete analysis of all adapter files

**Generated:** 2025-08-19 12:20:13  
**Number of files:** 8  
**Status:** ✅ Complete

---

## 📁 redengine_adapter.cpp
**Path:** $filePath  
**Size:** 19044 bytes  
**Last modified:** 2025-08-19 03:26:29

### 📥 Includes
```cpp
#include <windows.h>
```

### 🏗️ Classes
```cpp
class REDengine4Adapter : public Plugin {
```

### ⚙️ Adapter Functions
```cpp
REDengine4Adapter() : m_state(std::make_unique<REDengine4State>())
```

### ⚙️ General Functions
```cpp
void on_initialize()
```

### 📄 Complete Content
```cpp
/*
 * REDengine 4 Cross-Engine VR Adapter for uevr
 * Extends uevr's plugin architecture for REDengine 4 compatibility
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 * Based on uevr Plugin System (MIT License)
 */

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <MinHook.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <DirectXMath.h>

// uevr Plugin API includes - corrected paths
#include <uevr/Plugin.hpp>
#include <uevr/API.hpp>
#include "redengine_adapter.hpp"
#include "uobject_hook.hpp"

using namespace DirectX;

namespace uevr {
namespace redengine4 {

class REDengine4Adapter : public Plugin {
private:
    // REDengine 4 specific state
    struct REDengine4State {
        ID3D12Device* device = nullptr;
        IDXGISwapChain3* swap_chain = nullptr;
        ID3D12CommandQueue* command_queue = nullptr;
        ID3D12GraphicsCommandList* command_list = nullptr;
        
        // Camera matrices from REDengine 4
        struct CameraMatrices {
            XMMATRIX view_matrix;
            XMMATRIX projection_matrix;
            XMMATRIX view_projection_matrix;
            bool matrices_valid = false;
        } camera;
        
        // VR rendering state
        bool vr_enabled = false;
        bool stereo_rendering_active = false;
        
        // REDengine 4 specific rendering state
        UINT display_width = 0;
        UINT display_height = 0;
        UINT render_width = 0;
        UINT render_height = 0;
        
        // VR camera parameters
        float world_scale = 100.0f;
        float ipd = 0.064f;
        float near_clip = 0.1f;
        float far_clip = 2000.0f;
    };
    
    std::unique_ptr<REDengine4State> m_state;
    std::unique_ptr<UObjectHookManager> m_uobject_hook_manager;
    
    // Hook function pointers
    typedef HRESULT(WINAPI* Present1_t)(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags, const DXGI_PRESENT_PARAMETERS* pPresentParameters);
    typedef HRESULT(WINAPI* ResizeBuffers_t)(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    typedef HRESULT(WINAPI* ExecuteCommandLists_t)(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);
    
    Present1_t m_original_present1 = nullptr;
    ResizeBuffers_t m_original_resize_buffers = nullptr;
    ExecuteCommandLists_t m_original_execute_command_lists = nullptr;
    
    // REDengine 4 constant buffer and descriptor table analysis
    struct DescriptorTableInfo {
        UINT root_parameter_index;
        UINT descriptor_range_start;
        UINT descriptor_range_count;
        D3D12_DESCRIPTOR_RANGE_TYPE range_type;
        bool valid = false;
    };
    
    struct ConstantBufferInfo {
        UINT root_parameter_index;
        UINT descriptor_index;
        UINT size;
        UINT view_matrix_offset;
        UINT projection_matrix_offset;
        bool valid = false;
    };
    
    std::vector<DescriptorTableInfo> m_descriptor_tables;
    std::vector<ConstantBufferInfo> m_cb_info;
    
    // REDengine 4 root signature analysis
    struct RootSignatureInfo {
        ID3D12RootSignature* root_signature = nullptr;
        std::vector<D3D12_ROOT_PARAMETER> parameters;
        bool valid = false;
    };
    
    std::unordered_map<ID3D12RootSignature*, RootSignatureInfo> m_root_signatures;

public:
    REDengine4Adapter() : m_state(std::make_unique<REDengine4State>()) {
        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            // Log error
            return;
        }
        
        // Initialize UObjectHook manager
        m_uobject_hook_manager = std::make_unique<UObjectHookManager>();
        
        // Initialize default VR parameters
        m_state->world_scale = 100.0f;
        m_state->ipd = 0.064f;
        m_state->near_clip = 0.1f;
        m_state->far_clip = 2000.0f;
    }
    
    virtual ~REDengine4Adapter() {
        if (m_state) {
            unhook_all();
        }
        MH_Uninitialize();
    }

    // uevr Plugin callbacks
    virtual void on_initialize() override {
        // Initialize REDengine 4 hooks
        hook_redengine4();
        
        // Initialize UObjectHook system
        if (m_uobject_hook_manager) {
            m_uobject_hook_manager->hook_redengine4_object_system();
            m_uobject_hook_manager->scan_redengine4_objects();
        }
        
        // Enable VR mode
        m_state->vr_enabled = true;
    }
    
    virtual void on_device_reset() override {
        // Handle device reset for REDengine 4
        if (m_state && m_state->device) {
            // Re-establish hooks if needed
            hook_redengine4();
        }
    }
    
    virtual void on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv) override {
        // Post-process REDengine 4 rendering for VR
        if (m_state && m_state->vr_enabled) {
            post_process_redengine4_rendering(command_list, rt, rtv);
        }
    }
    
    // Main VR integration methods
    void enable_vr_mode() {
        m_state->vr_enabled = true;
        m_state->stereo_rendering_active = true;
    }
    
    void disable_vr_mode() {
        m_state->vr_enabled = false;
        m_state->stereo_rendering_active = false;
    }
    
    // UObjectHook integration methods
    bool attach_weapon_to_right_controller(const std::string& weapon_name) {
        if (m_uobject_hook_manager) {
            return m_uobject_hook_manager->attach_object(weapon_name, AttachmentType::WEAPON_RIGHT);
        }
        return false;
    }
    
    bool attach_utility_to_left_controller(const std::string& utility_name) {
        if (m_uobject_hook_manager) {
            return m_uobject_hook_manager->attach_object(utility_name, AttachmentType::UTILITY_LEFT);
        }
        return false;
    }
    
    bool adjust_attachment(const std::string& object_name, const DirectX::XMFLOAT3& location, const DirectX::XMFLOAT4& rotation) {
        if (m_uobject_hook_manager) {
            return m_uobject_hook_manager->adjust_attachment(object_name, location, rotation);
        }
        return false;
    }
    
    bool make_attachment_permanent(const std::string& object_name) {
        if (m_uobject_hook_manager) {
            return m_uobject_hook_manager->make_permanent(object_name);
        }
        return false;
    }
    
    void save_uobject_hook_state() {
        if (m_uobject_hook_manager) {
            m_uobject_hook_manager->save_state();
        }
    }
    
    std::vector<std::string> get_discovered_objects() {
        if (m_uobject_hook_manager) {
            return m_uobject_hook_manager->discover_objects();
        }
        return {};
    }
    
    // Camera matrix extraction and VR transformation
    bool extract_camera_matrices_from_redengine4() {
        if (!m_state->command_list) {
            return false;
        }
        
        // Analyze command list for camera matrices
        // This is where we'd implement the actual matrix extraction based on RenderDoc analysis
        
        // For now, create default VR matrices
        create_default_vr_matrices();
        return true;
    }
    
    void create_default_vr_matrices() {
        // Create default VR view and projection matrices
        XMVECTOR eye_position = XMVectorSet(0.0f, 1.7f, 0.0f, 1.0f);
        XMVECTOR focus_position = XMVectorSet(0.0f, 1.7f, -1.0f, 1.0f);
        XMVECTOR up_direction = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        
        m_state->camera.view_matrix = XMMatrixLookAtLH(eye_position, focus_position, up_direction);
        
        // Create VR projection matrix
        float fov_y = XMConvertToRadians(85.0f);
        float aspect_ratio = static_cast<float>(m_state->display_width) / static_cast<float>(m_state->display_height);
        
        m_state->camera.projection_matrix = XMMatrixPerspectiveFovLH(fov_y, aspect_ratio, m_state->near_clip, m_state->far_clip);
        
        // Combine matrices
        m_state->camera.view_projection_matrix = XMMatrixMultiply(m_state->camera.view_matrix, m_state->camera.projection_matrix);
        
        m_state->camera.matrices_valid = true;
    }
    
    // VR stereo rendering implementation
    void apply_stereo_rendering_to_redengine4(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv) {
        if (!m_state->camera.matrices_valid) {
            return;
        }
        
        // Apply left eye rendering
        XMMATRIX left_eye_projection = create_stereo_projection_matrix(-m_state->ipd * 0.5f);
        apply_eye_rendering(command_list, rt, rtv, left_eye_projection, 0);
        
        // Apply right eye rendering
        XMMATRIX right_eye_projection = create_stereo_projection_matrix(m_state->ipd * 0.5f);
        apply_eye_rendering(command_list, rt, rtv, right_eye_projection, 1);
    }
    
    XMMATRIX create_stereo_projection_matrix(float eye_offset) {
        // Create stereo projection matrix with eye offset
        float fov_y = XMConvertToRadians(85.0f);
        float aspect_ratio = static_cast<float>(m_state->display_width) / static_cast<float>(m_state->display_height);
        
        XMMATRIX projection = XMMatrixPerspectiveFovLH(fov_y, aspect_ratio, m_state->near_clip, m_state->far_clip);
        
        // Apply eye offset
        XMMATRIX offset = XMMatrixTranslation(eye_offset, 0.0f, 0.0f);
        
        return XMMatrixMultiply(offset, projection);
    }
    
    void apply_eye_rendering(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv, const XMMATRIX& eye_projection, int eye_index) {
        // Set viewport for this eye
        D3D12_VIEWPORT viewport;
        viewport.TopLeftX = static_cast<float>(eye_index * m_state->display_width / 2);
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(m_state->display_width / 2);
        viewport.Height = static_cast<float>(m_state->display_height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        
        command_list->RSSetViewports(1, &viewport);
        
        // Set scissor rect for this eye
        D3D12_RECT scissor_rect;
        scissor_rect.left = eye_index * m_state->display_width / 2;
        scissor_rect.top = 0;
        scissor_rect.right = (eye_index + 1) * m_state->display_width / 2;
        scissor_rect.bottom = m_state->display_height;
        
        command_list->RSSetScissorRects(1, &scissor_rect);
        
        // Apply eye-specific projection matrix
        // This would involve updating constant buffers with the eye projection matrix
        update_constant_buffer_with_projection(eye_projection);
    }
    
    void update_constant_buffer_with_projection(const XMMATRIX& projection_matrix) {
        // Update REDengine 4 constant buffers with VR projection matrix
        // This is where we'd implement the actual constant buffer update
        
        // For now, store the matrix for later use
        m_state->camera.projection_matrix = projection_matrix;
    }

private:
    bool hook_redengine4() {
        // Hook IDXGISwapChain3::Present1 (REDengine 4 uses Present1)
        if (m_state->swap_chain) {
            void** vtable = *reinterpret_cast<void***>(m_state->swap_chain);
            
            if (MH_CreateHook(vtable[22], &hooked_present1, reinterpret_cast<void**>(&m_original_present1)) != MH_OK) {
                return false;
            }
            
            if (MH_CreateHook(vtable[13], &hooked_resize_buffers, reinterpret_cast<void**>(&m_original_resize_buffers)) != MH_OK) {
                return false;
            }
            
            if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
                return false;
            }
        }
        
        // Hook ID3D12CommandQueue::ExecuteCommandLists for command list analysis
        if (m_state->command_queue) {
            void** vtable = *reinterpret_cast<void***>(m_state->command_queue);
            
            if (MH_CreateHook(vtable[10], &hooked_execute_command_lists, reinterpret_cast<void**>(&m_original_execute_command_lists)) != MH_OK) {
                return false;
            }
            
            if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
                return false;
            }
        }
        
        return true;
    }
    
    void unhook_all() {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);
    }
    
    // Hook implementations
    static HRESULT WINAPI hooked_present1(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags, const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
        auto* adapter = static_cast<REDengine4Adapter*>(uevr::detail::g_plugin);
        if (adapter) {
            adapter->on_redengine4_present1(pSwapChain, SyncInterval, Flags, pPresentParameters);
        }
        
        if (adapter && adapter->m_original_present1) {
            return adapter->m_original_present1(pSwapChain, SyncInterval, Flags, pPresentParameters);
        }
        
        return S_OK;
    }
    
    static HRESULT WINAPI hooked_resize_buffers(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        auto* adapter = static_cast<REDengine4Adapter*>(uevr::detail::g_plugin);
        if (adapter) {
            adapter->on_redengine4_resize_buffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
        }
        
        if (adapter && adapter->m_original_resize_buffers) {
            return adapter->m_original_resize_buffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
        }
        
        return S_OK;
    }
    
    static HRESULT WINAPI hooked_execute_command_lists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists) {
        auto* adapter = static_cast<REDengine4Adapter*>(uevr::detail::g_plugin);
        if (adapter) {
            adapter->on_redengine4_execute_command_lists(pCommandQueue, NumCommandLists, ppCommandLists);
        }
        
        if (adapter && adapter->m_original_execute_command_lists) {
            return adapter->m_original_execute_command_lists(pCommandQueue, NumCommandLists, ppCommandLists);
        }
        
        return S_OK;
    }
    
    void on_redengine4_present1(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags, const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
        // Extract camera matrices from REDengine 4 constant buffers
        if (m_state && m_state->command_list) {
            extract_camera_matrices_from_redengine4();
        }
        
        // Enable VR rendering if not already active
        if (!m_state->vr_enabled) {
            m_state->vr_enabled = true;
        }
        
        // Apply VR post-processing
        if (m_state->vr_enabled && m_state->stereo_rendering_active) {
            // This would be called by uevr's post-render callback
        }
    }
    
    void on_redengine4_resize_buffers(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        // Handle resolution changes for VR
        if (m_state && m_state->vr_enabled) {
            // Adjust VR rendering parameters
            adjust_vr_rendering_parameters(Width, Height);
        }
    }
    
    void on_redengine4_execute_command_lists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists) {
        // Analyze command lists for camera matrix information
        for (UINT i = 0; i < NumCommandLists; ++i) {
            analyze_command_list(ppCommandLists[i]);
        }
    }
    
    void analyze_command_list(ID3D12CommandList* command_list) {
        // Analyze D3D12 command list for root signature and descriptor table usage
        // This is where we'd implement the command list analysis logic based on RenderDoc analysis
        
        // For now, placeholder implementation
        // TODO: Implement based on CP2077 RenderDoc captures
    }
    
    void extract_camera_matrices() {
        // Analyze constant buffers and descriptor tables to find view and projection matrices
        // This is where we'd implement the matrix extraction logic based on RenderDoc analysis
        
        // For now, placeholder implementation
        // TODO: Implement based on CP2077 RenderDoc captures
        m_state->camera.matrices_valid = false;
    }
    
    void post_process_redengine4_rendering(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv) {
        // Apply VR-specific post-processing to REDengine 4 rendering
        if (m_state && m_state->camera.matrices_valid) {
            // Apply stereo rendering transformations
            apply_stereo_rendering_to_redengine4(command_list, rt, rtv);
        }
    }
    
    void apply_stereo_rendering(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv) {
        // Implement stereo rendering for REDengine 4
        // This would use uevr's stereo rendering framework
        // TODO: Implement stereo rendering pipeline
    }
    
    void adjust_vr_rendering_parameters(UINT width, UINT height) {
        // Adjust VR rendering parameters based on resolution changes
        m_state->display_width = width;
        m_state->display_height = height;
        
        // Calculate VR render resolution
        m_state->render_width = width;
        m_state->render_height = height;
        
        // TODO: Implement VR parameter adjustment
    }
};

// Plugin instance
static std::unique_ptr<REDengine4Adapter> g_redengine4_adapter;

} // namespace redengine4
} // namespace uevr

// Plugin entry points
extern "C" __declspec(dllexport) void uevr_plugin_required_version(uevr_PluginVersion* version) {
    version->major = uevr_PLUGIN_VERSION_MAJOR;
    version->minor = uevr_PLUGIN_VERSION_MINOR;
    version->patch = uevr_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool uevr_plugin_initialize(const uevr_PluginInitializeParam* param) {
    // Initialize uevr API
    auto& api = uevr::API::initialize(param);
    
    // Create REDengine 4 adapter instance
    uevr::redengine4::g_redengine4_adapter = std::make_unique<uevr::redengine4::REDengine4Adapter>();
    
    // Initialize the adapter
    uevr::redengine4::g_redengine4_adapter->on_initialize();
    
    return true;
}

extern "C" __declspec(dllexport) void uevr_plugin_shutdown() {
    uevr::redengine4::g_redengine4_adapter.reset();
}

// Main plugin entry point for uevr
extern "C" __declspec(dllexport) bool uevr_plugin_main() {
    if (uevr::redengine4::g_redengine4_adapter) {
        // Enable VR mode
        uevr::redengine4::g_redengine4_adapter->enable_vr_mode();
        return true;
    }
    return false;
}

```

---

## 📁 redengine_adapter.hpp
**Path:** $filePath  
**Size:** 5684 bytes  
**Last modified:** 2025-08-19 03:26:28

### 📥 Includes
```cpp
#include <windows.h>
```

### 🏗️ Classes
```cpp
class REDengine4Adapter {
```

### ⚙️ Adapter Functions
```cpp
REDengine4Adapter()
```

### ⚙️ General Functions
```cpp
void enable_vr_mode()
```

### 📄 Complete Content
```cpp
/*
 * REDengine 4 Cross-Engine VR Adapter Header for uevr
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 * Based on uevr Plugin System (MIT License)
 */

#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

// Forward declarations
namespace uevr {
    class Plugin;
}

namespace uevr {
namespace redengine4 {

class REDengine4Adapter {
public:
    REDengine4Adapter();
    virtual ~REDengine4Adapter();
    
    // Main VR integration methods
    void enable_vr_mode();
    void disable_vr_mode();
    
    // Camera matrix extraction and VR transformation
    bool extract_camera_matrices_from_redengine4();
    void create_default_vr_matrices();
    
    // VR stereo rendering implementation
    void apply_stereo_rendering_to_redengine4(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv);
    XMMATRIX create_stereo_projection_matrix(float eye_offset);
    void apply_eye_rendering(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv, const XMMATRIX& eye_projection, int eye_index);
    
    // Hook management
    bool hook_redengine4();
    void unhook_all();
    
    // Hook callbacks
    void on_redengine4_present1(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags, const DXGI_PRESENT_PARAMETERS* pPresentParameters);
    void on_redengine4_resize_buffers(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    void on_redengine4_execute_command_lists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);
    
    // Internal processing
    void analyze_command_list(ID3D12CommandList* command_list);
    void extract_camera_matrices();
    void post_process_redengine4_rendering(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv);
    void apply_stereo_rendering(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv);
    void adjust_vr_rendering_parameters(UINT width, UINT height);
    void update_constant_buffer_with_projection(const XMMATRIX& projection_matrix);

private:
    // REDengine 4 specific state
    struct REDengine4State {
        ID3D12Device* device = nullptr;
        IDXGISwapChain3* swap_chain = nullptr;
        ID3D12CommandQueue* command_queue = nullptr;
        ID3D12GraphicsCommandList* command_list = nullptr;
        
        // Camera matrices from REDengine 4
        struct CameraMatrices {
            XMMATRIX view_matrix;
            XMMATRIX projection_matrix;
            XMMATRIX view_projection_matrix;
            bool matrices_valid = false;
        } camera;
        
        // VR rendering state
        bool vr_enabled = false;
        bool stereo_rendering_active = false;
        
        // REDengine 4 specific rendering state
        UINT display_width = 0;
        UINT display_height = 0;
        UINT render_width = 0;
        UINT render_height = 0;
        
        // VR camera parameters
        float world_scale = 100.0f;
        float ipd = 0.064f;
        float near_clip = 0.1f;
        float far_clip = 2000.0f;
    };
    
    std::unique_ptr<REDengine4State> m_state;
    
    // Hook function pointers
    typedef HRESULT(WINAPI* Present1_t)(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags, const DXGI_PRESENT_PARAMETERS* pPresentParameters);
    typedef HRESULT(WINAPI* ResizeBuffers_t)(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    typedef HRESULT(WINAPI* ExecuteCommandLists_t)(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);
    
    Present1_t m_original_present1 = nullptr;
    ResizeBuffers_t m_original_resize_buffers = nullptr;
    ExecuteCommandLists_t m_original_execute_command_lists = nullptr;
    
    // REDengine 4 constant buffer and descriptor table analysis
    struct DescriptorTableInfo {
        UINT root_parameter_index;
        UINT descriptor_range_start;
        UINT descriptor_range_count;
        D3D12_DESCRIPTOR_RANGE_TYPE range_type;
        bool valid = false;
    };
    
    struct ConstantBufferInfo {
        UINT root_parameter_index;
        UINT descriptor_index;
        UINT size;
        UINT view_matrix_offset;
        UINT projection_matrix_offset;
        bool valid = false;
    };
    
    std::vector<DescriptorTableInfo> m_descriptor_tables;
    std::vector<ConstantBufferInfo> m_cb_info;
    
    // REDengine 4 root signature analysis
    struct RootSignatureInfo {
        ID3D12RootSignature* root_signature = nullptr;
        std::vector<D3D12_ROOT_PARAMETER> parameters;
        bool valid = false;
    };
    
    std::unordered_map<ID3D12RootSignature*, RootSignatureInfo> m_root_signatures;
    
    // Static hook implementations
    static HRESULT WINAPI hooked_present1(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags, const DXGI_PRESENT_PARAMETERS* pPresentParameters);
    static HRESULT WINAPI hooked_resize_buffers(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    static HRESULT WINAPI hooked_execute_command_lists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);
};

} // namespace redengine4
} // namespace uevr

```

---

## 📁 redengine4_adapter.cpp
**Path:** $filePath  
**Size:** 9671 bytes  
**Last modified:** 2025-08-19 03:26:30

### 📥 Includes
```cpp
#include <windows.h>
```

### 🏗️ Classes
```cpp

```

### ⚙️ Adapter Functions
```cpp
if (g_adapter && g_adapter->is_initialized())
```

### ⚙️ General Functions
```cpp
void REDengine4Adapter::on_initialize()
```

### 📄 Complete Content
```cpp
/*
 * REDengine 4 Cross-Engine VR Adapter for uevr
 * 
 * Extends uevr's plugin architecture to support CD Projekt Red's REDengine 4
 * Target: Cyberpunk 2077 (Cyberpunk2077.exe) and other REDengine 4 titles
 * 
 * Based on uevr Plugin API: include/uevr/Plugin.hpp
 * Integration: Cyber Engine Tweaks + D3D12 hooks
 */

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <MinHook.h>
#include <spdlog/spdlog.h>

#include "redengine4_adapter.hpp"

// Global adapter instance
static REDengine4Adapter* g_adapter = nullptr;

// D3D12 hook function pointers
static ID3D12CommandQueue::ExecuteCommandLists_t g_original_execute_command_lists = nullptr;
static IDXGISwapChain3::Present1_t g_original_present1 = nullptr;

// Hook implementations
void WINAPI Hook_ExecuteCommandLists(ID3D12CommandQueue* command_queue, UINT num_command_lists, 
                                     ID3D12CommandList* const* pp_command_lists) {
    if (g_adapter && g_adapter->is_initialized()) {
        g_adapter->on_command_queue_execute(command_queue, num_command_lists, pp_command_lists);
    }
    g_original_execute_command_lists(command_queue, num_command_lists, pp_command_lists);
}

HRESULT WINAPI Hook_Present1(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags, 
                             const DXGI_PRESENT_PARAMETERS* p_present_parameters) {
    if (g_adapter && g_adapter->is_initialized()) {
        g_adapter->on_swapchain_present1(swap_chain);
    }
    return g_original_present1(swap_chain, sync_interval, flags, p_present_parameters);
}

// REDengine 4 adapter implementation
REDengine4Adapter::REDengine4Adapter() 
    : m_initialized(false)
    , m_swap_chain(nullptr)
    , m_device(nullptr)
    , m_command_queue(nullptr)
    , m_render_target(nullptr)
    , m_depth_stencil(nullptr)
{
    spdlog::info("[REDengine 4 Adapter] Initializing...");
}

REDengine4Adapter::~REDengine4Adapter() {
    cleanup();
}

**Status**: Translation in progress - Header file and second implementation file complete  
**Next**: Continue with remaining sections and files
