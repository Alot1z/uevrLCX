#include "uevr/hooks/DirectXHooks.hpp"
#include <spdlog/spdlog.h>
#include <MinHook.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include <memory>
#include <vector>

namespace uevr {
namespace Hooks {

DirectXHooks::DirectXHooks() 
    : m_hooks_installed(false)
    , m_d3d11_present_hook(nullptr)
    , m_d3d11_draw_hook(nullptr)
    , m_d3d12_present_hook(nullptr)
    , m_d3d12_execute_hook(nullptr)
    , m_swap_chain_present_hook(nullptr)
    , m_device_create_hook(nullptr)
    , m_swap_chain_create_hook(nullptr) {
    
    spdlog::info("[DirectXHooks] DirectX Hooks initialized");
}

DirectXHooks::~DirectXHooks() {
    removeHooks();
    spdlog::info("[DirectXHooks] DirectX Hooks cleanup completed");
}

bool DirectXHooks::installHooks() {
    if (m_hooks_installed) {
        spdlog::warn("[DirectXHooks] Hooks already installed");
        return true;
    }

    try {
        spdlog::info("[DirectXHooks] Installing DirectX hooks...");
        
        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            spdlog::error("[DirectXHooks] Failed to initialize MinHook");
            return false;
        }
        
        // Install D3D11 hooks
        if (!installD3D11Hooks()) {
            spdlog::warn("[DirectXHooks] D3D11 hooks installation failed");
        }
        
        // Install D3D12 hooks
        if (!installD3D12Hooks()) {
            spdlog::warn("[DirectXHooks] D3D12 hooks installation failed");
        }
        
        // Install DXGI hooks
        if (!installDXGIHooks()) {
            spdlog::warn("[DirectXHooks] DXGI hooks installation failed");
        }
        
        // Enable all hooks
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
            spdlog::error("[DirectXHooks] Failed to enable hooks");
            return false;
        }
        
        m_hooks_installed = true;
        spdlog::info("[DirectXHooks] DirectX hooks installed successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception during hook installation: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("[DirectXHooks] Unknown error during hook installation");
        return false;
    }
}

void DirectXHooks::removeHooks() {
    if (!m_hooks_installed) {
        return;
    }

    try {
        spdlog::info("[DirectXHooks] Removing DirectX hooks...");
        
        // Disable all hooks first
        MH_DisableHook(MH_ALL_HOOKS);
        
        // Uninstall D3D11 hooks
        if (m_d3d11_present_hook) {
            MH_UncreateHook(m_d3d11_present_hook);
            m_d3d11_present_hook = nullptr;
        }
        
        if (m_d3d11_draw_hook) {
            MH_UncreateHook(m_d3d11_draw_hook);
            m_d3d11_draw_hook = nullptr;
        }
        
        // Uninstall D3D12 hooks
        if (m_d3d12_present_hook) {
            MH_UncreateHook(m_d3d12_present_hook);
            m_d3d12_present_hook = nullptr;
        }
        
        if (m_d3d12_execute_hook) {
            MH_UncreateHook(m_d3d12_execute_hook);
            m_d3d12_execute_hook = nullptr;
        }
        
        // Uninstall DXGI hooks
        if (m_swap_chain_present_hook) {
            MH_UncreateHook(m_swap_chain_present_hook);
            m_swap_chain_present_hook = nullptr;
        }
        
        if (m_device_create_hook) {
            MH_UncreateHook(m_device_create_hook);
            m_device_create_hook = nullptr;
        }
        
        if (m_swap_chain_create_hook) {
            MH_UncreateHook(m_swap_chain_create_hook);
            m_swap_chain_create_hook = nullptr;
        }
        
        // Uninitialize MinHook
        MH_Uninitialize();
        
        m_hooks_installed = false;
        spdlog::info("[DirectXHooks] DirectX hooks removed successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception during hook removal: {}", e.what());
    } catch (...) {
        spdlog::error("[DirectXHooks] Unknown error during hook removal");
    }
}

bool DirectXHooks::installD3D11Hooks() {
    try {
        // Get D3D11 device creation function
        HMODULE d3d11_module = GetModuleHandleA("d3d11.dll");
        if (!d3d11_module) {
            spdlog::warn("[DirectXHooks] D3D11 module not found");
            return false;
        }
        
        // Hook D3D11Present
        auto d3d11_present = reinterpret_cast<LPVOID>(GetProcAddress(d3d11_module, "D3D11Present"));
        if (d3d11_present) {
            if (MH_CreateHook(d3d11_present, &DirectXHooks::D3D11Present_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_d3d11_present)) == MH_OK) {
                m_d3d11_present_hook = d3d11_present;
                spdlog::info("[DirectXHooks] D3D11 Present hook installed");
            }
        }
        
        // Hook D3D11Draw
        auto d3d11_draw = reinterpret_cast<LPVOID>(GetProcAddress(d3d11_module, "D3D11Draw"));
        if (d3d11_draw) {
            if (MH_CreateHook(d3d11_draw, &DirectXHooks::D3D11Draw_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_d3d11_draw)) == MH_OK) {
                m_d3d11_draw_hook = d3d11_draw;
                spdlog::info("[DirectXHooks] D3D11 Draw hook installed");
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception during D3D11 hook installation: {}", e.what());
        return false;
    }
}

bool DirectXHooks::installD3D12Hooks() {
    try {
        // Get D3D12 device creation function
        HMODULE d3d12_module = GetModuleHandleA("d3d12.dll");
        if (!d3d12_module) {
            spdlog::warn("[DirectXHooks] D3D12 module not found");
            return false;
        }
        
        // Hook D3D12Present
        auto d3d12_present = reinterpret_cast<LPVOID>(GetProcAddress(d3d12_module, "D3D12Present"));
        if (d3d12_present) {
            if (MH_CreateHook(d3d12_present, &DirectXHooks::D3D12Present_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_d3d12_present)) == MH_OK) {
                m_d3d12_present_hook = d3d12_present;
                spdlog::info("[DirectXHooks] D3D12 Present hook installed");
            }
        }
        
        // Hook D3D12ExecuteCommandLists
        auto d3d12_execute = reinterpret_cast<LPVOID>(GetProcAddress(d3d12_module, "D3D12ExecuteCommandLists"));
        if (d3d12_execute) {
            if (MH_CreateHook(d3d12_execute, &DirectXHooks::D3D12ExecuteCommandLists_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_d3d12_execute)) == MH_OK) {
                m_d3d12_execute_hook = d3d12_execute;
                spdlog::info("[DirectXHooks] D3D12 Execute Command Lists hook installed");
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception during D3D12 hook installation: {}", e.what());
        return false;
    }
}

bool DirectXHooks::installDXGIHooks() {
    try {
        // Get DXGI module
        HMODULE dxgi_module = GetModuleHandleA("dxgi.dll");
        if (!dxgi_module) {
            spdlog::warn("[DirectXHooks] DXGI module not found");
            return false;
        }
        
        // Hook SwapChain::Present
        auto swap_chain_present = reinterpret_cast<LPVOID>(GetProcAddress(dxgi_module, "SwapChain_Present"));
        if (swap_chain_present) {
            if (MH_CreateHook(swap_chain_present, &DirectXHooks::SwapChainPresent_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_swap_chain_present)) == MH_OK) {
                m_swap_chain_present_hook = swap_chain_present;
                spdlog::info("[DirectXHooks] SwapChain Present hook installed");
            }
        }
        
        // Hook D3D11CreateDeviceAndSwapChain
        auto device_create = reinterpret_cast<LPVOID>(GetProcAddress(dxgi_module, "D3D11CreateDeviceAndSwapChain"));
        if (device_create) {
            if (MH_CreateHook(device_create, &DirectXHooks::D3D11CreateDeviceAndSwapChain_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_device_create)) == MH_OK) {
                m_device_create_hook = device_create;
                spdlog::info("[DirectXHooks] D3D11 Create Device and SwapChain hook installed");
            }
        }
        
        // Hook CreateSwapChain
        auto swap_chain_create = reinterpret_cast<LPVOID>(GetProcAddress(dxgi_module, "CreateSwapChain"));
        if (swap_chain_create) {
            if (MH_CreateHook(swap_chain_create, &DirectXHooks::CreateSwapChain_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_swap_chain_create)) == MH_OK) {
                m_swap_chain_create_hook = swap_chain_create;
                spdlog::info("[DirectXHooks] Create SwapChain hook installed");
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception during DXGI hook installation: {}", e.what());
        return false;
    }
}

// Hook callback functions
HRESULT DirectXHooks::D3D11Present_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    try {
        spdlog::debug("[DirectXHooks] D3D11 Present called");
        
        // Call original function
        HRESULT result = m_original_d3d11_present(pSwapChain, SyncInterval, Flags);
        
        // Post-processing for VR
        if (SUCCEEDED(result)) {
            // Notify framework about present
            if (m_present_callback) {
                m_present_callback(PresentType::D3D11, pSwapChain);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception in D3D11 Present hook: {}", e.what());
        return m_original_d3d11_present(pSwapChain, SyncInterval, Flags);
    }
}

HRESULT DirectXHooks::D3D11Draw_Hook(ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation) {
    try {
        spdlog::debug("[DirectXHooks] D3D11 Draw called");
        
        // Call original function
        HRESULT result = m_original_d3d11_draw(pContext, VertexCount, StartVertexLocation);
        
        // Post-processing for VR
        if (SUCCEEDED(result)) {
            // Notify framework about draw call
            if (m_draw_callback) {
                m_draw_callback(DrawType::D3D11, pContext);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception in D3D11 Draw hook: {}", e.what());
        return m_original_d3d11_draw(pContext, VertexCount, StartVertexLocation);
    }
}

HRESULT DirectXHooks::D3D12Present_Hook(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags, UINT BufferToPresent) {
    try {
        spdlog::debug("[DirectXHooks] D3D12 Present called");
        
        // Call original function
        HRESULT result = m_original_d3d12_present(pSwapChain, SyncInterval, Flags, BufferToPresent);
        
        // Post-processing for VR
        if (SUCCEEDED(result)) {
            // Notify framework about present
            if (m_present_callback) {
                m_present_callback(PresentType::D3D12, pSwapChain);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception in D3D12 Present hook: {}", e.what());
        return m_original_d3d12_present(pSwapChain, SyncInterval, Flags, BufferToPresent);
    }
}

HRESULT DirectXHooks::D3D12ExecuteCommandLists_Hook(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists) {
    try {
        spdlog::debug("[DirectXHooks] D3D12 Execute Command Lists called");
        
        // Call original function
        HRESULT result = m_original_d3d12_execute(pCommandQueue, NumCommandLists, ppCommandLists);
        
        // Post-processing for VR
        if (SUCCEEDED(result)) {
            // Notify framework about command list execution
            if (m_execute_callback) {
                m_execute_callback(ExecuteType::D3D12, pCommandQueue, ppCommandLists, NumCommandLists);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception in D3D12 Execute Command Lists hook: {}", e.what());
        return m_original_d3d12_execute(pCommandQueue, NumCommandLists, ppCommandLists);
    }
}

HRESULT DirectXHooks::SwapChainPresent_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    try {
        spdlog::debug("[DirectXHooks] SwapChain Present called");
        
        // Call original function
        HRESULT result = m_original_swap_chain_present(pSwapChain, SyncInterval, Flags);
        
        // Post-processing for VR
        if (SUCCEEDED(result)) {
            // Notify framework about present
            if (m_present_callback) {
                m_present_callback(PresentType::DXGI, pSwapChain);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception in SwapChain Present hook: {}", e.what());
        return m_original_swap_chain_present(pSwapChain, SyncInterval, Flags);
    }
}

HRESULT DirectXHooks::D3D11CreateDeviceAndSwapChain_Hook(
    IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
    UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
    UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext) {
    
    try {
        spdlog::info("[DirectXHooks] D3D11 Create Device and SwapChain called");
        
        // Call original function
        HRESULT result = m_original_device_create(pAdapter, DriverType, Software, Flags,
                                                pFeatureLevels, FeatureLevels, SDKVersion,
                                                pSwapChainDesc, ppSwapChain, ppDevice,
                                                pFeatureLevel, ppImmediateContext);
        
        // Post-processing for VR
        if (SUCCEEDED(result)) {
            // Notify framework about device creation
            if (m_device_create_callback) {
                m_device_create_callback(DeviceType::D3D11, ppDevice, ppImmediateContext, ppSwapChain);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception in D3D11 Create Device and SwapChain hook: {}", e.what());
        return m_original_device_create(pAdapter, DriverType, Software, Flags,
                                     pFeatureLevels, FeatureLevels, SDKVersion,
                                     pSwapChainDesc, ppSwapChain, ppDevice,
                                     pFeatureLevel, ppImmediateContext);
    }
}

HRESULT DirectXHooks::CreateSwapChain_Hook(
    IDXGIFactory* pFactory, IUnknown* pDevice, const DXGI_SWAP_CHAIN_DESC* pDesc,
    IDXGISwapChain** ppSwapChain) {
    
    try {
        spdlog::info("[DirectXHooks] Create SwapChain called");
        
        // Call original function
        HRESULT result = m_original_swap_chain_create(pFactory, pDevice, pDesc, ppSwapChain);
        
        // Post-processing for VR
        if (SUCCEEDED(result)) {
            // Notify framework about swap chain creation
            if (m_swap_chain_create_callback) {
                m_swap_chain_create_callback(pFactory, pDevice, pDesc, ppSwapChain);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception in Create SwapChain hook: {}", e.what());
        return m_original_swap_chain_create(pFactory, pDevice, pDesc, ppSwapChain);
    }
}

// Callback setters
void DirectXHooks::setPresentCallback(PresentCallback callback) {
    m_present_callback = callback;
}

void DirectXHooks::setDrawCallback(DrawCallback callback) {
    m_draw_callback = callback;
}

void DirectXHooks::setExecuteCallback(ExecuteCallback callback) {
    m_execute_callback = callback;
}

void DirectXHooks::setDeviceCreateCallback(DeviceCreateCallback callback) {
    m_device_create_callback = callback;
}

void DirectXHooks::setSwapChainCreateCallback(SwapChainCreateCallback callback) {
    m_swap_chain_create_callback = callback;
}

// Utility functions
bool DirectXHooks::areHooksValid() const {
    return m_hooks_installed && 
           (m_d3d11_present_hook || m_d3d11_draw_hook || 
            m_d3d12_present_hook || m_d3d12_execute_hook ||
            m_swap_chain_present_hook || m_device_create_hook || m_swap_chain_create_hook);
}

bool DirectXHooks::restoreHooks() {
    try {
        spdlog::info("[DirectXHooks] Restoring DirectX hooks...");
        
        // Remove existing hooks
        removeHooks();
        
        // Reinstall hooks
        return installHooks();
        
    } catch (const std::exception& e) {
        spdlog::error("[DirectXHooks] Exception during hook restoration: {}", e.what());
        return false;
    }
}

} // namespace Hooks
} // namespace uevr
