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
