/*
 * REDengine 4 Cross-Engine VR Adapter for uevr
 * 
 * Header file defining the REDengine 4 adapter interface
 * Extends uevr's plugin architecture for CD Projekt Red's REDengine 4
 */

#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <chrono>
#include <vector>
#include <memory>

// Forward declarations
struct uevr_PluginVersion;
struct uevr_PluginInitializeParam;

// REDengine 4 adapter class
class REDengine4Adapter {
public:
    REDengine4Adapter();
    ~REDengine4Adapter();
    
    // uevr plugin interface methods
    void on_initialize();
    void on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList* command_list, 
                                         ID3D12Resource* rt, 
                                         D3D12_CPU_DESCRIPTOR_HANDLE* rtv);
    void on_device_reset();
    
    // Accessors
    bool is_initialized() const { return m_initialized; }
    
private:
    // D3D12 hook setup
    bool setup_d3d12_hooks();
    
    // Command queue event handlers
    void on_command_queue_execute(ID3D12CommandQueue* command_queue, 
                                 UINT num_command_lists, 
                                 ID3D12CommandList* const* pp_command_lists);
    
    // SwapChain event handlers
    void on_swapchain_present1(IDXGISwapChain3* swap_chain);
    
    // Core VR functionality
    void extract_camera_matrices_from_command_lists(ID3D12CommandQueue* command_queue,
                                                   UINT num_command_lists, 
                                                   ID3D12CommandList* const* pp_command_lists);
    void apply_vr_camera_transformations();
    void render_stereo_frame_dx12(ID3D12GraphicsCommandList* command_list, 
                                 ID3D12Resource* rt, 
                                 D3D12_CPU_DESCRIPTOR_HANDLE* rtv);
    void render_eye(ID3D12GraphicsCommandList* command_list, int eye_index);
    void composite_stereo_frame(ID3D12GraphicsCommandList* command_list,
                               ID3D12Resource* main_rt,
                               D3D12_CPU_DESCRIPTOR_HANDLE* main_rtv);
    
    // Resource management
    void create_stereo_resources(UINT width, UINT height);
    void cleanup_stereo_resources();
    void cleanup_resources();
    void cleanup();
    
    // VR mode management
    void enable_vr_mode();
    
    // Performance monitoring
    void update_performance_metrics();
    void log_performance_metrics();
    
private:
    // State
    bool m_initialized;
    
    // D3D12 objects
    IDXGISwapChain3* m_swap_chain;
    ID3D12Device* m_device;
    ID3D12CommandQueue* m_command_queue;
    ID3D12Resource* m_render_target;
    ID3D12Resource* m_depth_stencil;
    
    // Stereo rendering resources
    ID3D12Resource* m_stereo_render_targets[2];  // Left and right eye
    ID3D12Resource* m_stereo_depth_stencils[2];  // Left and right eye depth
    D3D12_CPU_DESCRIPTOR_HANDLE m_stereo_rtv_handles[2];
    D3D12_CPU_DESCRIPTOR_HANDLE m_stereo_dsv_handle;
    
    // Display dimensions
    UINT m_display_width;
    UINT m_display_height;
    
    // Camera matrices (extracted from REDengine 4)
    struct CameraMatrices {
        DirectX::XMMATRIX view_matrix;
        DirectX::XMMATRIX projection_matrix;
        DirectX::XMMATRIX view_projection_matrix;
        DirectX::XMMATRIX left_view_matrix;
        DirectX::XMMATRIX right_view_matrix;
        bool matrices_valid;
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
        std::vector<double> frame_times;
        double avg_frame_time;
        float vr_latency;
        float gpu_usage;
        float cpu_usage;
    } m_performance_monitor;
    
    // Frame tracking
    uint64_t m_frame_count;
    std::chrono::high_resolution_clock::time_point m_last_frame_time;
};

// Plugin entry points
extern "C" {
    __declspec(dllexport) void uevr_plugin_required_version(uevr_PluginVersion* version);
    __declspec(dllexport) bool uevr_plugin_initialize(const uevr_PluginInitializeParam* param);
}
