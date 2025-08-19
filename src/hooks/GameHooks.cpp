#include "GameHooks.h"
#include <spdlog/spdlog.h>
#include <MinHook.h>
#include <iostream>
#include <chrono>
#include <algorithm>

namespace UEVR {
namespace Hooks {

// Global hook manager instance
std::unique_ptr<GameHooks> g_gameHooks;

// Hook implementations
HRESULT __stdcall GameHooks::presentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    try {
        if (g_gameHooks && g_gameHooks->m_present_callback) {
            return g_gameHooks->m_present_callback(pSwapChain, SyncInterval, Flags);
        }
        
        // Call original function if no callback is set
        if (g_gameHooks) {
            auto it = std::find_if(g_gameHooks->m_hooks.begin(), g_gameHooks->m_hooks.end(),
                [](const HookInfo& hook) { return hook.type == HookType::D3D11_PRESENT; });
            
            if (it != g_gameHooks->m_hooks.end() && it->original_function) {
                using PresentFunc = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
                return reinterpret_cast<PresentFunc>(it->original_function)(pSwapChain, SyncInterval, Flags);
            }
        }
        
        return S_OK;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Present hook exception: {}", e.what());
        return S_OK;
    }
}

HRESULT __stdcall GameHooks::resizeBuffersHook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    try {
        if (g_gameHooks && g_gameHooks->m_resize_buffers_callback) {
            return g_gameHooks->m_resize_buffers_callback(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
        }
        
        // Call original function if no callback is set
        if (g_gameHooks) {
            auto it = std::find_if(g_gameHooks->m_hooks.begin(), g_gameHooks->m_hooks.end(),
                [](const HookInfo& hook) { return hook.type == HookType::D3D11_RESIZE_BUFFERS; });
            
            if (it != g_gameHooks->m_hooks.end() && it->original_function) {
                using ResizeBuffersFunc = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
                return reinterpret_cast<ResizeBuffersFunc>(it->original_function)(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
            }
        }
        
        return S_OK;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] ResizeBuffers hook exception: {}", e.what());
        return S_OK;
    }
}

void __stdcall GameHooks::executeCommandListsHook(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists) {
    try {
        if (g_gameHooks && g_gameHooks->m_execute_command_lists_callback) {
            g_gameHooks->m_execute_command_lists_callback(pCommandQueue, NumCommandLists, ppCommandLists);
            return;
        }
        
        // Call original function if no callback is set
        if (g_gameHooks) {
            auto it = std::find_if(g_gameHooks->m_hooks.begin(), g_gameHooks->m_hooks.end(),
                [](const HookInfo& hook) { return hook.type == HookType::D3D12_EXECUTE_COMMAND_LISTS; });
            
            if (it != g_gameHooks->m_hooks.end() && it->original_function) {
                using ExecuteCommandListsFunc = void(__stdcall*)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
                reinterpret_cast<ExecuteCommandListsFunc>(it->original_function)(pCommandQueue, NumCommandLists, ppCommandLists);
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] ExecuteCommandLists hook exception: {}", e.what());
    }
}

// GameHooks class implementation
GameHooks::GameHooks() : m_initialized(false), m_hooks_installed(false) {
    try {
        spdlog::info("[GameHooks] Initializing hook system...");
        
        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            spdlog::error("[GameHooks] Failed to initialize MinHook");
            return;
        }
        
        // Initialize hook information
        initializeHookInfo();
        
        m_initialized = true;
        spdlog::info("[GameHooks] Hook system initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Initialization exception: {}", e.what());
    }
}

GameHooks::~GameHooks() {
    try {
        cleanup();
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Destructor exception: {}", e.what());
    }
}

void GameHooks::initializeHookInfo() {
    try {
        m_hooks.clear();
        
        // D3D11 hooks
        m_hooks.push_back({HookType::D3D11_PRESENT, HookStatus::NOT_INSTALLED, "D3D11 Present Hook", nullptr, nullptr, false});
        m_hooks.push_back({HookType::D3D11_RESIZE_BUFFERS, HookStatus::NOT_INSTALLED, "D3D11 ResizeBuffers Hook", nullptr, nullptr, false});
        
        // D3D12 hooks
        m_hooks.push_back({HookType::D3D12_EXECUTE_COMMAND_LISTS, HookStatus::NOT_INSTALLED, "D3D12 ExecuteCommandLists Hook", nullptr, nullptr, false});
        m_hooks.push_back({HookType::D3D12_PRESENT, HookStatus::NOT_INSTALLED, "D3D12 Present Hook", nullptr, nullptr, false});
        
        // DXGI hooks
        m_hooks.push_back({HookType::DXGI_PRESENT, HookStatus::NOT_INSTALLED, "DXGI Present Hook", nullptr, nullptr, false});
        m_hooks.push_back({HookType::DXGI_RESIZE_BUFFERS, HookStatus::NOT_INSTALLED, "DXGI ResizeBuffers Hook", nullptr, nullptr, false});
        
        // OpenGL hooks
        m_hooks.push_back({HookType::OPENGL_SWAP_BUFFERS, HookStatus::NOT_INSTALLED, "OpenGL SwapBuffers Hook", nullptr, nullptr, false});
        
        // Vulkan hooks
        m_hooks.push_back({HookType::VULKAN_QUEUE_PRESENT, HookStatus::NOT_INSTALLED, "Vulkan Queue Present Hook", nullptr, nullptr, false});
        
        spdlog::info("[GameHooks] Initialized {} hook types", m_hooks.size());
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Hook info initialization exception: {}", e.what());
    }
}

bool GameHooks::initialize() {
    try {
        if (m_initialized) {
            spdlog::warn("[GameHooks] Already initialized");
            return true;
        }
        
        spdlog::info("[GameHooks] Initializing hook system...");
        
        // Initialize MinHook if not already done
        if (MH_Initialize() != MH_OK) {
            spdlog::error("[GameHooks] Failed to initialize MinHook");
            return false;
        }
        
        m_initialized = true;
        spdlog::info("[GameHooks] Hook system initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Initialization exception: {}", e.what());
        return false;
    }
}

void GameHooks::cleanup() {
    try {
        if (m_hooks_installed) {
            uninstallHooks();
        }
        
        if (m_initialized) {
            MH_Uninitialize();
            m_initialized = false;
            spdlog::info("[GameHooks] Hook system cleaned up");
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Cleanup exception: {}", e.what());
    }
}

bool GameHooks::installHooks() {
    try {
        if (!m_initialized) {
            spdlog::error("[GameHooks] Hook system not initialized");
            return false;
        }
        
        if (m_hooks_installed) {
            spdlog::warn("[GameHooks] Hooks already installed");
            return true;
        }
        
        spdlog::info("[GameHooks] Installing hooks...");
        
        // Install D3D11 hooks
        if (!installD3D11Hooks()) {
            spdlog::warn("[GameHooks] Failed to install D3D11 hooks");
        }
        
        // Install D3D12 hooks
        if (!installD3D12Hooks()) {
            spdlog::warn("[GameHooks] Failed to install D3D12 hooks");
        }
        
        // Install DXGI hooks
        if (!installDXGIHooks()) {
            spdlog::warn("[GameHooks] Failed to install DXGI hooks");
        }
        
        // Enable all hooks
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
            spdlog::error("[GameHooks] Failed to enable hooks");
            return false;
        }
        
        m_hooks_installed = true;
        spdlog::info("[GameHooks] Hooks installed successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Hook installation exception: {}", e.what());
        return false;
    }
}

void GameHooks::uninstallHooks() {
    try {
        if (!m_hooks_installed) {
            return;
        }
        
        spdlog::info("[GameHooks] Uninstalling hooks...");
        
        // Disable all hooks
        if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK) {
            spdlog::warn("[GameHooks] Failed to disable some hooks");
        }
        
        // Unhook all hooks
        if (MH_Uninitialize() != MH_OK) {
            spdlog::warn("[GameHooks] Failed to uninitialize some hooks");
        }
        
        // Reinitialize MinHook for future use
        if (MH_Initialize() != MH_OK) {
            spdlog::warn("[GameHooks] Failed to reinitialize MinHook");
        }
        
        // Reset hook status
        for (auto& hook : m_hooks) {
            hook.status = HookStatus::NOT_INSTALLED;
            hook.original_function = nullptr;
            hook.hooked_function = nullptr;
            hook.is_active = false;
        }
        
        m_hooks_installed = false;
        spdlog::info("[GameHooks] Hooks uninstalled successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Hook uninstallation exception: {}", e.what());
    }
}

bool GameHooks::isHooksInstalled() const {
    return m_hooks_installed;
}

HookStatus GameHooks::getHookStatus(HookType type) const {
    try {
        auto it = std::find_if(m_hooks.begin(), m_hooks.end(),
            [type](const HookInfo& hook) { return hook.type == type; });
        
        if (it != m_hooks.end()) {
            return it->status;
        }
        
        return HookStatus::NOT_INSTALLED;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Hook status query exception: {}", e.what());
        return HookStatus::FAILED;
    }
}

std::vector<HookInfo> GameHooks::getAllHookInfo() const {
    return m_hooks;
}

// Callback registration
void GameHooks::setPresentCallback(PresentCallback callback) {
    m_present_callback = callback;
}

void GameHooks::setResizeBuffersCallback(ResizeBuffersCallback callback) {
    m_resize_buffers_callback = callback;
}

void GameHooks::setExecuteCommandListsCallback(ExecuteCommandListsCallback callback) {
    m_execute_command_lists_callback = callback;
}

void GameHooks::setCustomHookCallback(CustomHookCallback callback) {
    m_custom_hook_callback = callback;
}

// Engine-specific hook installation
bool GameHooks::installREEngineHooks() {
    try {
        spdlog::info("[GameHooks] Installing RE Engine hooks...");
        
        // RE Engine uses D3D11, so install D3D11 hooks
        bool success = installD3D11Hooks();
        
        if (success) {
            spdlog::info("[GameHooks] RE Engine hooks installed successfully");
        } else {
            spdlog::warn("[GameHooks] RE Engine hooks installation failed");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] RE Engine hook installation exception: {}", e.what());
        return false;
    }
}

bool GameHooks::installREDengine4Hooks() {
    try {
        spdlog::info("[GameHooks] Installing REDengine 4 hooks...");
        
        // REDengine 4 uses D3D12, so install D3D12 hooks
        bool success = installD3D12Hooks();
        
        if (success) {
            spdlog::info("[GameHooks] REDengine 4 hooks installed successfully");
        } else {
            spdlog::warn("[GameHooks] REDengine 4 hooks installation failed");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] REDengine 4 hook installation exception: {}", e.what());
        return false;
    }
}

bool GameHooks::installMTFrameworkHooks() {
    try {
        spdlog::info("[GameHooks] Installing MT Framework hooks...");
        
        // MT Framework uses D3D11, so install D3D11 hooks
        bool success = installD3D11Hooks();
        
        if (success) {
            spdlog::info("[GameHooks] MT Framework hooks installed successfully");
        } else {
            spdlog::warn("[GameHooks] MT Framework hooks installation failed");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] MT Framework hook installation exception: {}", e.what());
        return false;
    }
}

bool GameHooks::installUnrealEngineHooks() {
    try {
        spdlog::info("[GameHooks] Installing Unreal Engine hooks...");
        
        // Unreal Engine can use both D3D11 and D3D12
        bool d3d11_success = installD3D11Hooks();
        bool d3d12_success = installD3D12Hooks();
        
        bool success = d3d11_success || d3d12_success;
        
        if (success) {
            spdlog::info("[GameHooks] Unreal Engine hooks installed successfully");
        } else {
            spdlog::warn("[GameHooks] Unreal Engine hooks installation failed");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Unreal Engine hook installation exception: {}", e.what());
        return false;
    }
}

bool GameHooks::installUnityEngineHooks() {
    try {
        spdlog::info("[GameHooks] Installing Unity Engine hooks...");
        
        // Unity Engine can use multiple APIs, try D3D11 first
        bool success = installD3D11Hooks();
        
        if (success) {
            spdlog::info("[GameHooks] Unity Engine hooks installed successfully");
        } else {
            spdlog::warn("[GameHooks] Unity Engine hooks installation failed");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Unity Engine hook installation exception: {}", e.what());
        return false;
    }
}

// Internal hook installation methods
bool GameHooks::installD3D11Hooks() {
    try {
        spdlog::info("[GameHooks] Installing D3D11 hooks...");
        
        // Hook D3D11 Present
        auto it = std::find_if(m_hooks.begin(), m_hooks.end(),
            [](const HookInfo& hook) { return hook.type == HookType::D3D11_PRESENT; });
        
        if (it != m_hooks.end()) {
            it->status = HookStatus::INSTALLING;
            
            // Find D3D11 Present function
            HMODULE d3d11Module = GetModuleHandleA("d3d11.dll");
            if (d3d11Module) {
                void* presentFunc = GetProcAddress(d3d11Module, "D3D11Present");
                if (presentFunc) {
                    if (MH_CreateHook(presentFunc, &presentHook, &it->original_function) == MH_OK) {
                        it->status = HookStatus::INSTALLED;
                        it->hooked_function = &presentHook;
                        it->is_active = true;
                        it->install_time = std::chrono::steady_clock::now();
                        spdlog::info("[GameHooks] D3D11 Present hook installed");
                    } else {
                        it->status = HookStatus::FAILED;
                        spdlog::error("[GameHooks] Failed to create D3D11 Present hook");
                    }
                } else {
                    it->status = HookStatus::FAILED;
                    spdlog::error("[GameHooks] D3D11 Present function not found");
                }
            } else {
                it->status = HookStatus::FAILED;
                spdlog::error("[GameHooks] D3D11 module not found");
            }
        }
        
        // Hook D3D11 ResizeBuffers
        it = std::find_if(m_hooks.begin(), m_hooks.end(),
            [](const HookInfo& hook) { return hook.type == HookType::D3D11_RESIZE_BUFFERS; });
        
        if (it != m_hooks.end()) {
            it->status = HookStatus::INSTALLING;
            
            // Find D3D11 ResizeBuffers function
            HMODULE d3d11Module = GetModuleHandleA("d3d11.dll");
            if (d3d11Module) {
                void* resizeBuffersFunc = GetProcAddress(d3d11Module, "D3D11ResizeBuffers");
                if (resizeBuffersFunc) {
                    if (MH_CreateHook(resizeBuffersFunc, &resizeBuffersHook, &it->original_function) == MH_OK) {
                        it->status = HookStatus::INSTALLED;
                        it->hooked_function = &resizeBuffersHook;
                        it->is_active = true;
                        it->install_time = std::chrono::steady_clock::now();
                        spdlog::info("[GameHooks] D3D11 ResizeBuffers hook installed");
                    } else {
                        it->status = HookStatus::FAILED;
                        spdlog::error("[GameHooks] Failed to create D3D11 ResizeBuffers hook");
                    }
                } else {
                    it->status = HookStatus::FAILED;
                    spdlog::error("[GameHooks] D3D11 ResizeBuffers function not found");
                }
            } else {
                it->status = HookStatus::FAILED;
                spdlog::error("[GameHooks] D3D11 module not found");
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] D3D11 hook installation exception: {}", e.what());
        return false;
    }
}

bool GameHooks::installD3D12Hooks() {
    try {
        spdlog::info("[GameHooks] Installing D3D12 hooks...");
        
        // Hook D3D12 ExecuteCommandLists
        auto it = std::find_if(m_hooks.begin(), m_hooks.end(),
            [](const HookInfo& hook) { return hook.type == HookType::D3D12_EXECUTE_COMMAND_LISTS; });
        
        if (it != m_hooks.end()) {
            it->status = HookStatus::INSTALLING;
            
            // Find D3D12 ExecuteCommandLists function
            HMODULE d3d12Module = GetModuleHandleA("d3d12.dll");
            if (d3d12Module) {
                void* executeCommandListsFunc = GetProcAddress(d3d12Module, "D3D12ExecuteCommandLists");
                if (executeCommandListsFunc) {
                    if (MH_CreateHook(executeCommandListsFunc, &executeCommandListsHook, &it->original_function) == MH_OK) {
                        it->status = HookStatus::INSTALLED;
                        it->hooked_function = &executeCommandListsHook;
                        it->is_active = true;
                        it->install_time = std::chrono::steady_clock::now();
                        spdlog::info("[GameHooks] D3D12 ExecuteCommandLists hook installed");
                    } else {
                        it->status = HookStatus::FAILED;
                        spdlog::error("[GameHooks] Failed to create D3D12 ExecuteCommandLists hook");
                    }
                } else {
                    it->status = HookStatus::FAILED;
                    spdlog::error("[GameHooks] D3D12 ExecuteCommandLists function not found");
                }
            } else {
                it->status = HookStatus::FAILED;
                spdlog::error("[GameHooks] D3D12 module not found");
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] D3D12 hook installation exception: {}", e.what());
        return false;
    }
}

bool GameHooks::installDXGIHooks() {
    try {
        spdlog::info("[GameHooks] Installing DXGI hooks...");
        
        // DXGI hooks are typically handled through D3D11/D3D12 hooks
        // This is a placeholder for future DXGI-specific hooks
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] DXGI hook installation exception: {}", e.what());
        return false;
    }
}

bool GameHooks::installOpenGLHooks() {
    try {
        spdlog::info("[GameHooks] Installing OpenGL hooks...");
        
        // OpenGL hooks would go here
        // This is a placeholder for future OpenGL support
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] OpenGL hook installation exception: {}", e.what());
        return false;
    }
}

bool GameHooks::installVulkanHooks() {
    try {
        spdlog::info("[GameHooks] Installing Vulkan hooks...");
        
        // Vulkan hooks would go here
        // This is a placeholder for future Vulkan support
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Vulkan hook installation exception: {}", e.what());
        return false;
    }
}

// Hook verification and testing
bool GameHooks::verifyHooks() {
    try {
        if (!m_hooks_installed) {
            spdlog::warn("[GameHooks] No hooks installed to verify");
            return false;
        }
        
        spdlog::info("[GameHooks] Verifying hooks...");
        
        int active_hooks = 0;
        for (const auto& hook : m_hooks) {
            if (hook.status == HookStatus::INSTALLED && hook.is_active) {
                active_hooks++;
            }
        }
        
        spdlog::info("[GameHooks] Found {} active hooks", active_hooks);
        return active_hooks > 0;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Hook verification exception: {}", e.what());
        return false;
    }
}

bool GameHooks::testHookFunctionality() {
    try {
        if (!m_hooks_installed) {
            spdlog::warn("[GameHooks] No hooks installed to test");
            return false;
        }
        
        spdlog::info("[GameHooks] Testing hook functionality...");
        
        // Test hook functionality by calling custom callback if available
        if (m_custom_hook_callback) {
            bool result = m_custom_hook_callback();
            spdlog::info("[GameHooks] Custom hook test result: {}", result);
            return result;
        }
        
        spdlog::info("[GameHooks] No custom hook test available");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Hook functionality test exception: {}", e.what());
        return false;
    }
}

// Public interface functions
bool initializeGameHooks() {
    try {
        if (!g_gameHooks) {
            g_gameHooks = std::make_unique<GameHooks>();
        }
        return g_gameHooks->initialize();
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to initialize game hooks: {}", e.what());
        return false;
    }
}

void cleanupGameHooks() {
    try {
        if (g_gameHooks) {
            g_gameHooks->cleanup();
        }
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to cleanup game hooks: {}", e.what());
    }
}

bool installGameHooks() {
    try {
        if (!g_gameHooks) {
            spdlog::error("[GameHooks] Game hooks not initialized");
            return false;
        }
        return g_gameHooks->installHooks();
        
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to install game hooks: {}", e.what());
        return false;
    }
}

void uninstallGameHooks() {
    try {
        if (g_gameHooks) {
            g_gameHooks->uninstallHooks();
        }
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to uninstall game hooks: {}", e.what());
    }
}

bool areGameHooksInstalled() {
    try {
        return g_gameHooks && g_gameHooks->isHooksInstalled();
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to check hook installation status: {}", e.what());
        return false;
    }
}

// Hook status queries
HookStatus getHookStatus(HookType type) {
    try {
        return g_gameHooks ? g_gameHooks->getHookStatus(type) : HookStatus::NOT_INSTALLED;
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to get hook status: {}", e.what());
        return HookStatus::FAILED;
    }
}

std::vector<HookInfo> getAllHookInfo() {
    try {
        return g_gameHooks ? g_gameHooks->getAllHookInfo() : std::vector<HookInfo>();
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to get hook info: {}", e.what());
        return std::vector<HookInfo>();
    }
}

// Callback registration
void setPresentCallback(PresentCallback callback) {
    try {
        if (g_gameHooks) {
            g_gameHooks->setPresentCallback(callback);
        }
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to set present callback: {}", e.what());
    }
}

void setResizeBuffersCallback(ResizeBuffersCallback callback) {
    try {
        if (g_gameHooks) {
            g_gameHooks->setResizeBuffersCallback(callback);
        }
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to set resize buffers callback: {}", e.what());
    }
}

void setExecuteCommandListsCallback(ExecuteCommandListsCallback callback) {
    try {
        if (g_gameHooks) {
            g_gameHooks->setExecuteCommandListsCallback(callback);
        }
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to set execute command lists callback: {}", e.what());
    }
}

void setCustomHookCallback(CustomHookCallback callback) {
    try {
        if (g_gameHooks) {
            g_gameHooks->setCustomHookCallback(callback);
        }
    } catch (const std::exception& e) {
        spdlog::error("[GameHooks] Failed to set custom hook callback: {}", e.what());
    }
}

} // namespace Hooks
} // namespace UEVR
