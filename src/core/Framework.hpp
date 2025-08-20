#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <filesystem>

namespace UEVR {
namespace Core {

// Forward declarations
class Service;
class Plugin;
class Event;
class EventListener;

// Event types
enum class EventType {
    SystemInitialized,
    SystemShutdown,
    ServiceRegistered,
    ServiceUnregistered,
    PluginLoaded,
    PluginUnloaded,
    ErrorOccurred,
    WarningIssued,
    ConfigurationChanged,
    PerformanceAlert,
    VRSystemReady,
    EngineDetected,
    AdapterLoaded
};

// Event structure
struct Event {
    EventType type;
    std::string source;
    std::string message;
    std::chrono::steady_clock::time_point timestamp;
    std::map<std::string, std::string> data;
    int severity_level;
    std::string stack_trace;
};

// Event listener type
using EventListener = std::function<void(const Event&)>;

// Service interface
class IService {
public:
    virtual ~IService() = default;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::string getName() const = 0;
    virtual bool isInitialized() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual bool isCritical() const = 0;
    virtual void update(float delta_time) = 0;
};

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual void update(float delta_time) = 0;
};

// Configuration manager
class ConfigurationManager {
public:
    bool loadConfiguration(const std::string& config_file);
    bool saveConfiguration(const std::string& config_file);
    bool getValue(const std::string& key, std::string& value) const;
    bool setValue(const std::string& key, const std::string& value);
    bool hasKey(const std::string& key) const;
    void reloadConfiguration();
    std::vector<std::string> getAllKeys() const;

private:
    std::map<std::string, std::string> m_config;
    std::string m_config_file;
    std::chrono::steady_clock::time_point m_last_modified;
    mutable std::mutex m_config_mutex;
};

// Logging manager
class LoggingManager {
public:
    enum class LogLevel {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
        CRITICAL = 5
    };

    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    void log(LogLevel level, const std::string& source, const std::string& message);
    void logEvent(const Event& event);
    void setLogFile(const std::string& log_file);
    void enableConsoleOutput(bool enable);
    void enableFileOutput(bool enable);

private:
    LogLevel m_current_level = LogLevel::INFO;
    std::string m_log_file;
    bool m_console_enabled = true;
    bool m_file_enabled = true;
    std::mutex m_log_mutex;
};

// Performance monitor
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        float cpu_usage;
        float memory_usage;
        float gpu_usage;
        float frame_time;
        float fps;
        uint64_t frame_count;
        std::chrono::steady_clock::time_point last_update;
    };

    void update();
    PerformanceMetrics getCurrentMetrics() const;
    void setTargetFPS(float target_fps);
    void enablePerformanceLogging(bool enable);
    void recordPerformanceEvent(const std::string& event_name, float duration);

private:
    PerformanceMetrics m_current_metrics;
    float m_target_fps = 90.0f;
    bool m_performance_logging_enabled = false;
    std::map<std::string, std::vector<float>> m_performance_history;
    std::mutex m_metrics_mutex;
};

// Main framework class
class Framework {
public:
    // Singleton access
    static Framework& getInstance();
    
    // Lifecycle management
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    bool isShuttingDown() const;
    
    // Service management
    template<typename T>
    T* getService() const;
    
    bool registerService(const std::string& name, std::unique_ptr<IService> service);
    bool unregisterService(const std::string& name);
    bool hasService(const std::string& name) const;
    std::vector<std::string> getServiceNames() const;
    
    // Plugin management
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& name);
    bool reloadPlugin(const std::string& name);
    std::vector<std::string> getLoadedPlugins() const;
    bool isPluginLoaded(const std::string& name) const;
    
    // Event system
    void dispatchEvent(const Event& event);
    void addEventListener(EventType type, EventListener listener);
    void removeEventListener(EventType type, EventListener listener);
    void clearEventListeners(EventType type);
    
    // Configuration
    bool loadConfiguration(const std::string& config_file);
    bool saveConfiguration(const std::string& config_file);
    ConfigurationManager& getConfigurationManager();
    
    // Logging
    void setLogLevel(int level);
    void log(int level, const std::string& message);
    LoggingManager& getLoggingManager();
    
    // Performance
    PerformanceMonitor& getPerformanceMonitor();
    void update(float delta_time);
    
    // System information
    std::string getSystemInfo() const;
    std::string getVersion() const;
    std::string getBuildDate() const;
    
    // Error handling
    void reportError(const std::string& source, const std::string& message);
    void reportWarning(const std::string& source, const std::string& message);
    std::vector<Event> getErrorHistory() const;
    void clearErrorHistory();

private:
    Framework() = default;
    ~Framework() = default;
    Framework(const Framework&) = delete;
    Framework& operator=(const Framework&) = delete;
    
    // Internal state
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_shutting_down{false};
    std::map<std::string, std::unique_ptr<IService>> m_services;
    std::map<std::string, std::unique_ptr<IPlugin>> m_plugins;
    std::map<EventType, std::vector<EventListener>> m_event_listeners;
    
    // Managers
    std::unique_ptr<ConfigurationManager> m_config_manager;
    std::unique_ptr<LoggingManager> m_logging_manager;
    std::unique_ptr<PerformanceMonitor> m_performance_monitor;
    
    // Error history
    std::vector<Event> m_error_history;
    mutable std::mutex m_error_mutex;
    
    // Thread safety
    mutable std::mutex m_services_mutex;
    mutable std::mutex m_plugins_mutex;
    mutable std::mutex m_events_mutex;
    
    // Helper methods
    bool initializeServices();
    void shutdownServices();
    bool initializePlugins();
    void shutdownPlugins();
    bool resolvePluginDependencies(const std::string& plugin_name);
    void logEvent(const Event& event);
    void validateSystemState();
    void cleanupResources();
    
    // System information
    std::string m_version = "2.0.0";
    std::string m_build_date = __DATE__ " " __TIME__;
};

} // namespace Core
} // namespace UEVR
