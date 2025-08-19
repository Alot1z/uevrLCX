#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <windows.h>
#include <mutex>
#include <shared_mutex>

// Forward declarations
class DirectXHooks;
class OpenGLHooks;
class VulkanHooks;
class EngineHooks;

namespace uevr {
namespace Hooks {

// Hook priority enumeration
enum class HookPriority {
    LOWEST = 0,
    LOW = 1,
    NORMAL = 2,
    HIGH = 3,
    HIGHEST = 4,
    CRITICAL = 5
};

// Hook category enumeration
enum class HookCategory {
    GRAPHICS = 0,
    ENGINE,
    INPUT,
    AUDIO,
    PHYSICS,
    NETWORK,
    MEMORY,
    FILE_SYSTEM,
    CUSTOM
};

// Hook state enumeration
enum class HookState {
    UNINSTALLED = 0,
    INSTALLING,
    INSTALLED,
    FAILED,
    REMOVING,
    DISABLED
};

// Hook information structure
struct HookInfo {
    std::string id;
    std::string name;
    std::string description;
    HookCategory category;
    HookPriority priority;
    HookState state;
    std::string target_function;
    std::string target_module;
    std::function<void(const void*, void*)> callback;
    std::function<void()> cleanup_callback;
    bool enabled;
    bool critical;
    bool auto_cleanup;
    std::chrono::steady_clock::time_point install_time;
    std::chrono::steady_clock::time_point last_call_time;
    uint64_t call_count;
    double average_execution_time;
    double last_execution_time;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflicts;
    std::unordered_map<std::string, std::string> metadata;
};

// Hook manager configuration
struct HookManagerConfig {
    bool enable_auto_management = true;
    bool enable_dependency_resolution = true;
    bool enable_conflict_detection = true;
    bool enable_performance_monitoring = true;
    bool enable_error_logging = true;
    bool enable_call_tracking = true;
    bool enable_hook_validation = true;
    bool enable_safety_checks = true;
    bool enable_thread_safety = true;
    uint32_t max_hook_count = 10000;
    uint32_t hook_timeout_ms = 10000;
    uint32_t max_retry_attempts = 3;
    double performance_threshold_ms = 100.0;
    std::string log_file_path = "uevr_hook_manager.log";
    std::vector<std::string> excluded_processes;
    std::vector<std::string> excluded_modules;
    std::vector<std::string> excluded_functions;
};

// Hook manager statistics
struct HookManagerStats {
    uint64_t total_hooks_registered = 0;
    uint64_t total_hooks_installed = 0;
    uint64_t total_hooks_removed = 0;
    uint64_t total_hooks_failed = 0;
    uint64_t total_hook_calls = 0;
    uint64_t total_hook_errors = 0;
    double average_hook_installation_time = 0.0;
    double average_hook_execution_time = 0.0;
    double average_hook_removal_time = 0.0;
    std::chrono::steady_clock::time_point first_hook_time;
    std::chrono::steady_clock::time_point last_hook_time;
    std::unordered_map<HookCategory, uint64_t> category_hook_counts;
    std::unordered_map<HookPriority, uint64_t> priority_hook_counts;
    std::unordered_map<HookState, uint64_t> state_hook_counts;
    std::unordered_map<std::string, uint64_t> module_hook_counts;
    std::unordered_map<std::string, uint64_t> function_hook_counts;
    uint64_t total_dependency_resolutions = 0;
    uint64_t total_conflict_resolutions = 0;
    uint64_t total_validation_checks = 0;
    uint64_t total_safety_checks = 0;
};

// Hook dependency information
struct HookDependency {
    std::string hook_id;
    std::string dependency_id;
    bool required;
    bool resolved;
    std::chrono::steady_clock::time_point resolution_time;
    std::string resolution_error;
};

// Hook conflict information
struct HookConflict {
    std::string hook_id_1;
    std::string hook_id_2;
    std::string conflict_type;
    std::string conflict_description;
    bool resolved;
    std::chrono::steady_clock::time_point detection_time;
    std::chrono::steady_clock::time_point resolution_time;
    std::string resolution_method;
};

/**
 * @brief Hook Manager
 * 
 * This class manages all hooks in the uevr framework, providing a centralized
 * system for hook installation, removal, monitoring, and management.
 * 
 * Features:
 * - Centralized hook management
 * - Dependency resolution
 * - Conflict detection and resolution
 * - Performance monitoring
 * - Error handling and logging
 * - Thread safety
 * - Hook validation and safety checks
 * - Automatic cleanup
 * - Statistics and reporting
 * - Configuration management
 */
class HookManager {
public:
    // Singleton access
    static HookManager& getInstance();
    
    // Constructor and destructor
    explicit HookManager(const HookManagerConfig& config = HookManagerConfig{});
    ~HookManager();
    
    // Copy and move operations
    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;
    HookManager(HookManager&&) = delete;
    HookManager& operator=(HookManager&&) = delete;

    // Core hook management
    bool registerHook(const HookInfo& hook_info);
    bool unregisterHook(const std::string& hook_id);
    bool installHook(const std::string& hook_id);
    bool removeHook(const std::string& hook_id);
    bool enableHook(const std::string& hook_id);
    bool disableHook(const std::string& hook_id);
    
    // Batch operations
    bool installAllHooks();
    bool removeAllHooks();
    bool enableAllHooks();
    bool disableAllHooks();
    bool installHooksByCategory(HookCategory category);
    bool removeHooksByCategory(HookCategory category);
    bool installHooksByPriority(HookPriority priority);
    bool removeHooksByPriority(HookPriority priority);
    
    // Hook information and queries
    const HookInfo* getHookInfo(const std::string& hook_id) const;
    std::vector<HookInfo> getHooksByCategory(HookCategory category) const;
    std::vector<HookInfo> getHooksByPriority(HookPriority priority) const;
    std::vector<HookInfo> getHooksByState(HookState state) const;
    std::vector<HookInfo> getHooksByModule(const std::string& module_name) const;
    std::vector<HookInfo> getHooksByFunction(const std::string& function_name) const;
    std::vector<HookInfo> getAllHooks() const;
    
    // Hook state management
    bool isHookInstalled(const std::string& hook_id) const;
    bool isHookEnabled(const std::string& hook_id) const;
    HookState getHookState(const std::string& hook_id) const;
    bool hasHook(const std::string& hook_id) const;
    size_t getHookCount() const;
    size_t getHookCountByCategory(HookCategory category) const;
    size_t getHookCountByPriority(HookPriority priority) const;
    
    // Dependency and conflict management
    bool resolveDependencies(const std::string& hook_id);
    bool resolveAllDependencies();
    std::vector<HookDependency> getHookDependencies(const std::string& hook_id) const;
    std::vector<HookConflict> getHookConflicts(const std::string& hook_id) const;
    std::vector<HookConflict> getAllConflicts() const;
    bool resolveConflict(const std::string& conflict_id);
    bool resolveAllConflicts();
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceThreshold(double threshold_ms);
    double getPerformanceThreshold() const;
    void resetPerformanceStats(const std::string& hook_id);
    void resetAllPerformanceStats();
    
    // Statistics and reporting
    const HookManagerStats& getStatistics() const;
    void resetStatistics();
    std::string getStatisticsReport() const;
    void exportStatistics(const std::string& file_path) const;
    void exportHookList(const std::string& file_path) const;
    
    // Configuration management
    const HookManagerConfig& getConfig() const;
    void updateConfig(const HookManagerConfig& config);
    void resetConfig();
    
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
    void validateAllHooks() const;
    void performAllSafetyChecks();
    
    // Hook validation and safety
    bool validateHook(const std::string& hook_id) const;
    bool validateAllHooks() const;
    bool checkHookIntegrity(const std::string& hook_id) const;
    bool checkAllHookIntegrity() const;
    void performSafetyChecks(const std::string& hook_id);
    void performAllSafetyChecks();
    
    // Utility functions
    std::string getHookStateString(const std::string& hook_id) const;
    std::string getHookCategoryString(HookCategory category) const;
    std::string getHookPriorityString(HookPriority priority) const;
    HookCategory stringToHookCategory(const std::string& category_string) const;
    HookPriority stringToHookPriority(const std::string& priority_string) const;
    
    // Graphics API hooks access
    DirectXHooks* getDirectXHooks() const;
    OpenGLHooks* getOpenGLHooks() const;
    VulkanHooks* getVulkanHooks() const;
    EngineHooks* getEngineHooks() const;

protected:
    // Internal hook management
    bool installHookInternal(const std::string& hook_id);
    bool removeHookInternal(const std::string& hook_id);
    bool enableHookInternal(const std::string& hook_id);
    bool disableHookInternal(const std::string& hook_id);
    
    // Dependency and conflict resolution
    bool resolveHookDependencies(const std::string& hook_id);
    bool detectHookConflicts(const std::string& hook_id);
    bool resolveHookConflict(const HookConflict& conflict);
    
    // Hook validation
    bool validateHookInstallation(const std::string& hook_id) const;
    bool validateHookRemoval(const std::string& hook_id) const;
    bool validateHookConfiguration(const HookInfo& hook_info) const;
    
    // Statistics management
    void updateHookStatistics(const std::string& hook_id, double execution_time);
    void updateManagerStatistics();
    
    // Error management
    void setLastError(const std::string& error);
    void logError(const std::string& error);
    
    // Internal utilities
    bool initializeMinHook();
    void cleanupMinHook();
    bool isMinHookInitialized() const;
    std::string generateHookId(const HookInfo& hook_info) const;
    bool shouldHookProcess() const;
    bool shouldHookModule(const std::string& module_name) const;
    bool shouldHookFunction(const std::string& function_name) const;

private:
    // Configuration and state
    HookManagerConfig m_config;
    HookManagerStats m_stats;
    
    // Hook storage
    std::unordered_map<std::string, HookInfo> m_hooks;
    std::unordered_map<HookCategory, std::vector<std::string>> m_category_hooks;
    std::unordered_map<HookPriority, std::vector<std::string>> m_priority_hooks;
    std::unordered_map<HookState, std::vector<std::string>> m_state_hooks;
    std::unordered_map<std::string, std::vector<std::string>> m_module_hooks;
    std::unordered_map<std::string, std::vector<std::string>> m_function_hooks;
    
    // Dependency and conflict tracking
    std::vector<HookDependency> m_dependencies;
    std::vector<HookConflict> m_conflicts;
    std::unordered_map<std::string, std::vector<std::string>> m_hook_dependencies;
    std::unordered_map<std::string, std::vector<std::string>> m_hook_conflicts;
    
    // Graphics API hooks
    std::unique_ptr<DirectXHooks> m_directx_hooks;
    std::unique_ptr<OpenGLHooks> m_opengl_hooks;
    std::unique_ptr<VulkanHooks> m_vulkan_hooks;
    std::unique_ptr<EngineHooks> m_engine_hooks;
    
    // Performance monitoring
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
    bool m_initialized;
    std::chrono::steady_clock::time_point m_initialization_time;
    
    // Thread safety
    mutable std::shared_mutex m_hooks_mutex;
    mutable std::shared_mutex m_dependencies_mutex;
    mutable std::shared_mutex m_conflicts_mutex;
    mutable std::shared_mutex m_stats_mutex;
    
    // Private helper methods
    void initializeDefaultHooks();
    void cleanupResources();
    void setHookState(const std::string& hook_id, HookState new_state);
    void updateHookCategory(const std::string& hook_id, HookCategory old_category, HookCategory new_category);
    void updateHookPriority(const std::string& hook_id, HookPriority old_priority, HookPriority new_priority);
    void updateHookStateWithOldState(const std::string& hook_id, HookState old_state, HookState new_state);
    bool validateConfig() const;
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

} // namespace Hooks
} // namespace uevr
