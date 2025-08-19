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

void REDengine4Adapter::on_initialize() {
    spdlog::info("[REDengine 4 Adapter] on_initialize called");
    
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        spdlog::error("[REDengine 4 Adapter] Failed to initialize MinHook");
        return;
    }
    
    // Setup D3D12 hooks
    if (!setup_d3d12_hooks()) {
        spdlog::error("[REDengine 4 Adapter] Failed to setup D3D12 hooks");
        return;
    }
    
    m_initialized = true;
    spdlog::info("[REDengine 4 Adapter] Initialization complete");
}

void REDengine4Adapter::on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList* command_list, 
                                                         ID3D12Resource* rt, 
                                                         D3D12_CPU_DESCRIPTOR_HANDLE* rtv) {
    if (!m_initialized) return;
    
    // Handle D3D12 VR framework rendering
    render_stereo_frame_dx12(command_list, rt, rtv);
}

void REDengine4Adapter::on_device_reset() {
    if (!m_initialized) return;
    
    spdlog::info("[REDengine 4 Adapter] Device reset detected");
    cleanup_resources();
}

bool REDengine4Adapter::setup_d3d12_hooks() {
    // Hook ID3D12CommandQueue::ExecuteCommandLists
    if (MH_CreateHook(&ID3D12CommandQueue::ExecuteCommandLists, &Hook_ExecuteCommandLists, 
                       reinterpret_cast<LPVOID*>(&g_original_execute_command_lists)) != MH_OK) {
        spdlog::error("[REDengine 4 Adapter] Failed to hook ExecuteCommandLists");
        return false;
    }
    
    // Hook IDXGISwapChain3::Present1
    if (MH_CreateHook(&IDXGISwapChain3::Present1, &Hook_Present1,
                       reinterpret_cast<LPVOID*>(&g_original_present1)) != MH_OK) {
        spdlog::error("[REDengine 4 Adapter] Failed to hook Present1");
        return false;
    }
    
    // Enable all hooks
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        spdlog::error("[REDengine 4 Adapter] Failed to enable hooks");
        return false;
    }
    
    spdlog::info("[REDengine 4 Adapter] D3D12 hooks installed successfully");
    return true;
}

void REDengine4Adapter::on_command_queue_execute(ID3D12CommandQueue* command_queue, 
                                                UINT num_command_lists, 
                                                ID3D12CommandList* const* pp_command_lists) {
    if (!command_queue) return;
    
    // Store command queue reference
    m_command_queue = command_queue;
    
    // Get device from command queue
    if (!m_device) {
        if (FAILED(command_queue->GetDevice(__uuidof(ID3D12Device), reinterpret_cast<void**>(&m_device)))) {
            return;
        }
    }
    
    // Extract camera matrices from command lists
    extract_camera_matrices_from_commands(num_command_lists, pp_command_lists);
}

void REDengine4Adapter::on_swapchain_present1(IDXGISwapChain3* swap_chain) {
    if (!swap_chain) return;
    
    // Store swap chain reference
    m_swap_chain = swap_chain;
    
    // Get device from swap chain
    if (!m_device) {
        if (FAILED(swap_chain->GetDevice(__uuidof(ID3D12Device), reinterpret_cast<void**>(&m_device)))) {
            return;
        }
    }
    
    // Render stereo frame
    render_stereo_frame();
}

void REDengine4Adapter::extract_camera_matrices_from_commands(UINT num_command_lists, 
                                                             ID3D12CommandList* const* pp_command_lists) {
    if (!m_device) return;
    
    // TODO: Implement camera matrix extraction from REDengine 4 command lists
    // This requires RenderDoc analysis to identify the correct root signatures and descriptor tables
    
    // For now, log that we're attempting extraction
    static int frame_count = 0;
    if (++frame_count % 60 == 0) { // Log every 60 frames
        spdlog::debug("[REDengine 4 Adapter] Extracting camera matrices from commands (frame {})", frame_count);
    }
}

void REDengine4Adapter::render_stereo_frame() {
    if (!m_device) return;
    
    // TODO: Implement stereo rendering pipeline for D3D12
    // This will involve:
    // 1. Creating left/right eye viewports
    // 2. Setting up stereo projection matrices
    // 3. Using instanced rendering for stereo
    
    static int frame_count = 0;
    if (++frame_count % 60 == 0) { // Log every 60 frames
        spdlog::debug("[REDengine 4 Adapter] Rendering stereo frame (frame {})", frame_count);
    }
}

void REDengine4Adapter::render_stereo_frame_dx12(ID3D12GraphicsCommandList* command_list, 
                                                ID3D12Resource* rt, 
                                                D3D12_CPU_DESCRIPTOR_HANDLE* rtv) {
    if (!command_list || !rt) return;
    
    // TODO: Implement D3D12 stereo rendering using uevr's framework
    // This will integrate with uevr's existing D3D12 VR rendering pipeline
    
    static int frame_count = 0;
    if (++frame_count % 60 == 0) { // Log every 60 frames
        spdlog::debug("[REDengine 4 Adapter] D3D12 stereo frame (frame {})", frame_count);
    }
}

void REDengine4Adapter::create_resources(UINT width, UINT height) {
    if (!m_device || !m_swap_chain) return;
    
    // TODO: Implement D3D12 resource creation for stereo rendering
    // This will involve:
    // 1. Creating render target textures for left/right eyes
    // 2. Setting up descriptor heaps for stereo resources
    // 3. Creating depth stencil resources
    
    spdlog::info("[REDengine 4 Adapter] Resources creation for {}x{} (D3D12)", width, height);
}

void REDengine4Adapter::cleanup_resources() {
    // TODO: Implement D3D12 resource cleanup
    // Release all D3D12 resources and reset pointers
    
    spdlog::info("[REDengine 4 Adapter] Resources cleaned up");
}

void REDengine4Adapter::cleanup() {
    spdlog::info("[REDengine 4 Adapter] Cleaning up...");
    
    // Disable hooks
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    
    // Clean up resources
    cleanup_resources();
    
    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }
    
    m_command_queue = nullptr;
    m_swap_chain = nullptr;
    m_initialized = false;
    
    spdlog::info("[REDengine 4 Adapter] Cleanup complete");
}

// Plugin entry point
extern "C" __declspec(dllexport) void uevr_plugin_required_version(uevr_PluginVersion* version) {
    version->major = uevr_PLUGIN_VERSION_MAJOR;
    version->minor = uevr_PLUGIN_VERSION_MINOR;
    version->patch = uevr_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool uevr_plugin_initialize(const uevr_PluginInitializeParam* param) {
    spdlog::info("[REDengine 4 Adapter] Plugin initialization requested");
    
    // Create adapter instance
    g_adapter = new REDengine4Adapter();
    
    // Initialize uevr API
    auto& api = uevr::API::initialize(param);
    
    // Initialize adapter
    g_adapter->on_initialize();
    
    spdlog::info("[REDengine 4 Adapter] Plugin initialization complete");
    return true;
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            spdlog::info("[REDengine 4 Adapter] DLL loaded");
            break;
            
        case DLL_PROCESS_DETACH:
            if (g_adapter) {
                delete g_adapter;
                g_adapter = nullptr;
            }
            spdlog::info("[REDengine 4 Adapter] DLL unloaded");
            break;
    }
    
    return TRUE;
}
