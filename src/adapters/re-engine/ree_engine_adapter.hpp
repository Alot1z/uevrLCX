/*
 * RE Engine Cross-Engine VR Adapter for uevr
 * 
 * Header file defining the RE Engine adapter interface
 * Extends uevr's plugin architecture for Capcom's RE Engine
 */

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <MinHook.h>
#include <spdlog/spdlog.h>
#include <memory>

// Forward declarations
// NOTE: We kept the original UEVR naming convention (uppercase with underscores)
// instead of changing to small letters because this matches the official UEVR API
// and maintains compatibility with existing plugin interfaces
struct UEVR_PluginVersion;
struct UEVR_PluginInitializeParam;

// RE Engine adapter class
class REEngineAdapter {
public:
    REEngineAdapter();
    ~REEngineAdapter();
    
    // uevr plugin interface methods
    void on_initialize();
    void on_post_render_vr_framework_dx11(ID3D11DeviceContext* device_context, 
                                         ID3D11RenderTargetView* rt, 
                                         ID3D11DepthStencilView* dsv);
    void on_device_reset();
    
    // Accessors
    bool is_initialized() const { return m_initialized; }
    
private:
    // D3D11 hook setup
    bool setup_d3d11_hooks();
    
    // SwapChain event handlers
    void on_swapchain_resize(UINT width, UINT height);
    
    // Core VR functionality
    void extract_camera_matrices();
    bool analyze_buffer_for_camera_matrices(float* buffer_data, UINT buffer_size);
    bool is_valid_view_matrix(float* matrix_data);
    bool is_valid_projection_matrix(float* matrix_data);
    void apply_vr_camera_transformations();
    void render_stereo_frame();
    void render_eye(int eye_index);
    void composite_stereo_frame();
    
    // Resource management
    void create_resources(UINT width, UINT height);
    void create_stereo_resources(UINT width, UINT height);
    void cleanup_resources();
    void cleanup_stereo_resources();
    void cleanup();
    
private:
    // State
    bool m_initialized;
    
    // D3D11 objects
    IDXGISwapChain* m_swap_chain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_device_context;
    ID3D11RenderTargetView* m_render_target;
    ID3D11DepthStencilView* m_depth_stencil;
    
    // Stereo rendering resources
    ID3D11Texture2D* m_stereo_render_targets[2];  // Left and right eye
    ID3D11RenderTargetView* m_stereo_rtvs[2];    // Left and right eye RTVs
    ID3D11Texture2D* m_stereo_depth_stencils[2]; // Left and right eye depth
    ID3D11DepthStencilView* m_stereo_dsvs[2];    // Left and right eye DSVs
    
    // Display dimensions
    UINT m_display_width;
    UINT m_display_height;
    
    // Camera matrices (extracted from RE Engine)
    struct CameraMatrices {
        float view[16];
        float projection[16];
        float left_view[16];
        float right_view[16];
        bool valid;
    } m_camera_matrices;
    
    // VR configuration
    struct VRConfig {
        float world_scale;
        float ipd;
        float near_clip;
        float far_clip;
    } m_vr_config;
    
    // VR state
    bool m_vr_enabled;
    
    // Performance monitoring
    struct PerformanceMonitor {
        double avg_frame_time;
        float vr_latency;
        float gpu_usage;
        float cpu_usage;
    } m_performance_monitor;
    
    // Frame tracking
    uint64_t m_frame_count;
    double m_last_frame_time;
};

// Plugin entry points
extern "C" {
    __declspec(dllexport) void uevr_plugin_required_version(UEVR_PluginVersion* version);
    __declspec(dllexport) bool uevr_plugin_initialize(const UEVR_PluginInitializeParam* param);
}
