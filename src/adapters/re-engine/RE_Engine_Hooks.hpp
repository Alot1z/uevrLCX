/*
RE Engine Cross-Engine VR Adapter for uevr
Hook function declarations and forward references

Based on uevr Plugin API: SOURCECODE/uevr/include/uevr/Plugin.hpp
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <MinHook.h>
#include <DirectXMath.h>

// Forward declarations
struct RE_Engine_Camera_Data;
struct RE_Engine_Render_State;
struct RE_Engine_Matrix_Data;

// RE Engine hook function types
typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* ResizeBuffers_t)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
typedef HRESULT(__stdcall* DrawIndexed_t)(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
typedef HRESULT(__stdcall* ClearRenderTargetView_t)(ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]);

// Hook function signatures
namespace RE_Engine_Hooks {
    
    // D3D11 SwapChain hooks
    HRESULT __stdcall Present_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    HRESULT __stdcall ResizeBuffers_Hook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    
    // D3D11 Device hooks
    HRESULT __stdcall CreateDeviceAndSwapChain_Hook(DXGI_ADAPTER_DESC* pAdapterDesc, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, ID3D11DeviceContext** ppImmediateContext, D3D_FEATURE_LEVEL* pSelectedFeatureLevel);
    
    // Rendering pipeline hooks
    HRESULT __stdcall DrawIndexed_Hook(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
    HRESULT __stdcall ClearRenderTargetView_Hook(ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]);
    
    // Camera and matrix hooks
    void __fastcall UpdateViewMatrix_Hook(void* pThis, void* edx, DirectX::XMMATRIX* pViewMatrix);
    void __fastcall UpdateProjectionMatrix_Hook(void* pThis, void* edx, DirectX::XMMATRIX* pProjectionMatrix);
    
    // Input and controller hooks
    void __fastcall ProcessInput_Hook(void* pThis, void* edx, void* pInputData);
    void __fastcall UpdateControllerState_Hook(void* pThis, void* edx, void* pControllerData);
    
    // Memory and object hooks
    void* __fastcall AllocateGameObject_Hook(void* pThis, void* edx, size_t size, void* pAllocator);
    void __fastcall FreeGameObject_Hook(void* pThis, void* edx, void* pObject);
    
    // Hook management functions
    bool InitializeHooks();
    void CleanupHooks();
    bool IsHooked();
    
    // Hook state management
    extern Present_t OriginalPresent;
    extern ResizeBuffers_t OriginalResizeBuffers;
    extern DrawIndexed_t OriginalDrawIndexed;
    extern ClearRenderTargetView_t OriginalClearRenderTargetView;
    
    // RE Engine specific data structures
    extern RE_Engine_Camera_Data* g_pCameraData;
    extern RE_Engine_Render_State* g_pRenderState;
    extern RE_Engine_Matrix_Data* g_pMatrixData;
    
    // Hook status flags
    extern bool g_bHooksInitialized;
    extern bool g_bVRModeEnabled;
    extern bool g_bStereoRenderingEnabled;
}

// RE Engine memory layout structures
struct RE_Engine_Memory_Layout {
    uintptr_t base_address;
    uintptr_t camera_offset;
    uintptr_t renderer_offset;
    uintptr_t input_offset;
    uintptr_t object_pool_offset;
    
    // Memory protection flags
    DWORD original_protection;
    bool is_writable;
};

// RE Engine hook configuration
struct RE_Engine_Hook_Config {
    bool enable_swapchain_hooks;
    bool enable_device_hooks;
    bool enable_rendering_hooks;
    bool enable_camera_hooks;
    bool enable_input_hooks;
    bool enable_memory_hooks;
    
    // Hook timing and safety
    UINT hook_delay_ms;
    bool use_safe_hooking;
    bool enable_hook_validation;
    
    // Performance settings
    bool enable_performance_monitoring;
    bool enable_frame_timing;
    bool enable_memory_tracking;
};

// Global hook configuration
extern RE_Engine_Hook_Config g_HookConfig;
