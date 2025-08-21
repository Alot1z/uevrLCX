#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace UEVR {

// Forward declarations
class ConfigurationManager;
class Logger;
class PluginManager;

/**
 * @brief Plugin capability flags
 */
enum class PluginCapability : uint32_t {
    NONE = 0,
    VR_RENDERING = 1 << 0,
    ENGINE_HOOKS = 1 << 1,
    INPUT_HANDLING = 1 << 2,
    AUDIO_PROCESSING = 1 << 3,
    NETWORK_COMMUNICATION = 1 << 4,
    FILE_SYSTEM_ACCESS = 1 << 5,
    REGISTRY_ACCESS = 1 << 6,
    MEMORY_MANIPULATION = 1 << 7,
    THREAD_CREATION = 1 << 8,
    SYSTEM_CALLS = 1 << 9,
    ALL = 0xFFFFFFFF
};

inline PluginCapability operator|(PluginCapability a, PluginCapability b) {
    return static_cast<PluginCapability>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline PluginCapability operator&(PluginCapability a, PluginCapability b) {
    return static_cast<PluginCapability>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool HasCapability(PluginCapability caps, PluginCapability flag) {
    return (caps & flag) == flag;
}

/**
 * @brief Plugin event types
 */
enum class PluginEvent {
    BEFORE_FRAME_RENDER,
    AFTER_FRAME_RENDER,
    BEFORE_VR_SUBMIT,
    AFTER_VR_SUBMIT,
    INPUT_RECEIVED,
    CONFIG_CHANGED,
    ENGINE_DETECTED,
    ENGINE_LOST,
    VR_DEVICE_CONNECTED,
    VR_DEVICE_DISCONNECTED,
    GAME_STARTED,
    GAME_STOPPED,
    PROFILE_LOADED,
    PROFILE_UNLOADED,
    ERROR_OCCURRED,
    WARNING_OCCURRED
};

/**
 * @brief Plugin context structure
 * 
 * Contains shared resources and services available to plugins
 */
struct PluginContext {
    std::shared_ptr<ConfigurationManager> configManager;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<PluginManager> pluginManager;
    
    // VR-related context
    void* vrSystem;
    void* vrDevice;
    void* vrDisplay;
    
    // Engine-related context
    void* engineAdapter;
    void* gameProfile;
    
    // Rendering context
    void* renderDevice;
    void* renderContext;
    void* swapChain;
    
    // Input context
    void* inputManager;
    void* vrControllers;
    
    // Performance context
    void* performanceMonitor;
    
    // Memory context
    void* memoryManager;
    
    PluginContext() : 
        vrSystem(nullptr), vrDevice(nullptr), vrDisplay(nullptr),
        engineAdapter(nullptr), gameProfile(nullptr),
        renderDevice(nullptr), renderContext(nullptr), swapChain(nullptr),
        inputManager(nullptr), vrControllers(nullptr),
        performanceMonitor(nullptr), memoryManager(nullptr) {}
};

/**
 * @brief Plugin event data structure
 */
struct PluginEventData {
    PluginEvent type;
    std::string source;
    std::unordered_map<std::string, std::string> parameters;
    void* userData;
    
    PluginEventData() : type(PluginEvent::ERROR_OCCURRED), userData(nullptr) {}
    PluginEventData(PluginEvent t, const std::string& src) : 
        type(t), source(src), userData(nullptr) {}
};

/**
 * @brief Base interface for all plugins in UEVR-LCX
 * 
 * This interface defines the contract that all plugins must implement
 * to be loaded and managed by the PluginManager.
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    /**
     * @brief Initialize the plugin
     * @param context Plugin context with shared resources
     * @return true if initialization successful, false otherwise
     */
    virtual bool Initialize(const PluginContext& context) = 0;
    
    /**
     * @brief Shutdown the plugin
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Get plugin name
     * @return Plugin name as string
     */
    virtual std::string GetName() const = 0;
    
    /**
     * @brief Get plugin version
     * @return Plugin version as string (e.g., "1.0.0")
     */
    virtual std::string GetVersion() const = 0;
    
    /**
     * @brief Get plugin description
     * @return Plugin description as string
     */
    virtual std::string GetDescription() const = 0;
    
    /**
     * @brief Get plugin author
     * @return Plugin author as string
     */
    virtual std::string GetAuthor() const = 0;
    
    /**
     * @brief Get plugin website/homepage
     * @return Plugin website URL as string
     */
    virtual std::string GetWebsite() const = 0;
    
    /**
     * @brief Get plugin license
     * @return Plugin license as string
     */
    virtual std::string GetLicense() const = 0;
    
    /**
     * @brief Check if plugin is enabled
     * @return true if enabled, false otherwise
     */
    virtual bool IsEnabled() const = 0;
    
    /**
     * @brief Enable or disable the plugin
     * @param enabled New enabled state
     */
    virtual void SetEnabled(bool enabled) = 0;
    
    /**
     * @brief Update plugin (called every frame)
     * @param deltaTime Time since last update in seconds
     */
    virtual void Update(float deltaTime) = 0;
    
    /**
     * @brief Get plugin priority (higher = more important)
     * @return Plugin priority value (0-1000)
     */
    virtual int GetPriority() const = 0;
    
    /**
     * @brief Set plugin priority
     * @param priority New priority value
     */
    virtual void SetPriority(int priority) = 0;
    
    /**
     * @brief Check if plugin requires restart on configuration change
     * @return true if restart required, false otherwise
     */
    virtual bool RequiresRestart() const = 0;
    
    /**
     * @brief Get plugin capabilities
     * @return Plugin capabilities as flags
     */
    virtual PluginCapability GetCapabilities() const = 0;
    
    /**
     * @brief Get minimum required API version
     * @return Minimum API version as string
     */
    virtual std::string GetMinimumAPIVersion() const = 0;
    
    /**
     * @brief Get maximum supported API version
     * @return Maximum API version as string
     */
    virtual std::string GetMaximumAPIVersion() const = 0;
    
    /**
     * @brief Get plugin dependencies
     * @return Vector of dependency names
     */
    virtual std::vector<std::string> GetDependencies() const = 0;
    
    /**
     * @brief Get plugin optional dependencies
     * @return Vector of optional dependency names
     */
    virtual std::vector<std::string> GetOptionalDependencies() const = 0;
    
    /**
     * @brief Get plugin incompatibilities
     * @return Vector of incompatible plugin names
     */
    virtual std::vector<std::string> GetIncompatibilities() const = 0;
    
    /**
     * @brief Handle plugin event
     * @param eventData Event data structure
     * @return true if event was handled, false otherwise
     */
    virtual bool HandleEvent(const PluginEventData& eventData) = 0;
    
    /**
     * @brief Get plugin configuration schema
     * @return Configuration schema as JSON string
     */
    virtual std::string GetConfigurationSchema() const = 0;
    
    /**
     * @brief Get plugin configuration
     * @return Configuration as JSON string
     */
    virtual std::string GetConfiguration() const = 0;
    
    /**
     * @brief Set plugin configuration
     * @param config Configuration as JSON string
     * @return true if configuration was applied successfully, false otherwise
     */
    virtual bool SetConfiguration(const std::string& config) = 0;
    
    /**
     * @brief Validate plugin configuration
     * @param config Configuration as JSON string
     * @return true if configuration is valid, false otherwise
     */
    virtual bool ValidateConfiguration(const std::string& config) const = 0;
    
    /**
     * @brief Reset plugin configuration to defaults
     */
    virtual void ResetConfiguration() = 0;
    
    /**
     * @brief Get plugin statistics
     * @return Statistics as JSON string
     */
    virtual std::string GetStatistics() const = 0;
    
    /**
     * @brief Get plugin error log
     * @return Vector of error messages
     */
    virtual std::vector<std::string> GetErrors() const = 0;
    
    /**
     * @brief Clear plugin error log
     */
    virtual void ClearErrors() = 0;
    
    /**
     * @brief Get plugin warning log
     * @return Vector of warning messages
     */
    virtual std::vector<std::string> GetWarnings() const = 0;
    
    /**
     * @brief Clear plugin warning log
     */
    virtual void ClearWarnings() = 0;
    
    /**
     * @brief Check if plugin is compatible with current system
     * @return true if compatible, false otherwise
     */
    virtual bool IsCompatible() const = 0;
    
    /**
     * @brief Get compatibility issues
     * @return Vector of compatibility issue descriptions
     */
    virtual std::vector<std::string> GetCompatibilityIssues() const = 0;
    
    /**
     * @brief Get plugin performance metrics
     * @return Performance metrics as JSON string
     */
    virtual std::string GetPerformanceMetrics() const = 0;
    
    /**
     * @brief Reset plugin performance metrics
     */
    virtual void ResetPerformanceMetrics() = 0;
    
    /**
     * @brief Get plugin memory usage
     * @return Memory usage in bytes
     */
    virtual size_t GetMemoryUsage() const = 0;
    
    /**
     * @brief Get plugin CPU usage
     * @return CPU usage as percentage (0.0 - 100.0)
     */
    virtual double GetCPUUsage() const = 0;
    
    /**
     * @brief Check if plugin has updates available
     * @return true if updates available, false otherwise
     */
    virtual bool HasUpdatesAvailable() const = 0;
    
    /**
     * @brief Get available update information
     * @return Update information as JSON string
     */
    virtual std::string GetUpdateInformation() const = 0;
    
    /**
     * @brief Apply plugin update
     * @param updateData Update data
     * @return true if update was applied successfully, false otherwise
     */
    virtual bool ApplyUpdate(const std::string& updateData) = 0;
    
    /**
     * @brief Get plugin help text
     * @return Help text as string
     */
    virtual std::string GetHelpText() const = 0;
    
    /**
     * @brief Get plugin command list
     * @return Vector of available commands
     */
    virtual std::vector<std::string> GetCommands() const = 0;
    
    /**
     * @brief Execute plugin command
     * @param command Command name
     * @param parameters Command parameters
     * @return Command result as string
     */
    virtual std::string ExecuteCommand(const std::string& command, const std::vector<std::string>& parameters) = 0;
    
    /**
     * @brief Check if plugin supports hot reload
     * @return true if hot reload is supported, false otherwise
     */
    virtual bool SupportsHotReload() const = 0;
    
    /**
     * @brief Prepare plugin for hot reload
     * @return true if preparation successful, false otherwise
     */
    virtual bool PrepareForHotReload() = 0;
    
    /**
     * @brief Complete hot reload process
     * @return true if hot reload successful, false otherwise
     */
    virtual bool CompleteHotReload() = 0;
};

/**
 * @brief Base plugin class with default implementations
 * 
 * Plugins can inherit from this class to get default implementations
 * for most interface methods, and only override what they need.
 */
class BasePlugin : public IPlugin {
protected:
    std::string m_name;
    std::string m_version;
    std::string m_description;
    std::string m_author;
    std::string m_website;
    std::string m_license;
    bool m_enabled;
    int m_priority;
    PluginCapability m_capabilities;
    PluginContext m_context;
    
    mutable std::vector<std::string> m_errors;
    mutable std::vector<std::string> m_warnings;
    
public:
    BasePlugin(const std::string& name, const std::string& version, const std::string& description);
    virtual ~BasePlugin() = default;
    
    // Default implementations
    std::string GetName() const override { return m_name; }
    std::string GetVersion() const override { return m_version; }
    std::string GetDescription() const override { return m_description; }
    std::string GetAuthor() const override { return m_author; }
    std::string GetWebsite() const override { return m_website; }
    std::string GetLicense() const override { return m_license; }
    
    bool IsEnabled() const override { return m_enabled; }
    void SetEnabled(bool enabled) override { m_enabled = enabled; }
    
    int GetPriority() const override { return m_priority; }
    void SetPriority(int priority) override { m_priority = priority; }
    
    bool RequiresRestart() const override { return false; }
    PluginCapability GetCapabilities() const override { return m_capabilities; }
    
    std::string GetMinimumAPIVersion() const override { return "1.0.0"; }
    std::string GetMaximumAPIVersion() const override { return "1.0.0"; }
    
    std::vector<std::string> GetDependencies() const override { return {}; }
    std::vector<std::string> GetOptionalDependencies() const override { return {}; }
    std::vector<std::string> GetIncompatibilities() const override { return {}; }
    
    bool HandleEvent(const PluginEventData& eventData) override { return false; }
    
    std::string GetConfigurationSchema() const override { return "{}"; }
    std::string GetConfiguration() const override { return "{}"; }
    bool SetConfiguration(const std::string& config) override { return true; }
    bool ValidateConfiguration(const std::string& config) const override { return true; }
    void ResetConfiguration() override {}
    
    std::string GetStatistics() const override { return "{}"; }
    
    std::vector<std::string> GetErrors() const override { return m_errors; }
    void ClearErrors() override { m_errors.clear(); }
    std::vector<std::string> GetWarnings() const override { return m_warnings; }
    void ClearWarnings() override { m_warnings.clear(); }
    
    bool IsCompatible() const override { return true; }
    std::vector<std::string> GetCompatibilityIssues() const override { return {}; }
    
    std::string GetPerformanceMetrics() const override { return "{}"; }
    void ResetPerformanceMetrics() override {}
    
    size_t GetMemoryUsage() const override { return 0; }
    double GetCPUUsage() const override { return 0.0; }
    
    bool HasUpdatesAvailable() const override { return false; }
    std::string GetUpdateInformation() const override { return "{}"; }
    bool ApplyUpdate(const std::string& updateData) override { return false; }
    
    std::string GetHelpText() const override { return "No help available"; }
    std::vector<std::string> GetCommands() const override { return {}; }
    std::string ExecuteCommand(const std::string& command, const std::vector<std::string>& parameters) override { return "Command not found"; }
    
    bool SupportsHotReload() const override { return false; }
    bool PrepareForHotReload() override { return false; }
    bool CompleteHotReload() override { return false; }

protected:
    void LogError(const std::string& message) const;
    void LogWarning(const std::string& message) const;
};

} // namespace UEVR

// Plugin export macros
#ifdef _WIN32
#define PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
#define PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
#endif

// Standard plugin factory functions that must be implemented by all plugins
PLUGIN_EXPORT UEVR::IPlugin* CreatePlugin();
PLUGIN_EXPORT void DestroyPlugin(UEVR::IPlugin* plugin);
PLUGIN_EXPORT const char* GetPluginAPIVersion();
PLUGIN_EXPORT const char* GetPluginName();
PLUGIN_EXPORT const char* GetPluginVersion();
