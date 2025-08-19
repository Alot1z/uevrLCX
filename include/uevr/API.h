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
class CrossEngineAdapterRegistry;
class HookManager;
class VRModManager;

namespace uevr {

// API version information
#define uevr_API_VERSION_MAJOR 1
#define uevr_API_VERSION_MINOR 0
#define uevr_API_VERSION_PATCH 0
#define uevr_API_VERSION_STRING "1.0.0"

// API state enumeration
enum class APIState {
    UNINITIALIZED = 0,
    INITIALIZING,
    INITIALIZED,
    RUNNING,
    PAUSED,
    STOPPING,
    STOPPED,
    ERROR,
    CLEANING_UP
};

// API initialization flags
enum class APIInitFlags : uint32_t {
    NONE = 0x00000000,
    ENABLE_LOGGING = 0x00000001,
    ENABLE_HOOKING = 0x00000002,
    ENABLE_ADAPTERS = 0x00000004,
    ENABLE_MODS = 0x00000008,
    ENABLE_PLUGINS = 0x00000010,
    ENABLE_PERFORMANCE_MONITORING = 0x00000020,
    ENABLE_ERROR_HANDLING = 0x00000040,
    ENABLE_DEBUG_MODE = 0x00000080,
    ENABLE_CONFIG_FILE = 0x00000100,
    ENABLE_CACHE = 0x00000200,
    ENABLE_STATISTICS = 0x00000400,
    ENABLE_CALLBACKS = 0x00000800,
    ENABLE_THREADING = 0x00001000,
    ENABLE_SHARED_MEMORY = 0x00002000,
    ENABLE_IMGUI = 0x00004000,
    ENABLE_NETWORK = 0x00008000,
    ENABLE_CRASH_REPORTING = 0x00010000,
    ENABLE_UPDATE_CHECKING = 0x00020000,
    ENABLE_BACKUP_RESTORE = 0x00040000,
    ENABLE_MIGRATION = 0x00080000,
    DEFAULT = ENABLE_LOGGING | ENABLE_HOOKING | ENABLE_ADAPTERS | ENABLE_MODS | ENABLE_PLUGINS | ENABLE_ERROR_HANDLING,
    ALL = 0xFFFFFFFF
};

// API configuration structure
struct APIConfig {
    // Core settings
    std::string application_name = "uevr";
    std::string application_version = uevr_API_VERSION_STRING;
    std::string config_directory = "config";
    std::string cache_directory = "cache";
    std::string log_directory = "logs";
    std::string plugin_directory = "plugins";
    std::string backup_directory = "backups";
    
    // Feature flags
    APIInitFlags init_flags = APIInitFlags::DEFAULT;
    
    // Performance settings
    uint32_t max_threads = 0; // 0 = auto-detect
    uint32_t max_memory_mb = 0; // 0 = unlimited
    uint32_t performance_threshold_ms = 16; // 60 FPS target
    bool enable_vsync = true;
    bool enable_frame_limiting = true;
    uint32_t max_fps = 60;
    
    // Logging settings
    std::string log_level = "INFO";
    std::string log_format = "detailed";
    bool enable_file_logging = true;
    bool enable_console_logging = true;
    bool enable_remote_logging = false;
    std::string remote_log_endpoint = "";
    uint32_t max_log_file_size_mb = 100;
    uint32_t max_log_files = 10;
    
    // Hook settings
    bool enable_directx_hooks = true;
    bool enable_opengl_hooks = true;
    bool enable_vulkan_hooks = true;
    bool enable_engine_hooks = true;
    bool enable_safe_hooking = true;
    bool enable_hook_validation = true;
    uint32_t hook_timeout_ms = 5000;
    
    // Adapter settings
    bool enable_adapter_auto_discovery = true;
    bool enable_adapter_caching = true;
    bool enable_adapter_validation = true;
    uint32_t adapter_scan_timeout_ms = 30000;
    uint32_t max_adapters = 1000;
    
    // Mod settings
    bool enable_mod_auto_loading = true;
    bool enable_mod_validation = true;
    bool enable_mod_caching = true;
    bool enable_mod_hot_reload = false;
    uint32_t mod_scan_interval_ms = 5000;
    
    // Plugin settings
    bool enable_plugin_auto_loading = true;
    bool enable_plugin_validation = true;
    bool enable_plugin_sandboxing = true;
    bool enable_plugin_hot_reload = false;
    uint32_t plugin_scan_interval_ms = 10000;
    
    // Error handling settings
    bool enable_crash_reporting = true;
    bool enable_error_recovery = true;
    bool enable_graceful_degradation = true;
    uint32_t max_error_retries = 3;
    uint32_t error_recovery_timeout_ms = 10000;
    
    // Network settings
    bool enable_network_features = false;
    std::string network_host = "localhost";
    uint16_t network_port = 8080;
    bool enable_ssl = false;
    std::string ssl_certificate = "";
    std::string ssl_private_key = "";
    
    // Update settings
    bool enable_auto_updates = false;
    std::string update_server = "https://updates.uevr.com";
    bool enable_update_notifications = true;
    bool enable_beta_updates = false;
    
    // Backup settings
    bool enable_auto_backup = true;
    uint32_t backup_interval_hours = 24;
    uint32_t max_backup_files = 10;
    bool enable_backup_compression = true;
    
    // Advanced settings
    bool enable_experimental_features = false;
    bool enable_developer_mode = false;
    bool enable_profiling = false;
    bool enable_memory_tracking = false;
    bool enable_thread_affinity = false;
    std::string cpu_affinity_mask = "";
    
    // Custom settings
    std::unordered_map<std::string, std::string> custom_settings;
};

// API statistics structure
struct APIStats {
    // General statistics
    uint64_t total_runtime_ms = 0;
    uint64_t total_frames_processed = 0;
    uint64_t total_adapters_loaded = 0;
    uint64_t total_mods_loaded = 0;
    uint64_t total_plugins_loaded = 0;
    uint64_t total_hooks_installed = 0;
    
    // Performance statistics
    double average_frame_time_ms = 0.0;
    double min_frame_time_ms = 0.0;
    double max_frame_time_ms = 0.0;
    double average_fps = 0.0;
    double min_fps = 0.0;
    double max_fps = 0.0;
    
    // Memory statistics
    uint64_t current_memory_usage_bytes = 0;
    uint64_t peak_memory_usage_bytes = 0;
    uint64_t total_memory_allocated_bytes = 0;
    uint64_t total_memory_freed_bytes = 0;
    
    // Error statistics
    uint64_t total_errors = 0;
    uint64_t total_warnings = 0;
    uint64_t total_crashes = 0;
    uint64_t total_recoveries = 0;
    
    // Hook statistics
    uint64_t total_hook_calls = 0;
    uint64_t total_hook_failures = 0;
    double average_hook_time_ms = 0.0;
    
    // Adapter statistics
    uint64_t total_adapter_calls = 0;
    uint64_t total_adapter_failures = 0;
    double average_adapter_time_ms = 0.0;
    
    // Mod statistics
    uint64_t total_mod_calls = 0;
    uint64_t total_mod_failures = 0;
    double average_mod_time_ms = 0.0;
    
    // Plugin statistics
    uint64_t total_plugin_calls = 0;
    uint64_t total_plugin_failures = 0;
    double average_plugin_time_ms = 0.0;
    
    // Timestamps
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point last_update_time;
    std::chrono::steady_clock::time_point last_error_time;
    std::chrono::steady_clock::time_point last_warning_time;
};

// API error information
struct APIError {
    std::string error_code;
    std::string error_message;
    std::string error_details;
    std::string error_location;
    std::string error_stack_trace;
    std::chrono::steady_clock::time_point error_time;
    uint32_t error_severity = 0; // 0 = info, 1 = warning, 2 = error, 3 = critical
    bool is_recoverable = false;
    std::vector<std::string> error_context;
    std::unordered_map<std::string, std::string> error_metadata;
};

// API callback types
using APIStateChangeCallback = std::function<void(APIState old_state, APIState new_state)>;
using APIErrorCallback = std::function<void(const APIError& error)>;
using APIFrameCallback = std::function<void(uint64_t frame_number, double frame_time_ms)>;
using APIAdapterCallback = std::function<void(const std::string& adapter_id, bool loaded)>;
using APIModCallback = std::function<void(const std::string& mod_id, bool loaded)>;
using APIPluginCallback = std::function<void(const std::string& plugin_id, bool loaded)>;
using APIHookCallback = std::function<void(const std::string& hook_id, bool installed)>;

/**
 * @brief uevr API Main Interface
 * 
 * This class provides the main public interface for the uevr framework.
 * It manages the overall lifecycle, configuration, and provides access to
 * all major subsystems including adapters, hooks, mods, and plugins.
 * 
 * Features:
 * - Framework initialization and shutdown
 * - Configuration management
 * - State management
 * - Subsystem access
 * - Error handling and recovery
 * - Performance monitoring
 * - Statistics and reporting
 * - Callback management
 * - Threading support
 * - Memory management
 * - Logging and debugging
 * - Plugin system
 * - Hot reloading
 * - Backup and restore
 * - Update management
 * - Network features
 * - Crash reporting
 */
class API {
public:
    // Singleton access
    static API& getInstance();
    
    // Constructor and destructor
    explicit API(const APIConfig& config = APIConfig{});
    ~API();
    
    // Copy and move operations
    API(const API&) = delete;
    API& operator=(const API&) = delete;
    API(API&&) = delete;
    API& operator=(API&&) = delete;

    // Core API management
    bool initialize();
    bool initialize(const APIConfig& config);
    void shutdown();
    bool isInitialized() const;
    APIState getState() const;
    bool isRunning() const;
    bool isPaused() const;
    bool isStopped() const;
    
    // Configuration management
    const APIConfig& getConfig() const;
    void updateConfig(const APIConfig& config);
    void resetConfig();
    bool saveConfig(const std::string& config_path = "") const;
    bool loadConfig(const std::string& config_path);
    bool validateConfig() const;
    
    // State management
    bool start();
    bool stop();
    bool pause();
    bool resume();
    bool restart();
    bool reset();
    
    // Runtime management
    void run();
    void runOnce();
    void runForDuration(std::chrono::milliseconds duration);
    void runUntil(std::function<bool()> stop_condition);
    void setFrameRate(uint32_t fps);
    uint32_t getFrameRate() const;
    void setVSync(bool enable);
    bool isVSyncEnabled() const;
    
    // Subsystem access
    CrossEngineAdapterRegistry* getAdapterRegistry() const;
    HookManager* getHookManager() const;
    VRModManager* getVRModManager() const;
    
    // Adapter management
    bool loadAdapter(const std::string& adapter_id);
    bool unloadAdapter(const std::string& adapter_id);
    bool isAdapterLoaded(const std::string& adapter_id) const;
    std::vector<std::string> getLoadedAdapters() const;
    std::vector<std::string> getAvailableAdapters() const;
    
    // Mod management
    bool loadMod(const std::string& mod_id);
    bool unloadMod(const std::string& mod_id);
    bool isModLoaded(const std::string& mod_id) const;
    std::vector<std::string> getLoadedMods() const;
    std::vector<std::string> getAvailableMods() const;
    
    // Plugin management
    bool loadPlugin(const std::string& plugin_id);
    bool unloadPlugin(const std::string& plugin_id);
    bool isPluginLoaded(const std::string& plugin_id) const;
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getAvailablePlugins() const;
    
    // Hook management
    bool installHook(const std::string& hook_id);
    bool uninstallHook(const std::string& hook_id);
    bool isHookInstalled(const std::string& hook_id) const;
    std::vector<std::string> getInstalledHooks() const;
    std::vector<std::string> getAvailableHooks() const;
    
    // Error handling
    const APIError& getLastError() const;
    void clearLastError();
    std::vector<APIError> getErrorHistory() const;
    bool hasErrors() const;
    bool hasWarnings() const;
    bool hasCriticalErrors() const;
    void enableErrorRecovery(bool enable);
    bool isErrorRecoveryEnabled() const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceThreshold(double threshold_ms);
    double getPerformanceThreshold() const;
    void resetPerformanceStats();
    
    // Statistics and reporting
    const APIStats& getStatistics() const;
    void resetStatistics();
    std::string getStatisticsReport() const;
    void exportStatistics(const std::string& file_path) const;
    void exportConfiguration(const std::string& file_path) const;
    
    // Logging and debugging
    void setLogLevel(const std::string& level);
    std::string getLogLevel() const;
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    void enableProfiling(bool enable);
    bool isProfilingEnabled() const;
    void dumpState() const;
    void validateAllSystems() const;
    
    // Callback management
    void setStateChangeCallback(APIStateChangeCallback callback);
    void setErrorCallback(APIErrorCallback callback);
    void setFrameCallback(APIFrameCallback callback);
    void setAdapterCallback(APIAdapterCallback callback);
    void setModCallback(APIModCallback callback);
    void setPluginCallback(APIPluginCallback callback);
    void setHookCallback(APIHookCallback callback);
    void clearCallbacks();
    
    // Plugin system
    bool scanForPlugins();
    bool reloadPlugins();
    bool enablePluginHotReload(bool enable);
    bool isPluginHotReloadEnabled() const;
    
    // Hot reloading
    bool enableHotReload(bool enable);
    bool isHotReloadEnabled() const;
    bool reloadConfiguration();
    bool reloadAdapters();
    bool reloadMods();
    bool reloadHooks();
    
    // Backup and restore
    bool createBackup(const std::string& backup_name = "");
    bool restoreBackup(const std::string& backup_name);
    std::vector<std::string> getAvailableBackups() const;
    bool deleteBackup(const std::string& backup_name);
    bool enableAutoBackup(bool enable);
    bool isAutoBackupEnabled() const;
    
    // Update management
    bool checkForUpdates();
    bool performUpdate();
    std::string getCurrentVersion() const;
    std::string getLatestVersion() const;
    bool isUpdateAvailable() const;
    void enableAutoUpdates(bool enable);
    bool isAutoUpdateEnabled() const;
    
    // Network features
    bool enableNetworkFeatures(bool enable);
    bool isNetworkEnabled() const;
    bool connectToServer(const std::string& host, uint16_t port);
    bool disconnectFromServer();
    bool isConnected() const;
    
    // Utility functions
    std::string getStateString() const;
    std::string getVersionString() const;
    std::string getBuildInfo() const;
    std::string getSystemInfo() const;
    bool isFeatureEnabled(const std::string& feature_name) const;
    void enableFeature(const std::string& feature_name, bool enable);

protected:
    // Internal API management
    bool initializeInternal();
    void shutdownInternal();
    bool startInternal();
    bool stopInternal();
    bool pauseInternal();
    bool resumeInternal();
    
    // Subsystem management
    bool initializeSubsystems();
    void shutdownSubsystems();
    bool initializeAdapters();
    bool initializeHooks();
    bool initializeMods();
    bool initializePlugins();
    
    // Runtime management
    void runInternal();
    void runFrame();
    void updateStatistics();
    void checkPerformance();
    void handleErrors();
    
    // Configuration management
    bool loadDefaultConfig();
    bool validateConfigInternal() const;
    
    // Error management
    void setLastError(const APIError& error);
    void logError(const APIError& error);
    bool handleError(const APIError& error);
    bool canRecoverFromError(const APIError& error) const;
    
    // Performance management
    void updatePerformanceStats();
    void checkPerformanceThresholds();
    void optimizePerformance();
    
    // Callback management
    void notifyStateChange(APIState old_state, APIState new_state);
    void notifyError(const APIError& error);
    void notifyFrame(uint64_t frame_number, double frame_time_ms);
    void notifyAdapter(const std::string& adapter_id, bool loaded);
    void notifyMod(const std::string& mod_id, bool loaded);
    void notifyPlugin(const std::string& plugin_id, bool loaded);
    void notifyHook(const std::string& hook_id, bool installed);

private:
    // Configuration and state
    APIConfig m_config;
    APIStats m_stats;
    APIState m_state;
    
    // Subsystems
    std::unique_ptr<CrossEngineAdapterRegistry> m_adapter_registry;
    std::unique_ptr<HookManager> m_hook_manager;
    std::unique_ptr<VRModManager> m_vr_mod_manager;
    
    // Callbacks
    APIStateChangeCallback m_state_change_callback;
    APIErrorCallback m_error_callback;
    APIFrameCallback m_frame_callback;
    APIAdapterCallback m_adapter_callback;
    APIModCallback m_mod_callback;
    APIPluginCallback m_plugin_callback;
    APIHookCallback m_hook_callback;
    
    // Runtime state
    bool m_running;
    bool m_paused;
    uint64_t m_frame_number;
    std::chrono::steady_clock::time_point m_last_frame_time;
    std::chrono::steady_clock::time_point m_start_time;
    
    // Performance monitoring
    bool m_performance_monitoring_enabled;
    double m_performance_threshold;
    std::vector<double> m_frame_times;
    size_t m_max_frame_time_samples;
    
    // Error handling
    APIError m_last_error;
    std::vector<APIError> m_error_history;
    bool m_error_recovery_enabled;
    uint32_t m_error_retry_count;
    
    // Features
    bool m_hot_reload_enabled;
    bool m_plugin_hot_reload_enabled;
    bool m_auto_backup_enabled;
    bool m_auto_update_enabled;
    bool m_network_enabled;
    
    // Internal utilities
    void setState(APIState new_state);
    void cleanupResources();
    bool validateStateTransition(APIState new_state) const;
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

// Utility functions
std::string getAPIVersionString();
uint32_t getAPIVersionMajor();
uint32_t getAPIVersionMinor();
uint32_t getAPIVersionPatch();
bool isAPIVersionCompatible(uint32_t major, uint32_t minor, uint32_t patch);

} // namespace uevr