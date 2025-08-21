#include "PluginManager.hpp"
#include "IPlugin.hpp"
#include "Logger.hpp"
#include "ConfigurationManager.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>

#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <dlfcn.h>
#include <sys/stat.h>
#endif

namespace UEVR {

PluginManager::PluginManager()
    : m_initialized(false)
    , m_hotReloadEnabled(false)
    , m_shutdownRequested(false)
{
}

PluginManager::~PluginManager() {
    Shutdown();
}

bool PluginManager::Initialize(std::shared_ptr<ConfigurationManager> configManager, std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_initialized.load()) {
        return true; // Already initialized
    }
    
    m_configManager = configManager;
    m_logger = logger;
    
    if (!m_configManager || !m_logger) {
        return false;
    }
    
    // Load configuration
    LoadPluginConfiguration();
    
    // Initialize plugin directories
    auto directories = m_configManager->GetStringArray(CONFIG_SECTION, CONFIG_DIRECTORIES);
    if (directories.empty()) {
        // Default plugin directories
        m_pluginDirectories = { "plugins", "mods", "extensions" };
    } else {
        m_pluginDirectories = directories;
    }
    
    // Create plugin directories if they don't exist
    for (const auto& dir : m_pluginDirectories) {
        std::filesystem::create_directories(dir);
    }
    
    // Enable hot reload if configured
    bool hotReloadEnabled = m_configManager->GetBool(CONFIG_SECTION, CONFIG_HOT_RELOAD, false);
    SetHotReloadEnabled(hotReloadEnabled);
    
    // Auto-load plugins if configured
    bool autoLoad = m_configManager->GetBool(CONFIG_SECTION, CONFIG_AUTO_LOAD, true);
    if (autoLoad) {
        // Scan and load plugins from all directories
        int totalPlugins = 0;
        for (const auto& dir : m_pluginDirectories) {
            totalPlugins += ScanPlugins(dir);
        }
        
        if (m_logger) {
            m_logger->Info("PluginManager", "Found {} plugins in {} directories", totalPlugins, m_pluginDirectories.size());
        }
        
        // Load plugins in specified order
        auto loadOrder = GetPluginLoadOrder();
        for (const auto& pluginName : loadOrder) {
            LoadPlugin(pluginName);
        }
    }
    
    m_initialized = true;
    
    if (m_logger) {
        m_logger->Info("PluginManager", "Initialized successfully with {} loaded plugins", GetLoadedPlugins().size());
    }
    
    return true;
}

void PluginManager::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized.load()) {
        return; // Already shutdown
    }
    
    // Request shutdown
    m_shutdownRequested = true;
    
    // Stop hot reload thread
    if (m_hotReloadThread.joinable()) {
        m_hotReloadCondition.notify_all();
        m_hotReloadThread.join();
    }
    
    // Save plugin configuration
    SavePluginConfiguration();
    
    // Shutdown all plugins in reverse order
    auto loadedPlugins = GetLoadedPlugins();
    std::reverse(loadedPlugins.begin(), loadedPlugins.end());
    
    for (auto& plugin : loadedPlugins) {
        if (plugin && plugin->IsEnabled()) {
            try {
                plugin->Shutdown();
                if (m_logger) {
                    m_logger->Info("PluginManager", "Shutdown plugin: {}", plugin->GetName());
                }
            } catch (const std::exception& e) {
                if (m_logger) {
                    m_logger->Error("PluginManager", "Error shutting down plugin {}: {}", plugin->GetName(), e.what());
                }
            }
        }
    }
    
    // Unload all plugins
    std::vector<std::string> pluginNames;
    for (const auto& pair : m_plugins) {
        pluginNames.push_back(pair.first);
    }
    
    for (const auto& pluginName : pluginNames) {
        UnloadPluginInternal(pluginName);
    }
    
    m_plugins.clear();
    m_loadOrder.clear();
    
    m_initialized = false;
    
    if (m_logger) {
        m_logger->Info("PluginManager", "Shutdown completed");
    }
}

int PluginManager::ScanPlugins(const std::string& directory) {
    if (!std::filesystem::exists(directory)) {
        return 0;
    }
    
    int pluginsFound = 0;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                const auto& path = entry.path();
                
                // Check for plugin files (DLL on Windows, SO on Linux)
#ifdef _WIN32
                if (path.extension() == ".dll") {
#else
                if (path.extension() == ".so") {
#endif
                    if (ValidatePluginFile(path.string())) {
                        if (LoadPluginFromFile(path.string())) {
                            pluginsFound++;
                        }
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        if (m_logger) {
            m_logger->Error("PluginManager", "Error scanning directory {}: {}", directory, e.what());
        }
    }
    
    return pluginsFound;
}

bool PluginManager::LoadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_plugins.find(pluginName);
    if (it == m_plugins.end()) {
        if (m_logger) {
            m_logger->Warning("PluginManager", "Plugin not found: {}", pluginName);
        }
        return false;
    }
    
    auto& entry = it->second;
    if (entry->info->loaded) {
        return true; // Already loaded
    }
    
    // Check dependencies
    if (!CheckPluginDependencies(pluginName)) {
        if (m_logger) {
            m_logger->Error("PluginManager", "Plugin {} has unmet dependencies", pluginName);
        }
        return false;
    }
    
    // Load the plugin module
    entry->moduleHandle = LoadModuleFromFile(entry->info->filepath);
    if (!entry->moduleHandle) {
        if (m_logger) {
            m_logger->Error("PluginManager", "Failed to load module for plugin: {}", pluginName);
        }
        return false;
    }
    
    // Get plugin factory function
    CreatePluginFunc createFunc = reinterpret_cast<CreatePluginFunc>(
        GetProcAddressPlatform(entry->moduleHandle, "CreatePlugin"));
    
    if (!createFunc) {
        if (m_logger) {
            m_logger->Error("PluginManager", "Plugin {} does not export CreatePlugin function", pluginName);
        }
        UnloadModule(entry->moduleHandle);
        entry->moduleHandle = nullptr;
        return false;
    }
    
    // Create plugin instance
    try {
        IPlugin* pluginPtr = createFunc();
        if (!pluginPtr) {
            if (m_logger) {
                m_logger->Error("PluginManager", "Failed to create plugin instance: {}", pluginName);
            }
            UnloadModule(entry->moduleHandle);
            entry->moduleHandle = nullptr;
            return false;
        }
        
        entry->instance = std::shared_ptr<IPlugin>(pluginPtr, [this, pluginName](IPlugin* ptr) {
            // Custom deleter that calls the plugin's destroy function
            auto it = m_plugins.find(pluginName);
            if (it != m_plugins.end() && it->second->moduleHandle) {
                DestroyPluginFunc destroyFunc = reinterpret_cast<DestroyPluginFunc>(
                    GetProcAddressPlatform(it->second->moduleHandle, "DestroyPlugin"));
                if (destroyFunc) {
                    destroyFunc(ptr);
                } else {
                    delete ptr; // Fallback to regular delete
                }
            } else {
                delete ptr; // Fallback to regular delete
            }
        });
        
        // Initialize the plugin
        if (!entry->instance->Initialize()) {
            if (m_logger) {
                m_logger->Error("PluginManager", "Failed to initialize plugin: {}", pluginName);
            }
            entry->instance.reset();
            UnloadModule(entry->moduleHandle);
            entry->moduleHandle = nullptr;
            return false;
        }
        
        // Update plugin info
        entry->info->loaded = true;
        entry->info->enabled = true;
        entry->info->name = entry->instance->GetName();
        entry->info->version = entry->instance->GetVersion();
        entry->info->description = entry->instance->GetDescription();
        entry->info->priority = entry->instance->GetPriority();
        entry->info->requiresRestart = entry->instance->RequiresRestart();
        
        // Sort plugins by priority
        SortPluginsByPriority();
        
        LogPluginEvent(pluginName, "Loaded", "Successfully loaded and initialized");
        
        if (m_logger) {
            m_logger->Info("PluginManager", "Loaded plugin: {} v{}", 
                entry->info->name, entry->info->version);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        if (m_logger) {
            m_logger->Error("PluginManager", "Exception loading plugin {}: {}", pluginName, e.what());
        }
        
        if (entry->moduleHandle) {
            UnloadModule(entry->moduleHandle);
            entry->moduleHandle = nullptr;
        }
        
        return false;
    }
}

bool PluginManager::UnloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return UnloadPluginInternal(pluginName);
}

bool PluginManager::UnloadPluginInternal(const std::string& pluginName) {
    auto it = m_plugins.find(pluginName);
    if (it == m_plugins.end()) {
        return false;
    }
    
    auto& entry = it->second;
    if (!entry->info->loaded) {
        return true; // Already unloaded
    }
    
    // Shutdown the plugin
    if (entry->instance && entry->instance->IsEnabled()) {
        try {
            entry->instance->Shutdown();
        } catch (const std::exception& e) {
            if (m_logger) {
                m_logger->Error("PluginManager", "Exception shutting down plugin {}: {}", pluginName, e.what());
            }
        }
    }
    
    // Reset the plugin instance (this will call the custom deleter)
    entry->instance.reset();
    
    // Unload the module
    if (entry->moduleHandle) {
        UnloadModule(entry->moduleHandle);
        entry->moduleHandle = nullptr;
    }
    
    // Update plugin info
    entry->info->loaded = false;
    entry->info->enabled = false;
    
    LogPluginEvent(pluginName, "Unloaded", "Successfully unloaded");
    
    if (m_logger) {
        m_logger->Info("PluginManager", "Unloaded plugin: {}", pluginName);
    }
    
    return true;
}

bool PluginManager::ReloadPlugin(const std::string& pluginName) {
    if (!UnloadPlugin(pluginName)) {
        return false;
    }
    
    // Wait a moment for the module to be fully unloaded
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    return LoadPlugin(pluginName);
}

bool PluginManager::EnablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_plugins.find(pluginName);
    if (it == m_plugins.end() || !it->second->info->loaded) {
        return false;
    }
    
    if (it->second->instance && !it->second->instance->IsEnabled()) {
        it->second->instance->SetEnabled(true);
        it->second->info->enabled = true;
        
        LogPluginEvent(pluginName, "Enabled", "Plugin enabled");
        
        if (m_logger) {
            m_logger->Info("PluginManager", "Enabled plugin: {}", pluginName);
        }
        
        return true;
    }
    
    return false;
}

bool PluginManager::DisablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_plugins.find(pluginName);
    if (it == m_plugins.end() || !it->second->info->loaded) {
        return false;
    }
    
    if (it->second->instance && it->second->instance->IsEnabled()) {
        it->second->instance->SetEnabled(false);
        it->second->info->enabled = false;
        
        LogPluginEvent(pluginName, "Disabled", "Plugin disabled");
        
        if (m_logger) {
            m_logger->Info("PluginManager", "Disabled plugin: {}", pluginName);
        }
        
        return true;
    }
    
    return false;
}

std::shared_ptr<IPlugin> PluginManager::GetPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_plugins.find(pluginName);
    if (it != m_plugins.end() && it->second->info->loaded) {
        return it->second->instance;
    }
    
    return nullptr;
}

std::shared_ptr<PluginInfo> PluginManager::GetPluginInfo(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_plugins.find(pluginName);
    if (it != m_plugins.end()) {
        return it->second->info;
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::GetLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::shared_ptr<IPlugin>> loadedPlugins;
    
    for (const auto& pair : m_plugins) {
        if (pair.second->info->loaded && pair.second->instance) {
            loadedPlugins.push_back(pair.second->instance);
        }
    }
    
    return loadedPlugins;
}

std::vector<std::shared_ptr<PluginInfo>> PluginManager::GetAllPluginInfo() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::shared_ptr<PluginInfo>> allInfo;
    
    for (const auto& pair : m_plugins) {
        allInfo.push_back(pair.second->info);
    }
    
    return allInfo;
}
