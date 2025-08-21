#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <thread>
#include <condition_variable>

namespace UEVR {

// Forward declarations
class IPlugin;
class Logger;
class ConfigurationManager;

/**
 * @brief Plugin information structure
 */
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string filepath;
    std::string checksum;
    int priority;
    bool enabled;
    bool loaded;
    bool requiresRestart;
    std::chrono::system_clock::time_point lastModified;
    
    PluginInfo() : priority(0), enabled(false), loaded(false), requiresRestart(false) {}
};

/**
 * @brief Plugin dependency structure
 */
struct PluginDependency {
    std::string name;
    std::string minVersion;
    std::string maxVersion;
    bool optional;
    
    PluginDependency() : optional(false) {}
};

/**
 * @brief Plugin manager class
 * 
 * Manages loading, unloading, and lifecycle of all plugins in UEVR-LCX
 */
class PluginManager {
public:
    /**
     * @brief Constructor
     */
    PluginManager();
    
    /**
     * @brief Destructor
     */
    ~PluginManager();
    
    /**
     * @brief Initialize the plugin manager
     * @param configManager Configuration manager instance
     * @param logger Logger instance
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(std::shared_ptr<ConfigurationManager> configManager, std::shared_ptr<Logger> logger);
    
    /**
     * @brief Shutdown the plugin manager
     */
    void Shutdown();
    
    /**
     * @brief Scan for plugins in specified directory
     * @param directory Directory to scan for plugins
     * @return Number of plugins found
     */
    int ScanPlugins(const std::string& directory);
    
    /**
     * @brief Load a plugin by name
     * @param pluginName Name of the plugin to load
     * @return true if plugin loaded successfully, false otherwise
     */
    bool LoadPlugin(const std::string& pluginName);
    
    /**
     * @brief Unload a plugin by name
     * @param pluginName Name of the plugin to unload
     * @return true if plugin unloaded successfully, false otherwise
     */
    bool UnloadPlugin(const std::string& pluginName);
    
    /**
     * @brief Reload a plugin by name
     * @param pluginName Name of the plugin to reload
     * @return true if plugin reloaded successfully, false otherwise
     */
    bool ReloadPlugin(const std::string& pluginName);
    
    /**
     * @brief Enable a plugin
     * @param pluginName Name of the plugin to enable
     * @return true if plugin enabled successfully, false otherwise
     */
    bool EnablePlugin(const std::string& pluginName);
    
    /**
     * @brief Disable a plugin
     * @param pluginName Name of the plugin to disable
     * @return true if plugin disabled successfully, false otherwise
     */
    bool DisablePlugin(const std::string& pluginName);
    
    /**
     * @brief Get plugin by name
     * @param pluginName Name of the plugin
     * @return Plugin instance or nullptr if not found
     */
    std::shared_ptr<IPlugin> GetPlugin(const std::string& pluginName) const;
    
    /**
     * @brief Get plugin information
     * @param pluginName Name of the plugin
     * @return Plugin information or nullptr if not found
     */
    std::shared_ptr<PluginInfo> GetPluginInfo(const std::string& pluginName) const;
    
    /**
     * @brief Get all loaded plugins
     * @return Vector of loaded plugin instances
     */
    std::vector<std::shared_ptr<IPlugin>> GetLoadedPlugins() const;
    
    /**
     * @brief Get all plugin information
     * @return Vector of plugin information
     */
    std::vector<std::shared_ptr<PluginInfo>> GetAllPluginInfo() const;
    
    /**
     * @brief Check if plugin is loaded
     * @param pluginName Name of the plugin
     * @return true if plugin is loaded, false otherwise
     */
    bool IsPluginLoaded(const std::string& pluginName) const;
    
    /**
     * @brief Check if plugin is enabled
     * @param pluginName Name of the plugin
     * @return true if plugin is enabled, false otherwise
     */
    bool IsPluginEnabled(const std::string& pluginName) const;
    
    /**
     * @brief Update all loaded plugins
     * @param deltaTime Time since last update in seconds
     */
    void UpdatePlugins(float deltaTime);
    
    /**
     * @brief Set plugin priority
     * @param pluginName Name of the plugin
     * @param priority New priority value
     * @return true if priority set successfully, false otherwise
     */
    bool SetPluginPriority(const std::string& pluginName, int priority);
    
    /**
     * @brief Get plugin priority
     * @param pluginName Name of the plugin
     * @return Plugin priority or -1 if not found
     */
    int GetPluginPriority(const std::string& pluginName) const;
    
    /**
     * @brief Check plugin dependencies
     * @param pluginName Name of the plugin
     * @return true if all dependencies are satisfied, false otherwise
     */
    bool CheckPluginDependencies(const std::string& pluginName) const;
    
    /**
     * @brief Get plugin dependencies
     * @param pluginName Name of the plugin
     * @return Vector of plugin dependencies
     */
    std::vector<PluginDependency> GetPluginDependencies(const std::string& pluginName) const;
    
    /**
     * @brief Add plugin dependency
     * @param pluginName Name of the plugin
     * @param dependency Dependency to add
     * @return true if dependency added successfully, false otherwise
     */
    bool AddPluginDependency(const std::string& pluginName, const PluginDependency& dependency);
    
    /**
     * @brief Remove plugin dependency
     * @param pluginName Name of the plugin
     * @param dependencyName Name of the dependency to remove
     * @return true if dependency removed successfully, false otherwise
     */
    bool RemovePluginDependency(const std::string& pluginName, const std::string& dependencyName);
    
    /**
     * @brief Hot reload plugins (reload changed plugins)
     * @return Number of plugins reloaded
     */
    int HotReloadPlugins();
    
    /**
     * @brief Enable hot reload monitoring
     * @param enabled true to enable hot reload, false to disable
     */
    void SetHotReloadEnabled(bool enabled);
    
    /**
     * @brief Check if hot reload is enabled
     * @return true if hot reload is enabled, false otherwise
     */
    bool IsHotReloadEnabled() const;
    
    /**
     * @brief Get plugin load order
     * @return Vector of plugin names in load order
     */
    std::vector<std::string> GetPluginLoadOrder() const;
    
    /**
     * @brief Set plugin load order
     * @param loadOrder Vector of plugin names in desired load order
     * @return true if load order set successfully, false otherwise
     */
    bool SetPluginLoadOrder(const std::vector<std::string>& loadOrder);
    
    /**
     * @brief Save plugin configuration
     * @return true if configuration saved successfully, false otherwise
     */
    bool SavePluginConfiguration();
    
    /**
     * @brief Load plugin configuration
     * @return true if configuration loaded successfully, false otherwise
     */
    bool LoadPluginConfiguration();
    
    /**
     * @brief Get plugin statistics
     * @return Plugin statistics as JSON string
     */
    std::string GetPluginStatistics() const;
    
    /**
     * @brief Validate plugin integrity
     * @param pluginName Name of the plugin
     * @return true if plugin is valid, false otherwise
     */
    bool ValidatePluginIntegrity(const std::string& pluginName) const;
    
    /**
     * @brief Get plugin error log
     * @param pluginName Name of the plugin
     * @return Vector of error messages
     */
    std::vector<std::string> GetPluginErrors(const std::string& pluginName) const;
    
    /**
     * @brief Clear plugin error log
     * @param pluginName Name of the plugin
     */
    void ClearPluginErrors(const std::string& pluginName);

private:
    // Internal structures
    struct PluginEntry {
        std::shared_ptr<IPlugin> instance;
        std::shared_ptr<PluginInfo> info;
        std::vector<PluginDependency> dependencies;
        std::vector<std::string> errors;
        void* moduleHandle; // Platform-specific module handle
        std::string checksum;
        
        PluginEntry() : moduleHandle(nullptr) {}
    };
    
    // Member variables
    mutable std::mutex m_mutex;
    std::unordered_map<std::string, std::unique_ptr<PluginEntry>> m_plugins;
    std::vector<std::string> m_loadOrder;
    std::shared_ptr<ConfigurationManager> m_configManager;
    std::shared_ptr<Logger> m_logger;
    std::atomic<bool> m_initialized;
    std::atomic<bool> m_hotReloadEnabled;
    std::thread m_hotReloadThread;
    std::condition_variable m_hotReloadCondition;
    std::atomic<bool> m_shutdownRequested;
    
    // Plugin directories
    std::vector<std::string> m_pluginDirectories;
    
    // Internal methods
    bool LoadPluginFromFile(const std::string& filepath);
    bool UnloadPluginInternal(const std::string& pluginName);
    void* LoadModuleFromFile(const std::string& filepath);
    bool UnloadModule(void* moduleHandle);
    std::string CalculateFileChecksum(const std::string& filepath) const;
    bool ValidatePluginFile(const std::string& filepath) const;
    void SortPluginsByPriority();
    void HotReloadWorker();
    bool CheckPluginChanged(const std::string& pluginName) const;
    void LogPluginEvent(const std::string& pluginName, const std::string& event, const std::string& details = "");
    
    // Platform-specific methods
#ifdef _WIN32
    void* LoadLibraryPlatform(const std::string& filepath);
    bool FreeLibraryPlatform(void* handle);
    void* GetProcAddressPlatform(void* handle, const std::string& symbolName);
#else
    void* LoadLibraryPlatform(const std::string& filepath);
    bool FreeLibraryPlatform(void* handle);
    void* GetProcAddressPlatform(void* handle, const std::string& symbolName);
#endif
    
    // Plugin factory function type
    typedef IPlugin* (*CreatePluginFunc)();
    typedef void (*DestroyPluginFunc)(IPlugin*);
    
    // Configuration keys
    static constexpr const char* CONFIG_SECTION = "PluginManager";
    static constexpr const char* CONFIG_DIRECTORIES = "PluginDirectories";
    static constexpr const char* CONFIG_LOAD_ORDER = "LoadOrder";
    static constexpr const char* CONFIG_HOT_RELOAD = "HotReloadEnabled";
    static constexpr const char* CONFIG_AUTO_LOAD = "AutoLoadPlugins";
};

} // namespace UEVR