#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <filesystem>

// Forward declarations
class CrossEngineAdapter;
class GameProfile;

namespace uevr {
namespace Adapters {

// VR modification type enumeration
enum class VRModType {
    UNKNOWN = 0,
    CAMERA_MODIFICATION,
    CONTROLLER_MODIFICATION,
    DISPLAY_MODIFICATION,
    AUDIO_MODIFICATION,
    PERFORMANCE_MODIFICATION,
    INTERFACE_MODIFICATION,
    PHYSICS_MODIFICATION,
    ANIMATION_MODIFICATION,
    CUSTOM_MODIFICATION
};

// VR modification state enumeration
enum class VRModState {
    UNLOADED = 0,
    LOADING,
    LOADED,
    INITIALIZING,
    INITIALIZED,
    STARTING,
    RUNNING,
    PAUSED,
    STOPPING,
    STOPPED,
    ERROR,
    CLEANING_UP
};

// VR modification priority enumeration
enum class VRModPriority {
    LOWEST = 0,
    LOW = 1,
    NORMAL = 2,
    HIGH = 3,
    HIGHEST = 4,
    CRITICAL = 5
};

// VR modification configuration
struct VRModConfig {
    std::string mod_name;
    std::string mod_version;
    std::string mod_description;
    std::string mod_author;
    std::string mod_website;
    VRModType mod_type;
    VRModPriority priority;
    bool enabled;
    bool auto_load;
    bool auto_start;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflicts;
    std::unordered_map<std::string, std::string> settings;
    std::unordered_map<std::string, bool> feature_flags;
    std::unordered_map<std::string, double> numeric_settings;
    std::unordered_map<std::string, std::vector<std::string>> list_settings;
    std::string config_file_path;
    std::string log_file_path;
};

// VR modification statistics
struct VRModStats {
    uint64_t total_sessions = 0;
    uint64_t total_playtime_seconds = 0;
    uint64_t total_modifications = 0;
    uint64_t total_errors = 0;
    double average_session_length = 0.0;
    double average_fps_impact = 0.0;
    double average_memory_usage = 0.0;
    std::chrono::steady_clock::time_point first_session_time;
    std::chrono::steady_clock::time_point last_session_time;
    std::unordered_map<std::string, uint64_t> feature_usage_counts;
    std::unordered_map<std::string, uint64_t> error_counts;
    std::unordered_map<std::string, double> setting_usage_counts;
};

// VR modification validation result
struct VRModValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> info;
    std::chrono::steady_clock::time_point validation_time;
    std::string validation_method;
};

// VR modification compatibility information
struct VRModCompatibility {
    bool compatible;
    std::string compatibility_reason;
    double compatibility_score;
    std::vector<std::string> required_features;
    std::vector<std::string> missing_features;
    std::vector<std::string> conflicting_features;
    std::vector<std::string> recommended_settings;
    std::chrono::steady_clock::time_point compatibility_check_time;
};

// VR modification callback types
using VRModInitializationCallback = std::function<bool()>;
using VRModCleanupCallback = std::function<void()>;
using VRModUpdateCallback = std::function<void(float)>;
using VRModRenderCallback = std::function<void()>;
using VRModErrorCallback = std::function<void(const std::string&)>;

/**
 * @brief VR Modification Manager
 * 
 * This class manages VR modifications for cross-engine adapters in the uevr framework.
 * It provides a comprehensive system for loading, managing, and coordinating VR mods,
 * enabling enhanced VR experiences across different game engines.
 * 
 * Features:
 * - VR modification management
 * - Multiple modification types (Camera, Controller, Display, etc.)
 * - Mod dependency resolution
 * - Mod conflict detection and resolution
 * - Performance monitoring and statistics
 * - Error handling and logging
 * - Mod validation and compatibility checking
 * - Custom mod callbacks
 * - Thread-safe operations
 * - Configuration management
 * - Statistics and reporting
 */
class VRModManager {
public:
    // Constructor and destructor
    explicit VRModManager();
    ~VRModManager();
    
    // Copy and move operations
    VRModManager(const VRModManager&) = delete;
    VRModManager& operator=(const VRModManager&) = delete;
    VRModManager(VRModManager&&) noexcept;
    VRModManager& operator=(VRModManager&&) noexcept;

    // Core mod management
    bool registerMod(const VRModConfig& mod_config);
    bool unregisterMod(const std::string& mod_name);
    bool loadMod(const std::string& mod_name);
    bool unloadMod(const std::string& mod_name);
    bool startMod(const std::string& mod_name);
    bool stopMod(const std::string& mod_name);
    bool pauseMod(const std::string& mod_name);
    bool resumeMod(const std::string& mod_name);
    
    // Batch operations
    bool loadAllMods();
    bool unloadAllMods();
    bool startAllMods();
    bool stopAllMods();
    bool pauseAllMods();
    bool resumeAllMods();
    bool loadModsByType(VRModType type);
    bool unloadModsByType(VRModType type);
    bool startModsByType(VRModType type);
    bool stopModsByType(VRModType type);
    
    // Mod information and queries
    const VRModConfig* getModConfig(const std::string& mod_name) const;
    VRModConfig* getModConfig(const std::string& mod_name);
    const VRModStats& getModStats(const std::string& mod_name) const;
    VRModStats* getModStats(const std::string& mod_name);
    VRModState getModState(const std::string& mod_name) const;
    std::vector<VRModConfig> getModsByType(VRModType type) const;
    std::vector<VRModConfig> getModsByState(VRModState state) const;
    std::vector<VRModConfig> getModsByPriority(VRModPriority priority) const;
    std::vector<VRModConfig> getAllMods() const;
    
    // Mod state management
    bool isModLoaded(const std::string& mod_name) const;
    bool isModRunning(const std::string& mod_name) const;
    bool isModPaused(const std::string& mod_name) const;
    bool isModEnabled(const std::string& mod_name) const;
    bool hasMod(const std::string& mod_name) const;
    size_t getModCount() const;
    size_t getModCountByType(VRModType type) const;
    size_t getModCountByState(VRModState state) const;
    
    // Mod configuration management
    bool updateModConfig(const std::string& mod_name, const VRModConfig& config);
    bool updateModSetting(const std::string& mod_name, const std::string& key, const std::string& value);
    bool updateModSetting(const std::string& mod_name, const std::string& key, bool value);
    bool updateModSetting(const std::string& mod_name, const std::string& key, double value);
    bool updateModSetting(const std::string& mod_name, const std::string& key, const std::vector<std::string>& value);
    bool removeModSetting(const std::string& mod_name, const std::string& key);
    bool hasModSetting(const std::string& mod_name, const std::string& key) const;
    
    // Mod setting access
    std::string getModSetting(const std::string& mod_name, const std::string& key, const std::string& default_value = "") const;
    bool getModBoolSetting(const std::string& mod_name, const std::string& key, bool default_value = false) const;
    double getModNumericSetting(const std::string& mod_name, const std::string& key, double default_value = 0.0) const;
    std::vector<std::string> getModListSetting(const std::string& mod_name, const std::string& key, const std::vector<std::string>& default_value = {}) const;
    
    // Mod feature flag management
    bool isModFeatureEnabled(const std::string& mod_name, const std::string& feature) const;
    void enableModFeature(const std::string& mod_name, const std::string& feature);
    void disableModFeature(const std::string& mod_name, const std::string& feature);
    void toggleModFeature(const std::string& mod_name, const std::string& feature);
    std::vector<std::string> getModEnabledFeatures(const std::string& mod_name) const;
    std::vector<std::string> getModDisabledFeatures(const std::string& mod_name) const;
    
    // Mod dependency and conflict management
    bool resolveModDependencies(const std::string& mod_name);
    bool resolveAllModDependencies();
    std::vector<std::string> getModDependencies(const std::string& mod_name) const;
    std::vector<std::string> getModConflicts(const std::string& mod_name) const;
    bool checkModCompatibility(const std::string& mod_name, const std::string& other_mod_name) const;
    
    // Mod validation
    VRModValidationResult validateMod(const std::string& mod_name) const;
    VRModValidationResult validateAllMods() const;
    bool isModValid(const std::string& mod_name) const;
    std::vector<std::string> getModValidationErrors(const std::string& mod_name) const;
    std::vector<std::string> getModValidationWarnings(const std::string& mod_name) const;
    
    // Mod compatibility checking
    VRModCompatibility checkModCompatibility(const std::string& mod_name, const CrossEngineAdapter* adapter) const;
    bool isModCompatibleWithAdapter(const std::string& mod_name, const CrossEngineAdapter* adapter) const;
    double getModCompatibilityScore(const std::string& mod_name, const CrossEngineAdapter* adapter) const;
    
    // Mod statistics management
    void updateModSessionStats(const std::string& mod_name, uint64_t session_duration_seconds);
    void updateModPerformanceStats(const std::string& mod_name, double fps_impact, double memory_usage);
    void updateModFeatureUsageStats(const std::string& mod_name, const std::string& feature);
    void updateModErrorStats(const std::string& mod_name, const std::string& error_type);
    void updateModSettingUsageStats(const std::string& mod_name, const std::string& setting_name);
    void resetModStatistics(const std::string& mod_name);
    void resetAllModStatistics();
    
    // Mod callbacks
    bool setModInitializationCallback(const std::string& mod_name, VRModInitializationCallback callback);
    bool setModCleanupCallback(const std::string& mod_name, VRModCleanupCallback callback);
    bool setModUpdateCallback(const std::string& mod_name, VRModUpdateCallback callback);
    bool setModRenderCallback(const std::string& mod_name, VRModRenderCallback callback);
    bool setModErrorCallback(const std::string& mod_name, VRModErrorCallback callback);
    void clearModCallbacks(const std::string& mod_name);
    void clearAllModCallbacks();
    
    // Mod lifecycle management
    void updateMods(float delta_time);
    void renderMods();
    bool initializeMods();
    void cleanupMods();
    void pauseMods();
    void resumeMods();
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceThreshold(double threshold_ms);
    double getPerformanceThreshold() const;
    void resetPerformanceStats();
    
    // Statistics and reporting
    std::string getModManagerStatistics() const;
    void exportModList(const std::string& file_path) const;
    void exportModStatistics(const std::string& file_path) const;
    
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
    void dumpModState() const;
    void validateAllMods() const;
    
    // Utility functions
    std::string getModTypeString(VRModType type) const;
    std::string getModStateString(VRModState state) const;
    std::string getModPriorityString(VRModPriority priority) const;
    VRModType stringToModType(const std::string& type_string) const;
    VRModState stringToModState(const std::string& state_string) const;
    VRModPriority stringToModPriority(const std::string& priority_string) const;

protected:
    // Internal mod management
    bool loadModInternal(const std::string& mod_name);
    bool unloadModInternal(const std::string& mod_name);
    bool startModInternal(const std::string& mod_name);
    bool stopModInternal(const std::string& mod_name);
    bool pauseModInternal(const std::string& mod_name);
    bool resumeModInternal(const std::string& mod_name);
    
    // Mod dependency and conflict resolution
    bool resolveModDependenciesInternal(const std::string& mod_name);
    bool detectModConflicts(const std::string& mod_name);
    bool resolveModConflict(const std::string& mod_name, const std::string& conflicting_mod_name);
    
    // Mod validation methods
    VRModValidationResult validateModInternal(const std::string& mod_name) const;
    bool validateModConfiguration(const VRModConfig& config) const;
    bool validateModDependencies(const std::string& mod_name) const;
    bool validateModConflicts(const std::string& mod_name) const;
    
    // Mod compatibility methods
    VRModCompatibility checkModCompatibilityInternal(const std::string& mod_name, const CrossEngineAdapter* adapter) const;
    bool checkModFeatureCompatibility(const std::string& mod_name, const CrossEngineAdapter* adapter) const;
    bool checkModSettingCompatibility(const std::string& mod_name, const CrossEngineAdapter* adapter) const;
    
    // Mod statistics methods
    void updateModSessionStatsInternal(const std::string& mod_name, uint64_t session_duration_seconds);
    void updateModPerformanceStatsInternal(const std::string& mod_name, double fps_impact, double memory_usage);
    void updateModFeatureUsageStatsInternal(const std::string& mod_name, const std::string& feature);
    void updateModErrorStatsInternal(const std::string& mod_name, const std::string& error_type);
    void updateModSettingUsageStatsInternal(const std::string& mod_name, const std::string& setting_name);
    
    // Error management
    void setLastError(const std::string& error);
    void logError(const std::string& error);
    
    // Internal utilities
    std::string generateModId(const VRModConfig& config) const;
    bool shouldLoadMod(const std::string& mod_name) const;
    bool shouldStartMod(const std::string& mod_name) const;
    bool validateConfig() const;

private:
    // Mod storage
    std::unordered_map<std::string, VRModConfig> m_mod_configs;
    std::unordered_map<std::string, VRModStats> m_mod_stats;
    std::unordered_map<std::string, VRModState> m_mod_states;
    std::unordered_map<VRModType, std::vector<std::string>> m_type_mods;
    std::unordered_map<VRModState, std::vector<std::string>> m_state_mods;
    std::unordered_map<VRModPriority, std::vector<std::string>> m_priority_mods;
    
    // Mod callbacks
    std::unordered_map<std::string, VRModInitializationCallback> m_initialization_callbacks;
    std::unordered_map<std::string, VRModCleanupCallback> m_cleanup_callbacks;
    std::unordered_map<std::string, VRModUpdateCallback> m_update_callbacks;
    std::unordered_map<std::string, VRModRenderCallback> m_render_callbacks;
    std::unordered_map<std::string, VRModErrorCallback> m_error_callbacks;
    
    // Mod dependencies and conflicts
    std::unordered_map<std::string, std::vector<std::string>> m_mod_dependencies;
    std::unordered_map<std::string, std::vector<std::string>> m_mod_conflicts;
    
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
    bool m_initialized;
    std::chrono::steady_clock::time_point m_initialization_time;
    
    // Private helper methods
    void initializeDefaultMods();
    void cleanupResources();
    void setModState(const std::string& mod_name, VRModState new_state);
    void updateModType(const std::string& mod_name, VRModType old_type, VRModType new_type);
    void updateModPriority(const std::string& mod_name, VRModPriority old_priority, VRModPriority new_priority);
    void updateModStateWithOldState(const std::string& mod_name, VRModState old_state, VRModState new_state);
    bool validateConfig() const;
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

} // namespace Adapters
} // namespace uevr
