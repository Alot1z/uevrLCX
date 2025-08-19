#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <windows.h>

// Forward declarations
class CrossEngineAdapter;
class GameProfile;

namespace uevr {
namespace Adapters {

// Engine hook type enumeration
enum class EngineHookType {
    UNKNOWN = 0,
    ENGINE_INITIALIZATION,
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

// Engine hook state enumeration
enum class EngineHookState {
    UNINSTALLED = 0,
    INSTALLING,
    INSTALLED,
    FAILED,
    REMOVING,
    DISABLED
};

// Engine hook priority enumeration
enum class EngineHookPriority {
    LOWEST = 0,
    LOW = 1,
    NORMAL = 2,
    HIGH = 3,
    HIGHEST = 4,
    CRITICAL = 5
};

// Engine hook callback function type
using EngineHookCallback = std::function<void(const void*, void*)>;

// Engine hook information structure
struct EngineHookInfo {
    std::string id;
    std::string name;
    std::string description;
    EngineHookType type;
    EngineHookPriority priority;
    EngineHookState state;
    std::string target_function;
    std::string target_module;
    EngineHookCallback callback;
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

// Engine hooks configuration
struct EngineHooksConfig {
    bool enable_engine_hooks = true;
    bool enable_rendering_hooks = true;
    bool enable_input_hooks = true;
    bool enable_audio_hooks = true;
    bool enable_physics_hooks = true;
    bool enable_animation_hooks = true;
    bool enable_network_hooks = true;
    bool enable_script_hooks = true;
    bool enable_memory_hooks = false;
    bool enable_file_hooks = false;
    bool enable_debug_hooks = false;
    bool enable_performance_monitoring = true;
    bool enable_error_logging = true;
    bool enable_call_tracking = true;
    bool enable_hook_validation = true;
    bool enable_safety_checks = true;
    uint32_t max_hook_count = 1000;
    uint32_t hook_timeout_ms = 5000;
    uint32_t max_retry_attempts = 3;
    double performance_threshold_ms = 100.0;
    std::string log_file_path = "uevr_engine_hooks.log";
    std::vector<std::string> excluded_processes;
    std::vector<std::string> excluded_modules;
    std::vector<std::string> excluded_functions;
};

// Engine hooks statistics
struct EngineHooksStats {
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
    std::unordered_map<EngineHookType, uint64_t> hook_type_call_counts;
    std::unordered_map<std::string, uint64_t> hook_name_call_counts;
    std::unordered_map<std::string, uint64_t> module_hook_counts;
    std::unordered_map<std::string, uint64_t> function_hook_counts;
    uint64_t total_dependency_resolutions = 0;
    uint64_t total_conflict_resolutions = 0;
    uint64_t total_validation_checks = 0;
    uint64_t total_safety_checks = 0;
};

// Engine hook dependency information
struct EngineHookDependency {
    std::string hook_id;
    std::string dependency_id;
    bool required;
    bool resolved;
    std::chrono::steady_clock::time_point resolution_time;
    std::string resolution_error;
};

// Engine hook conflict information
struct EngineHookConflict {
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
 * @brief Engine Hooks Manager
 * 
 * This class manages engine-specific hooks for cross-engine adapters in the uevr framework.
 * It provides a comprehensive hooking system for intercepting engine calls,
 * enabling VR modifications, performance monitoring, and debugging capabilities.
 * 
 * Features:
 * - Engine-specific hook management
 * - Multiple hook types (Engine, Rendering, Input, etc.)
 * - Hook dependency resolution
 * - Hook conflict detection and resolution
 * - Performance monitoring and statistics
 * - Error handling and logging
 * - Hook validation and safety checks
 * - Custom hook callbacks
 * - Thread-safe operations
 * - Configuration management
 * - Statistics and reporting
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
    bool registerHook(const EngineHookInfo& hook_info);
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
    bool installHooksByType(EngineHookType type);
    bool removeHooksByType(EngineHookType type);
    bool installHooksByPriority(EngineHookPriority priority);
    bool removeHooksByPriority(EngineHookPriority priority);
    
    // Hook information and queries
    const EngineHookInfo* getHookInfo(const std::string& hook_id) const;
    std::vector<EngineHookInfo> getHooksByType(EngineHookType type) const;
    std::vector<EngineHookInfo> getHooksByPriority(EngineHookPriority priority) const;
    std::vector<EngineHookInfo> getHooksByState(EngineHookState state) const;
    std::vector<EngineHookInfo> getHooksByModule(const std::string& module_name) const;
    std::vector<EngineHookInfo> getHooksByFunction(const std::string& function_name) const;
    std::vector<EngineHookInfo> getAllHooks() const;
    
    // Hook state management
    bool isHookInstalled(const std::string& hook_id) const;
    bool isHookEnabled(const std::string& hook_id) const;
    EngineHookState getHookState(const std::string& hook_id) const;
    bool hasHook(const std::string& hook_id) const;
    size_t getHookCount() const;
    size_t getHookCountByType(EngineHookType type) const;
    size_t getHookCountByPriority(EngineHookPriority priority) const;
    
    // Dependency and conflict management
    bool resolveDependencies(const std::string& hook_id);
    bool resolveAllDependencies();
    std::vector<EngineHookDependency> getHookDependencies(const std::string& hook_id) const;
    std::vector<EngineHookConflict> getHookConflicts(const std::string& hook_id) const;
    std::vector<EngineHookConflict> getAllConflicts() const;
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
    const EngineHooksStats& getStatistics() const;
    void resetStatistics();
    std::string getStatisticsReport() const;
    void exportStatistics(const std::string& file_path) const;
    void exportHookList(const std::string& file_path) const;
    
    // Configuration management
    const EngineHooksConfig& getConfig() const;
    void updateConfig(const EngineHooksConfig& config);
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
    std::string getHookTypeString(EngineHookType type) const;
    std::string getHookPriorityString(EngineHookPriority priority) const;
    EngineHookType stringToHookType(const std::string& type_string) const;
    EngineHookPriority stringToHookPriority(const std::string& priority_string) const;

protected:
    // Internal hook management
    bool installHookInternal(const std::string& hook_id);
    bool removeHookInternal(const std::string& hook_id);
    bool enableHookInternal(const std::string& hook_id);
    bool disableHookInternal(const std::string& hook_id);
    
    // Dependency and conflict resolution
    bool resolveHookDependencies(const std::string& hook_id);
    bool detectHookConflicts(const std::string& hook_id);
    bool resolveHookConflict(const EngineHookConflict& conflict);
    
    // Hook validation
    bool validateHookInstallation(const std::string& hook_id) const;
    bool validateHookRemoval(const std::string& hook_id) const;
    bool validateHookConfiguration(const EngineHookInfo& hook_info) const;
    
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
    std::string generateHookId(const EngineHookInfo& hook_info) const;
    bool shouldHookProcess() const;
    bool shouldHookModule(const std::string& module_name) const;
    bool shouldHookFunction(const std::string& function_name) const;

private:
    // Configuration and state
    EngineHooksConfig m_config;
    EngineHooksStats m_stats;
    
    // Hook storage
    std::unordered_map<std::string, EngineHookInfo> m_hooks;
    std::unordered_map<EngineHookType, std::vector<std::string>> m_type_hooks;
    std::unordered_map<EngineHookPriority, std::vector<std::string>> m_priority_hooks;
    std::unordered_map<EngineHookState, std::vector<std::string>> m_state_hooks;
    std::unordered_map<std::string, std::vector<std::string>> m_module_hooks;
    std::unordered_map<std::string, std::vector<std::string>> m_function_hooks;
    
    // Dependency and conflict tracking
    std::vector<EngineHookDependency> m_dependencies;
    std::vector<EngineHookConflict> m_conflicts;
    std::unordered_map<std::string, std::vector<std::string>> m_hook_dependencies;
    std::unordered_map<std::string, std::vector<std::string>> m_hook_conflicts;
    
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
    bool m_hooks_installed;
    std::chrono::steady_clock::time_point m_initialization_time;
    
    // Private helper methods
    void initializeDefaultHooks();
    void cleanupResources();
    void setHookState(const std::string& hook_id, EngineHookState new_state);
    void updateHookType(const std::string& hook_id, EngineHookType old_type, EngineHookType new_type);
    void updateHookPriority(const std::string& hook_id, EngineHookPriority old_priority, EngineHookPriority new_priority);
    void updateHookStateWithOldState(const std::string& hook_id, EngineHookState old_state, EngineHookState new_state);
    bool validateConfig() const;
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

} // namespace Adapters
} // namespace uevr
