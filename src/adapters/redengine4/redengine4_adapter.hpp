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
    void extract_camera_matrices_from_commands(UINT num_command_lists, 
                                             ID3D12CommandList* const* pp_command_lists);
    void render_stereo_frame();
    void render_stereo_frame_dx12(ID3D12GraphicsCommandList* command_list, 
                                 ID3D12Resource* rt, 
                                 D3D12_CPU_DESCRIPTOR_HANDLE* rtv);
    
    // Resource management
    void create_resources(UINT width, UINT height);
    void cleanup_resources();
    void cleanup();
    
private:
    // State
    bool m_initialized;
    
    // D3D12 objects
    IDXGISwapChain3* m_swap_chain;
    ID3D12Device* m_device;
    ID3D12CommandQueue* m_command_queue;
    ID3D12Resource* m_render_target;
    ID3D12Resource* m_depth_stencil;
    
    // Camera matrices (to be extracted from REDengine 4)
    struct {
        float view[16];
        float projection[16];
        bool valid;
    } m_camera_matrices;
};

// Plugin entry points
extern "C" {
    __declspec(dllexport) void uevr_plugin_required_version(uevr_PluginVersion* version);
    __declspec(dllexport) bool uevr_plugin_initialize(const uevr_PluginInitializeParam* param);
}
