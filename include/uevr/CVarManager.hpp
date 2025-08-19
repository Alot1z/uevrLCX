/*
 * UEVR CVar Manager
 * 
 * Console variable management system for UEVR
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <variant>
#include <chrono>
#include <mutex>

namespace uevr {

// CVar value types
using CVarValue = std::variant<bool, int32_t, uint32_t, float, double, std::string>;

// CVar flags
enum class CVarFlags : uint32_t {
    NONE = 0x00000000,
    READONLY = 0x00000001,        // Cannot be modified
    HIDDEN = 0x00000002,          // Hidden from console
    CHEAT = 0x00000004,           // Cheat code (disabled in release)
    DEBUG = 0x00000008,           // Debug only
    DEVELOPMENT = 0x00000010,     // Development only
    ARCHIVE = 0x00000020,         // Save to config file
    NO_RESTORE = 0x00000040,      // Don't restore from config
    SYSTEM = 0x00000080,          // System variable
    USER = 0x00000100,            // User variable
    EXPERT = 0x00000200,          // Expert user only
    ADVANCED = 0x00000400,        // Advanced user only
    BEGINNER = 0x00000800,        // Beginner friendly
    PERFORMANCE = 0x00001000,     // Performance critical
    SECURITY = 0x00002000,        // Security related
    NETWORK = 0x00004000,         // Network related
    RENDER = 0x00008000,          // Rendering related
    AUDIO = 0x00010000,           // Audio related
    INPUT = 0x00020000,           // Input related
    PHYSICS = 0x00040000,         // Physics related
    AI = 0x00080000,              // AI related
    GAMEPLAY = 0x00100000,        // Gameplay related
    UI = 0x00200000,              // UI related
    VR = 0x00400000,              // VR related
    MOD = 0x00800000,             // Mod related
    PLUGIN = 0x01000000,          // Plugin related
    CUSTOM = 0x02000000           // Custom category
};

// CVar change callback
using CVarChangeCallback = std::function<void(const std::string&, const CVarValue&, const CVarValue&)>;

// CVar validation callback
using CVarValidationCallback = std::function<bool(const std::string&, const CVarValue&)>;

// CVar information
struct CVarInfo {
    std::string name;
    std::string description;
    std::string category;
    CVarValue value;
    CVarValue default_value;
    CVarValue min_value;
    CVarValue max_value;
    CVarFlags flags;
    std::vector<std::string> allowed_values;
    std::chrono::system_clock::time_point last_modified;
    uint64_t modification_count;
    std::string help_text;
    std::string example_usage;
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> metadata;
};

// CVar change event
struct CVarChangeEvent {
    std::string cvar_name;
    CVarValue old_value;
    CVarValue new_value;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    std::string reason;
};

// CVar manager configuration
struct CVarManagerConfig {
    bool enable_auto_save;
    bool enable_change_callbacks;
    bool enable_validation;
    bool enable_logging;
    bool enable_profiling;
    uint32_t max_cvars;
    uint32_t max_categories;
    std::string config_file_path;
    std::string backup_file_path;
    std::chrono::seconds auto_save_interval;
    std::vector<std::string> protected_cvars;
    std::vector<std::string> excluded_cvars;
};

// CVar manager statistics
struct CVarManagerStats {
    uint64_t total_cvars;
    uint64_t modified_cvars;
    uint64_t read_only_cvars;
    uint64_t hidden_cvars;
    uint64_t cheat_cvars;
    uint64_t debug_cvars;
    uint64_t total_modifications;
    uint64_t total_validations;
    uint64_t total_callbacks;
    std::chrono::system_clock::time_point last_modification;
    std::chrono::system_clock::time_point last_validation;
    std::chrono::system_clock::time_point last_callback;
};

// CVar manager class
class CVarManager {
public:
    // Singleton access
    static CVarManager& getInstance();
    
    // Constructor and destructor
    CVarManager();
    ~CVarManager();
    
    // Copy and move operations
    CVarManager(const CVarManager&) = delete;
    CVarManager& operator=(const CVarManager&) = delete;
    CVarManager(CVarManager&&) = delete;
    CVarManager& operator=(CVarManager&&) = delete;
    
    // Configuration
    void configure(const CVarManagerConfig& config);
    CVarManagerConfig getConfig() const;
    
    // CVar registration
    bool registerCVar(const std::string& name,
                     const CVarValue& default_value,
                     const std::string& description = "",
                     const std::string& category = "General",
                     CVarFlags flags = CVarFlags::NONE);
    
    bool registerBoolCVar(const std::string& name,
                          bool default_value,
                          const std::string& description = "",
                          const std::string& category = "General",
                          CVarFlags flags = CVarFlags::NONE);
    
    bool registerIntCVar(const std::string& name,
                         int32_t default_value,
                         int32_t min_value = std::numeric_limits<int32_t>::min(),
                         int32_t max_value = std::numeric_limits<int32_t>::max(),
                         const std::string& description = "",
                         const std::string& category = "General",
                         CVarFlags flags = CVarFlags::NONE);
    
    bool registerUIntCVar(const std::string& name,
                          uint32_t default_value,
                          uint32_t min_value = 0,
                          uint32_t max_value = std::numeric_limits<uint32_t>::max(),
                          const std::string& description = "",
                          const std::string& category = "General",
                          CVarFlags flags = CVarFlags::NONE);
    
    bool registerFloatCVar(const std::string& name,
                           float default_value,
                           float min_value = -std::numeric_limits<float>::max(),
                           float max_value = std::numeric_limits<float>::max(),
                           const std::string& description = "",
                           const std::string& category = "General",
                           CVarFlags flags = CVarFlags::NONE);
    
    bool registerDoubleCVar(const std::string& name,
                            double default_value,
                            double min_value = -std::numeric_limits<double>::max(),
                            double max_value = std::numeric_limits<double>::max(),
                            const std::string& description = "",
                            const std::string& category = "General",
                            CVarFlags flags = CVarFlags::NONE);
    
    bool registerStringCVar(const std::string& name,
                            const std::string& default_value,
                            const std::vector<std::string>& allowed_values = {},
                            const std::string& description = "",
                            const std::string& category = "General",
                            CVarFlags flags = CVarFlags::NONE);
    
    // CVar unregistration
    bool unregisterCVar(const std::string& name);
    void unregisterAllCVars();
    
    // CVar value access
    CVarValue getCVar(const std::string& name) const;
    bool setCVar(const std::string& name, const CVarValue& value, const std::string& source = "");
    
    // Type-specific getters
    bool getBoolCVar(const std::string& name) const;
    int32_t getIntCVar(const std::string& name) const;
    uint32_t getUIntCVar(const std::string& name) const;
    float getFloatCVar(const std::string& name) const;
    double getDoubleCVar(const std::string& name) const;
    std::string getStringCVar(const std::string& name) const;
    
    // Type-specific setters
    bool setBoolCVar(const std::string& name, bool value, const std::string& source = "");
    bool setIntCVar(const std::string& name, int32_t value, const std::string& source = "");
    bool setUIntCVar(const std::string& name, uint32_t value, const std::string& source = "");
    bool setFloatCVar(const std::string& name, float value, const std::string& source = "");
    bool setDoubleCVar(const std::string& name, double value, const std::string& source = "");
    bool setStringCVar(const std::string& name, const std::string& value, const std::string& source = "");
    
    // CVar information
    CVarInfo getCVarInfo(const std::string& name) const;
    std::vector<CVarInfo> getAllCVars() const;
    std::vector<CVarInfo> getCVarsByCategory(const std::string& category) const;
    std::vector<CVarInfo> getCVarsByFlags(CVarFlags flags) const;
    
    // CVar validation
    bool isValidCVar(const std::string& name) const;
    bool validateCVarValue(const std::string& name, const CVarValue& value) const;
    std::vector<std::string> getCVarValidationIssues(const std::string& name, const CVarValue& value) const;
    
    // CVar callbacks
    void setCVarChangeCallback(const std::string& name, CVarChangeCallback callback);
    void setGlobalChangeCallback(CVarChangeCallback callback);
    void clearCVarCallbacks(const std::string& name);
    void clearAllCallbacks();
    
    // CVar validation callbacks
    void setCVarValidationCallback(const std::string& name, CVarValidationCallback callback);
    void setGlobalValidationCallback(CVarValidationCallback callback);
    void clearValidationCallbacks(const std::string& name);
    void clearAllValidationCallbacks();
    
    // CVar persistence
    bool saveCVars(const std::string& file_path = "");
    bool loadCVars(const std::string& file_path = "");
    bool exportCVars(const std::string& file_path, bool as_json = true);
    bool importCVars(const std::string& file_path, bool as_json = true);
    
    // CVar reset
    bool resetCVar(const std::string& name);
    void resetAllCVars();
    void resetCVarsByCategory(const std::string& category);
    void resetCVarsByFlags(CVarFlags flags);
    
    // CVar search
    std::vector<std::string> searchCVars(const std::string& query) const;
    std::vector<std::string> getCVarCategories() const;
    std::vector<std::string> getCVarTags() const;
    
    // CVar help
    std::string getCVarHelp(const std::string& name) const;
    std::string getCategoryHelp(const std::string& category) const;
    std::string getUsageExamples() const;
    
    // CVar statistics
    CVarManagerStats getStats() const;
    void resetStats();
    
    // CVar monitoring
    void enableCVarMonitoring(bool enable);
    bool isCVarMonitoringEnabled() const;
    void setMonitoringInterval(std::chrono::milliseconds interval);
    std::chrono::milliseconds getMonitoringInterval() const;
    
    // CVar debugging
    void enableCVarDebugging(bool enable);
    bool isCVarDebuggingEnabled() const;
    void setDebugLevel(int level);
    int getDebugLevel() const;
    
    // CVar profiling
    void enableCVarProfiling(bool enable);
    bool isCVarProfilingEnabled() const;
    void setProfilingThreshold(uint64_t threshold);
    uint64_t getProfilingThreshold() const;
    
    // CVar logging
    void enableCVarLogging(bool enable);
    bool isCVarLoggingEnabled() const;
    void setLogFile(const std::string& log_file_path);
    std::string getLogFile() const;

protected:
    // Internal CVar management
    bool registerCVarInternal(const CVarInfo& cvar_info);
    bool unregisterCVarInternal(const std::string& name);
    bool setCVarInternal(const std::string& name, const CVarValue& value, const std::string& source);
    
    // CVar validation
    bool validateCVarInternal(const std::string& name, const CVarValue& value) const;
    bool checkCVarFlags(const std::string& name, const CVarValue& value) const;
    
    // CVar callbacks
    void notifyCVarChange(const std::string& name, const CVarValue& old_value, const CVarValue& new_value, const std::string& source);
    bool executeValidationCallbacks(const std::string& name, const CVarValue& value);
    
    // CVar persistence
    bool serializeCVars(const std::string& file_path, bool as_json);
    bool deserializeCVars(const std::string& file_path, bool as_json);
    
    // CVar monitoring
    void startCVarMonitoring();
    void stopCVarMonitoring();
    void monitorCVars();
    
    // CVar debugging
    void debugCVar(const std::string& name, const CVarValue& value, const std::string& operation);
    
    // CVar profiling
    void profileCVar(const std::string& name, const CVarValue& value, const std::string& operation);
    
    // CVar logging
    void logCVar(const std::string& name, const CVarValue& value, const std::string& operation);

private:
    // Configuration
    CVarManagerConfig m_config;
    
    // CVar storage
    std::unordered_map<std::string, CVarInfo> m_cvars;
    std::unordered_map<std::string, std::vector<std::string>> m_categories;
    std::unordered_map<std::string, std::vector<std::string>> m_tags;
    
    // Callbacks
    std::unordered_map<std::string, CVarChangeCallback> m_cvar_change_callbacks;
    CVarChangeCallback m_global_change_callback;
    std::unordered_map<std::string, CVarValidationCallback> m_cvar_validation_callbacks;
    CVarValidationCallback m_global_validation_callback;
    
    // Statistics
    CVarManagerStats m_stats;
    
    // Monitoring
    bool m_cvar_monitoring_enabled;
    std::chrono::milliseconds m_monitoring_interval;
    std::thread m_monitoring_thread;
    std::atomic<bool> m_monitoring_running;
    
    // Debugging and profiling
    bool m_cvar_debugging_enabled;
    int m_debug_level;
    bool m_cvar_profiling_enabled;
    uint64_t m_profiling_threshold;
    
    // Logging
    bool m_cvar_logging_enabled;
    std::string m_log_file_path;
    
    // Synchronization
    mutable std::mutex m_cvars_mutex;
    mutable std::mutex m_callbacks_mutex;
    mutable std::mutex m_stats_mutex;
    mutable std::mutex m_config_mutex;
    
    // Internal state
    std::atomic<bool> m_initialized;
    uint64_t m_next_cvar_id;
};

// Utility functions
namespace CVarUtils {
    
    // CVar creation helpers
    CVarInfo createCVarInfo(const std::string& name,
                           const CVarValue& default_value,
                           const std::string& description,
                           const std::string& category,
                           CVarFlags flags);
    
    // CVar validation helpers
    bool isValidCVarInfo(const CVarInfo& cvar_info);
    std::vector<std::string> validateCVarInfo(const CVarInfo& cvar_info);
    
    // CVar value helpers
    bool isValidCVarValue(const CVarValue& value);
    std::string valueToString(const CVarValue& value);
    CVarValue stringToValue(const std::string& string_value, const CVarInfo& cvar_info);
    
    // CVar serialization helpers
    std::string serializeCVarInfo(const CVarInfo& cvar_info);
    bool deserializeCVarInfo(const std::string& data, CVarInfo& cvar_info);
    
    // CVar comparison helpers
    bool compareCVarValues(const CVarValue& lhs, const CVarValue& rhs);
    bool isSimilarCVar(const CVarInfo& lhs, const CVarInfo& rhs);
    
    // CVar grouping helpers
    std::vector<std::vector<CVarInfo>> groupCVarsByCategory(const std::vector<CVarInfo>& cvars);
    std::vector<std::vector<CVarInfo>> groupCVarsByFlags(const std::vector<CVarInfo>& cvars);
    std::vector<std::vector<CVarInfo>> groupCVarsByType(const std::vector<CVarInfo>& cvars);
}

// Global CVar manager instance
extern CVarManager* g_cvar_manager;

// Convenience functions
bool registerCVar(const std::string& name,
                 const CVarValue& default_value,
                 const std::string& description = "",
                 const std::string& category = "General",
                 CVarFlags flags = CVarFlags::NONE);

bool setCVar(const std::string& name, const CVarValue& value, const std::string& source = "");
CVarValue getCVar(const std::string& name);

// CVar flags operators
CVarFlags operator|(CVarFlags lhs, CVarFlags rhs);
CVarFlags operator&(CVarFlags lhs, CVarFlags rhs);
CVarFlags operator^(CVarFlags lhs, CVarFlags rhs);
CVarFlags operator~(CVarFlags flags);
CVarFlags& operator|=(CVarFlags& lhs, CVarFlags rhs);
CVarFlags& operator&=(CVarFlags& lhs, CVarFlags rhs);
CVarFlags& operator^=(CVarFlags& lhs, CVarFlags rhs);

// CVar flags conversion
std::string flagsToString(CVarFlags flags);
CVarFlags stringToFlags(const std::string& flags_string);

} // namespace uevr
