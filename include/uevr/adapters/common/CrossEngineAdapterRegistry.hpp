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
class AdapterFactory;
class GameProfile;

namespace uevr {
namespace Adapters {

// Adapter registry state enumeration
enum class AdapterRegistryState {
    UNINITIALIZED = 0,
    INITIALIZING,
    INITIALIZED,
    SCANNING,
    SCAN_COMPLETE,
    ERROR,
    CLEANING_UP
};

// Adapter discovery method enumeration
enum class AdapterDiscoveryMethod {
    UNKNOWN = 0,
    MANUAL_REGISTRATION,
    AUTO_SCAN,
    PLUGIN_DISCOVERY,
    CONFIG_FILE,
    DYNAMIC_LOADING,
    CUSTOM
};

// Adapter registration information
struct AdapterRegistrationInfo {
    std::string adapter_id;
    std::string adapter_name;
    std::string adapter_version;
    std::string adapter_description;
    std::string adapter_author;
    std::string adapter_website;
    std::vector<std::string> supported_engines;
    std::vector<std::string> supported_games;
    std::vector<std::string> required_features;
    std::vector<std::string> optional_features;
    AdapterDiscoveryMethod discovery_method;
    std::chrono::steady_clock::time_point registration_time;
    std::chrono::steady_clock::time_point last_access_time;
    uint64_t access_count;
    bool enabled;
    bool auto_load;
    std::unordered_map<std::string, std::string> metadata;
};

// Adapter registry configuration
struct AdapterRegistryConfig {
    bool enable_auto_discovery = true;
    bool enable_plugin_scanning = true;
    bool enable_config_file_loading = true;
    bool enable_dynamic_loading = true;
    bool enable_adapter_caching = true;
    bool enable_performance_monitoring = true;
    bool enable_error_logging = true;
    bool enable_debug_mode = false;
    uint32_t max_registered_adapters = 1000;
    uint32_t scan_timeout_ms = 30000;
    uint32_t max_retry_attempts = 3;
    std::string plugin_directory = "plugins/adapters";
    std::string config_file_path = "config/adapters.json";
    std::string cache_file_path = "cache/adapters.cache";
    std::vector<std::string> excluded_adapters;
    std::vector<std::string> preferred_adapters;
    std::vector<std::string> required_adapters;
};

// Adapter registry statistics
struct AdapterRegistryStats {
    uint64_t total_adapters_registered = 0;
    uint64_t total_adapters_unregistered = 0;
    uint64_t total_discovery_attempts = 0;
    uint64_t total_discovery_successes = 0;
    uint64_t total_discovery_failures = 0;
    uint64_t total_plugin_scans = 0;
    uint64_t total_config_loads = 0;
    double average_discovery_time = 0.0;
    double average_registration_time = 0.0;
    std::chrono::steady_clock::time_point first_registration_time;
    std::chrono::steady_clock::time_point last_registration_time;
    std::unordered_map<AdapterDiscoveryMethod, uint64_t> discovery_method_counts;
    std::unordered_map<std::string, uint64_t> engine_support_counts;
    std::unordered_map<std::string, uint64_t> game_support_counts;
    std::unordered_map<std::string, uint64_t> feature_requirement_counts;
};

// Adapter discovery result
struct AdapterDiscoveryResult {
    bool success;
    std::string error_message;
    std::vector<AdapterRegistrationInfo> discovered_adapters;
    AdapterDiscoveryMethod discovery_method;
    double discovery_time_ms;
    std::chrono::steady_clock::time_point discovery_time;
    std::vector<std::string> discovery_errors;
    std::vector<std::string> discovery_warnings;
};

// Adapter compatibility check result
struct AdapterCompatibilityResult {
    bool compatible;
    std::string compatibility_reason;
    double compatibility_score;
    std::vector<std::string> required_features;
    std::vector<std::string> missing_features;
    std::vector<std::string> conflicting_features;
    std::vector<std::string> recommended_settings;
    std::chrono::steady_clock::time_point compatibility_check_time;
};

// Adapter registry callback types
using AdapterRegistrationCallback = std::function<void(const AdapterRegistrationInfo&)>;
using AdapterUnregistrationCallback = std::function<void(const std::string&)>;
using AdapterDiscoveryCallback = std::function<void(const AdapterDiscoveryResult&)>;
using AdapterCompatibilityCallback = std::function<void(const AdapterCompatibilityResult&)>;

/**
 * @brief Cross-Engine Adapter Registry
 * 
 * This class manages the registration and discovery of cross-engine adapters
 * in the uevr framework. It provides a centralized system for adapter management,
 * discovery, and compatibility checking.
 * 
 * Features:
 * - Adapter registration and management
 * - Automatic adapter discovery
 * - Plugin scanning and loading
 * - Configuration file support
 * - Adapter compatibility checking
 * - Performance monitoring and statistics
 * - Error handling and logging
 * - Adapter caching and persistence
 * - Thread-safe operations
 * - Configuration management
 * - Statistics and reporting
 */
class CrossEngineAdapterRegistry {
public:
    // Singleton access
    static CrossEngineAdapterRegistry& getInstance();
    
    // Constructor and destructor
    explicit CrossEngineAdapterRegistry(const AdapterRegistryConfig& config = AdapterRegistryConfig{});
    ~CrossEngineAdapterRegistry();
    
    // Copy and move operations
    CrossEngineAdapterRegistry(const CrossEngineAdapterRegistry&) = delete;
    CrossEngineAdapterRegistry& operator=(const CrossEngineAdapterRegistry&) = delete;
    CrossEngineAdapterRegistry(CrossEngineAdapterRegistry&&) = delete;
    CrossEngineAdapterRegistry& operator=(CrossEngineAdapterRegistry&&) = delete;

    // Core registry management
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    AdapterRegistryState getState() const;
    
    // Adapter registration
    bool registerAdapter(const AdapterRegistrationInfo& adapter_info);
    bool unregisterAdapter(const std::string& adapter_id);
    bool isAdapterRegistered(const std::string& adapter_id) const;
    bool enableAdapter(const std::string& adapter_id);
    bool disableAdapter(const std::string& adapter_id);
    
    // Adapter discovery
    AdapterDiscoveryResult discoverAdapters();
    AdapterDiscoveryResult discoverAdaptersInDirectory(const std::string& directory_path);
    AdapterDiscoveryResult discoverAdaptersFromConfig(const std::string& config_path);
    AdapterDiscoveryResult discoverAdaptersFromPlugins(const std::string& plugin_directory);
    bool scanForAdapters();
    bool reloadAdapters();
    
    // Adapter information and queries
    const AdapterRegistrationInfo* getAdapterInfo(const std::string& adapter_id) const;
    AdapterRegistrationInfo* getAdapterInfo(const std::string& adapter_id);
    std::vector<AdapterRegistrationInfo> getAdaptersByEngine(const std::string& engine_name) const;
    std::vector<AdapterRegistrationInfo> getAdaptersByGame(const std::string& game_name) const;
    std::vector<AdapterRegistrationInfo> getAdaptersByFeature(const std::string& feature_name) const;
    std::vector<AdapterRegistrationInfo> getEnabledAdapters() const;
    std::vector<AdapterRegistrationInfo> getDisabledAdapters() const;
    std::vector<AdapterRegistrationInfo> getAllAdapters() const;
    
    // Adapter state management
    bool isAdapterEnabled(const std::string& adapter_id) const;
    bool isAdapterAutoLoad(const std::string& adapter_id) const;
    size_t getAdapterCount() const;
    size_t getAdapterCountByEngine(const std::string& engine_name) const;
    size_t getAdapterCountByGame(const std::string& game_name) const;
    
    // Adapter compatibility checking
    AdapterCompatibilityResult checkAdapterCompatibility(const std::string& adapter_id, const std::string& game_name) const;
    AdapterCompatibilityResult checkAdapterCompatibility(const std::string& adapter_id, const GameProfile* game_profile) const;
    bool isAdapterCompatibleWithGame(const std::string& adapter_id, const std::string& game_name) const;
    double getAdapterCompatibilityScore(const std::string& adapter_id, const std::string& game_name) const;
    
    // Adapter factory integration
    bool registerAdapterFactory(const std::string& adapter_id, std::function<std::shared_ptr<CrossEngineAdapter>()> factory);
    bool unregisterAdapterFactory(const std::string& adapter_id);
    std::shared_ptr<CrossEngineAdapter> createAdapter(const std::string& adapter_id);
    std::vector<std::string> getAvailableAdapterFactories() const;
    
    // Adapter caching
    void enableAdapterCaching(bool enable);
    bool isAdapterCachingEnabled() const;
    void clearAdapterCache();
    bool saveAdapterCache(const std::string& cache_path = "") const;
    bool loadAdapterCache(const std::string& cache_path = "");
    
    // Configuration management
    const AdapterRegistryConfig& getConfig() const;
    void updateConfig(const AdapterRegistryConfig& config);
    void resetConfig();
    bool saveConfig(const std::string& config_path = "") const;
    bool loadConfig(const std::string& config_path);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceThreshold(double threshold_ms);
    double getPerformanceThreshold() const;
    void resetPerformanceStats();
    
    // Statistics and reporting
    const AdapterRegistryStats& getStatistics() const;
    void resetStatistics();
    std::string getStatisticsReport() const;
    void exportStatistics(const std::string& file_path) const;
    void exportAdapterList(const std::string& file_path) const;
    
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
    void dumpRegistryState() const;
    void validateAllAdapters() const;
    
    // Callback management
    void setRegistrationCallback(AdapterRegistrationCallback callback);
    void setUnregistrationCallback(AdapterUnregistrationCallback callback);
    void setDiscoveryCallback(AdapterDiscoveryCallback callback);
    void setCompatibilityCallback(AdapterCompatibilityCallback callback);
    void clearCallbacks();
    
    // Utility functions
    std::string getRegistryStateString() const;
    std::string getDiscoveryMethodString(AdapterDiscoveryMethod method) const;
    AdapterDiscoveryMethod stringToDiscoveryMethod(const std::string& method_string) const;

protected:
    // Internal registry management
    bool initializeInternal();
    void shutdownInternal();
    bool registerAdapterInternal(const AdapterRegistrationInfo& adapter_info);
    bool unregisterAdapterInternal(const std::string& adapter_id);
    
    // Adapter discovery methods
    AdapterDiscoveryResult discoverAdaptersInternal();
    AdapterDiscoveryResult discoverAdaptersInDirectoryInternal(const std::string& directory_path);
    AdapterDiscoveryResult discoverAdaptersFromConfigInternal(const std::string& config_path);
    AdapterDiscoveryResult discoverAdaptersFromPluginsInternal(const std::string& plugin_directory);
    
    // Adapter factory management
    bool registerAdapterFactoryInternal(const std::string& adapter_id, std::function<std::shared_ptr<CrossEngineAdapter>()> factory);
    bool unregisterAdapterFactoryInternal(const std::string& adapter_id);
    
    // Adapter caching methods
    bool saveAdapterCacheInternal(const std::string& cache_path) const;
    bool loadAdapterCacheInternal(const std::string& cache_path);
    
    // Configuration methods
    bool saveConfigInternal(const std::string& config_path) const;
    bool loadConfigInternal(const std::string& config_path);
    
    // Compatibility checking methods
    AdapterCompatibilityResult checkAdapterCompatibilityInternal(const std::string& adapter_id, const std::string& game_name) const;
    AdapterCompatibilityResult checkAdapterCompatibilityInternal(const std::string& adapter_id, const GameProfile* game_profile) const;
    
    // Statistics management
    void updateRegistrationStatistics(const AdapterRegistrationInfo& adapter_info, double registration_time);
    void updateDiscoveryStatistics(const AdapterDiscoveryResult& result, double discovery_time);
    void updateRegistryStatistics();
    
    // Error management
    void setLastError(const std::string& error);
    void logError(const std::string& error);
    
    // Internal utilities
    std::string generateAdapterId(const AdapterRegistrationInfo& adapter_info) const;
    bool shouldRegisterAdapter(const AdapterRegistrationInfo& adapter_info) const;
    bool validateConfig() const;

private:
    // Configuration and state
    AdapterRegistryConfig m_config;
    AdapterRegistryStats m_stats;
    AdapterRegistryState m_state;
    
    // Adapter storage
    std::unordered_map<std::string, AdapterRegistrationInfo> m_registered_adapters;
    std::unordered_map<std::string, std::vector<std::string>> m_engine_adapters;
    std::unordered_map<std::string, std::vector<std::string>> m_game_adapters;
    std::unordered_map<std::string, std::vector<std::string>> m_feature_adapters;
    std::unordered_map<std::string, std::vector<std::string>> m_enabled_adapters;
    std::unordered_map<std::string, std::vector<std::string>> m_disabled_adapters;
    
    // Adapter factory storage
    std::unordered_map<std::string, std::function<std::shared_ptr<CrossEngineAdapter>()>> m_adapter_factories;
    
    // Callback storage
    AdapterRegistrationCallback m_registration_callback;
    AdapterUnregistrationCallback m_unregistration_callback;
    AdapterDiscoveryCallback m_discovery_callback;
    AdapterCompatibilityCallback m_compatibility_callback;
    
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
    void initializeDefaultAdapters();
    void cleanupResources();
    void setState(AdapterRegistryState new_state);
    void updateEngineAdapters(const std::string& adapter_id, const std::vector<std::string>& old_engines, const std::vector<std::string>& new_engines);
    void updateGameAdapters(const std::string& adapter_id, const std::vector<std::string>& old_games, const std::vector<std::string>& new_games);
    void updateFeatureAdapters(const std::string& adapter_id, const std::vector<std::string>& old_features, const std::vector<std::string>& new_features);
    bool validateConfig() const;
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

} // namespace Adapters
} // namespace uevr
