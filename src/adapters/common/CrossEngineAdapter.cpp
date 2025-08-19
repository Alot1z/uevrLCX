/*
 * UEVR Cross Engine Adapter Implementation
 * 
 * Base implementation for cross-engine VR adapters
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#include "uevr/adapters/common/CrossEngineAdapter.hpp"
#include "uevr/adapters/common/GameProfile.hpp"
#include "uevr/adapters/common/EngineHooks.hpp"
#include "uevr/adapters/common/VRModManager.hpp"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <chrono>
#include <thread>

namespace uevr {
namespace Adapters {

CrossEngineAdapter::CrossEngineAdapter(const Config& config)
    : m_config(config)
    , m_state(State::UNINITIALIZED)
    , m_vr_initialized(false)
    , m_game_profile(nullptr)
    , m_engine_hooks(nullptr)
    , m_vr_mod_manager(nullptr)
    , m_frame_count(0)
    , m_last_frame_time(std::chrono::steady_clock::now()) {
    
    spdlog::info("[CrossEngineAdapter] Adapter '{}' created for engine type {}", 
                config.name, static_cast<int>(config.engine_type));
}

CrossEngineAdapter::~CrossEngineAdapter() {
    spdlog::info("[CrossEngineAdapter] Adapter '{}' destroyed", m_config.name);
    cleanup();
}

CrossEngineAdapter::CrossEngineAdapter(CrossEngineAdapter&& other) noexcept
    : m_config(std::move(other.m_config))
    , m_state(other.m_state)
    , m_vr_initialized(other.m_vr_initialized)
    , m_game_profile(std::move(other.m_game_profile))
    , m_engine_hooks(std::move(other.m_engine_hooks))
    , m_vr_mod_manager(std::move(other.m_vr_mod_manager))
    , m_frame_count(other.m_frame_count)
    , m_last_frame_time(other.m_last_frame_time)
    , m_settings(std::move(other.m_settings)) {
    
    other.m_state = State::UNINITIALIZED;
    other.m_vr_initialized = false;
    other.m_frame_count = 0;
}

CrossEngineAdapter& CrossEngineAdapter::operator=(CrossEngineAdapter&& other) noexcept {
    if (this != &other) {
        cleanup();
        
        m_config = std::move(other.m_config);
        m_state = other.m_state;
        m_vr_initialized = other.m_vr_initialized;
        m_game_profile = std::move(other.m_game_profile);
        m_engine_hooks = std::move(other.m_engine_hooks);
        m_vr_mod_manager = std::move(other.m_vr_mod_manager);
        m_frame_count = other.m_frame_count;
        m_last_frame_time = other.m_last_frame_time;
        m_settings = std::move(other.m_settings);
        
        other.m_state = State::UNINITIALIZED;
        other.m_vr_initialized = false;
        other.m_frame_count = 0;
    }
    return *this;
}

bool CrossEngineAdapter::initialize() {
    if (m_state >= State::INITIALIZED) {
        spdlog::warn("[CrossEngineAdapter] Adapter '{}' already initialized", m_config.name);
        return true;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Initializing adapter '{}'...", m_config.name);
        
        m_state = State::INITIALIZING;
        
        // Create components
        m_game_profile = std::make_unique<GameProfile>();
        m_engine_hooks = std::make_unique<EngineHooks>();
        m_vr_mod_manager = std::make_unique<VRModManager>();
        
        // Initialize components
        if (!m_game_profile->initialize()) {
            spdlog::error("[CrossEngineAdapter] Failed to initialize game profile");
            m_state = State::ERROR;
            return false;
        }
        
        if (!m_engine_hooks->initialize()) {
            spdlog::error("[CrossEngineAdapter] Failed to initialize engine hooks");
            m_state = State::ERROR;
            return false;
        }
        
        if (!m_vr_mod_manager->initialize()) {
            spdlog::error("[CrossEngineAdapter] Failed to initialize VR mod manager");
            m_state = State::ERROR;
            return false;
        }
        
        // Load configuration
        loadConfig();
        
        m_state = State::INITIALIZED;
        spdlog::info("[CrossEngineAdapter] Adapter '{}' initialized successfully", m_config.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during initialization: {}", e.what());
        m_state = State::ERROR;
        return false;
    }
}

bool CrossEngineAdapter::start() {
    if (m_state < State::INITIALIZED) {
        spdlog::error("[CrossEngineAdapter] Cannot start - adapter not initialized");
        return false;
    }

    if (m_state == State::RUNNING) {
        spdlog::warn("[CrossEngineAdapter] Adapter '{}' already running", m_config.name);
        return true;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Starting adapter '{}'...", m_config.name);
        
        // Start components
        if (!m_engine_hooks->start()) {
            spdlog::error("[CrossEngineAdapter] Failed to start engine hooks");
            return false;
        }
        
        if (!m_vr_mod_manager->start()) {
            spdlog::error("[CrossEngineAdapter] Failed to start VR mod manager");
            return false;
        }
        
        m_state = State::RUNNING;
        m_last_frame_time = std::chrono::steady_clock::now();
        
        spdlog::info("[CrossEngineAdapter] Adapter '{}' started successfully", m_config.name);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during start: {}", e.what());
        return false;
    }
}

void CrossEngineAdapter::stop() {
    if (m_state != State::RUNNING) {
        spdlog::warn("[CrossEngineAdapter] Adapter '{}' not running", m_config.name);
        return;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Stopping adapter '{}'...", m_config.name);
        
        // Stop components
        m_vr_mod_manager->stop();
        m_engine_hooks->stop();
        
        m_state = State::INITIALIZED;
        
        spdlog::info("[CrossEngineAdapter] Adapter '{}' stopped successfully", m_config.name);
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during stop: {}", e.what());
    }
}

void CrossEngineAdapter::pause() {
    if (m_state != State::RUNNING) {
        spdlog::warn("[CrossEngineAdapter] Cannot pause - adapter not running");
        return;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Pausing adapter '{}'...", m_config.name);
        
        // Pause components
        m_vr_mod_manager->pause();
        m_engine_hooks->pause();
        
        m_state = State::PAUSED;
        
        spdlog::info("[CrossEngineAdapter] Adapter '{}' paused successfully", m_config.name);
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during pause: {}", e.what());
    }
}

void CrossEngineAdapter::resume() {
    if (m_state != State::PAUSED) {
        spdlog::warn("[CrossEngineAdapter] Cannot resume - adapter not paused");
        return;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Resuming adapter '{}'...", m_config.name);
        
        // Resume components
        m_engine_hooks->resume();
        m_vr_mod_manager->resume();
        
        m_state = State::RUNNING;
        
        spdlog::info("[CrossEngineAdapter] Adapter '{}' resumed successfully", m_config.name);
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during resume: {}", e.what());
    }
}

void CrossEngineAdapter::shutdown() {
    if (m_state == State::SHUTDOWN) {
        return;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Shutting down adapter '{}'...", m_config.name);
        
        m_state = State::SHUTDOWN;
        
        // Shutdown components
        if (m_vr_mod_manager) {
            m_vr_mod_manager->shutdown();
        }
        
        if (m_engine_hooks) {
            m_engine_hooks->shutdown();
        }
        
        if (m_game_profile) {
            m_game_profile->shutdown();
        }
        
        spdlog::info("[CrossEngineAdapter] Adapter '{}' shutdown completed", m_config.name);
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during shutdown: {}", e.what());
    }
}

void CrossEngineAdapter::cleanup() {
    if (m_state == State::UNINITIALIZED) {
        return;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Cleaning up adapter '{}'...", m_config.name);
        
        // Cleanup components
        if (m_vr_mod_manager) {
            m_vr_mod_manager->cleanup();
        }
        
        if (m_engine_hooks) {
            m_engine_hooks->cleanup();
        }
        
        if (m_game_profile) {
            m_game_profile->cleanup();
        }
        
        // Reset state
        m_state = State::UNINITIALIZED;
        m_vr_initialized = false;
        m_frame_count = 0;
        
        spdlog::info("[CrossEngineAdapter] Adapter '{}' cleanup completed", m_config.name);
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during cleanup: {}", e.what());
    }
}

void CrossEngineAdapter::onFrameBegin(float delta_time) {
    if (m_state != State::RUNNING) {
        return;
    }

    try {
        // Update frame count and timing
        m_frame_count++;
        auto current_time = std::chrono::steady_clock::now();
        m_last_frame_time = current_time;
        
        // Notify components
        if (m_engine_hooks) {
            m_engine_hooks->onFrameBegin(delta_time);
        }
        
        if (m_vr_mod_manager) {
            m_vr_mod_manager->onFrameBegin(delta_time);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in onFrameBegin: {}", e.what());
    }
}

void CrossEngineAdapter::onFrameEnd(float delta_time) {
    if (m_state != State::RUNNING) {
        return;
    }

    try {
        // Notify components
        if (m_vr_mod_manager) {
            m_vr_mod_manager->onFrameEnd(delta_time);
        }
        
        if (m_engine_hooks) {
            m_engine_hooks->onFrameEnd(delta_time);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in onFrameEnd: {}", e.what());
    }
}

void CrossEngineAdapter::onPrePresent() {
    if (m_state != State::RUNNING) {
        return;
    }

    try {
        // Notify components
        if (m_vr_mod_manager) {
            m_vr_mod_manager->onPrePresent();
        }
        
        if (m_engine_hooks) {
            m_engine_hooks->onPrePresent();
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in onPrePresent: {}", e.what());
    }
}

void CrossEngineAdapter::onPostPresent() {
    if (m_state != State::RUNNING) {
        return;
    }

    try {
        // Notify components
        if (m_engine_hooks) {
            m_engine_hooks->onPostPresent();
        }
        
        if (m_vr_mod_manager) {
            m_vr_mod_manager->onPostPresent();
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in onPostPresent: {}", e.what());
    }
}

bool CrossEngineAdapter::initializeVR() {
    if (m_vr_initialized) {
        spdlog::warn("[CrossEngineAdapter] VR already initialized");
        return true;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Initializing VR...");
        
        // Initialize VR system
        if (!m_vr_mod_manager->initializeVR()) {
            spdlog::error("[CrossEngineAdapter] Failed to initialize VR");
            return false;
        }
        
        m_vr_initialized = true;
        spdlog::info("[CrossEngineAdapter] VR initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during VR initialization: {}", e.what());
        return false;
    }
}

void CrossEngineAdapter::shutdownVR() {
    if (!m_vr_initialized) {
        return;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Shutting down VR...");
        
        m_vr_mod_manager->shutdownVR();
        m_vr_initialized = false;
        
        spdlog::info("[CrossEngineAdapter] VR shutdown completed");
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during VR shutdown: {}", e.what());
    }
}

void CrossEngineAdapter::updateVR(float delta_time) {
    if (!m_vr_initialized || m_state != State::RUNNING) {
        return;
    }

    try {
        m_vr_mod_manager->updateVR(delta_time);
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in updateVR: {}", e.what());
    }
}

void CrossEngineAdapter::renderVR() {
    if (!m_vr_initialized || m_state != State::RUNNING) {
        return;
    }

    try {
        m_vr_mod_manager->renderVR();
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in renderVR: {}", e.what());
    }
}

void CrossEngineAdapter::handleVRInput() {
    if (!m_vr_initialized || m_state != State::RUNNING) {
        return;
    }

    try {
        m_vr_mod_manager->handleVRInput();
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in handleVRInput: {}", e.what());
    }
}

void CrossEngineAdapter::updateControllers() {
    if (!m_vr_initialized || m_state != State::RUNNING) {
        return;
    }

    try {
        m_vr_mod_manager->updateControllers();
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in updateControllers: {}", e.what());
    }
}

bool CrossEngineAdapter::detectEngine() {
    try {
        spdlog::info("[CrossEngineAdapter] Detecting game engine...");
        
        // Engine detection logic would go here
        // For now, we'll assume the engine type from config
        spdlog::info("[CrossEngineAdapter] Engine type: {}", static_cast<int>(m_config.engine_type));
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during engine detection: {}", e.what());
        return false;
    }
}

bool CrossEngineAdapter::initializeEngineHooks() {
    if (!m_engine_hooks) {
        spdlog::error("[CrossEngineAdapter] Engine hooks not available");
        return false;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Initializing engine hooks...");
        
        if (!m_engine_hooks->installHooks()) {
            spdlog::error("[CrossEngineAdapter] Failed to install engine hooks");
            return false;
        }
        
        spdlog::info("[CrossEngineAdapter] Engine hooks initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during engine hooks initialization: {}", e.what());
        return false;
    }
}

void CrossEngineAdapter::removeEngineHooks() {
    if (!m_engine_hooks) {
        return;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Removing engine hooks...");
        
        m_engine_hooks->removeHooks();
        
        spdlog::info("[CrossEngineAdapter] Engine hooks removed");
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during engine hooks removal: {}", e.what());
    }
}

void CrossEngineAdapter::processEngineData() {
    if (m_state != State::RUNNING) {
        return;
    }

    try {
        if (m_engine_hooks) {
            m_engine_hooks->processEngineData();
        }
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception in processEngineData: {}", e.what());
    }
}

bool CrossEngineAdapter::loadGameProfile(const std::string& game_name) {
    if (!m_game_profile) {
        spdlog::error("[CrossEngineAdapter] Game profile not available");
        return false;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Loading game profile: {}", game_name);
        
        if (!m_game_profile->load(game_name)) {
            spdlog::error("[CrossEngineAdapter] Failed to load game profile");
            return false;
        }
        
        spdlog::info("[CrossEngineAdapter] Game profile loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during game profile loading: {}", e.what());
        return false;
    }
}

bool CrossEngineAdapter::saveGameProfile() {
    if (!m_game_profile) {
        spdlog::error("[CrossEngineAdapter] Game profile not available");
        return false;
    }

    try {
        spdlog::info("[CrossEngineAdapter] Saving game profile...");
        
        if (!m_game_profile->save()) {
            spdlog::error("[CrossEngineAdapter] Failed to save game profile");
            return false;
        }
        
        spdlog::info("[CrossEngineAdapter] Game profile saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during game profile saving: {}", e.what());
        return false;
    }
}

GameProfile* CrossEngineAdapter::getGameProfile() const {
    return m_game_profile.get();
}

bool CrossEngineAdapter::loadConfig(const std::string& config_path) {
    try {
        spdlog::info("[CrossEngineAdapter] Loading configuration from: {}", config_path);
        
        if (!std::filesystem::exists(config_path)) {
            spdlog::warn("[CrossEngineAdapter] Configuration file not found, using defaults");
            return false;
        }
        
        std::ifstream file(config_path);
        if (!file.is_open()) {
            spdlog::error("[CrossEngineAdapter] Failed to open configuration file");
            return false;
        }
        
        nlohmann::json config_json;
        file >> config_json;
        
        // Load settings from JSON
        if (config_json.contains("settings")) {
            auto settings = config_json["settings"];
            for (auto it = settings.begin(); it != settings.end(); ++it) {
                m_settings[it.key()] = it.value().get<std::string>();
            }
        }
        
        spdlog::info("[CrossEngineAdapter] Configuration loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during configuration loading: {}", e.what());
        return false;
    }
}

bool CrossEngineAdapter::saveConfig(const std::string& config_path) {
    try {
        spdlog::info("[CrossEngineAdapter] Saving configuration to: {}", config_path);
        
        // Create directory if it doesn't exist
        std::filesystem::path path(config_path);
        std::filesystem::create_directories(path.parent_path());
        
        nlohmann::json config_json;
        
        // Save settings to JSON
        config_json["settings"] = m_settings;
        
        std::ofstream file(config_path);
        if (!file.is_open()) {
            spdlog::error("[CrossEngineAdapter] Failed to create configuration file");
            return false;
        }
        
        file << config_json.dump(4);
        
        spdlog::info("[CrossEngineAdapter] Configuration saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during configuration saving: {}", e.what());
        return false;
    }
}

void CrossEngineAdapter::resetConfig() {
    try {
        spdlog::info("[CrossEngineAdapter] Resetting configuration to defaults...");
        
        m_settings.clear();
        
        // Set default settings
        m_settings["vr_enabled"] = "false";
        m_settings["vr_mode"] = "stereo";
        m_settings["performance_profile"] = "medium";
        m_settings["debug_enabled"] = "false";
        
        spdlog::info("[CrossEngineAdapter] Configuration reset to defaults");
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during configuration reset: {}", e.what());
    }
}

void CrossEngineAdapter::reloadConfig() {
    try {
        spdlog::info("[CrossEngineAdapter] Reloading configuration...");
        
        // Save current settings
        auto current_settings = m_settings;
        
        // Load configuration
        if (loadConfig("uevr_config.json")) {
            spdlog::info("[CrossEngineAdapter] Configuration reloaded successfully");
        } else {
            // Restore previous settings
            m_settings = current_settings;
            spdlog::warn("[CrossEngineAdapter] Configuration reload failed, restored previous settings");
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[CrossEngineAdapter] Exception during configuration reload: {}", e.what());
    }
}

bool CrossEngineAdapter::hasSetting(const std::string& key) const {
    return m_settings.find(key) != m_settings.end();
}

void CrossEngineAdapter::removeSetting(const std::string& key) {
    auto it = m_settings.find(key);
    if (it != m_settings.end()) {
        m_settings.erase(it);
        spdlog::debug("[CrossEngineAdapter] Removed setting: {}", key);
    }
}

} // namespace Adapters
} // namespace uevr
