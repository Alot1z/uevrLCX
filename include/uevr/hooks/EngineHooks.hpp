#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <windows.h>

// Forward declarations
class DirectXHooks;
class OpenGLHooks;
class VulkanHooks;

namespace uevr {
namespace Hooks {

// Engine type enumeration
enum class EngineType {
    UNKNOWN = 0,
    UNREAL_ENGINE_4,
    UNREAL_ENGINE_5,
    UNITY,
    CRYENGINE,
    LUMBERYARD,
    CUSTOM
};

// Engine version enumeration
enum class EngineVersion {
    UNKNOWN = 0,
    UE4_20 = 420,
    UE4_21 = 421,
    UE4_22 = 422,
    UE4_23 = 423,
    UE4_24 = 424,
    UE4_25 = 425,
    UE4_26 = 426,
    UE4_27 = 427,
    UE5_0 = 500,
    UE5_1 = 501,
    UE5_2 = 502,
    UE5_3 = 503,
    UE5_4 = 504,
    UE5_5 = 505,
    UNITY_2019 = 2019,
    UNITY_2020 = 2020,
    UNITY_2021 = 2021,
    UNITY_2022 = 2022,
    UNITY_2023 = 2023,
    CRYENGINE_5 = 5,
    CRYENGINE_6 = 6,
    LUMBERYARD_1 = 1
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
    ENGINE_INITIALIZATION = 0,
    ENGINE_SHUTDOWN,
    FRAME_BEGIN,
    FRAME_END,
    SCENE_LOADING,
    SCENE_UNLOADING,
    OBJECT_CREATION,
    OBJECT_DESTRUCTION,
    RENDERING_BEGIN,
    RENDERING_END,
    INPUT_PROCESSING,
    AUDIO_PROCESSING,
    PHYSICS_UPDATE,
    ANIMATION_UPDATE,
    NETWORK_UPDATE,
    SCRIPT_EXECUTION,
    MEMORY_ALLOCATION,
    MEMORY_DEALLOCATION,
    FILE_OPERATIONS,
    NETWORK_OPERATIONS,
    CUSTOM
};

// Hook callback function type
using EngineHookCallback = std::function<void(const void*, void*)>;

// Hook information structure
struct EngineHookInfo {
    HookType type;
    std::string name;
    std::string description;
    EngineHookCallback callback;
    bool enabled;
    bool critical;
    std::chrono::steady_clock::time_point install_time;
    std::chrono::steady_clock::time_point last_call_time;
    uint64_t call_count;
    double average_execution_time;
};

// Engine information structure
struct EngineInfo {
    EngineType type;
    EngineVersion version;
    std::string name;
    std::string version_string;
    std::string build_number;
    std::string build_date;
    std::string engine_path;
    std::string project_path;
    bool is_debug_build;
    bool is_development_build;
    bool is_shipping_build;
    bool is_test_build;
    std::vector<std::string> loaded_modules;
    std::vector<std::string> loaded_plugins;
    std::unordered_map<std::string, std::string> engine_settings;
    std::unordered_map<std::string, std::string> project_settings;
};

// Engine hooks configuration
struct EngineHooksConfig {
    bool enable_engine_hooks = true;
    bool enable_directx_hooks = true;
    bool enable_opengl_hooks = true;
    bool enable_vulkan_hooks = true;
    bool enable_unreal_hooks = true;
    bool enable_unity_hooks = true;
    bool enable_cryengine_hooks = true;
    bool enable_lumberyard_hooks = true;
    bool enable_custom_hooks = true;
    bool enable_debug_hooks = false;
    bool enable_performance_monitoring = true;
    bool enable_error_logging = true;
    bool enable_call_tracking = true;
    bool enable_memory_tracking = false;
    bool enable_file_tracking = false;
    bool enable_network_tracking = false;
    uint32_t max_hook_count = 1000;
    uint32_t hook_timeout_ms = 5000;
    std::string log_file_path = "uevr_engine_hooks.log";
    std::vector<std::string> excluded_processes;
    std::vector<std::string> excluded_modules;
};

// Engine hooks statistics
struct EngineHooksStats {
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
    std::unordered_map<EngineType, uint64_t> engine_type_call_counts;
    uint64_t total_memory_allocations = 0;
    uint64_t total_memory_deallocations = 0;
    uint64_t total_file_operations = 0;
    uint64_t total_network_operations = 0;
};

/**
 * @brief Engine Hooks Manager
 * 
 * This class manages all engine-specific hooks for the uevr framework.
 * It provides a comprehensive hooking system for intercepting engine calls,
 * enabling VR modifications, performance monitoring, and debugging capabilities.
 * 
 * Features:
 * - Multiple engine support (Unreal, Unity, CryEngine, Lumberyard)
 * - Multiple graphics API support (DirectX, OpenGL, Vulkan)
 * - Engine detection and version identification
 * - Multiple hook types (Engine, Rendering, Input, etc.)
 * - Performance monitoring and statistics
 * - Error handling and logging
 * - Hook validation and safety checks
 * - Custom hook callbacks
 * - Thread-safe operations
 * - Memory and resource tracking
 */
class EngineHooks {
public:
    // Constructor and destructor
    explicit EngineHooks(const EngineHooksConfig& config = EngineHooksConfig{});
    ~EngineHooks();
    
    // Copy and move operations
    EngineHooks(const EngineHooks&) = delete;
    EngineHooks& operator=(const EngineHooks&) = delete;
    EngineHooks(EngineHooks&&) noexcept;
    EngineHooks& operator=(EngineHooks&&) noexcept;

    // Core hook management
    bool installHooks();
    void removeHooks();
    bool isHooksInstalled() const;
    HookState getHookState() const;
    
    // Engine detection
    EngineType detectEngineType() const;
    EngineVersion detectEngineVersion() const;
    const EngineInfo& getEngineInfo() const;
    std::string getEngineTypeString() const;
    std::string getEngineVersionString() const;
    bool isEngineSupported(EngineType type) const;
    bool isEngineVersionSupported(EngineVersion version) const;
    
    // Hook installation for specific types
    bool installEngineHook();
    bool installRenderingHook();
    bool installInputHook();
    bool installAudioHook();
    bool installPhysicsHook();
    bool installAnimationHook();
    bool installNetworkHook();
    bool installScriptHook();
    bool installMemoryHook();
    bool installFileHook();
    bool installCustomHook(HookType type, const std::string& name, EngineHookCallback callback);
    
    // Hook removal for specific types
    void removeEngineHook();
    void removeRenderingHook();
    void removeInputHook();
    void removeAudioHook();
    void removePhysicsHook();
    void removeAnimationHook();
    void removeNetworkHook();
    void removeScriptHook();
    void removeMemoryHook();
    void removeFileHook();
    void removeCustomHook(const std::string& name);
    
    // Hook management
    bool enableHook(const std::string& name);
    bool disableHook(const std::string& name);
    bool isHookEnabled(const std::string& name) const;
    void enableAllHooks();
    void disableAllHooks();
    
    // Hook information and statistics
    const std::vector<EngineHookInfo>& getInstalledHooks() const;
    const EngineHookInfo* getHookInfo(const std::string& name) const;
    const EngineHooksStats& getStatistics() const;
    void resetStatistics();
    
    // Configuration management
    const EngineHooksConfig& getConfig() const;
    void updateConfig(const EngineHooksConfig& config);
    void resetConfig();
    
    // Graphics API hooks access
    DirectXHooks* getDirectXHooks() const;
    OpenGLHooks* getOpenGLHooks() const;
    VulkanHooks* getVulkanHooks() const;
    
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
    
    // Hook validation and safety
    bool validateHooks() const;
    bool checkHookIntegrity() const;
    void performSafetyChecks();
    
    // Utility functions
    std::string getHookStateString() const;
    std::string getStatisticsReport() const;
    void exportStatistics(const std::string& file_path) const;

protected:
    // Internal hook management
    bool installEngineSpecificHooks();
    bool installGraphicsAPIHooks();
    void removeEngineSpecificHooks();
    void removeGraphicsAPIHooks();
    
    // Hook validation
    bool validateHookInstallation() const;
    bool validateHookRemoval() const;
    
    // Statistics management
    void updateStatistics(const EngineHookInfo& hook, double execution_time);
    void logHookCall(const EngineHookInfo& hook);
    
    // Error management
    void setLastError(const std::string& error);
    void logError(const std::string& error);
    
    // Internal utilities
    bool initializeMinHook();
    void cleanupMinHook();
    bool isMinHookInitialized() const;
    bool shouldHookProcess() const;
    bool shouldHookModule(const std::string& module_name) const;

private:
    // Configuration and state
    EngineHooksConfig m_config;
    HookState m_hook_state;
    EngineInfo m_engine_info;
    
    // Hook storage
    std::vector<EngineHookInfo> m_installed_hooks;
    std::unordered_map<std::string, size_t> m_hook_name_map;
    
    // Graphics API hooks
    std::unique_ptr<DirectXHooks> m_directx_hooks;
    std::unique_ptr<OpenGLHooks> m_opengl_hooks;
    std::unique_ptr<VulkanHooks> m_vulkan_hooks;
    
    // Statistics and monitoring
    EngineHooksStats m_stats;
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
    bool installHookInternal(const EngineHookInfo& hook_info);
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
