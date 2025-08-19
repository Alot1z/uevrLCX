#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <type_traits>

// Forward declarations
class CrossEngineAdapter;
class GameProfile;

namespace uevr {
namespace Adapters {

// Adapter type enumeration
enum class AdapterType {
    UNKNOWN = 0,
    MT_FRAMEWORK,
    RE_ENGINE,
    REDENGINE4,
    UNREAL_ENGINE,
    UNITY_ENGINE,
    CRYENGINE,
    LUMBERYARD,
    CUSTOM
};

// Adapter state enumeration
enum class AdapterState {
    UNINITIALIZED = 0,
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

// Adapter priority enumeration
enum class AdapterPriority {
    LOWEST = 0,
    LOW = 1,
    NORMAL = 2,
    HIGH = 3,
    HIGHEST = 4,
    CRITICAL = 5
};

// Adapter creation result
struct AdapterCreationResult {
    bool success;
    std::string error_message;
    std::shared_ptr<CrossEngineAdapter> adapter;
    AdapterType detected_type;
    std::string detected_version;
    double detection_time_ms;
    std::chrono::steady_clock::time_point creation_time;
};

// Adapter factory configuration
struct AdapterFactoryConfig {
    bool enable_auto_detection = true;
    bool enable_fallback_adapters = true;
    bool enable_custom_adapters = true;
    bool enable_adapter_caching = true;
    bool enable_performance_monitoring = true;
    bool enable_error_logging = true;
    bool enable_debug_mode = false;
    uint32_t max_cached_adapters = 100;
    uint32_t adapter_timeout_ms = 10000;
    uint32_t max_retry_attempts = 3;
    double performance_threshold_ms = 100.0;
    std::string log_file_path = "uevr_adapter_factory.log";
    std::vector<std::string> excluded_processes;
    std::vector<std::string> excluded_modules;
    std::vector<std::string> excluded_adapters;
    std::vector<std::string> preferred_adapters;
    std::vector<std::string> fallback_adapters;
};

// Adapter factory statistics
struct AdapterFactoryStats {
    uint64_t total_adapters_created = 0;
    uint64_t total_adapters_destroyed = 0;
    uint64_t total_creation_attempts = 0;
    uint64_t total_creation_failures = 0;
    uint64_t total_auto_detections = 0;
    uint64_t total_fallback_creations = 0;
    double average_creation_time = 0.0;
    double average_detection_time = 0.0;
    std::chrono::steady_clock::time_point first_creation_time;
    std::chrono::steady_clock::time_point last_creation_time;
    std::unordered_map<AdapterType, uint64_t> adapter_type_creation_counts;
    std::unordered_map<std::string, uint64_t> process_creation_counts;
    std::unordered_map<std::string, uint64_t> module_creation_counts;
    std::unordered_map<std::string, uint64_t> error_counts;
};

// Adapter detection information
struct AdapterDetectionInfo {
    AdapterType detected_type;
    std::string detected_version;
    std::string detected_name;
    std::string detection_method;
    double confidence_score;
    std::chrono::steady_clock::time_point detection_time;
    std::vector<std::string> detection_evidence;
    std::unordered_map<std::string, std::string> detection_metadata;
};

// Adapter factory callback types
using AdapterCreationCallback = std::function<void(const AdapterCreationResult&)>;
using AdapterDestructionCallback = std::function<void(const std::string&)>;
using AdapterDetectionCallback = std::function<void(const AdapterDetectionInfo&)>;

/**
 * @brief Adapter Factory
 * 
 * This class provides a factory pattern implementation for creating cross-engine
 * adapters in the uevr framework. It handles adapter detection, creation,
 * caching, and lifecycle management.
 * 
 * Features:
 * - Automatic adapter detection
 * - Factory pattern implementation
 * - Adapter caching and reuse
 * - Fallback adapter support
 * - Performance monitoring
 * - Error handling and logging
 * - Custom adapter registration
 * - Thread safety
 * - Configuration management
 * - Statistics and reporting
 */
class AdapterFactory {
public:
    // Singleton access
    static AdapterFactory& getInstance();
    
    // Constructor and destructor
    explicit AdapterFactory(const AdapterFactoryConfig& config = AdapterFactoryConfig{});
    ~AdapterFactory();
    
    // Copy and move operations
    AdapterFactory(const AdapterFactory&) = delete;
    AdapterFactory& operator=(const AdapterFactory&) = delete;
    AdapterFactory(AdapterFactory&&) = delete;
    AdapterFactory& operator=(AdapterFactory&&) = delete;

    // Core factory methods
    AdapterCreationResult createAdapter(const std::string& process_name);
    AdapterCreationResult createAdapter(const std::string& process_name, AdapterType preferred_type);
    AdapterCreationResult createAdapter(const std::string& process_name, const std::string& preferred_adapter);
    bool destroyAdapter(const std::string& adapter_id);
    bool destroyAllAdapters();
    
    // Adapter detection
    AdapterDetectionInfo detectAdapter(const std::string& process_name);
    AdapterDetectionInfo detectAdapter(const std::string& process_name, AdapterType expected_type);
    std::vector<AdapterDetectionInfo> detectAllPossibleAdapters(const std::string& process_name);
    bool isAdapterSupported(const std::string& process_name) const;
    bool isAdapterSupported(AdapterType type) const;
    
    // Adapter management
    std::shared_ptr<CrossEngineAdapter> getAdapter(const std::string& adapter_id) const;
    std::vector<std::shared_ptr<CrossEngineAdapter>> getAllAdapters() const;
    std::vector<std::shared_ptr<CrossEngineAdapter>> getAdaptersByType(AdapterType type) const;
    std::vector<std::shared_ptr<CrossEngineAdapter>> getAdaptersByState(AdapterState state) const;
    size_t getAdapterCount() const;
    size_t getAdapterCountByType(AdapterType type) const;
    size_t getAdapterCountByState(AdapterState state) const;
    
    // Custom adapter registration
    bool registerCustomAdapter(const std::string& name, std::function<std::shared_ptr<CrossEngineAdapter>()> creator);
    bool unregisterCustomAdapter(const std::string& name);
    bool isCustomAdapterRegistered(const std::string& name) const;
    std::vector<std::string> getRegisteredCustomAdapters() const;
    
    // Adapter caching
    void enableAdapterCaching(bool enable);
    bool isAdapterCachingEnabled() const;
    void clearAdapterCache();
    void setMaxCachedAdapters(uint32_t max_count);
    uint32_t getMaxCachedAdapters() const;
    size_t getCachedAdapterCount() const;
    
    // Fallback adapter management
    void enableFallbackAdapters(bool enable);
    bool isFallbackAdaptersEnabled() const;
    void addFallbackAdapter(const std::string& adapter_name);
    void removeFallbackAdapter(const std::string& adapter_name);
    void clearFallbackAdapters();
    std::vector<std::string> getFallbackAdapters() const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceThreshold(double threshold_ms);
    double getPerformanceThreshold() const;
    void resetPerformanceStats();
    
    // Statistics and reporting
    const AdapterFactoryStats& getStatistics() const;
    void resetStatistics();
    std::string getStatisticsReport() const;
    void exportStatistics(const std::string& file_path) const;
    void exportAdapterList(const std::string& file_path) const;
    
    // Configuration management
    const AdapterFactoryConfig& getConfig() const;
    void updateConfig(const AdapterFactoryConfig& config);
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
    void dumpFactoryState() const;
    void validateAllAdapters() const;
    
    // Callback management
    void setCreationCallback(AdapterCreationCallback callback);
    void setDestructionCallback(AdapterDestructionCallback callback);
    void setDetectionCallback(AdapterDetectionCallback callback);
    void clearCallbacks();
    
    // Utility functions
    std::string getAdapterTypeString(AdapterType type) const;
    std::string getAdapterStateString(AdapterState state) const;
    std::string getAdapterPriorityString(AdapterPriority priority) const;
    AdapterType stringToAdapterType(const std::string& type_string) const;
    AdapterState stringToAdapterState(const std::string& state_string) const;
    AdapterPriority stringToAdapterPriority(const std::string& priority_string) const;

protected:
    // Internal factory methods
    AdapterCreationResult createAdapterInternal(const std::string& process_name, AdapterType preferred_type);
    AdapterCreationResult createAdapterInternal(const std::string& process_name, const std::string& preferred_adapter);
    bool destroyAdapterInternal(const std::string& adapter_id);
    
    // Adapter detection methods
    AdapterDetectionInfo detectAdapterInternal(const std::string& process_name);
    AdapterDetectionInfo detectAdapterInternal(const std::string& process_name, AdapterType expected_type);
    std::vector<AdapterDetectionInfo> detectAllPossibleAdaptersInternal(const std::string& process_name);
    
    // Custom adapter management
    bool registerCustomAdapterInternal(const std::string& name, std::function<std::shared_ptr<CrossEngineAdapter>()> creator);
    bool unregisterCustomAdapterInternal(const std::string& name);
    
    // Adapter caching methods
    void cacheAdapter(const std::string& adapter_id, std::shared_ptr<CrossEngineAdapter> adapter);
    std::shared_ptr<CrossEngineAdapter> getCachedAdapter(const std::string& adapter_id) const;
    void removeCachedAdapter(const std::string& adapter_id);
    void cleanupExpiredCache();
    
    // Fallback adapter methods
    AdapterCreationResult createFallbackAdapter(const std::string& process_name);
    std::vector<std::string> getAvailableFallbackAdapters(const std::string& process_name) const;
    
    // Statistics management
    void updateCreationStatistics(const AdapterCreationResult& result, double creation_time);
    void updateDetectionStatistics(const AdapterDetectionInfo& info, double detection_time);
    void updateFactoryStatistics();
    
    // Error management
    void setLastError(const std::string& error);
    void logError(const std::string& error);
    
    // Internal utilities
    std::string generateAdapterId(const std::string& process_name, AdapterType type) const;
    bool shouldCreateAdapter(const std::string& process_name) const;
    bool shouldUseCachedAdapter(const std::string& process_name) const;
    bool validateConfig() const;

private:
    // Configuration and state
    AdapterFactoryConfig m_config;
    AdapterFactoryStats m_stats;
    
    // Adapter storage
    std::unordered_map<std::string, std::shared_ptr<CrossEngineAdapter>> m_active_adapters;
    std::unordered_map<std::string, std::shared_ptr<CrossEngineAdapter>> m_cached_adapters;
    std::unordered_map<AdapterType, std::vector<std::string>> m_type_adapters;
    std::unordered_map<AdapterState, std::vector<std::string>> m_state_adapters;
    
    // Custom adapter storage
    std::unordered_map<std::string, std::function<std::shared_ptr<CrossEngineAdapter>()>> m_custom_adapters;
    
    // Fallback adapter storage
    std::vector<std::string> m_fallback_adapters;
    
    // Callback storage
    AdapterCreationCallback m_creation_callback;
    AdapterDestructionCallback m_destruction_callback;
    AdapterDetectionCallback m_detection_callback;
    
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
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

} // namespace Adapters
} // namespace uevr
