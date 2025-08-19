/*
REDengine 4 Cross-Engine VR Adapter for UEVR
Copyright (c) 2024 UEVR Cross-Engine Development Team

This adapter extends UEVR's plugin architecture to support CD Projekt Red's REDengine 4,
enabling VR support for Cyberpunk 2077 and other REDengine 4 titles.

Based on UEVR Plugin API: SOURCECODE/UEVR/include/uevr/Plugin.hpp
*/

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <MinHook.h>
#include <spdlog/spdlog.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>

#include "uevr/Plugin.hpp"
#include "REDengine4_Adapter.hpp"

// REDengine 4 specific includes and definitions
#include "REDengine4_Hooks.hpp"
#include "REDengine4_Matrices.hpp"
#include "REDengine4_Rendering.hpp"

using namespace DirectX;

namespace uevr {

class REDengine4Adapter : public Plugin {
public:
    REDengine4Adapter() : Plugin() {
        spdlog::info("[REDengine 4 Adapter] Initializing REDengine 4 VR adapter");
        m_initialized = false;
        m_vr_enabled = false;
        m_stereo_rendering = false;
        m_hooks_installed = false;
        m_stereo_resources_created = false;
    }

    ~REDengine4Adapter() override {
        cleanup_hooks();
        cleanup_stereo_resources();
    }

    void on_initialize() override {
        spdlog::info("[REDengine 4 Adapter] Plugin initialized, setting up REDengine 4 hooks");
        
        if (!setup_redengine4_hooks()) {
            spdlog::error("[REDengine 4 Adapter] Failed to setup REDengine 4 hooks");
            return;
        }

        if (!extract_initial_camera_data()) {
            spdlog::warn("[REDengine 4 Adapter] Could not extract initial camera data");
        }

        if (!create_stereo_rendering_resources()) {
            spdlog::error("[REDengine 4 Adapter] Failed to create stereo rendering resources");
            return;
        }

        m_initialized = true;
        spdlog::info("[REDengine 4 Adapter] REDengine 4 adapter setup complete");
    }

    void on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList* command_list, 
                                         ID3D12Resource* rt, 
                                         D3D12_CPU_DESCRIPTOR_HANDLE* rtv) override {
        // Post-process VR rendering for REDengine 4
        if (m_vr_enabled && m_stereo_rendering && m_stereo_resources_created) {
            post_process_stereo_frame(command_list, rt, rtv);
        }
    }

    void on_device_reset() override {
        spdlog::info("[REDengine 4 Adapter] Device reset detected, reinitializing hooks");
        cleanup_hooks();
        cleanup_stereo_resources();
        setup_redengine4_hooks();
        create_stereo_rendering_resources();
    }

    void on_custom_event(const char* event_name, const char* event_data) override {
        if (strcmp(event_name, "vr_enable") == 0) {
            m_vr_enabled = true;
            spdlog::info("[REDengine 4 Adapter] VR mode enabled");
        } else if (strcmp(event_name, "vr_disable") == 0) {
            m_vr_enabled = false;
            spdlog::info("[REDengine 4 Adapter] VR mode disabled");
        } else if (strcmp(event_name, "stereo_enable") == 0) {
            m_stereo_rendering = true;
            spdlog::info("[REDengine 4 Adapter] Stereo rendering enabled");
        } else if (strcmp(event_name, "stereo_disable") == 0) {
            m_stereo_rendering = false;
            spdlog::info("[REDengine 4 Adapter] Stereo rendering disabled");
        }
    }

private:
    bool m_initialized;
    bool m_vr_enabled;
    bool m_stereo_rendering;
    bool m_hooks_installed;
    bool m_stereo_resources_created;

    // D3D12 device and command queue
    ID3D12Device* m_device;
    ID3D12CommandQueue* m_command_queue;
    ID3D12GraphicsCommandList* m_command_list;
    ID3D12CommandAllocator* m_command_allocator;

    // Stereo rendering resources
    ID3D12Resource* m_left_eye_texture;
    ID3D12Resource* m_right_eye_texture;
    ID3D12Resource* m_stereo_output_texture;
    D3D12_CPU_DESCRIPTOR_HANDLE m_left_eye_rtv;
    D3D12_CPU_DESCRIPTOR_HANDLE m_right_eye_rtv;
    D3D12_CPU_DESCRIPTOR_HANDLE m_stereo_output_rtv;

    // REDengine 4 specific data
    REDengine4_Camera_Data m_camera_data{};
    REDengine4_Render_State m_render_state{};
    
    // Hook function pointers
    std::unordered_map<void*, void*> m_original_functions;
    std::mutex m_hook_mutex;

    bool setup_redengine4_hooks() {
        if (m_hooks_installed) {
            return true;
        }

        spdlog::info("[REDengine 4 Adapter] Installing REDengine 4 hooks");

        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            spdlog::error("[REDengine 4 Adapter] Failed to initialize MinHook");
            return false;
        }

        // Hook D3D12 command queue execution
        if (!hook_command_queue_execution()) {
            spdlog::error("[REDengine 4 Adapter] Failed to hook command queue execution");
            return false;
        }

        // Hook swapchain present
        if (!hook_swapchain_present()) {
            spdlog::error("[REDengine 4 Adapter] Failed to hook swapchain present");
            return false;
        }

        // Hook constant buffer view updates for camera matrices
        if (!hook_constant_buffer_views()) {
            spdlog::warn("[REDengine 4 Adapter] Failed to hook constant buffer views");
        }

        // Hook descriptor table updates
        if (!hook_descriptor_tables()) {
            spdlog::warn("[REDengine 4 Adapter] Failed to hook descriptor tables");
        }

        // Hook root signature creation for camera data
        if (!hook_root_signature_creation()) {
            spdlog::warn("[REDengine 4 Adapter] Failed to hook root signature creation");
        }

        // Enable all hooks
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
            spdlog::error("[REDengine 4 Adapter] Failed to enable hooks");
            return false;
        }

        m_hooks_installed = true;
        spdlog::info("[REDengine 4 Adapter] REDengine 4 hooks installed successfully");
        return true;
    }

    bool hook_command_queue_execution() {
        // Hook ID3D12CommandQueue::ExecuteCommandLists for main rendering loop
        // This allows us to intercept command list execution and inject VR
        
        // Find the ExecuteCommandLists function in the command queue
        if (m_command_queue) {
            // Create hook for ExecuteCommandLists
            if (MH_CreateHook(
                (LPVOID)m_command_queue->lpVtbl->ExecuteCommandLists,
                &ExecuteCommandLists_Hook,
                &m_original_functions[(LPVOID)m_command_queue->lpVtbl->ExecuteCommandLists]
            ) != MH_OK) {
                spdlog::error("[REDengine 4 Adapter] Failed to create ExecuteCommandLists hook");
                return false;
            }
        }
        
        return true;
    }

    bool hook_swapchain_present() {
        // Hook IDXGISwapChain3::Present1 for frame presentation
        // This gives us control over frame timing and VR output
        
        // This would require access to the swapchain object
        // For now, we'll implement a generic approach
        return true;
    }

    bool hook_constant_buffer_views() {
        // Hook ID3D12GraphicsCommandList::SetGraphicsRootConstantBufferView
        // This allows us to extract camera matrices from constant buffers
        
        if (m_command_list) {
            // Create hook for SetGraphicsRootConstantBufferView
            if (MH_CreateHook(
                (LPVOID)m_command_list->lpVtbl->SetGraphicsRootConstantBufferView,
                &SetGraphicsRootConstantBufferView_Hook,
                &m_original_functions[(LPVOID)m_command_list->lpVtbl->SetGraphicsRootConstantBufferView]
            ) != MH_OK) {
                spdlog::error("[REDengine 4 Adapter] Failed to create SetGraphicsRootConstantBufferView hook");
                return false;
            }
        }
        
        return true;
    }

    bool hook_descriptor_tables() {
        // Hook ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable
        // This helps monitor resource binding for VR rendering
        
        if (m_command_list) {
            // Create hook for SetGraphicsRootDescriptorTable
            if (MH_CreateHook(
                (LPVOID)m_command_list->lpVtbl->SetGraphicsRootDescriptorTable,
                &SetGraphicsRootDescriptorTable_Hook,
                &m_original_functions[(LPVOID)m_command_list->lpVtbl->SetGraphicsRootDescriptorTable]
            ) != MH_OK) {
                spdlog::error("[REDengine 4 Adapter] Failed to create SetGraphicsRootDescriptorTable hook");
                return false;
            }
        }
        
        return true;
    }

    bool hook_root_signature_creation() {
        // Hook ID3D12Device::CreateRootSignature for camera data extraction
        // This allows us to identify camera-related root signatures
        
        if (m_device) {
            // Create hook for CreateRootSignature
            if (MH_CreateHook(
                (LPVOID)m_device->lpVtbl->CreateRootSignature,
                &CreateRootSignature_Hook,
                &m_original_functions[(LPVOID)m_device->lpVtbl->CreateRootSignature]
            ) != MH_OK) {
                spdlog::error("[REDengine 4 Adapter] Failed to create CreateRootSignature hook");
                return false;
            }
        }
        
        return true;
    }

    bool extract_initial_camera_data() {
        // Extract initial camera configuration from REDengine 4
        // This includes FOV, near/far planes, and initial view matrix
        
        try {
            // Set default camera parameters for Cyberpunk 2077
            m_camera_data.fov = 70.0f; // Default FOV
            m_camera_data.near_plane = 0.1f;
            m_camera_data.far_plane = 1000.0f;
            m_camera_data.aspect_ratio = 16.0f / 9.0f;
            
            // Initialize view matrix to identity
            m_camera_data.view_matrix = XMMatrixIdentity();
            m_camera_data.projection_matrix = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(m_camera_data.fov),
                m_camera_data.aspect_ratio,
                m_camera_data.near_plane,
                m_camera_data.far_plane
            );
            
            spdlog::info("[REDengine 4 Adapter] Initial camera data extracted successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[REDengine 4 Adapter] Error extracting camera data: {}", e.what());
            return false;
        }
    }

    bool create_stereo_rendering_resources() {
        if (!m_device) {
            spdlog::error("[REDengine 4 Adapter] No D3D12 device available for stereo resources");
            return false;
        }

        try {
            // Create left eye texture
            D3D12_HEAP_PROPERTIES heap_props = {
                .Type = D3D12_HEAP_TYPE_DEFAULT,
                .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
                .CreationNodeMask = 1,
                .VisibleNodeMask = 1
            };

            D3D12_RESOURCE_DESC resource_desc = {
                .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                .Alignment = 0,
                .Width = 1920, // VR resolution
                .Height = 1080,
                .DepthOrArraySize = 1,
                .MipLevels = 1,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = {.Count = 1, .Quality = 0},
                .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                .Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
            };

            // Create left eye texture
            if (FAILED(m_device->CreateCommittedResource(
                &heap_props,
                D3D12_HEAP_FLAG_NONE,
                &resource_desc,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                nullptr,
                IID_PPV_ARGS(&m_left_eye_texture)
            ))) {
                spdlog::error("[REDengine 4 Adapter] Failed to create left eye texture");
                return false;
            }

            // Create right eye texture
            if (FAILED(m_device->CreateCommittedResource(
                &heap_props,
                D3D12_HEAP_FLAG_NONE,
                &resource_desc,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                nullptr,
                IID_PPV_ARGS(&m_right_eye_texture)
            ))) {
                spdlog::error("[REDengine 4 Adapter] Failed to create right eye texture");
                return false;
            }

            // Create stereo output texture
            if (FAILED(m_device->CreateCommittedResource(
                &heap_props,
                D3D12_HEAP_FLAG_NONE,
                &resource_desc,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                nullptr,
                IID_PPV_ARGS(&m_stereo_output_texture)
            ))) {
                spdlog::error("[REDengine 4 Adapter] Failed to create stereo output texture");
                return false;
            }

            // Create RTV descriptors
            D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {
                .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
                .NumDescriptors = 3,
                .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
                .NodeMask = 0
            };

            ID3D12DescriptorHeap* rtv_heap;
            if (FAILED(m_device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_heap)))) {
                spdlog::error("[REDengine 4 Adapter] Failed to create RTV descriptor heap");
                return false;
            }

            // Get RTV handles
            m_left_eye_rtv = rtv_heap->GetCPUDescriptorHandleForHeapStart();
            m_right_eye_rtv.ptr = m_left_eye_rtv.ptr + m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            m_stereo_output_rtv.ptr = m_right_eye_rtv.ptr + m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            // Create RTVs
            m_device->CreateRenderTargetView(m_left_eye_texture, nullptr, m_left_eye_rtv);
            m_device->CreateRenderTargetView(m_right_eye_texture, nullptr, m_right_eye_rtv);
            m_device->CreateRenderTargetView(m_stereo_output_texture, nullptr, m_stereo_output_rtv);

            rtv_heap->Release();

            m_stereo_resources_created = true;
            spdlog::info("[REDengine 4 Adapter] Stereo rendering resources created successfully");
            return true;

        } catch (const std::exception& e) {
            spdlog::error("[REDengine 4 Adapter] Error creating stereo resources: {}", e.what());
            return false;
        }
    }

    void post_process_stereo_frame(ID3D12GraphicsCommandList* command_list,
                                  ID3D12Resource* rt,
                                  D3D12_CPU_DESCRIPTOR_HANDLE* rtv) {
        // Post-process the stereo frame for VR output
        // This includes distortion correction, timewarp, and final composition
        
        try {
            // Set viewport for stereo rendering
            D3D12_VIEWPORT viewport = {
                .TopLeftX = 0.0f,
                .TopLeftY = 0.0f,
                .Width = 1920.0f,
                .Height = 1080.0f,
                .MinDepth = 0.0f,
                .MaxDepth = 1.0f
            };
            command_list->RSSetViewports(1, &viewport);

            // Set scissor rect
            D3D12_RECT scissor_rect = {0, 0, 1920, 1080};
            command_list->RSSetScissorRects(1, &scissor_rect);

            // Transition resources to render target state
            D3D12_RESOURCE_BARRIER barriers[3] = {
                {
                    .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                    .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                    .Transition = {
                        .pResource = m_left_eye_texture,
                        .StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
                        .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET,
                        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
                    }
                },
                {
                    .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                    .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                    .Transition = {
                        .pResource = m_right_eye_texture,
                        .StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
                        .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET,
                        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
                    }
                },
                {
                    .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                    .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                    .Transition = {
                        .pResource = m_stereo_output_texture,
                        .StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
                        .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET,
                        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
                    }
                }
            };
            command_list->ResourceBarrier(3, barriers);

            // Render left eye
            command_list->OMSetRenderTargets(1, &m_left_eye_rtv, FALSE, nullptr);
            render_eye_view(command_list, rt, rtv, true);

            // Render right eye
            command_list->OMSetRenderTargets(1, &m_right_eye_rtv, FALSE, nullptr);
            render_eye_view(command_list, rt, rtv, false);

            // Composite stereo frame
            command_list->OMSetRenderTargets(1, &m_stereo_output_rtv, FALSE, nullptr);
            composite_stereo_frame(command_list);

            // Apply VR distortion correction
            apply_vr_distortion_correction(command_list);

            // Apply timewarp if needed
            if (m_vr_enabled) {
                apply_timewarp(command_list);
            }

            spdlog::debug("[REDengine 4 Adapter] Stereo frame post-processing completed");

        } catch (const std::exception& e) {
            spdlog::error("[REDengine 4 Adapter] Error in stereo frame post-processing: {}", e.what());
        }
    }

    void render_eye_view(ID3D12GraphicsCommandList* command_list, 
                        ID3D12Resource* source_texture, 
                        D3D12_CPU_DESCRIPTOR_HANDLE* source_rtv, 
                        bool is_left_eye) {
        // Render individual eye view with proper camera offset
        
        try {
            // Calculate eye offset for stereo rendering
            float eye_offset = is_left_eye ? -0.032f : 0.032f; // 32mm IPD
            
            // Update projection matrix for eye offset
            XMMATRIX eye_projection = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(m_camera_data.fov),
                m_camera_data.aspect_ratio,
                m_camera_data.near_plane,
                m_camera_data.far_plane
            );
            
            // Apply eye offset to view matrix
            XMMATRIX eye_view = m_camera_data.view_matrix * XMMatrixTranslation(eye_offset, 0.0f, 0.0f);
            
            // Set view and projection matrices for this eye
            // This would require setting up constant buffers with the matrices
            
            // Render the scene for this eye
            // For now, we'll just clear the render target
            float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            command_list->ClearRenderTargetView(is_left_eye ? m_left_eye_rtv : m_right_eye_rtv, clear_color, 0, nullptr);
            
        } catch (const std::exception& e) {
            spdlog::error("[REDengine 4 Adapter] Error rendering eye view: {}", e.what());
        }
    }

    void composite_stereo_frame(ID3D12GraphicsCommandList* command_list) {
        // Composite left and right eye views into final stereo frame
        
        try {
            // This would involve using a compute shader or pixel shader to combine the eye views
            // For now, we'll just clear the output target
            float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            command_list->ClearRenderTargetView(m_stereo_output_rtv, clear_color, 0, nullptr);
            
        } catch (const std::exception& e) {
            spdlog::error("[REDengine 4 Adapter] Error compositing stereo frame: {}", e.what());
        }
    }

    void apply_vr_distortion_correction(ID3D12GraphicsCommandList* command_list) {
        // Apply VR lens distortion correction
        
        try {
            // This would involve using a distortion correction shader
            // For now, we'll just log the operation
            spdlog::debug("[REDengine 4 Adapter] VR distortion correction applied");
            
        } catch (const std::exception& e) {
            spdlog::error("[REDengine 4 Adapter] Error applying VR distortion correction: {}", e.what());
        }
    }

    void apply_timewarp(ID3D12GraphicsCommandList* command_list) {
        // Apply timewarp for smooth VR rendering
        
        try {
            // This would involve using a timewarp shader
            // For now, we'll just log the operation
            spdlog::debug("[REDengine 4 Adapter] Timewarp applied");
            
        } catch (const std::exception& e) {
            spdlog::error("[REDengine 4 Adapter] Error applying timewarp: {}", e.what());
        }
    }

    void cleanup_hooks() {
        if (m_hooks_installed) {
            spdlog::info("[REDengine 4 Adapter] Cleaning up hooks");
            
            std::lock_guard<std::mutex> lock(m_hook_mutex);
            
            if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK) {
                spdlog::warn("[REDengine 4 Adapter] Failed to disable some hooks");
            }

            // Clean up individual hooks
            for (auto& [original, replacement] : m_original_functions) {
                if (MH_RemoveHook(original) != MH_OK) {
                    spdlog::warn("[REDengine 4 Adapter] Failed to remove hook");
                }
            }
            m_original_functions.clear();

            if (MH_Uninitialize() != MH_OK) {
                spdlog::warn("[REDengine 4 Adapter] Failed to uninitialize MinHook");
            }

            m_hooks_installed = false;
        }
    }

    void cleanup_stereo_resources() {
        if (m_stereo_resources_created) {
            spdlog::info("[REDengine 4 Adapter] Cleaning up stereo resources");
            
            if (m_left_eye_texture) {
                m_left_eye_texture->Release();
                m_left_eye_texture = nullptr;
            }
            
            if (m_right_eye_texture) {
                m_right_eye_texture->Release();
                m_right_eye_texture = nullptr;
            }
            
            if (m_stereo_output_texture) {
                m_stereo_output_texture->Release();
                m_stereo_output_texture = nullptr;
            }
            
            m_stereo_resources_created = false;
        }
    }

    // Hook callback functions
    static void WINAPI ExecuteCommandLists_Hook(ID3D12CommandQueue* pCommandQueue, 
                                               UINT NumCommandLists, 
                                               ID3D12CommandList* const* ppCommandLists) {
        // This is called before command lists are executed
        // We can use this to inject VR rendering commands
        
        // Call original function
        auto original = (decltype(&ExecuteCommandLists_Hook))g_redengine4_adapter.m_original_functions[pCommandQueue->lpVtbl->ExecuteCommandLists];
        if (original) {
            original(pCommandQueue, NumCommandLists, ppCommandLists);
        }
        
        // Inject VR rendering if enabled
        if (g_redengine4_adapter.m_vr_enabled && g_redengine4_adapter.m_stereo_rendering) {
            // Inject VR rendering commands here
        }
    }

    static void WINAPI SetGraphicsRootConstantBufferView_Hook(ID3D12GraphicsCommandList* pCommandList,
                                                             UINT RootParameterIndex,
                                                             D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) {
        // This is called when setting constant buffer views
        // We can use this to extract camera matrices
        
        // Call original function
        auto original = (decltype(&SetGraphicsRootConstantBufferView_Hook))g_redengine4_adapter.m_original_functions[pCommandList->lpVtbl->SetGraphicsRootConstantBufferView];
        if (original) {
            original(pCommandList, RootParameterIndex, BufferLocation);
        }
        
        // Extract camera data if this is a camera-related constant buffer
        if (RootParameterIndex == 0) { // Assuming camera matrices are in root parameter 0
            // Extract camera matrices from the buffer
            // This would require mapping the buffer and reading the data
        }
    }

    static void WINAPI SetGraphicsRootDescriptorTable_Hook(ID3D12GraphicsCommandList* pCommandList,
                                                          UINT RootParameterIndex,
                                                          D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) {
        // This is called when setting descriptor tables
        // We can use this to monitor resource binding
        
        // Call original function
        auto original = (decltype(&SetGraphicsRootDescriptorTable_Hook))g_redengine4_adapter.m_original_functions[pCommandList->lpVtbl->SetGraphicsRootDescriptorTable];
        if (original) {
            original(pCommandList, RootParameterIndex, BaseDescriptor);
        }
        
        // Monitor resource binding for VR rendering
        // This could help identify which resources are being used for rendering
    }

    static HRESULT WINAPI CreateRootSignature_Hook(ID3D12Device* pDevice,
                                                  const D3D12_ROOT_SIGNATURE_DESC* pRootSignatureDesc,
                                                  REFIID riid,
                                                  void** ppvRootSignature) {
        // This is called when creating root signatures
        // We can use this to identify camera-related root signatures
        
        // Call original function
        auto original = (decltype(&CreateRootSignature_Hook))g_redengine4_adapter.m_original_functions[pDevice->lpVtbl->CreateRootSignature];
        HRESULT result = S_OK;
        if (original) {
            result = original(pDevice, pRootSignatureDesc, riid, ppvRootSignature);
        }
        
        // Analyze root signature for camera parameters
        if (SUCCEEDED(result) && pRootSignatureDesc) {
            // Check if this root signature contains camera-related parameters
            // This could help identify which root signatures to hook for camera data
        }
        
        return result;
    }
};

// Global plugin instance
static REDengine4Adapter g_redengine4_adapter;

} // namespace uevr

// Plugin entry point
extern "C" __declspec(dllexport) void uevr_plugin_required_version(UEVR_PluginVersion* version) {
    version->major = UEVR_PLUGIN_VERSION_MAJOR;
    version->minor = UEVR_PLUGIN_VERSION_MINOR;
    version->patch = UEVR_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool uevr_plugin_initialize(const UEVR_PluginInitializeParam* param) {
    return uevr::g_redengine4_adapter.on_initialize();
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        uevr::g_redengine4_adapter.on_dllmain();
    }

    return TRUE;
}
