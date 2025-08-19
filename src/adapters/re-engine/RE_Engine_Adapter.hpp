/*
 * RE Engine Cross-Engine VR Adapter Header
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 * Based on uevr Plugin System (MIT License)
 */

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <memory>
#include <vector>

// Forward declarations
namespace uevr {
    class Plugin;
    class API;
}

namespace uevr {
namespace re_engine {

// RE Engine camera matrices structure
struct RE_Engine_Matrix_Data {
    float view[16];
    float projection[16];
    float viewproj[16];
    bool matrices_valid;
    
    RE_Engine_Matrix_Data() : matrices_valid(false) {
        // Initialize matrices to identity
        for (int i = 0; i < 16; ++i) {
            view[i] = (i % 5 == 0) ? 1.0f : 0.0f;
            projection[i] = (i % 5 == 0) ? 1.0f : 0.0f;
            viewproj[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        }
    }
};

// RE Engine constant buffer information
struct RE_Engine_CB_Info {
    UINT slot;
    UINT size;
    UINT view_matrix_offset;
    UINT projection_matrix_offset;
    bool valid;
    
    RE_Engine_CB_Info() : slot(0), size(0), view_matrix_offset(0), projection_matrix_offset(0), valid(false) {}
};

// RE Engine VR state
struct RE_Engine_VR_State {
    bool vr_enabled;
    bool stereo_rendering_active;
    bool horror_mode_enabled;
    bool audio_spatialization_active;
    
    RE_Engine_VR_State() : vr_enabled(false), stereo_rendering_active(false), horror_mode_enabled(false), audio_spatialization_active(false) {}
};

// Main RE Engine adapter class
class REEngineAdapter {
public:
    REEngineAdapter();
    virtual ~REEngineAdapter();
    
    // Initialization and cleanup
    bool initialize();
    void shutdown();
    
    // Hook management
    bool install_hooks();
    void uninstall_hooks();
    
    // Matrix extraction
    bool extract_camera_matrices();
    const RE_Engine_Matrix_Data& get_matrix_data() const;
    
    // VR state management
    bool is_vr_enabled() const;
    void set_vr_enabled(bool enabled);
    
    // Horror game VR optimization
    bool enable_horror_mode();
    bool setup_audio_spatialization();
    
    // Motion controller support
    bool setup_motion_controllers();
    
    // UI projection
    bool setup_ui_projection();

private:
    // DirectX objects
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    IDXGISwapChain* m_swapchain;
    
    // State data
    RE_Engine_Matrix_Data m_matrix_data;
    std::vector<RE_Engine_CB_Info> m_cb_info;
    RE_Engine_VR_State m_vr_state;
    
    // Hook state
    bool m_hooks_installed;
    bool m_engine_detected;
    
    // Hook function pointers
    using PresentFunc = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
    using ResizeBuffersFunc = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    
    PresentFunc m_original_present;
    ResizeBuffersFunc m_original_resize_buffers;
    
    // Internal methods
    bool detect_re_engine();
    bool check_re_engine_signatures();
    bool resolve_swapchain_methods(void** out_present, void** out_resize_buffers);
    void process_frame();
    void setup_camera_extraction();
    
    
    // Hook implementations
    static HRESULT __stdcall present_hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    static HRESULT __stdcall resize_buffers_hook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
};

} // namespace re_engine
} // namespace uevr
