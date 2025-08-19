#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>

// Forward declarations
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;

struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12GraphicsCommandList;
struct ID3D12Resource;
struct ID3D12DescriptorHeap;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

namespace uevr {
namespace Hooks {

// DirectX version enumeration
enum class DirectXVersion {
    UNKNOWN = 0,
    DIRECTX_11 = 11,
    DIRECTX_12 = 12
};

// Hook state enumeration
enum class HookState {
    UNINSTALLED = 0,
    INSTALLING,
    INSTALLED,
    FAILED,
    REMOVING
};

// Hook type enumeration
enum class HookType {
    PRESENT = 0,
    RESIZE_BUFFERS,
    CLEAR_RENDER_TARGET_VIEW,
    CLEAR_DEPTH_STENCIL_VIEW,
    DRAW_INDEXED,
    DRAW,
    DISPATCH,
    COPY_RESOURCE,
    UPDATE_SUBRESOURCE,
    MAP,
    UNMAP,
    CUSTOM
};

// Hook callback function type
using HookCallback = std::function<void(const void*, void*)>;

// Hook information structure
struct HookInfo {
    HookType type;
    std::string name;
    std::string description;
    HookCallback callback;
    bool enabled;
    bool critical;
    std::chrono::steady_clock::time_point install_time;
    std::chrono::steady_clock::time_point last_call_time;
    uint64_t call_count;
    double average_execution_time;
};

// DirectX 11 specific hook data
struct D3D11HookData {
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    ID3D11RenderTargetView* render_target_view;
    ID3D11DepthStencilView* depth_stencil_view;
    ID3D11Texture2D* back_buffer;
    ID3D11Texture2D* depth_stencil;
    D3D11_VIEWPORT viewport;
    D3D11_RECT scissor_rect;
    bool vsync_enabled;
    UINT sync_interval;
    UINT present_flags;
};

// DirectX 12 specific hook data
struct D3D12HookData {
    ID3D12Device* device;
    ID3D12CommandQueue* command_queue;
    ID3D12GraphicsCommandList* command_list;
    ID3D12Resource* back_buffer;
    ID3D12Resource* depth_stencil;
    ID3D12DescriptorHeap* rtv_heap;
    ID3D12DescriptorHeap* dsv_heap;
    ID3D12DescriptorHeap* cbv_srv_uav_heap;
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissor_rect;
    bool vsync_enabled;
    UINT sync_interval;
    UINT present_flags;
};

// DirectX hooks configuration
struct DirectXHooksConfig {
    bool enable_d3d11_hooks = true;
    bool enable_d3d12_hooks = true;
    bool enable_dxgi_hooks = true;
    bool enable_present_hooks = true;
    bool enable_draw_hooks = true;
    bool enable_resource_hooks = true;
    bool enable_shader_hooks = true;
    bool enable_state_hooks = true;
    bool enable_debug_hooks = false;
    bool enable_performance_monitoring = true;
    bool enable_error_logging = true;
    bool enable_call_tracking = true;
    uint32_t max_hook_count = 1000;
    uint32_t hook_timeout_ms = 5000;
    std::string log_file_path = "uevr_directx_hooks.log";
};

// DirectX hooks statistics
struct DirectXHooksStats {
    uint64_t total_hooks_installed = 0;
    uint64_t total_hooks_removed = 0;
    uint64_t total_hook_calls = 0;
    uint64_t failed_hook_installations = 0;
    uint64_t failed_hook_removals = 0;
    double average_hook_installation_time = 0.0;
    double average_hook_execution_time = 0.0;
    std::chrono::steady_clock::time_point first_hook_time;
    std::chrono::steady_clock::time_point last_hook_time;
    std::unordered_map<HookType, uint64_t> hook_type_call_counts;
    std::unordered_map<std::string, uint64_t> hook_name_call_counts;
};

/**
 * @brief DirectX Hooks Manager
 * 
 * This class manages all DirectX 11/12 hooks for the uevr framework.
 * It provides a comprehensive hooking system for intercepting DirectX calls,
 * enabling VR modifications, performance monitoring, and debugging capabilities.
 * 
 * Features:
 * - DirectX 11 and 12 support
 * - Multiple hook types (Present, Draw, Resource, etc.)
 * - Performance monitoring and statistics
 * - Error handling and logging
 * - Hook validation and safety checks
 * - Custom hook callbacks
 * - Thread-safe operations
 */
class DirectXHooks {
public:
    // Constructor and destructor
    explicit DirectXHooks(const DirectXHooksConfig& config = DirectXHooksConfig{});
    ~DirectXHooks();
    
    // Copy and move operations
    DirectXHooks(const DirectXHooks&) = delete;
    DirectXHooks& operator=(const DirectXHooks&) = delete;
    DirectXHooks(DirectXHooks&&) noexcept;
    DirectXHooks& operator=(DirectXHooks&&) noexcept;

    // Core hook management
    bool installHooks();
    void removeHooks();
    bool isHooksInstalled() const;
    HookState getHookState() const;
    
    // DirectX version detection
    DirectXVersion detectDirectXVersion() const;
    bool isDirectX11Supported() const;
    bool isDirectX12Supported() const;
    
    // Hook installation for specific types
    bool installPresentHook();
    bool installDrawHook();
    bool installResourceHook();
    bool installShaderHook();
    bool installStateHook();
    bool installCustomHook(HookType type, const std::string& name, HookCallback callback);
    
    // Hook removal for specific types
    void removePresentHook();
    void removeDrawHook();
    void removeResourceHook();
    void removeShaderHook();
    void removeStateHook();
    void removeCustomHook(const std::string& name);
    
    // Hook management
    bool enableHook(const std::string& name);
    bool disableHook(const std::string& name);
    bool isHookEnabled(const std::string& name) const;
    void enableAllHooks();
    void disableAllHooks();
    
    // Hook information and statistics
    const std::vector<HookInfo>& getInstalledHooks() const;
    const HookInfo* getHookInfo(const std::string& name) const;
    const DirectXHooksStats& getStatistics() const;
    void resetStatistics();
    
    // Configuration management
    const DirectXHooksConfig& getConfig() const;
    void updateConfig(const DirectXHooksConfig& config);
    void resetConfig();
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceThreshold(double threshold_ms);
    double getPerformanceThreshold() const;
    
    // Error handling and logging
    const std::string& getLastError() const;
    void clearLastError();
    std::vector<std::string> getErrorHistory() const;
    void enableErrorLogging(bool enable);
    bool isErrorLoggingEnabled() const;
    
    // Debug and development features
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    void setLogLevel(int level);
    int getLogLevel() const;
    void dumpHookState() const;
    
    // DirectX specific data access
    const D3D11HookData* getD3D11Data() const;
    const D3D12HookData* getD3D12Data() const;
    bool hasD3D11Data() const;
    bool hasD3D12Data() const;
    
    // Hook validation and safety
    bool validateHooks() const;
    bool checkHookIntegrity() const;
    void performSafetyChecks();
    
    // Utility functions
    std::string getDirectXVersionString() const;
    std::string getHookStateString() const;
    std::string getStatisticsReport() const;
    void exportStatistics(const std::string& file_path) const;

protected:
    // Internal hook management
    bool installD3D11Hooks();
    bool installD3D12Hooks();
    bool installDXGIHooks();
    void removeD3D11Hooks();
    void removeD3D12Hooks();
    void removeDXGIHooks();
    
    // Hook validation
    bool validateHookInstallation() const;
    bool validateHookRemoval() const;
    
    // Statistics management
    void updateStatistics(const HookInfo& hook, double execution_time);
    void logHookCall(const HookInfo& hook);
    
    // Error management
    void setLastError(const std::string& error);
    void logError(const std::string& error);
    
    // Internal utilities
    bool initializeMinHook();
    void cleanupMinHook();
    bool isMinHookInitialized() const;

private:
    // Configuration and state
    DirectXHooksConfig m_config;
    HookState m_hook_state;
    DirectXVersion m_directx_version;
    
    // Hook storage
    std::vector<HookInfo> m_installed_hooks;
    std::unordered_map<std::string, size_t> m_hook_name_map;
    
    // DirectX data
    std::unique_ptr<D3D11HookData> m_d3d11_data;
    std::unique_ptr<D3D12HookData> m_d3d12_data;
    
    // Statistics and monitoring
    DirectXHooksStats m_stats;
    bool m_performance_monitoring_enabled;
    double m_performance_threshold;
    
    // Error handling
    std::string m_last_error;
    std::vector<std::string> m_error_history;
    bool m_error_logging_enabled;
    
    // Debug and development
    bool m_debug_mode;
    int m_log_level;
    
    // Internal state
    bool m_minhook_initialized;
    bool m_hooks_installed;
    std::chrono::steady_clock::time_point m_initialization_time;
    
    // Private helper methods
    void initializeDefaultHooks();
    void cleanupResources();
    bool installHookInternal(const HookInfo& hook_info);
    void removeHookInternal(const std::string& name);
    void updateHookState(HookState new_state);
    bool validateConfig() const;
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

} // namespace Hooks
} // namespace uevr
