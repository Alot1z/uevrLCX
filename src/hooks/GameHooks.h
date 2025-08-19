#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace UEVR {
namespace Hooks {

// Hook types for different rendering APIs
enum class HookType {
    D3D11_PRESENT,
    D3D11_RESIZE_BUFFERS,
    D3D12_EXECUTE_COMMAND_LISTS,
    D3D12_PRESENT,
    DXGI_PRESENT,
    DXGI_RESIZE_BUFFERS,
    OPENGL_SWAP_BUFFERS,
    VULKAN_QUEUE_PRESENT,
    CUSTOM_HOOK
};

// Hook status
enum class HookStatus {
    NOT_INSTALLED,
    INSTALLING,
    INSTALLED,
    FAILED,
    REMOVING,
    REMOVED
};

// Hook information structure
struct HookInfo {
    HookType type;
    HookStatus status;
    std::string description;
    void* original_function;
    void* hooked_function;
    bool is_active;
    std::chrono::steady_clock::time_point install_time;
    
    HookInfo() : type(HookType::CUSTOM_HOOK), status(HookStatus::NOT_INSTALLED),
                  original_function(nullptr), hooked_function(nullptr), is_active(false) {}
};

// Hook callback types
using PresentCallback = std::function<HRESULT(IDXGISwapChain*, UINT, UINT)>;
using ResizeBuffersCallback = std::function<HRESULT(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT)>;
using ExecuteCommandListsCallback = std::function<void(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*)>;
using CustomHookCallback = std::function<bool()>;

// Main hook management class
class GameHooks {
public:
    GameHooks();
    virtual ~GameHooks();
    
    // Initialization and cleanup
    bool initialize();
    void cleanup();
    
    // Hook management
    bool installHooks();
    void uninstallHooks();
    bool isHooksInstalled() const;
    
    // Hook status
    HookStatus getHookStatus(HookType type) const;
    std::vector<HookInfo> getAllHookInfo() const;
    
    // Callback registration
    void setPresentCallback(PresentCallback callback);
    void setResizeBuffersCallback(ResizeBuffersCallback callback);
    void setExecuteCommandListsCallback(ExecuteCommandListsCallback callback);
    void setCustomHookCallback(CustomHookCallback callback);
    
    // Engine-specific hook management
    bool installREEngineHooks();
    bool installREDengine4Hooks();
    bool installMTFrameworkHooks();
    bool installUnrealEngineHooks();
    bool installUnityEngineHooks();
    
    // Hook verification
    bool verifyHooks();
    bool testHookFunctionality();

private:
    // Hook state
    std::vector<HookInfo> m_hooks;
    bool m_initialized;
    bool m_hooks_installed;
    
    // Callbacks
    PresentCallback m_present_callback;
    ResizeBuffersCallback m_resize_buffers_callback;
    ExecuteCommandListsCallback m_execute_command_lists_callback;
    CustomHookCallback m_custom_hook_callback;
    
    // Internal hook methods
    bool installD3D11Hooks();
    bool installD3D12Hooks();
    bool installDXGIHooks();
    bool installOpenGLHooks();
    bool installVulkanHooks();
    
    // Hook implementations
    static HRESULT __stdcall presentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    static HRESULT __stdcall resizeBuffersHook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    static void __stdcall executeCommandListsHook(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);
    
    // Hook utilities
    bool findAndHookFunction(const std::string& module_name, const std::string& function_name, void** original, void* replacement);
    bool createTrampoline(void* target, void* replacement, void** original);
    void logHookInstallation(HookType type, bool success, const std::string& details = "");
};

// Global hook manager instance
extern std::unique_ptr<GameHooks> g_gameHooks;

// Public interface functions
bool initializeGameHooks();
void cleanupGameHooks();
bool installGameHooks();
void uninstallGameHooks();
bool areGameHooksInstalled();

// Hook status queries
HookStatus getHookStatus(HookType type);
std::vector<HookInfo> getAllHookInfo();

// Callback registration
void setPresentCallback(PresentCallback callback);
void setResizeBuffersCallback(ResizeBuffersCallback callback);
void setExecuteCommandListsCallback(ExecuteCommandListsCallback callback);
void setCustomHookCallback(CustomHookCallback callback);

} // namespace Hooks
} // namespace UEVR


