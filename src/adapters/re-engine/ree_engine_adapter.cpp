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
#include "SOURCECODE/UEVR/include/uevr/API.h"

// Global adapter instance
static REEngineAdapter* g_adapter = nullptr;

// D3D11 hook function pointers
typedef HRESULT (WINAPI *Present_t)(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
typedef HRESULT (WINAPI *ResizeBuffers_t)(IDXGISwapChain* This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

static Present_t g_original_present = nullptr;
static ResizeBuffers_t g_original_resize_buffers = nullptr;

// Hook implementations
HRESULT WINAPI Hook_Present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    if (g_adapter && g_adapter->is_initialized()) {
        // The header defines on_post_render_vr_framework_dx11, not on_swapchain_present
        // This function needs to be updated to match the header's signature
        // For now, we'll call a placeholder or adjust the hook
        // g_adapter->on_swapchain_present(swap_chain); // This was the old call
        // The correct call would involve passing device context, RT, DSV which are not available here directly
        // This hook needs to be re-evaluated based on how uevr expects the present hook to work.
        // For now, we'll just log and call original.
        spdlog::warn("[RE Engine Adapter] Hook_Present called, but on_post_render_vr_framework_dx11 needs device context, RT, DSV.");
    }
    return g_original_present(swap_chain, sync_interval, flags);
}

HRESULT WINAPI Hook_ResizeBuffers(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height,
                                   UINT new_format, UINT swap_chain_flags) {
    if (g_adapter && g_adapter->is_initialized()) {
        // The header defines on_swapchain_resize(UINT width, UINT height), not with swap_chain
        g_adapter->on_swapchain_resize(width, height);
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
    , m_display_width(0)
    , m_display_height(0)
    , m_vr_enabled(false)
{
    // Initialize all stereo rendering resources to nullptr
    for (int i = 0; i < 2; ++i) {
        m_stereo_render_targets[i] = nullptr;
        m_stereo_rtvs[i] = nullptr;
        m_stereo_depth_stencils[i] = nullptr;
        m_stereo_dsvs[i] = nullptr;
    }

    // Initialize camera matrices and VR config
    memset(&m_camera_matrices, 0, sizeof(m_camera_matrices));
    m_camera_matrices.valid = false;
    m_vr_config = {1.0f, 0.063f, 0.1f, 1000.0f}; // Default values

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

void REEngineAdapter::on_post_render_vr_framework_dx11(ID3D11DeviceContext* device_context,
                                                     ID3D11RenderTargetView* rt,
                                                     ID3D11DepthStencilView* dsv) {
    if (!m_initialized) return;

    m_device_context = device_context;
    m_render_target = rt;
    m_depth_stencil = dsv;

    // Handle frame presentation
    render_stereo_frame();
}

void REEngineAdapter::on_device_reset() {
    if (!m_initialized) return;

    spdlog::info("[RE Engine Adapter] Device reset detected");
    cleanup_resources();
}

bool REEngineAdapter::setup_d3d11_hooks() {
    // This part requires obtaining the IDXGISwapChain instance.
    // A common way is to create a dummy device and swapchain.
    // For a real hook, this would typically be done by finding an existing swapchain.
    // This is a placeholder for the actual hook logic.
    spdlog::info("[RE Engine Adapter] Attempting to setup D3D11 hooks...");

    // Dummy device creation to get a vtable for hooking
    ID3D11Device* dummy_device = nullptr;
    ID3D11DeviceContext* dummy_context = nullptr;
    IDXGISwapChain* dummy_swap_chain = nullptr;

    D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 1;
    sd.BufferDesc.Height = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow(); // Use current foreground window
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
        &sd, &dummy_swap_chain, &dummy_device, nullptr, &dummy_context
    );

    if (FAILED(hr)) {
        spdlog::error("[RE Engine Adapter] Failed to create dummy D3D11 device and swapchain for hooking: {:x}", hr);
        return false;
    }

    // Get vtable pointers
    void** swap_chain_vtable = *reinterpret_cast<void***>(dummy_swap_chain);

    // Hook IDXGISwapChain::Present
    if (MH_CreateHook(swap_chain_vtable[8], &Hook_Present,
                       reinterpret_cast<LPVOID*>(&g_original_present)) != MH_OK) {
        spdlog::error("[RE Engine Adapter] Failed to hook Present");
        return false;
    }

    // Hook IDXGISwapChain::ResizeBuffers
    if (MH_CreateHook(swap_chain_vtable[13], &Hook_ResizeBuffers,
                       reinterpret_cast<LPVOID*>(&g_original_resize_buffers)) != MH_OK) {
        spdlog::error("[RE Engine Adapter] Failed to hook ResizeBuffers");
        return false;
    }

    // Enable hooks
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        spdlog::error("[RE Engine Adapter] Failed to enable hooks");
        return false;
    }

    // Release dummy resources
    if (dummy_context) dummy_context->Release();
    if (dummy_device) dummy_device->Release();
    if (dummy_swap_chain) dummy_swap_chain->Release();

    spdlog::info("[RE Engine Adapter] D3D11 hooks setup attempted.");
    return true;
}

void REEngineAdapter::on_swapchain_resize(UINT width, UINT height) {
    spdlog::info("[RE Engine Adapter] Swapchain resized to {}x{}", width, height);
    m_display_width = width;
    m_display_height = height;
    cleanup_resources(); // Cleanup old resources
    create_resources(width, height); // Create new resources
}

void REEngineAdapter::extract_camera_matrices() {
    // This is a complex task specific to RE Engine's memory layout.
    // It would involve scanning memory for known matrix patterns or offsets.
    // For now, this is a placeholder.
    spdlog::warn("[RE Engine Adapter] extract_camera_matrices: Not implemented yet.");
    m_camera_matrices.valid = false; // Assume invalid until implemented
}

bool REEngineAdapter::analyze_buffer_for_camera_matrices(float* buffer_data, UINT buffer_size) {
    // Placeholder for analyzing a buffer for camera matrices
    spdlog::warn("[RE Engine Adapter] analyze_buffer_for_camera_matrices: Not implemented yet.");
    return false;
}

bool REEngineAdapter::is_valid_view_matrix(float* matrix_data) {
    // Placeholder for validating a view matrix
    spdlog::warn("[RE Engine Adapter] is_valid_view_matrix: Not implemented yet.");
    return true; // Assume valid for now
}

bool REEngineAdapter::is_valid_projection_matrix(float* matrix_data) {
    // Placeholder for validating a projection matrix
    spdlog::warn("[RE Engine Adapter] is_valid_projection_matrix: Not implemented yet.");
    return true; // Assume valid for now
}

void REEngineAdapter::apply_vr_camera_transformations() {
    // Placeholder for applying VR camera transformations
    spdlog::warn("[RE Engine Adapter] apply_vr_camera_transformations: Not implemented yet.");
}

void REEngineAdapter::render_stereo_frame() {
    if (!m_device_context || !m_render_target || !m_depth_stencil) {
        spdlog::error("[RE Engine Adapter] Cannot render stereo frame: D3D11 resources not set.");
        return;
    }

    spdlog::info("[RE Engine Adapter] Rendering stereo frame...");

    // Ensure stereo resources exist
    if (!m_stereo_render_targets[0] || !m_stereo_render_targets[1]) {
        create_stereo_resources(m_display_width / 2, m_display_height); // Assuming side-by-side for now
    }

    // 1. Extract camera matrices from the game
    extract_camera_matrices();
    if (!m_camera_matrices.valid) {
        spdlog::warn("[RE Engine Adapter] Camera matrices not valid, skipping VR rendering.");
        return;
    }

    // 2. Render left eye
    render_eye(0);

    // 3. Render right eye
    render_eye(1);

    // 4. Composite stereo frame back to the original swapchain render target
    composite_stereo_frame();

    spdlog::info("[RE Engine Adapter] Stereo frame rendered.");
}

void REEngineAdapter::render_eye(int eye_index) {
    if (!m_device_context || !m_stereo_rtvs[eye_index] || !m_stereo_dsvs[eye_index]) {
        spdlog::error("[RE Engine Adapter] Cannot render eye {}: Stereo resources not set.", eye_index);
        return;
    }

    spdlog::info("[RE Engine Adapter] Rendering eye {}...", eye_index);

    // Set render target and depth stencil for this eye
    m_device_context->OMSetRenderTargets(1, &m_stereo_rtvs[eye_index], m_stereo_dsvs[eye_index]);

    // Clear the eye's render target and depth buffer
    float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
    m_device_context->ClearRenderTargetView(m_stereo_rtvs[eye_index], clear_color);
    m_device_context->ClearDepthStencilView(m_stereo_dsvs[eye_index], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Apply VR camera transformations for this eye
    apply_vr_camera_transformations(); // This would use m_camera_matrices.left_view/right_view

    // Here, you would typically trigger the game's rendering pipeline
    // This is highly engine-specific and usually involves detouring game functions
    // For a "complex full" solution, this would involve deep hooks into the RE Engine's rendering loop.
    // For now, this is a conceptual placeholder.
    spdlog::warn("[RE Engine Adapter] Game rendering for eye {} not implemented yet.", eye_index);
}

void REEngineAdapter::composite_stereo_frame() {
    if (!m_device_context || !m_render_target) {
        spdlog::error("[RE Engine Adapter] Cannot composite stereo frame: Main render target not set.");
        return;
    }

    spdlog::info("[RE Engine Adapter] Compositing stereo frame...");

    // Set the original back buffer as the render target
    m_device_context->OMSetRenderTargets(1, &m_render_target, m_depth_stencil);

    // Copy the rendered eye textures to the main back buffer
    // This would typically involve a fullscreen quad pass with a shader
    // For simplicity, we'll just copy the left eye to the left half and right eye to the right half
    // This assumes a side-by-side stereo output format.

    D3D11_BOX dest_box_left = { 0, 0, 0, m_display_width / 2, m_display_height, 1 };
    D3D11_BOX dest_box_right = { m_display_width / 2, 0, 0, m_display_width, m_display_height, 1 };

    if (m_stereo_render_targets[0] && m_stereo_render_targets[1]) {
        m_device_context->CopySubresourceRegion(m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), nullptr), 0, 0, 0, 0, m_stereo_render_targets[0], 0, &dest_box_left);
        m_device_context->CopySubresourceRegion(m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), nullptr), 0, m_display_width / 2, 0, 0, m_stereo_render_targets[1], 0, &dest_box_right);
    } else {
        spdlog::error("[RE Engine Adapter] Stereo render targets not available for compositing.");
    }
}

void REEngineAdapter::create_resources(UINT width, UINT height) {
    spdlog::info("[RE Engine Adapter] Creating resources for {}x{}", width, height);
    // This function is called on resize.
    // It should create/recreate resources dependent on swapchain size.
    // For now, it will primarily manage stereo resources.
    create_stereo_resources(width / 2, height); // Assuming half-width for each eye
}

void REEngineAdapter::create_stereo_resources(UINT width, UINT height) {
    spdlog::info("[RE Engine Adapter] Creating stereo resources for {}x{}", width, height);
    cleanup_stereo_resources(); // Ensure old resources are cleaned up

    if (!m_device) {
        spdlog::error("[RE Engine Adapter] Cannot create stereo resources: D3D11 Device is null.");
        return;
    }

    D3D11_TEXTURE2D_DESC texture_desc = {};
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Or match swapchain format
    texture_desc.SampleDesc.Count = 1;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
    rtv_desc.Format = texture_desc.Format;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    D3D11_TEXTURE2D_DESC depth_stencil_desc = {};
    depth_stencil_desc.Width = width;
    depth_stencil_desc.Height = height;
    depth_stencil_desc.MipLevels = 1;
    depth_stencil_desc.ArraySize = 1;
    depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth/Stencil format
    depth_stencil_desc.SampleDesc.Count = 1;
    depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_desc.CPUAccessFlags = 0;
    depth_stencil_desc.MiscFlags = 0;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Format = depth_stencil_desc.Format;
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Flags = 0;

    for (int i = 0; i < 2; ++i) {
        HRESULT hr_tex = m_device->CreateTexture2D(&texture_desc, nullptr, &m_stereo_render_targets[i]);
        HRESULT hr_rtv = m_device->CreateRenderTargetView(m_stereo_render_targets[i], &rtv_desc, &m_stereo_rtvs[i]);
        HRESULT hr_ds_tex = m_device->CreateTexture2D(&depth_stencil_desc, nullptr, &m_stereo_depth_stencils[i]);
        HRESULT hr_dsv = m_device->CreateDepthStencilView(m_stereo_depth_stencils[i], &dsv_desc, &m_stereo_dsvs[i]);

        if (FAILED(hr_tex) || FAILED(hr_rtv) || FAILED(hr_ds_tex) || FAILED(hr_dsv)) {
            spdlog::error("[RE Engine Adapter] Failed to create stereo resources for eye {}: Tex={:x}, RTV={:x}, DSTex={:x}, DSV={:x}",
                          i, hr_tex, hr_rtv, hr_ds_tex, hr_dsv);
            cleanup_stereo_resources(); // Clean up partially created resources
            return;
        }
    }
    spdlog::info("[RE Engine Adapter] Stereo resources created successfully.");
}

void REEngineAdapter::cleanup_resources() {
    spdlog::info("[RE Engine Adapter] Cleaning up resources...");
    cleanup_stereo_resources();
    // Release other D3D11 resources if they were managed by the adapter
    // For now, m_swap_chain, m_device, m_device_context, m_render_target, m_depth_stencil
    // are assumed to be owned externally and not released here.
}

void REEngineAdapter::cleanup_stereo_resources() {
    spdlog::info("[RE Engine Adapter] Cleaning up stereo resources...");
    for (int i = 0; i < 2; ++i) {
        if (m_stereo_rtvs[i]) { m_stereo_rtvs[i]->Release(); m_stereo_rtvs[i] = nullptr; }
        if (m_stereo_render_targets[i]) { m_stereo_render_targets[i]->Release(); m_stereo_render_targets[i] = nullptr; }
        if (m_stereo_dsvs[i]) { m_stereo_dsvs[i]->Release(); m_stereo_dsvs[i] = nullptr; }
        if (m_stereo_depth_stencils[i]) { m_stereo_depth_stencils[i]->Release(); m_stereo_depth_stencils[i] = nullptr; }
    }
}

void REEngineAdapter::cleanup() {
    spdlog::info("[RE Engine Adapter] Performing full cleanup...");
    cleanup_resources();
    if (MH_Uninitialize() != MH_OK) {
        spdlog::error("[RE Engine Adapter] Failed to uninitialize MinHook");
    }
    // Do not release m_swap_chain, m_device, m_device_context, m_render_target, m_depth_stencil here
    // as they are likely owned by the game/UEVR framework.
}

// Plugin entry points
extern "C" {
    __declspec(dllexport) void uevr_plugin_required_version(UEVR_PluginVersion* version) {
        version->major = UEVR_PLUGIN_VERSION_MAJOR;
        version->minor = UEVR_PLUGIN_VERSION_MINOR;
        version->patch = UEVR_PLUGIN_VERSION_PATCH;
    }

    __declspec(dllexport) bool uevr_plugin_initialize(const UEVR_PluginInitializeParam* param) {
        if (!g_adapter) {
            g_adapter = new REEngineAdapter();
        }
        
        g_adapter->on_initialize();
        return true;
    }
}
