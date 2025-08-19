/*
 * UEVR UObject Hook
 * 
 * Unreal Engine object hooking and manipulation system
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <atomic>

namespace uevr {

// Forward declarations
class UObject;
class UClass;
class UFunction;
class UProperty;
class UStruct;
class UEnum;
class UScriptStruct;
class UInterface;

// UObject hook types
enum class UObjectHookType {
    PRE_CONSTRUCT,      // Before object construction
    POST_CONSTRUCT,     // After object construction
    PRE_DESTRUCT,       // Before object destruction
    POST_DESTRUCT,      // After object destruction
    PRE_TICK,           // Before object tick
    POST_TICK,          // After object tick
    PRE_RENDER,         // Before object rendering
    POST_RENDER,        // After object rendering
    PRE_FUNCTION_CALL,  // Before function execution
    POST_FUNCTION_CALL, // After function execution
    PROPERTY_ACCESS,    // Property read/write access
    MEMORY_ALLOCATION,  // Memory allocation/deallocation
    CUSTOM             // Custom hook type
};

// UObject hook priority
enum class UObjectHookPriority {
    LOWEST = 0,
    LOW = 25,
    NORMAL = 50,
    HIGH = 75,
    HIGHEST = 100,
    CRITICAL = 125
};

// UObject hook context
struct UObjectHookContext {
    UObject* target_object;
    UObjectHookType hook_type;
    UObjectHookPriority priority;
    std::string hook_name;
    std::string description;
    std::chrono::steady_clock::time_point timestamp;
    uint64_t call_count;
    bool enabled;
    bool persistent;
    std::unordered_map<std::string, std::string> metadata;
};

// UObject hook callback types
using UObjectHookCallback = std::function<bool(UObject*, UObjectHookType, void*)>;
using UObjectFilterCallback = std::function<bool(UObject*)>;
using UObjectValidationCallback = std::function<bool(UObject*, const std::string&)>;

// UObject hook result
struct UObjectHookResult {
    bool success;
    std::string message;
    uint64_t execution_time_ns;
    bool modified_object;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

// UObject hook configuration
struct UObjectHookConfig {
    bool enable_object_hooking;
    bool enable_function_hooking;
    bool enable_property_hooking;
    bool enable_memory_hooking;
    bool enable_validation;
    bool enable_logging;
    bool enable_profiling;
    uint32_t max_hooks_per_object;
    uint32_t max_total_hooks;
    uint64_t hook_timeout_ns;
    std::string log_file_path;
    std::vector<std::string> excluded_classes;
    std::vector<std::string> excluded_functions;
    std::vector<std::string> excluded_properties;
};

// UObject hook statistics
struct UObjectHookStats {
    uint64_t total_hooks;
    uint64_t active_hooks;
    uint64_t disabled_hooks;
    uint64_t total_executions;
    uint64_t successful_executions;
    uint64_t failed_executions;
    uint64_t total_execution_time_ns;
    uint64_t average_execution_time_ns;
    uint64_t max_execution_time_ns;
    uint64_t min_execution_time_ns;
    std::chrono::steady_clock::time_point last_execution_time;
    std::chrono::steady_clock::time_point last_hook_time;
};

// UObject hook manager class
class UObjectHookManager {
public:
    // Singleton access
    static UObjectHookManager& getInstance();
    
    // Constructor and destructor
    UObjectHookManager();
    ~UObjectHookManager();
    
    // Copy and move operations
    UObjectHookManager(const UObjectHookManager&) = delete;
    UObjectHookManager& operator=(const UObjectHookManager&) = delete;
    UObjectHookManager(UObjectHookManager&&) = delete;
    UObjectHookManager& operator=(UObjectHookManager&&) = delete;
    
    // Configuration
    void configure(const UObjectHookConfig& config);
    UObjectHookConfig getConfig() const;
    
    // Hook management
    bool installHook(UObject* target_object, 
                    UObjectHookType hook_type,
                    UObjectHookCallback callback,
                    const std::string& hook_name = "",
                    const std::string& description = "",
                    UObjectHookPriority priority = UObjectHookPriority::NORMAL);
    
    bool installClassHook(UClass* target_class,
                         UObjectHookType hook_type,
                         UObjectHookCallback callback,
                         const std::string& hook_name = "",
                         const std::string& description = "",
                         UObjectHookPriority priority = UObjectHookPriority::NORMAL);
    
    bool installFunctionHook(UFunction* target_function,
                           UObjectHookCallback callback,
                           const std::string& hook_name = "",
                           const std::string& description = "",
                           UObjectHookPriority priority = UObjectHookPriority::NORMAL);
    
    bool installPropertyHook(UProperty* target_property,
                           UObjectHookCallback callback,
                           const std::string& hook_name = "",
                           const std::string& description = "",
                           UObjectHookPriority priority = UObjectHookPriority::NORMAL);
    
    // Hook removal
    bool removeHook(const std::string& hook_name);
    bool removeHooksByObject(UObject* target_object);
    bool removeHooksByClass(UClass* target_class);
    bool removeHooksByType(UObjectHookType hook_type);
    void removeAllHooks();
    
    // Hook control
    bool enableHook(const std::string& hook_name);
    bool disableHook(const std::string& hook_name);
    bool isHookEnabled(const std::string& hook_name) const;
    
    // Hook execution
    UObjectHookResult executeHook(const std::string& hook_name, UObject* target_object, void* context = nullptr);
    UObjectHookResult executeHooks(UObject* target_object, UObjectHookType hook_type, void* context = nullptr);
    
    // Hook information
    UObjectHookContext getHook(const std::string& hook_name) const;
    std::vector<UObjectHookContext> getHooks() const;
    std::vector<UObjectHookContext> getHooksByObject(UObject* target_object) const;
    std::vector<UObjectHookContext> getHooksByClass(UClass* target_class) const;
    std::vector<UObjectHookContext> getHooksByType(UObjectHookType hook_type) const;
    
    // Hook validation
    bool validateHook(const UObjectHookContext& hook_context) const;
    std::vector<std::string> getHookValidationIssues(const UObjectHookContext& hook_context) const;
    
    // Hook statistics
    UObjectHookStats getStats() const;
    void resetStats();
    
    // Object filtering
    void setObjectFilter(UObjectFilterCallback filter_callback);
    void clearObjectFilter();
    bool shouldHookObject(UObject* target_object) const;
    
    // Object validation
    void setObjectValidator(UObjectValidationCallback validator_callback);
    void clearObjectValidator();
    bool validateObject(UObject* target_object, const std::string& operation) const;
    
    // Hook callbacks
    using HookExecutionCallback = std::function<void(const UObjectHookResult&)>;
    void setHookExecutionCallback(HookExecutionCallback callback);
    void clearHookExecutionCallback();
    
    // Hook persistence
    bool saveHooksToFile(const std::string& file_path);
    bool loadHooksFromFile(const std::string& file_path);
    bool exportHooksToJSON(const std::string& file_path);
    bool importHooksFromJSON(const std::string& file_path);
    
    // Hook monitoring
    void enableHookMonitoring(bool enable);
    bool isHookMonitoringEnabled() const;
    void setHookMonitoringInterval(std::chrono::milliseconds interval);
    std::chrono::milliseconds getHookMonitoringInterval() const;
    
    // Hook debugging
    void enableHookDebugging(bool enable);
    bool isHookDebuggingEnabled() const;
    void setDebugBreakOnHook(bool enable);
    bool isDebugBreakOnHookEnabled() const;
    
    // Hook profiling
    void enableHookProfiling(bool enable);
    bool isHookProfilingEnabled() const;
    void setProfilingThreshold(uint64_t threshold_ns);
    uint64_t getProfilingThreshold() const;
    
    // Hook logging
    void enableHookLogging(bool enable);
    bool isHookLoggingEnabled() const;
    void setLogLevel(int log_level);
    int getLogLevel() const;
    void setLogFile(const std::string& log_file_path);
    std::string getLogFile() const;

protected:
    // Internal hook management
    bool installHookInternal(const UObjectHookContext& hook_context, UObjectHookCallback callback);
    bool removeHookInternal(const std::string& hook_name);
    void updateHookStatistics(const UObjectHookResult& result);
    
    // Hook execution
    UObjectHookResult executeHookInternal(const UObjectHookContext& hook_context, UObject* target_object, void* context);
    void notifyHookExecution(const UObjectHookResult& result);
    
    // Hook validation
    bool validateHookInternal(const UObjectHookContext& hook_context) const;
    bool validateObjectInternal(UObject* target_object, const std::string& operation) const;
    
    // Hook persistence
    bool serializeHooks(const std::string& file_path, bool as_json);
    bool deserializeHooks(const std::string& file_path, bool as_json);
    
    // Hook monitoring
    void startHookMonitoring();
    void stopHookMonitoring();
    void monitorHooks();
    
    // Hook debugging
    void debugHook(const UObjectHookContext& hook_context, const UObjectHookResult& result);
    
    // Hook profiling
    void profileHook(const UObjectHookContext& hook_context, const UObjectHookResult& result);
    
    // Hook logging
    void logHook(const UObjectHookContext& hook_context, const UObjectHookResult& result);

private:
    // Configuration
    UObjectHookConfig m_config;
    
    // Hook storage
    std::unordered_map<std::string, std::pair<UObjectHookContext, UObjectHookCallback>> m_hooks;
    std::unordered_map<UObject*, std::vector<std::string>> m_object_hooks;
    std::unordered_map<UClass*, std::vector<std::string>> m_class_hooks;
    std::unordered_map<UFunction*, std::vector<std::string>> m_function_hooks;
    std::unordered_map<UProperty*, std::vector<std::string>> m_property_hooks;
    
    // Filters and validators
    UObjectFilterCallback m_object_filter;
    UObjectValidationCallback m_object_validator;
    
    // Callbacks
    HookExecutionCallback m_hook_execution_callback;
    
    // Statistics
    UObjectHookStats m_stats;
    
    // Monitoring
    bool m_hook_monitoring_enabled;
    std::chrono::milliseconds m_hook_monitoring_interval;
    std::thread m_monitoring_thread;
    std::atomic<bool> m_monitoring_running;
    
    // Debugging and profiling
    bool m_hook_debugging_enabled;
    bool m_debug_break_on_hook;
    bool m_hook_profiling_enabled;
    uint64_t m_profiling_threshold;
    
    // Logging
    bool m_hook_logging_enabled;
    int m_log_level;
    std::string m_log_file_path;
    
    // Synchronization
    mutable std::mutex m_hooks_mutex;
    mutable std::mutex m_stats_mutex;
    mutable std::mutex m_config_mutex;
    
    // Internal state
    std::atomic<bool> m_initialized;
    uint64_t m_next_hook_id;
};

// Utility functions
namespace UObjectHookUtils {
    
    // Hook creation helpers
    UObjectHookContext createHookContext(UObject* target_object,
                                        UObjectHookType hook_type,
                                        const std::string& hook_name,
                                        const std::string& description,
                                        UObjectHookPriority priority);
    
    // Hook validation helpers
    bool isValidHookContext(const UObjectHookContext& hook_context);
    std::vector<std::string> validateHookContext(const UObjectHookContext& hook_context);
    
    // Object validation helpers
    bool isValidUObject(UObject* object);
    bool isValidUClass(UClass* class_obj);
    bool isValidUFunction(UFunction* function);
    bool isValidUProperty(UProperty* property);
    
    // Hook execution helpers
    UObjectHookResult createHookResult(bool success, const std::string& message);
    void addHookResultWarning(UObjectHookResult& result, const std::string& warning);
    void addHookResultError(UObjectHookResult& result, const std::string& error);
    
    // Hook serialization helpers
    std::string serializeHookContext(const UObjectHookContext& hook_context);
    bool deserializeHookContext(const std::string& data, UObjectHookContext& hook_context);
    
    // Hook comparison helpers
    bool compareHookContexts(const UObjectHookContext& lhs, const UObjectHookContext& rhs);
    bool isSimilarHook(const UObjectHookContext& lhs, const UObjectHookContext& rhs);
    
    // Hook grouping helpers
    std::vector<std::vector<UObjectHookContext>> groupHooksByObject(const std::vector<UObjectHookContext>& hooks);
    std::vector<std::vector<UObjectHookContext>> groupHooksByType(const std::vector<UObjectHookContext>& hooks);
    std::vector<std::vector<UObjectHookContext>> groupHooksByPriority(const std::vector<UObjectHookContext>& hooks);
}

// Global hook manager instance
extern UObjectHookManager* g_uobject_hook_manager;

// Convenience functions
bool installUObjectHook(UObject* target_object, 
                        UObjectHookType hook_type,
                        UObjectHookCallback callback,
                        const std::string& hook_name = "",
                        const std::string& description = "",
                        UObjectHookPriority priority = UObjectHookPriority::NORMAL);

bool removeUObjectHook(const std::string& hook_name);
bool enableUObjectHook(const std::string& hook_name);
bool disableUObjectHook(const std::string& hook_name);

UObjectHookResult executeUObjectHook(const std::string& hook_name, UObject* target_object, void* context = nullptr);

// Hook type conversion
std::string hookTypeToString(UObjectHookType hook_type);
UObjectHookType stringToHookType(const std::string& hook_type_string);

// Hook priority conversion
std::string hookPriorityToString(UObjectHookPriority priority);
UObjectHookPriority stringToHookPriority(const std::string& priority_string);

} // namespace uevr
