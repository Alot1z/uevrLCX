/*
 * UEVR RE Engine Adapter
 * 
 * RE Engine (Capcom) adapter for UEVR
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include "uevr/adapters/common/CrossEngineAdapter.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace uevr {
namespace Adapters {

// RE Engine specific types
enum class REEngineVersion {
    UNKNOWN,
    RE_ENGINE_1,    // Resident Evil 7
    RE_ENGINE_2,    // Resident Evil 2 Remake
    RE_ENGINE_3,    // Resident Evil 3 Remake
    RE_ENGINE_4,    // Resident Evil 4 Remake
    RE_ENGINE_5,    // Resident Evil 8, Devil May Cry 5
    RE_ENGINE_6,    // Latest version
    CUSTOM
};

// RE Engine game types
enum class REGameType {
    UNKNOWN,
    RESIDENT_EVIL_7,
    RESIDENT_EVIL_2_REMAKE,
    RESIDENT_EVIL_3_REMAKE,
    RESIDENT_EVIL_4_REMAKE,
    RESIDENT_EVIL_8,
    DEVIL_MAY_CRY_5,
    MONSTER_HUNTER_RISE,
    MONSTER_HUNTER_WORLD,
    STREET_FIGHTER_6,
    CUSTOM
};

// RE Engine renderer types
enum class RERendererType {
    UNKNOWN,
    D3D11,
    D3D12,
    VULKAN,
    CUSTOM
};

// RE Engine specific configuration
struct REEngineConfig : public CrossEngineAdapter::Config {
    REEngineVersion engine_version;
    REGameType game_type;
    RERendererType renderer_type;
    bool enable_motion_blur;
    bool enable_depth_of_field;
    bool enable_ssao;
    bool enable_ssr;
    bool enable_tessellation;
    bool enable_ray_tracing;
    uint32_t shadow_resolution;
    uint32_t texture_quality;
    uint32_t anti_aliasing_quality;
    float fov_multiplier;
    float motion_scale;
    std::unordered_map<std::string, std::string> game_specific_settings;
};

// RE Engine specific VR statistics
struct REVRStats : public CrossEngineAdapter::VRStats {
    uint64_t re_engine_calls;
    uint64_t re_render_calls;
    uint64_t re_input_calls;
    uint64_t re_audio_calls;
    float re_engine_fps;
    float re_render_fps;
    float re_input_latency;
    float re_audio_latency;
    std::chrono::steady_clock::time_point last_re_engine_update;
};

// RE Engine adapter class
class REEngineAdapter : public CrossEngineAdapter {
public:
    // Constructor and destructor
    explicit REEngineAdapter(const REEngineConfig& config);
    virtual ~REEngineAdapter();

    // Copy and move operations
    REEngineAdapter(const REEngineAdapter&) = delete;
    REEngineAdapter& operator=(const REEngineAdapter&) = delete;
    REEngineAdapter(REEngineAdapter&&) noexcept;
    REEngineAdapter& operator=(REEngineAdapter&&) noexcept;

    // RE Engine specific initialization
    bool initializeREEngine();
    bool initializeRERenderer();
    bool initializeREInput();
    bool initializeREAudio();

    // RE Engine detection
    bool detectREEngine();
    REEngineVersion detectREEngineVersion();
    REGameType detectREGameType();
    RERendererType detectRERendererType();

    // RE Engine hooks
    bool installREEngineHooks();
    bool installRERendererHooks();
    bool installREInputHooks();
    bool installREAudioHooks();
    void removeREEngineHooks();

    // RE Engine state management
    bool isREEngineInitialized() const;
    bool isRERendererInitialized() const;
    bool isREInputInitialized() const;
    bool isREAudioInitialized() const;

    // RE Engine data processing
    void processREEngineData();
    void processRERenderData();
    void processREInputData();
    void processREAudioData();

    // RE Engine specific VR methods
    bool initializeREVR();
    void shutdownREVR();
    void updateREVR(float delta_time);
    void renderREVR();

    // RE Engine configuration
    void loadREEngineConfig();
    void saveREEngineConfig();
    void resetREEngineConfig();
    REEngineConfig getREEngineConfig() const;

    // RE Engine game profiles
    bool loadREGameProfile(const std::string& game_name);
    bool saveREGameProfile();
    std::vector<std::string> getSupportedREGames() const;

    // RE Engine performance
    void setREPerformanceProfile(CrossEngineAdapter::PerformanceProfile profile);
    CrossEngineAdapter::PerformanceProfile getREPerformanceProfile() const;
    void optimizeREPerformance();

    // RE Engine debugging
    void enableREDebugMode(bool enable);
    bool isREDebugModeEnabled() const;
    void setREDebugLevel(int level);
    int getREDebugLevel() const;

    // RE Engine logging
    void enableRELogging(bool enable);
    bool isRELoggingEnabled() const;
    void setRELogFile(const std::string& log_file_path);
    std::string getRELogFile() const;

    // RE Engine monitoring
    void enableREMonitoring(bool enable);
    bool isREMonitoringEnabled() const;
    void setREMonitoringInterval(std::chrono::milliseconds interval);
    std::chrono::milliseconds getREMonitoringInterval() const;

    // RE Engine statistics
    REVRStats getREVRStats() const;
    void resetREVRStats();

    // RE Engine specific settings
    template<typename T>
    T getRESetting(const std::string& key, const T& default_value = T{}) const;

    template<typename T>
    void setRESetting(const std::string& key, const T& value);

    bool hasRESetting(const std::string& key) const;
    void removeRESetting(const std::string& key);

    // RE Engine validation
    bool validateREEngine() const;
    std::vector<std::string> getREEngineValidationIssues() const;

    // RE Engine compatibility
    bool isREEngineCompatible() const;
    bool isREGameCompatible(const std::string& game_name) const;
    std::vector<std::string> getRECompatibilityIssues() const;

protected:
    // Override methods from base class
    bool initialize() override;
    void cleanup() override;
    bool detectEngine() override;
    bool initializeEngineHooks() override;
    void removeEngineHooks() override;
    void processEngineData() override;

    // RE Engine specific internal methods
    bool initializeREEngineInternal();
    bool initializeRERendererInternal();
    bool initializeREInputInternal();
    bool initializeREAudioInternal();
    
    bool installREEngineHooksInternal();
    bool installRERendererHooksInternal();
    bool installREInputHooksInternal();
    bool installREAudioHooksInternal();
    
    void processREEngineDataInternal();
    void processRERenderDataInternal();
    void processREInputDataInternal();
    void processREAudioDataInternal();
    
    bool validateREEngineInternal() const;
    bool checkREEngineCompatibilityInternal() const;

private:
    // RE Engine specific state
    REEngineConfig m_re_config;
    REVRStats m_re_vr_stats;
    
    // RE Engine components
    bool m_re_engine_initialized;
    bool m_re_renderer_initialized;
    bool m_re_input_initialized;
    bool m_re_audio_initialized;
    
    // RE Engine hooks
    bool m_re_engine_hooks_installed;
    bool m_re_renderer_hooks_installed;
    bool m_re_input_hooks_installed;
    bool m_re_audio_hooks_installed;
    
    // RE Engine settings
    std::unordered_map<std::string, std::string> m_re_settings;
    
    // RE Engine debugging and monitoring
    bool m_re_debug_mode_enabled;
    int m_re_debug_level;
    bool m_re_logging_enabled;
    std::string m_re_log_file_path;
    bool m_re_monitoring_enabled;
    std::chrono::milliseconds m_re_monitoring_interval;
    
    // RE Engine performance
    CrossEngineAdapter::PerformanceProfile m_re_performance_profile;
    
    // RE Engine validation
    bool m_re_engine_validated;
    std::vector<std::string> m_re_validation_issues;
    bool m_re_engine_compatible;
    std::vector<std::string> m_re_compatibility_issues;
    
    // RE Engine monitoring thread
    std::thread m_re_monitoring_thread;
    std::atomic<bool> m_re_monitoring_running;
    
    // RE Engine synchronization
    mutable std::mutex m_re_config_mutex;
    mutable std::mutex m_re_stats_mutex;
    mutable std::mutex m_re_settings_mutex;
    mutable std::mutex m_re_validation_mutex;
};

// RE Engine adapter factory
class REEngineAdapterFactory {
public:
    // Singleton access
    static REEngineAdapterFactory& getInstance();
    
    // Constructor and destructor
    REEngineAdapterFactory();
    ~REEngineAdapterFactory();
    
    // Copy and move operations
    REEngineAdapterFactory(const REEngineAdapterFactory&) = delete;
    REEngineAdapterFactory& operator=(const REEngineAdapterFactory&) = delete;
    REEngineAdapterFactory(REEngineAdapterFactory&&) = delete;
    REEngineAdapterFactory& operator=(REEngineAdapterFactory&&) = delete;
    
    // Adapter creation
    std::unique_ptr<REEngineAdapter> createAdapter(const REEngineConfig& config);
    std::unique_ptr<REEngineAdapter> createAdapterForGame(const std::string& game_name);
    std::unique_ptr<REEngineAdapter> createAdapterForEngine(REEngineVersion engine_version);
    
    // Adapter discovery
    std::vector<REEngineConfig> getAvailableConfigs() const;
    std::vector<std::string> getSupportedGames() const;
    std::vector<REEngineVersion> getSupportedEngineVersions() const;
    
    // Adapter validation
    bool validateConfig(const REEngineConfig& config) const;
    std::vector<std::string> getConfigValidationIssues(const REEngineConfig& config) const;
    
    // Adapter optimization
    REEngineConfig optimizeConfig(const REEngineConfig& config, 
                                 CrossEngineAdapter::PerformanceProfile profile);
    REEngineConfig createDefaultConfig(REGameType game_type);
    REEngineConfig createPerformanceConfig(REGameType game_type, 
                                          CrossEngineAdapter::PerformanceProfile profile);

protected:
    // Internal factory methods
    std::unique_ptr<REEngineAdapter> createAdapterInternal(const REEngineConfig& config);
    REEngineConfig loadGameConfig(const std::string& game_name) const;
    REEngineConfig loadEngineConfig(REEngineVersion engine_version) const;
    
    // Configuration validation
    bool validateConfigInternal(const REEngineConfig& config) const;
    bool validateGameCompatibility(const REEngineConfig& config) const;
    bool validateEngineCompatibility(const REEngineConfig& config) const;
    
    // Configuration optimization
    REEngineConfig optimizeConfigInternal(const REEngineConfig& config, 
                                         CrossEngineAdapter::PerformanceProfile profile);
    void optimizePerformanceSettings(REEngineConfig& config, 
                                    CrossEngineAdapter::PerformanceProfile profile);
    void optimizeQualitySettings(REEngineConfig& config, 
                                CrossEngineAdapter::PerformanceProfile profile);

private:
    // Available configurations
    std::vector<REEngineConfig> m_available_configs;
    std::vector<std::string> m_supported_games;
    std::vector<REEngineVersion> m_supported_engine_versions;
    
    // Configuration storage
    std::unordered_map<std::string, REEngineConfig> m_game_configs;
    std::unordered_map<REEngineVersion, REEngineConfig> m_engine_configs;
    
    // Factory state
    bool m_initialized;
    mutable std::mutex m_factory_mutex;
};

// Utility functions
namespace REEngineUtils {
    
    // Engine version utilities
    std::string engineVersionToString(REEngineVersion version);
    REEngineVersion stringToEngineVersion(const std::string& version_string);
    
    // Game type utilities
    std::string gameTypeToString(REGameType game_type);
    REGameType stringToGameType(const std::string& game_type_string);
    
    // Renderer type utilities
    std::string rendererTypeToString(RERendererType renderer_type);
    RERendererType stringToRendererType(const std::string& renderer_type_string);
    
    // Configuration utilities
    REEngineConfig createDefaultConfig();
    REEngineConfig createConfigForGame(const std::string& game_name);
    REEngineConfig createConfigForEngine(REEngineVersion engine_version);
    
    // Validation utilities
    bool isValidREEngineConfig(const REEngineConfig& config);
    std::vector<std::string> validateREEngineConfig(const REEngineConfig& config);
    
    // Compatibility utilities
    bool isREEngineCompatible(const REEngineConfig& config);
    bool isREGameCompatible(const REEngineConfig& config, const std::string& game_name);
    std::vector<std::string> getRECompatibilityIssues(const REEngineConfig& config);
    
    // Performance utilities
    REEngineConfig optimizeREConfig(const REEngineConfig& config, 
                                   CrossEngineAdapter::PerformanceProfile profile);
    void adjustREQualitySettings(REEngineConfig& config, 
                                CrossEngineAdapter::PerformanceProfile profile);
    void adjustREPerformanceSettings(REEngineConfig& config, 
                                    CrossEngineAdapter::PerformanceProfile profile);
}

// Global RE Engine adapter factory instance
extern REEngineAdapterFactory* g_re_engine_adapter_factory;

// Convenience functions
std::unique_ptr<REEngineAdapter> createREEngineAdapter(const REEngineConfig& config);
std::unique_ptr<REEngineAdapter> createREEngineAdapterForGame(const std::string& game_name);
std::unique_ptr<REEngineAdapter> createREEngineAdapterForEngine(REEngineVersion engine_version);

REEngineConfig createREEngineConfig(const std::string& game_name);
REEngineConfig createREEngineConfig(REEngineVersion engine_version);
REEngineConfig createOptimizedREEngineConfig(const std::string& game_name, 
                                            CrossEngineAdapter::PerformanceProfile profile);

} // namespace Adapters
} // namespace uevr
