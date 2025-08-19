/*
 * RE Engine Cross-Engine VR Adapter for uevr
 * 
 * Extends uevr's plugin architecture to support Capcom's RE Engine
 * Target: Resident Evil 7 (re7.exe) and other RE Engine titles
 * 
 * Based on uevr Plugin API: include/uevr/Plugin.hpp
 * Integration: REFramework + D3D11 hooks
 */

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <MinHook.h>
#include <spdlog/spdlog.h>

#include "ree_engine_adapter.hpp"

// Global adapter instance
static REEngineAdapter* g_adapter = nullptr;

// D3D11 hook function pointers
static IDXGISwapChain::Present_t g_original_present = nullptr;
static IDXGISwapChain::ResizeBuffers_t g_original_resize_buffers = nullptr;

// Hook implementations
HRESULT WINAPI Hook_Present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    if (g_adapter && g_adapter->is_initialized()) {
        g_adapter->on_swapchain_present(swap_chain);
    }
    return g_original_present(swap_chain, sync_interval, flags);
}

HRESULT WINAPI Hook_ResizeBuffers(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, 
                                   UINT new_format, UINT swap_chain_flags) {
    if (g_adapter && g_adapter->is_initialized()) {
        g_adapter->on_swapchain_resize(swap_chain, width, height);
    }
    return g_original_resize_buffers(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
}

// RE Engine adapter implementation
REEngineAdapter::REEngineAdapter() 
    : m_initialized(false)
    , m_swap_chain(nullptr)
    , m_device(nullptr)
    , m_device_context(nullptr)
    , m_render_target(nullptr)
    , m_depth_stencil(nullptr)
{
    spdlog::info("[RE Engine Adapter] Initializing...");
}

REEngineAdapter::~REEngineAdapter() {
    cleanup();
}

void REEngineAdapter::on_initialize() {
    spdlog::info("[RE Engine Adapter] on_initialize called");
    
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        spdlog::error("[RE Engine Adapter] Failed to initialize MinHook");
        return;
    }
    
    // Setup D3D11 hooks
    if (!setup_d3d11_hooks()) {
        spdlog::error("[RE Engine Adapter] Failed to setup D3D11 hooks");
        return;
    }
    
    m_initialized = true;
    spdlog::info("[RE Engine Adapter] Initialization complete");
}

void REEngineAdapter::on_present() {
    if (!m_initialized) return;
    
    // Handle frame presentation
    render_stereo_frame();
}

void REEngineAdapter::on_device_reset() {
    if (!m_initialized) return;
    
    spdlog::info("[RE Engine Adapter] Device reset detected");
    cleanup_resources();
}

bool REEngineAdapter::setup_d3d11_hooks() {
    // Hook IDXGISwapChain::Present
    if (MH_CreateHook(&IDXGISwapChain::Present, &Hook_Present, 
                       reinterpret_cast<LPVOID*>(&g_original_present)) != MH_OK) {
        spdlog::error("[RE Engine Adapter] Failed to hook Present");
        return false;
    }
    
    // Hook IDXGISwapChain::ResizeBuffers
    if (MH_CreateHook(&IDXGISwapChain::ResizeBuffers, &Hook_ResizeBuffers,
                       reinterpret_cast<LPVOID*>(&g_original_resize_buffers)) != MH_OK) {
        spdlog::error("[RE Engine Adapter] Failed to hook ResizeBuffers");
        return false;
    }
    
    // Enable all hooks
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        spdlog::error("[RE Engine Adapter] Failed to enable hooks");
        return false;
    }
    
    spdlog::info("[RE Engine Adapter] D3D11 hooks installed successfully");
    return true;
}

void REEngineAdapter::on_swapchain_present(IDXGISwapChain* swap_chain) {
    if (!swap_chain) return;
    
    // Store swap chain reference
    m_swap_chain = swap_chain;
    
    // Get device and device context
    if (!m_device || !m_device_context) {
        if (FAILED(swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_device)))) {
            return;
        }
        m_device->GetImmediateContext(&m_device_context);
    }
    
    // Extract camera matrices from constant buffers
    extract_camera_matrices();
    
    // Render stereo frame
    render_stereo_frame();
}

void REEngineAdapter::on_swapchain_resize(IDXGISwapChain* swap_chain, UINT width, UINT height) {
    if (!swap_chain) return;
    
    spdlog::info("[RE Engine Adapter] SwapChain resize: {}x{}", width, height);
    
    // Clean up old resources
    cleanup_resources();
    
    // Create new render target and depth stencil
    create_resources(width, height);
}

void REEngineAdapter::extract_camera_matrices() {
    if (!m_device_context) return;
    
    // TODO: Implement camera matrix extraction from RE Engine constant buffers
    // This requires RenderDoc analysis to identify the correct CB slots and offsets
    
    // For now, log that we're attempting extraction
    static int frame_count = 0;
    if (++frame_count % 60 == 0) { // Log every 60 frames
        spdlog::debug("[RE Engine Adapter] Extracting camera matrices (frame {})", frame_count);
    }
}

void REEngineAdapter::render_stereo_frame() {
    if (!m_device_context || !m_render_target) return;
    
    // TODO: Implement stereo rendering pipeline
    // This will involve:
    // 1. Creating left/right eye viewports
    // 2. Setting up stereo projection matrices
    // 3. Rendering the scene twice with different view transforms
    
    static int frame_count = 0;
    if (++frame_count % 60 == 0) { // Log every 60 frames
        spdlog::debug("[RE Engine Adapter] Rendering stereo frame (frame {})", frame_count);
    }
}

void REEngineAdapter::create_resources(UINT width, UINT height) {
    if (!m_device || !m_swap_chain) return;
    
    // Create render target view
    ID3D11Texture2D* back_buffer = nullptr;
    if (SUCCEEDED(m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer)))) {
        m_device->CreateRenderTargetView(back_buffer, nullptr, &m_render_target);
        back_buffer->Release();
    }
    
    // Create depth stencil view
    D3D11_TEXTURE2D_DESC depth_desc = {};
    depth_desc.Width = width;
    depth_desc.Height = height;
    depth_desc.MipLevels = 1;
    depth_desc.ArraySize = 1;
    depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_desc.SampleDesc.Count = 1;
    depth_desc.SampleDesc.Quality = 0;
    depth_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    ID3D11Texture2D* depth_texture = nullptr;
    if (SUCCEEDED(m_device->CreateTexture2D(&depth_desc, nullptr, &depth_texture))) {
        m_device->CreateDepthStencilView(depth_texture, nullptr, &m_depth_stencil);
        depth_texture->Release();
    }
    
    spdlog::info("[RE Engine Adapter] Resources created for {}x{}", width, height);
}

void REEngineAdapter::cleanup_resources() {
    if (m_render_target) {
        m_render_target->Release();
        m_render_target = nullptr;
    }
    
    if (m_depth_stencil) {
        m_depth_stencil->Release();
        m_depth_stencil = nullptr;
    }
}

void REEngineAdapter::cleanup() {
    spdlog::info("[RE Engine Adapter] Cleaning up...");
    
    // Disable hooks
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    
    // Clean up resources
    cleanup_resources();
    
    if (m_device_context) {
        m_device_context->Release();
        m_device_context = nullptr;
    }
    
    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }
    
    m_swap_chain = nullptr;
    m_initialized = false;
    
    spdlog::info("[RE Engine Adapter] Cleanup complete");
}

// Plugin entry point
extern "C" __declspec(dllexport) void uevr_plugin_required_version(uevr_PluginVersion* version) {
    version->major = uevr_PLUGIN_VERSION_MAJOR;
    version->minor = uevr_PLUGIN_VERSION_MINOR;
    version->patch = uevr_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool uevr_plugin_initialize(const uevr_PluginInitializeParam* param) {
    spdlog::info("[RE Engine Adapter] Plugin initialization requested");
    
    // Create adapter instance
    g_adapter = new REEngineAdapter();
    
    // Initialize uevr API
    auto& api = uevr::API::initialize(param);
    
    // Initialize adapter
    g_adapter->on_initialize();
    
    spdlog::info("[RE Engine Adapter] Plugin initialization complete");
    return true;
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            spdlog::info("[RE Engine Adapter] DLL loaded");
            break;
            
        case DLL_PROCESS_DETACH:
            if (g_adapter) {
                delete g_adapter;
                g_adapter = nullptr;
            }
            spdlog::info("[RE Engine Adapter] DLL unloaded");
            break;
    }
    
    return TRUE;
}
