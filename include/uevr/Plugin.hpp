/*
This file (Plugin.hpp) is licensed under the MIT license and is separate from the rest of the uevr codebase.

Copyright (c) 2023 praydog

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Helper header to easily instantiate a plugin
// and get some initial callbacks setup
// the user can inherit from the Plugin class
// and set uevr::g_plugin to their plugin instance
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace uevr {

// Forward declarations
class Framework;
class CrossEngineAdapter;

/**
 * @brief Plugin interface for uevr system
 * 
 * This class provides the main entry point for uevr plugins,
 * managing initialization, configuration, and lifecycle.
 */
class Plugin {
public:
    /**
     * @brief Plugin configuration structure
     */
    struct Config {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string website;
        bool enabled = true;
        bool auto_load = false;
        std::vector<std::string> dependencies;
        std::unordered_map<std::string, std::string> settings;
    };

    /**
     * @brief Plugin state enumeration
     */
    enum class State {
        UNLOADED,       ///< Plugin has not been loaded
        LOADING,        ///< Plugin is currently loading
        LOADED,         ///< Plugin has been loaded successfully
        INITIALIZING,   ///< Plugin is initializing
        RUNNING,        ///< Plugin is running normally
        PAUSED,         ///< Plugin is paused
        ERROR,          ///< Plugin encountered an error
        UNLOADING       ///< Plugin is being unloaded
    };

    /**
     * @brief Plugin priority levels
     */
    enum class Priority {
        LOW = 0,        ///< Low priority (loaded last)
        NORMAL = 100,   ///< Normal priority
        HIGH = 200,     ///< High priority (loaded first)
        CRITICAL = 300  ///< Critical priority (always loaded first)
    };

public:
    /**
     * @brief Constructor
     * @param config Plugin configuration
     */
    explicit Plugin(const Config& config);
    
    /**
     * @brief Destructor
     */
    virtual ~Plugin();

    // Disable copy constructor and assignment
    Plugin(const Plugin&) = delete;
    Plugin& operator=(const Plugin&) = delete;

    // Allow move constructor and assignment
    Plugin(Plugin&&) noexcept;
    Plugin& operator=(Plugin&&) noexcept;

    /**
     * @brief Initialize the plugin
     * @return true if initialization was successful
     */
    virtual bool initialize();

    /**
     * @brief Start the plugin
     * @return true if start was successful
     */
    virtual bool start();

    /**
     * @brief Stop the plugin
     */
    virtual void stop();

    /**
     * @brief Pause the plugin
     */
    virtual void pause();

    /**
     * @brief Resume the plugin
     */
    virtual void resume();

    /**
     * @brief Cleanup plugin resources
     */
    virtual void cleanup();

    /**
     * @brief Update plugin logic (called each frame)
     * @param delta_time Time since last update in seconds
     */
    virtual void update(float delta_time);

    /**
     * @brief Render plugin UI (called each frame)
     */
    virtual void render();

    /**
     * @brief Handle plugin-specific messages
     * @param message_id Message identifier
     * @param data Message data
     * @return true if message was handled
     */
    virtual bool handleMessage(uint32_t message_id, const void* data);

    // Configuration access
    const Config& getConfig() const { return m_config; }
    Config& getConfig() { return m_config; }

    // State management
    State getState() const { return m_state; }
    bool isEnabled() const { return m_config.enabled; }
    bool isLoaded() const { return m_state >= State::LOADED; }
    bool isRunning() const { return m_state == State::RUNNING; }
    bool isPaused() const { return m_state == State::PAUSED; }
    bool hasError() const { return m_state == State::ERROR; }

    // Framework access
    Framework* getFramework() const { return m_framework; }
    void setFramework(Framework* framework) { m_framework = framework; }

    // Priority management
    Priority getPriority() const { return m_priority; }
    void setPriority(Priority priority) { m_priority = priority; }

    // Error handling
    const std::string& getLastError() const { return m_last_error; }
    void setLastError(const std::string& error);

    // Lifecycle events
    virtual void onLoad() {}
    virtual void onUnload() {}
    virtual void onStart() {}
    virtual void onStop() {}
    virtual void onPause() {}
    virtual void onResume() {}
    virtual void onError(const std::string& error) {}

    // Settings management
    template<typename T>
    T getSetting(const std::string& key, const T& default_value = T{}) const;

    template<typename T>
    void setSetting(const std::string& key, const T& value);

    bool hasSetting(const std::string& key) const;
    void removeSetting(const std::string& key);

    // Dependency management
    bool checkDependencies() const;
    void addDependency(const std::string& dependency);
    void removeDependency(const std::string& dependency);

    // Plugin information
    const std::string& getName() const { return m_config.name; }
    const std::string& getVersion() const { return m_config.version; }
    const std::string& getDescription() const { return m_config.description; }
    const std::string& getAuthor() const { return m_config.author; }
    const std::string& getWebsite() const { return m_config.website; }

protected:
    // Protected methods for derived classes
    void setState(State state);
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);

    // Framework integration
    bool registerWithFramework();
    void unregisterFromFramework();

private:
    Config m_config;
    State m_state;
    Priority m_priority;
    Framework* m_framework;
    std::string m_last_error;
    bool m_is_initialized;
    bool m_is_running;
    float m_accumulated_time;
    
    // Internal state
    void updateState();
    bool validateConfig() const;
    void loadSettings();
    void saveSettings();
};

/**
 * @brief Plugin manager for handling multiple plugins
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

    // Plugin management
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& name);
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool reloadPlugin(const std::string& name);

    // Plugin access
    Plugin* getPlugin(const std::string& name) const;
    std::vector<Plugin*> getAllPlugins() const;
    std::vector<Plugin*> getEnabledPlugins() const;
    std::vector<Plugin*> getPluginsByPriority(Plugin::Priority priority) const;

    // Plugin lifecycle
    bool initializeAllPlugins();
    void startAllPlugins();
    void stopAllPlugins();
    void pauseAllPlugins();
    void resumeAllPlugins();
    void cleanupAllPlugins();

    // Plugin updates
    void updateAllPlugins(float delta_time);
    void renderAllPlugins();

    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory);
    bool scanForPlugins(const std::string& directory);

    // Configuration
    bool loadPluginConfig(const std::string& path);
    bool savePluginConfig(const std::string& path);
    void setPluginDirectory(const std::string& directory);

    // Error handling
    std::vector<std::string> getPluginErrors() const;
    void clearPluginErrors();

private:
    std::unordered_map<std::string, std::unique_ptr<Plugin>> m_plugins;
    std::string m_plugin_directory;
    std::vector<std::string> m_plugin_errors;
    
    bool validatePlugin(Plugin* plugin) const;
    void sortPluginsByPriority();
};

// Template implementations
template<typename T>
T Plugin::getSetting(const std::string& key, const T& default_value) const {
    auto it = m_config.settings.find(key);
    if (it != m_config.settings.end()) {
        // Basic type conversion - can be extended for complex types
        if constexpr (std::is_same_v<T, std::string>) {
            return it->second;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::stoi(it->second);
        } else if constexpr (std::is_same_v<T, float>) {
            return std::stof(it->second);
        } else if constexpr (std::is_same_v<T, bool>) {
            return (it->second == "true" || it->second == "1");
        }
    }
    return default_value;
}

template<typename T>
void Plugin::setSetting(const std::string& key, const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        m_config.settings[key] = value;
    } else if constexpr (std::is_same_v<T, int>) {
        m_config.settings[key] = std::to_string(value);
    } else if constexpr (std::is_same_v<T, float>) {
        m_config.settings[key] = std::to_string(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        m_config.settings[key] = value ? "true" : "false";
    }
}

} // namespace uevr

// Backward compatibility alias
namespace uevr = uevr;
