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

// Forward declarations
struct uevr_PluginVersion;
struct uevr_PluginInitializeParam;

// RE Engine adapter class
class REEngineAdapter {
public:
    REEngineAdapter();
    ~REEngineAdapter();
    
    // uevr plugin interface methods
    void on_initialize();
    void on_present();
    void on_device_reset();
    
    // Accessors
    bool is_initialized() const { return m_initialized; }
    
private:
    // D3D11 hook setup
    bool setup_d3d11_hooks();
    
    // SwapChain event handlers
    void on_swapchain_present(IDXGISwapChain* swap_chain);
    void on_swapchain_resize(IDXGISwapChain* swap_chain, UINT width, UINT height);
    
    // Core VR functionality
    void extract_camera_matrices();
    void render_stereo_frame();
    
    // Resource management
    void create_resources(UINT width, UINT height);
    void cleanup_resources();
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
    
    // Camera matrices (to be extracted from RE Engine)
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
