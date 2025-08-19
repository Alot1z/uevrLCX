/*
 * UEVR Plugin Implementation
 * 
 * Plugin system implementation for UEVR
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#include "uevr/Plugin.hpp"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <chrono>
#include <thread>

namespace uevr {

Plugin::Plugin(const PluginInfo& info)
    : m_info(info)
    , m_state(PluginState::UNLOADED)
    , m_enabled(false)
    , m_initialized(false)
    , m_load_time(std::chrono::steady_clock::now())
    , m_last_update_time(std::chrono::steady_clock::now())
    , m_update_count(0)
    , m_error_count(0) {
    
    spdlog::info("[Plugin] Plugin '{}' (v{}) created", info.name, info.version);
}

Plugin::~Plugin() {
    spdlog::info("[Plugin] Plugin '{}' destroyed", m_info.name);
    unload();
}

bool Plugin::load() {
    if (m_state >= PluginState::LOADED) {
        spdlog::warn("[Plugin] Plugin '{}' already loaded", m_info.name);
        return true;
    }

    try {
        spdlog::info("[Plugin] Loading plugin '{}'...", m_info.name);
        
        m_state = PluginState::LOADING;
        
        // Load plugin configuration
        if (!loadConfig()) {
            spdlog::warn("[Plugin] Failed to load configuration for '{}', using defaults", m_info.name);
        }
        
        // Load plugin resources
        if (!loadResources()) {
            spdlog::warn("[Plugin] Failed to load resources for '{}'", m_info.name);
        }
        
        // Validate plugin
        if (!validate()) {
            spdlog::error("[Plugin] Plugin '{}' validation failed", m_info.name);
            m_state = PluginState::ERROR;
            return false;
        }
        
        m_state = PluginState::LOADED;
        m_load_time = std::chrono::steady_clock::now();
        
        spdlog::info("[Plugin] Plugin '{}' loaded successfully", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during loading of '{}': {}", m_info.name, e.what());
        m_state = PluginState::ERROR;
        m_error_count++;
        return false;
    }
}

bool Plugin::unload() {
    if (m_state == PluginState::UNLOADED) {
        return true;
    }

    try {
        spdlog::info("[Plugin] Unloading plugin '{}'...", m_info.name);
        
        // Stop plugin if running
        if (m_state == PluginState::RUNNING) {
            stop();
        }
        
        // Uninitialize if initialized
        if (m_initialized) {
            uninitialize();
        }
        
        // Unload resources
        unloadResources();
        
        // Reset state
        m_state = PluginState::UNLOADED;
        m_enabled = false;
        m_initialized = false;
        
        spdlog::info("[Plugin] Plugin '{}' unloaded successfully", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during unloading of '{}': {}", m_info.name, e.what());
        m_error_count++;
        return false;
    }
}

bool Plugin::initialize() {
    if (m_state < PluginState::LOADED) {
        spdlog::error("[Plugin] Cannot initialize '{}' - plugin not loaded", m_info.name);
        return false;
    }

    if (m_initialized) {
        spdlog::warn("[Plugin] Plugin '{}' already initialized", m_info.name);
        return true;
    }

    try {
        spdlog::info("[Plugin] Initializing plugin '{}'...", m_info.name);
        
        // Initialize plugin-specific components
        if (!onInitialize()) {
            spdlog::error("[Plugin] Plugin '{}' initialization failed", m_info.name);
            return false;
        }
        
        m_initialized = true;
        spdlog::info("[Plugin] Plugin '{}' initialized successfully", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during initialization of '{}': {}", m_info.name, e.what());
        m_error_count++;
        return false;
    }
}

void Plugin::uninitialize() {
    if (!m_initialized) {
        return;
    }

    try {
        spdlog::info("[Plugin] Uninitializing plugin '{}'...", m_info.name);
        
        // Call plugin-specific uninitialization
        onUninitialize();
        
        m_initialized = false;
        
        spdlog::info("[Plugin] Plugin '{}' uninitialized successfully", m_info.name);
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during uninitialization of '{}': {}", m_info.name, e.what());
        m_error_count++;
    }
}

bool Plugin::start() {
    if (!m_initialized) {
        spdlog::error("[Plugin] Cannot start '{}' - plugin not initialized", m_info.name);
        return false;
    }

    if (m_state == PluginState::RUNNING) {
        spdlog::warn("[Plugin] Plugin '{}' already running", m_info.name);
        return true;
    }

    try {
        spdlog::info("[Plugin] Starting plugin '{}'...", m_info.name);
        
        // Call plugin-specific start
        if (!onStart()) {
            spdlog::error("[Plugin] Plugin '{}' start failed", m_info.name);
            return false;
        }
        
        m_state = PluginState::RUNNING;
        m_last_update_time = std::chrono::steady_clock::now();
        
        spdlog::info("[Plugin] Plugin '{}' started successfully", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during start of '{}': {}", m_info.name, e.what());
        m_error_count++;
        return false;
    }
}

void Plugin::stop() {
    if (m_state != PluginState::RUNNING) {
        spdlog::warn("[Plugin] Cannot stop '{}' - plugin not running", m_info.name);
        return;
    }

    try {
        spdlog::info("[Plugin] Stopping plugin '{}'...", m_info.name);
        
        // Call plugin-specific stop
        onStop();
        
        m_state = PluginState::LOADED;
        
        spdlog::info("[Plugin] Plugin '{}' stopped successfully", m_info.name);
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during stop of '{}': {}", m_info.name, e.what());
        m_error_count++;
    }
}

void Plugin::update(float delta_time) {
    if (m_state != PluginState::RUNNING) {
        return;
    }

    try {
        // Call plugin-specific update
        onUpdate(delta_time);
        
        // Update statistics
        m_update_count++;
        m_last_update_time = std::chrono::steady_clock::now();
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during update of '{}': {}", m_info.name, e.what());
        m_error_count++;
    }
}

void Plugin::render() {
    if (m_state != PluginState::RUNNING) {
        return;
    }

    try {
        // Call plugin-specific render
        onRender();
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during render of '{}': {}", m_info.name, e.what());
        m_error_count++;
    }
}

bool Plugin::enable() {
    if (m_enabled) {
        spdlog::warn("[Plugin] Plugin '{}' already enabled", m_info.name);
        return true;
    }

    try {
        spdlog::info("[Plugin] Enabling plugin '{}'...", m_info.name);
        
        // Call plugin-specific enable
        if (!onEnable()) {
            spdlog::error("[Plugin] Plugin '{}' enable failed", m_info.name);
            return false;
        }
        
        m_enabled = true;
        
        spdlog::info("[Plugin] Plugin '{}' enabled successfully", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during enable of '{}': {}", m_info.name, e.what());
        m_error_count++;
        return false;
    }
}

void Plugin::disable() {
    if (!m_enabled) {
        return;
    }

    try {
        spdlog::info("[Plugin] Disabling plugin '{}'...", m_info.name);
        
        // Call plugin-specific disable
        onDisable();
        
        m_enabled = false;
        
        spdlog::info("[Plugin] Plugin '{}' disabled successfully", m_info.name);
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during disable of '{}': {}", m_info.name, e.what());
        m_error_count++;
    }
}

bool Plugin::reload() {
    try {
        spdlog::info("[Plugin] Reloading plugin '{}'...", m_info.name);
        
        // Save current state
        auto current_state = m_state;
        auto current_enabled = m_enabled;
        auto current_initialized = m_initialized;
        
        // Unload plugin
        if (!unload()) {
            spdlog::error("[Plugin] Failed to unload '{}' during reload", m_info.name);
            return false;
        }
        
        // Load plugin again
        if (!load()) {
            spdlog::error("[Plugin] Failed to load '{}' during reload", m_info.name);
            return false;
        }
        
        // Restore state if possible
        if (current_initialized) {
            if (!initialize()) {
                spdlog::warn("[Plugin] Failed to reinitialize '{}' during reload", m_info.name);
            }
        }
        
        if (current_enabled) {
            if (!enable()) {
                spdlog::warn("[Plugin] Failed to re-enable '{}' during reload", m_info.name);
            }
        }
        
        if (current_state == PluginState::RUNNING) {
            if (!start()) {
                spdlog::warn("[Plugin] Failed to restart '{}' during reload", m_info.name);
            }
        }
        
        spdlog::info("[Plugin] Plugin '{}' reloaded successfully", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during reload of '{}': {}", m_info.name, e.what());
        m_error_count++;
        return false;
    }
}

bool Plugin::isCompatible(const std::string& uevr_version) const {
    // Check version compatibility
    if (m_info.min_uevr_version.empty() && m_info.max_uevr_version.empty()) {
        return true; // No version constraints
    }
    
    // Simple version comparison (can be enhanced with proper semantic versioning)
    if (!m_info.min_uevr_version.empty() && uevr_version < m_info.min_uevr_version) {
        return false;
    }
    
    if (!m_info.max_uevr_version.empty() && uevr_version > m_info.max_uevr_version) {
        return false;
    }
    
    return true;
}

bool Plugin::hasDependency(const std::string& dependency_name) const {
    return std::find(m_info.dependencies.begin(), m_info.dependencies.end(), dependency_name) != m_info.dependencies.end();
}

bool Plugin::hasConflict(const std::string& conflict_name) const {
    return std::find(m_info.conflicts.begin(), m_info.conflicts.end(), conflict_name) != m_info.conflicts.end();
}

bool Plugin::hasFeature(const std::string& feature_name) const {
    return std::find(m_info.features.begin(), m_info.features.end(), feature_name) != m_info.features.end();
}

std::string Plugin::getMetadata(const std::string& key) const {
    auto it = m_info.metadata.find(key);
    if (it != m_info.metadata.end()) {
        return it->second;
    }
    return "";
}

void Plugin::setMetadata(const std::string& key, const std::string& value) {
    m_info.metadata[key] = value;
}

PluginInfo Plugin::getInfo() const {
    return m_info;
}

PluginState Plugin::getState() const {
    return m_state;
}

bool Plugin::isEnabled() const {
    return m_enabled;
}

bool Plugin::isInitialized() const {
    return m_initialized;
}

bool Plugin::isRunning() const {
    return m_state == PluginState::RUNNING;
}

bool Plugin::hasError() const {
    return m_state == PluginState::ERROR;
}

std::chrono::steady_clock::time_point Plugin::getLoadTime() const {
    return m_load_time;
}

std::chrono::steady_clock::time_point Plugin::getLastUpdateTime() const {
    return m_last_update_time;
}

uint64_t Plugin::getUpdateCount() const {
    return m_update_count;
}

uint32_t Plugin::getErrorCount() const {
    return m_error_count;
}

// Default implementations for virtual methods
bool Plugin::onInitialize() {
    // Default implementation does nothing
    return true;
}

void Plugin::onUninitialize() {
    // Default implementation does nothing
}

bool Plugin::onStart() {
    // Default implementation does nothing
    return true;
}

void Plugin::onStop() {
    // Default implementation does nothing
}

void Plugin::onUpdate(float delta_time) {
    // Default implementation does nothing
}

void Plugin::onRender() {
    // Default implementation does nothing
}

bool Plugin::onEnable() {
    // Default implementation does nothing
    return true;
}

void Plugin::onDisable() {
    // Default implementation does nothing
}

bool Plugin::loadConfig() {
    try {
        std::filesystem::path config_path = "plugins/" + m_info.name + "/config.json";
        
        if (!std::filesystem::exists(config_path)) {
            spdlog::debug("[Plugin] No configuration file found for '{}'", m_info.name);
            return false;
        }
        
        std::ifstream file(config_path);
        if (!file.is_open()) {
            spdlog::warn("[Plugin] Failed to open configuration file for '{}'", m_info.name);
            return false;
        }
        
        nlohmann::json config_json;
        file >> config_json;
        
        // Load configuration data
        // This can be overridden by derived classes
        
        spdlog::debug("[Plugin] Configuration loaded for '{}'", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::warn("[Plugin] Exception during configuration loading for '{}': {}", m_info.name, e.what());
        return false;
    }
}

bool Plugin::loadResources() {
    try {
        std::filesystem::path resource_path = "plugins/" + m_info.name + "/resources";
        
        if (!std::filesystem::exists(resource_path)) {
            spdlog::debug("[Plugin] No resource directory found for '{}'", m_info.name);
            return true; // Not an error
        }
        
        // Load plugin resources
        // This can be overridden by derived classes
        
        spdlog::debug("[Plugin] Resources loaded for '{}'", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::warn("[Plugin] Exception during resource loading for '{}': {}", m_info.name, e.what());
        return false;
    }
}

void Plugin::unloadResources() {
    try {
        // Unload plugin resources
        // This can be overridden by derived classes
        
        spdlog::debug("[Plugin] Resources unloaded for '{}'", m_info.name);
        
    } catch (const std::exception& e) {
        spdlog::warn("[Plugin] Exception during resource unloading for '{}': {}", m_info.name, e.what());
    }
}

bool Plugin::validate() {
    try {
        // Basic validation
        if (m_info.name.empty()) {
            spdlog::error("[Plugin] Plugin name is empty");
            return false;
        }
        
        if (m_info.version.empty()) {
            spdlog::error("[Plugin] Plugin version is empty");
            return false;
        }
        
        // Check for circular dependencies
        if (hasDependency(m_info.name)) {
            spdlog::error("[Plugin] Plugin '{}' depends on itself", m_info.name);
            return false;
        }
        
        // Additional validation can be implemented by derived classes
        
        spdlog::debug("[Plugin] Plugin '{}' validation passed", m_info.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Plugin] Exception during validation of '{}': {}", m_info.name, e.what());
        return false;
    }
}

} // namespace uevr
