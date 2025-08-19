#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <filesystem>
#include <nlohmann/json.hpp>

// Forward declarations
class CrossEngineAdapter;

namespace uevr {
namespace Adapters {

// Game type enumeration
enum class GameType {
    UNKNOWN = 0,
    ACTION,
    ADVENTURE,
    RPG,
    FPS,
    TPS,
    STRATEGY,
    SIMULATION,
    SPORTS,
    RACING,
    PUZZLE,
    PLATFORMER,
    STEALTH,
    SURVIVAL_HORROR,
    FIGHTING,
    MUSIC,
    EDUCATIONAL,
    CUSTOM
};

// Game engine enumeration
enum class GameEngine {
    UNKNOWN = 0,
    UNREAL_ENGINE_4,
    UNREAL_ENGINE_5,
    UNITY,
    CRYENGINE,
    LUMBERYARD,
    MT_FRAMEWORK,
    RE_ENGINE,
    REDENGINE4,
    CUSTOM
};

// Game profile state enumeration
enum class GameProfileState {
    UNLOADED = 0,
    LOADING,
    LOADED,
    ACTIVE,
    INACTIVE,
    ERROR,
    CORRUPTED
};

// VR mode enumeration
enum class VRMode {
    DISABLED = 0,
    ENABLED,
    FORCED,
    AUTO_DETECT,
    CUSTOM
};

// Performance profile enumeration
enum class PerformanceProfile {
    LOW = 0,
    MEDIUM,
    HIGH,
    ULTRA,
    CUSTOM
};

// Game profile configuration
struct GameProfileConfig {
    std::string profile_name;
    std::string game_name;
    std::string game_version;
    std::string game_executable;
    std::string game_directory;
    GameType game_type;
    GameEngine game_engine;
    std::string engine_version;
    VRMode vr_mode;
    PerformanceProfile performance_profile;
    bool auto_detect_settings;
    bool enable_vr_modifications;
    bool enable_performance_optimizations;
    bool enable_debug_features;
    bool enable_logging;
    std::string log_file_path;
    std::vector<std::string> required_mods;
    std::vector<std::string> optional_mods;
    std::vector<std::string> incompatible_mods;
    std::unordered_map<std::string, std::string> custom_settings;
    std::unordered_map<std::string, bool> feature_flags;
    std::unordered_map<std::string, double> numeric_settings;
    std::unordered_map<std::string, std::vector<std::string>> list_settings;
};

// Game profile statistics
struct GameProfileStats {
    uint64_t total_sessions = 0;
    uint64_t total_playtime_seconds = 0;
    uint64_t total_vr_sessions = 0;
    uint64_t total_vr_playtime_seconds = 0;
    uint64_t total_modifications = 0;
    uint64_t total_errors = 0;
    double average_session_length = 0.0;
    double average_vr_session_length = 0.0;
    double average_fps = 0.0;
    double average_vr_fps = 0.0;
    std::chrono::steady_clock::time_point first_session_time;
    std::chrono::steady_clock::time_point last_session_time;
    std::chrono::steady_clock::time_point last_vr_session_time;
    std::unordered_map<std::string, uint64_t> mod_usage_counts;
    std::unordered_map<std::string, uint64_t> error_counts;
    std::unordered_map<std::string, double> setting_usage_counts;
};

// Game profile validation result
struct GameProfileValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> info;
    std::chrono::steady_clock::time_point validation_time;
    std::string validation_method;
};

// Game profile compatibility information
struct GameProfileCompatibility {
    bool compatible;
    std::string compatibility_reason;
    double compatibility_score;
    std::vector<std::string> required_features;
    std::vector<std::string> missing_features;
    std::vector<std::string> conflicting_features;
    std::vector<std::string> recommended_settings;
    std::chrono::steady_clock::time_point compatibility_check_time;
};

/**
 * @brief Game Profile
 * 
 * This class manages game-specific configuration and profile information
 * for the uevr framework. It handles game detection, configuration,
 * validation, and compatibility checking.
 * 
 * Features:
 * - Game profile management
 * - Configuration storage and retrieval
 * - Profile validation and compatibility checking
 * - Statistics tracking
 * - JSON import/export
 * - Auto-detection capabilities
 * - Mod compatibility checking
 * - Performance profiling
 * - Error handling and logging
 * - Thread safety
 */
class GameProfile {
public:
    // Constructor and destructor
    explicit GameProfile(const GameProfileConfig& config = GameProfileConfig{});
    ~GameProfile();
    
    // Copy and move operations
    GameProfile(const GameProfile&) = delete;
    GameProfile& operator=(const GameProfile&) = delete;
    GameProfile(GameProfile&&) noexcept;
    GameProfile& operator=(GameProfile&&) noexcept;

    // Core profile management
    bool loadProfile(const std::string& profile_path);
    bool saveProfile(const std::string& profile_path = "") const;
    bool reloadProfile();
    bool resetProfile();
    bool validateProfile();
    
    // Profile information access
    const GameProfileConfig& getConfig() const;
    GameProfileConfig& getConfig();
    const GameProfileStats& getStats() const;
    GameProfileStats& getStats();
    GameProfileState getState() const;
    std::string getProfilePath() const;
    
    // Configuration management
    bool updateConfig(const GameProfileConfig& config);
    bool updateSetting(const std::string& key, const std::string& value);
    bool updateSetting(const std::string& key, bool value);
    bool updateSetting(const std::string& key, double value);
    bool updateSetting(const std::string& key, const std::vector<std::string>& value);
    bool removeSetting(const std::string& key);
    bool hasSetting(const std::string& key) const;
    
    // Setting access
    std::string getSetting(const std::string& key, const std::string& default_value = "") const;
    bool getBoolSetting(const std::string& key, bool default_value = false) const;
    double getNumericSetting(const std::string& key, double default_value = 0.0) const;
    std::vector<std::string> getListSetting(const std::string& key, const std::vector<std::string>& default_value = {}) const;
    
    // Feature flag management
    bool isFeatureEnabled(const std::string& feature) const;
    void enableFeature(const std::string& feature);
    void disableFeature(const std::string& feature);
    void toggleFeature(const std::string& feature);
    std::vector<std::string> getEnabledFeatures() const;
    std::vector<std::string> getDisabledFeatures() const;
    
    // Mod management
    bool addRequiredMod(const std::string& mod_name);
    bool removeRequiredMod(const std::string& mod_name);
    bool addOptionalMod(const std::string& mod_name);
    bool removeOptionalMod(const std::string& mod_name);
    bool addIncompatibleMod(const std::string& mod_name);
    bool removeIncompatibleMod(const std::string& mod_name);
    bool isModRequired(const std::string& mod_name) const;
    bool isModOptional(const std::string& mod_name) const;
    bool isModIncompatible(const std::string& mod_name) const;
    std::vector<std::string> getRequiredMods() const;
    std::vector<std::string> getOptionalMods() const;
    std::vector<std::string> getIncompatibleMods() const;
    
    // Profile validation
    GameProfileValidationResult validateProfileConfiguration() const;
    bool isProfileValid() const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getValidationWarnings() const;
    std::vector<std::string> getValidationInfo() const;
    
    // Compatibility checking
    GameProfileCompatibility checkCompatibility(const CrossEngineAdapter* adapter) const;
    bool isCompatibleWithAdapter(const CrossEngineAdapter* adapter) const;
    double getCompatibilityScore(const CrossEngineAdapter* adapter) const;
    std::vector<std::string> getMissingFeatures(const CrossEngineAdapter* adapter) const;
    std::vector<std::string> getConflictingFeatures(const CrossEngineAdapter* adapter) const;
    
    // Statistics management
    void updateSessionStats(uint64_t session_duration_seconds, bool vr_session = false);
    void updatePerformanceStats(double fps, bool vr_session = false);
    void updateModUsageStats(const std::string& mod_name);
    void updateErrorStats(const std::string& error_type);
    void updateSettingUsageStats(const std::string& setting_name);
    void resetStatistics();
    
    // Auto-detection
    bool autoDetectGameInfo();
    bool autoDetectEngineInfo();
    bool autoDetectVRSettings();
    bool autoDetectPerformanceSettings();
    bool autoDetectMods();
    
    // JSON import/export
    bool importFromJSON(const std::string& json_string);
    bool importFromJSONFile(const std::string& file_path);
    std::string exportToJSON() const;
    bool exportToJSONFile(const std::string& file_path) const;
    
    // Profile comparison
    bool isSimilarTo(const GameProfile& other) const;
    double getSimilarityScore(const GameProfile& other) const;
    std::vector<std::string> getDifferences(const GameProfile& other) const;
    
    // Utility functions
    std::string getGameTypeString() const;
    std::string getGameEngineString() const;
    std::string getVRModeString() const;
    std::string getPerformanceProfileString() const;
    std::string getProfileStateString() const;
    GameType stringToGameType(const std::string& type_string) const;
    GameEngine stringToGameEngine(const std::string& engine_string) const;
    VRMode stringToVRMode(const std::string& mode_string) const;
    PerformanceProfile stringToPerformanceProfile(const std::string& profile_string) const;
    
    // Profile management
    bool backupProfile(const std::string& backup_path) const;
    bool restoreProfile(const std::string& backup_path);
    bool createProfileTemplate(const std::string& template_path) const;
    bool loadProfileTemplate(const std::string& template_path);

protected:
    // Internal profile management
    bool loadProfileInternal(const std::string& profile_path);
    bool saveProfileInternal(const std::string& profile_path) const;
    bool validateProfileInternal();
    
    // Auto-detection methods
    bool autoDetectGameInfoInternal();
    bool autoDetectEngineInfoInternal();
    bool autoDetectVRSettingsInternal();
    bool autoDetectPerformanceSettingsInternal();
    bool autoDetectModsInternal();
    
    // Validation methods
    GameProfileValidationResult validateProfileConfigurationInternal() const;
    bool validateGameInfo() const;
    bool validateEngineInfo() const;
    bool validateVRSettings() const;
    bool validatePerformanceSettings() const;
    bool validateMods() const;
    
    // Compatibility methods
    GameProfileCompatibility checkCompatibilityInternal(const CrossEngineAdapter* adapter) const;
    bool checkFeatureCompatibility(const CrossEngineAdapter* adapter) const;
    bool checkModCompatibility(const CrossEngineAdapter* adapter) const;
    bool checkSettingCompatibility(const CrossEngineAdapter* adapter) const;
    
    // Statistics methods
    void updateSessionStatsInternal(uint64_t session_duration_seconds, bool vr_session);
    void updatePerformanceStatsInternal(double fps, bool vr_session);
    void updateModUsageStatsInternal(const std::string& mod_name);
    void updateErrorStatsInternal(const std::string& error_type);
    void updateSettingUsageStatsInternal(const std::string& setting_name);
    
    // JSON methods
    bool importFromJSONInternal(const nlohmann::json& json_data);
    nlohmann::json exportToJSONInternal() const;
    
    // Utility methods
    std::string generateProfilePath() const;
    bool ensureProfileDirectory() const;
    std::string sanitizeFileName(const std::string& filename) const;

private:
    // Configuration and state
    GameProfileConfig m_config;
    GameProfileStats m_stats;
    GameProfileState m_state;
    std::string m_profile_path;
    std::string m_profile_directory;
    
    // Internal state
    bool m_loaded;
    bool m_modified;
    std::chrono::steady_clock::time_point m_creation_time;
    std::chrono::steady_clock::time_point m_last_modified_time;
    std::chrono::steady_clock::time_point m_last_validation_time;
    
    // Private helper methods
    void initializeDefaultConfig();
    void cleanupResources();
    void setState(GameProfileState new_state);
    void markModified();
    bool validateConfig() const;
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

} // namespace Adapters
} // namespace uevr
