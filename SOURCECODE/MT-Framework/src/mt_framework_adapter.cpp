#include "mt_framework_adapter.hpp"
#include "mt_framework_types.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>

namespace uevr {
namespace MTFramework {

MTFrameworkAdapter::MTFrameworkAdapter() : 
    m_mtFrameworkModule(nullptr),
    m_renderer(nullptr),
    m_camera(nullptr),
    m_input(nullptr),
    m_vrEnabled(false),
    m_firstPersonMode(false),
    m_fovMultiplier(DEFAULT_VR_FOV_MULTIPLIER),
    m_lastFrameTime(0.0),
    m_avgFrameTime(TARGET_FRAME_TIME_MS),
    m_debugMode(false),
    m_hasErrors(false) {
    
    m_cameraOffset[0] = 0.0f;
    m_cameraOffset[1] = DEFAULT_CAMERA_OFFSET_Y;
    m_cameraOffset[2] = 0.0f;
}

MTFrameworkAdapter::~MTFrameworkAdapter() {
    shutdown();
}

// ===== ICrossEngineAdapter Implementation =====

std::string MTFrameworkAdapter::getName() const {
    return "MonsterHunterWorld-MTFramework";
}

std::string MTFrameworkAdapter::getEngineType() const {
    return MT_FRAMEWORK_ENGINE_ID;
}

std::string MTFrameworkAdapter::getGameExecutable() const {
    return "MonsterHunterWorld.exe";
}

std::string MTFrameworkAdapter::getVersion() const {
    return "1.0.0";
}

bool MTFrameworkAdapter::initialize() {
    if (m_mtFrameworkModule) {
        return true; // Already initialized
    }

    try {
        if (!initializeMTFramework()) {
            logMTFrameworkError("initialize", "Failed to initialize MT Framework");
            return false;
        }

        // Load default configuration
        loadConfiguration("");

        if (m_debugMode) {
            logDebug("MT Framework adapter initialized successfully", 0);
        }

        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("initialize", e.what());
        return false;
    }
}

void MTFrameworkAdapter::shutdown() {
    if (m_vrEnabled) {
        disableVR();
    }

    removeRenderingHooks();

    m_mtFrameworkModule = nullptr;
    m_renderer = nullptr;
    m_camera = nullptr;
    m_input = nullptr;

    if (m_debugMode) {
        logDebug("MT Framework adapter shutdown complete", 0);
    }
}

bool MTFrameworkAdapter::isReady() const {
    return m_mtFrameworkModule != nullptr && !m_hasErrors;
}

bool MTFrameworkAdapter::enableVR() {
    if (m_vrEnabled) {
        return true; // Already enabled
    }

    try {
        if (!installRenderingHooks()) {
            logMTFrameworkError("enableVR", "Failed to install rendering hooks");
            return false;
        }

        if (!convertCameraToVR()) {
            logMTFrameworkError("enableVR", "Failed to convert camera to VR");
            return false;
        }

        m_vrEnabled = true;

        if (m_debugMode) {
            logDebug("VR mode enabled successfully", 0);
        }

        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("enableVR", e.what());
        return false;
    }
}

void MTFrameworkAdapter::disableVR() {
    if (!m_vrEnabled) {
        return; // Already disabled
    }

    removeRenderingHooks();
    m_vrEnabled = false;

    if (m_debugMode) {
        logDebug("VR mode disabled", 0);
    }
}

bool MTFrameworkAdapter::isVREnabled() const {
    return m_vrEnabled;
}

bool MTFrameworkAdapter::installRenderingHooks() {
    try {
        if (!installDirectXHooks()) {
            return false;
        }

        if (!installCameraHooks()) {
            return false;
        }

        if (!installInputHooks()) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("installRenderingHooks", e.what());
        return false;
    }
}

void MTFrameworkAdapter::removeRenderingHooks() {
    // Implementation would remove all installed hooks
    if (m_debugMode) {
        logDebug("Rendering hooks removed", 0);
    }
}

bool MTFrameworkAdapter::areRenderingHooksActive() const {
    return m_vrEnabled && m_renderer != nullptr;
}

bool MTFrameworkAdapter::convertCameraToVR() {
    try {
        // Implementation would convert third-person camera to first-person VR camera
        m_firstPersonMode = true;
        
        if (m_debugMode) {
            logDebug("Camera converted to VR mode", 0);
        }
        
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("convertCameraToVR", e.what());
        return false;
    }
}

bool MTFrameworkAdapter::updateCameraMatrices(float* viewMatrix, float* projectionMatrix) {
    if (!m_camera || !viewMatrix || !projectionMatrix) {
        return false;
    }

    try {
        // Implementation would update camera matrices for VR rendering
        // For now, provide identity matrices as placeholders
        
        // Identity view matrix
        for (int i = 0; i < 16; ++i) {
            viewMatrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        }
        
        // Basic projection matrix
        float fov = 90.0f * m_fovMultiplier;
        float aspect = 16.0f / 9.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;
        
        float tanHalfFov = tan(fov * 0.5f * 3.14159f / 180.0f);
        
        projectionMatrix[0] = 1.0f / (aspect * tanHalfFov);
        projectionMatrix[1] = 0.0f;
        projectionMatrix[2] = 0.0f;
        projectionMatrix[3] = 0.0f;
        
        projectionMatrix[4] = 0.0f;
        projectionMatrix[5] = 1.0f / tanHalfFov;
        projectionMatrix[6] = 0.0f;
        projectionMatrix[7] = 0.0f;
        
        projectionMatrix[8] = 0.0f;
        projectionMatrix[9] = 0.0f;
        projectionMatrix[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        projectionMatrix[11] = -1.0f;
        
        projectionMatrix[12] = 0.0f;
        projectionMatrix[13] = 0.0f;
        projectionMatrix[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        projectionMatrix[15] = 0.0f;
        
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("updateCameraMatrices", e.what());
        return false;
    }
}

bool MTFrameworkAdapter::processVRInput(const void* leftControllerData, const void* rightControllerData) {
    if (!m_input) {
        return false;
    }

    try {
        // Implementation would process VR controller input
        // For now, just return success
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("processVRInput", e.what());
        return false;
    }
}

bool MTFrameworkAdapter::mapVRInputToGameActions() {
    try {
        // Implementation would map VR input to game actions
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("mapVRInputToGameActions", e.what());
        return false;
    }
}

double MTFrameworkAdapter::getFrameTime() const {
    return m_lastFrameTime;
}

double MTFrameworkAdapter::getAverageFrameTime() const {
    return m_avgFrameTime;
}

double MTFrameworkAdapter::getFPS() const {
    if (m_avgFrameTime <= 0.0) {
        return 0.0;
    }
    return 1000.0 / m_avgFrameTime;
}

bool MTFrameworkAdapter::loadConfiguration(const std::string& configPath) {
    try {
        // Load default configuration
        m_config[ConfigKeys::VR_ENABLED] = "false";
        m_config[ConfigKeys::FIRST_PERSON_MODE] = "false";
        m_config[ConfigKeys::FOV_MULTIPLIER] = std::to_string(DEFAULT_VR_FOV_MULTIPLIER);
        m_config[ConfigKeys::CAMERA_OFFSET_X] = "0.0";
        m_config[ConfigKeys::CAMERA_OFFSET_Y] = std::to_string(DEFAULT_CAMERA_OFFSET_Y);
        m_config[ConfigKeys::CAMERA_OFFSET_Z] = "0.0";
        m_config[ConfigKeys::DEBUG_MODE] = "false";
        m_config[ConfigKeys::PERFORMANCE_MONITORING] = "true";
        m_config[ConfigKeys::RENDERING_HOOKS] = "true";
        m_config[ConfigKeys::INPUT_HOOKS] = "true";
        m_config[ConfigKeys::CAMERA_HOOKS] = "true";

        // Load from file if specified
        if (!configPath.empty()) {
            std::ifstream file(configPath);
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    size_t pos = line.find('=');
                    if (pos != std::string::npos) {
                        std::string key = line.substr(0, pos);
                        std::string value = line.substr(pos + 1);
                        
                        // Trim whitespace
                        key.erase(0, key.find_first_not_of(" \t"));
                        key.erase(key.find_last_not_of(" \t") + 1);
                        value.erase(0, value.find_first_not_of(" \t"));
                        value.erase(value.find_last_not_of(" \t") + 1);
                        
                        if (validateConfigValue(key, value)) {
                            m_config[key] = value;
                        }
                    }
                }
                file.close();
            }
        }

        // Apply configuration
        m_debugMode = (m_config[ConfigKeys::DEBUG_MODE] == "true");
        m_fovMultiplier = std::stof(m_config[ConfigKeys::FOV_MULTIPLIER]);
        m_cameraOffset[0] = std::stof(m_config[ConfigKeys::CAMERA_OFFSET_X]);
        m_cameraOffset[1] = std::stof(m_config[ConfigKeys::CAMERA_OFFSET_Y]);
        m_cameraOffset[2] = std::stof(m_config[ConfigKeys::CAMERA_OFFSET_Z]);

        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("loadConfiguration", e.what());
        return false;
    }
}

bool MTFrameworkAdapter::saveConfiguration(const std::string& configPath) {
    try {
        if (configPath.empty()) {
            return false;
        }

        std::ofstream file(configPath);
        if (!file.is_open()) {
            return false;
        }

        for (const auto& pair : m_config) {
            file << pair.first << "=" << pair.second << std::endl;
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("saveConfiguration", e.what());
        return false;
    }
}

std::string MTFrameworkAdapter::getConfigValue(const std::string& key, const std::string& defaultValue) const {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        return it->second;
    }
    return defaultValue;
}

bool MTFrameworkAdapter::setConfigValue(const std::string& key, const std::string& value) {
    if (!validateConfigValue(key, value)) {
        return false;
    }

    m_config[key] = value;

    // Apply configuration changes
    if (key == ConfigKeys::DEBUG_MODE) {
        m_debugMode = (value == "true");
    } else if (key == ConfigKeys::FOV_MULTIPLIER) {
        m_fovMultiplier = std::stof(value);
    } else if (key == ConfigKeys::CAMERA_OFFSET_X) {
        m_cameraOffset[0] = std::stof(value);
    } else if (key == ConfigKeys::CAMERA_OFFSET_Y) {
        m_cameraOffset[1] = std::stof(value);
    } else if (key == ConfigKeys::CAMERA_OFFSET_Z) {
        m_cameraOffset[2] = std::stof(value);
    }

    return true;
}

std::string MTFrameworkAdapter::getLastError() const {
    return m_lastError;
}

bool MTFrameworkAdapter::hasErrors() const {
    return m_hasErrors;
}

void MTFrameworkAdapter::clearErrors() {
    m_lastError.clear();
    m_hasErrors = false;
}

void MTFrameworkAdapter::setDebugMode(bool enabled) {
    m_debugMode = enabled;
    m_config[ConfigKeys::DEBUG_MODE] = enabled ? "true" : "false";
}

bool MTFrameworkAdapter::isDebugModeEnabled() const {
    return m_debugMode;
}

void MTFrameworkAdapter::logDebug(const std::string& message, int level) {
    if (!m_debugMode) {
        return;
    }

    std::string levelStr;
    switch (level) {
        case 0: levelStr = "INFO"; break;
        case 1: levelStr = "WARNING"; break;
        case 2: levelStr = "ERROR"; break;
        default: levelStr = "UNKNOWN"; break;
    }

    std::cout << "[MTFramework] [" << levelStr << "] " << message << std::endl;
}

// ===== MT Framework Specific Features =====

void* MTFrameworkAdapter::getRenderer() const {
    return m_renderer;
}

void* MTFrameworkAdapter::getCamera() const {
    return m_camera;
}

void* MTFrameworkAdapter::getInput() const {
    return m_input;
}

bool MTFrameworkAdapter::isFirstPersonMode() const {
    return m_firstPersonMode;
}

bool MTFrameworkAdapter::setFirstPersonMode(bool enabled) {
    m_firstPersonMode = enabled;
    m_config[ConfigKeys::FIRST_PERSON_MODE] = enabled ? "true" : "false";
    return true;
}

bool MTFrameworkAdapter::getCameraOffset(float* offset) const {
    if (offset) {
        offset[0] = m_cameraOffset[0];
        offset[1] = m_cameraOffset[1];
        offset[2] = m_cameraOffset[2];
        return true;
    }
    return false;
}

void MTFrameworkAdapter::setCameraOffset(const float offset[3]) {
    if (offset) {
        m_cameraOffset[0] = offset[0];
        m_cameraOffset[1] = offset[1];
        m_cameraOffset[2] = offset[2];
        
        m_config[ConfigKeys::CAMERA_OFFSET_X] = std::to_string(offset[0]);
        m_config[ConfigKeys::CAMERA_OFFSET_Y] = std::to_string(offset[1]);
        m_config[ConfigKeys::CAMERA_OFFSET_Z] = std::to_string(offset[2]);
    }
}

float MTFrameworkAdapter::getFOVMultiplier() const {
    return m_fovMultiplier;
}

void MTFrameworkAdapter::setFOVMultiplier(float multiplier) {
    m_fovMultiplier = std::clamp(multiplier, MIN_FOV_MULTIPLIER, MAX_FOV_MULTIPLIER);
    m_config[ConfigKeys::FOV_MULTIPLIER] = std::to_string(m_fovMultiplier);
}

// ===== Private Helper Methods =====

bool MTFrameworkAdapter::initializeMTFramework() {
    try {
        // Implementation would initialize MT Framework module
        // For now, just set placeholder values
        m_mtFrameworkModule = reinterpret_cast<void*>(0x1000);
        m_renderer = reinterpret_cast<void*>(0x2000);
        m_camera = reinterpret_cast<void*>(0x3000);
        m_input = reinterpret_cast<void*>(0x4000);
        
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("initializeMTFramework", e.what());
        return false;
    }
}

bool MTFrameworkAdapter::installDirectXHooks() {
    try {
        // Implementation would install DirectX hooks
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("installDirectXHooks", e.what());
        return false;
    }
}

bool MTFrameworkAdapter::installCameraHooks() {
    try {
        // Implementation would install camera hooks
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("installCameraHooks", e.what());
        return false;
    }
}

bool MTFrameworkAdapter::installInputHooks() {
    try {
        // Implementation would install input hooks
        return true;
    } catch (const std::exception& e) {
        logMTFrameworkError("installInputHooks", e.what());
        return false;
    }
}

void MTFrameworkAdapter::updatePerformanceMetrics() {
    auto now = std::chrono::high_resolution_clock::now();
    static auto lastTime = now;
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime);
    m_lastFrameTime = duration.count() / 1000.0; // Convert to milliseconds
    
    // Update average frame time
    const double alpha = 0.1; // Smoothing factor
    m_avgFrameTime = (alpha * m_lastFrameTime) + ((1.0 - alpha) * m_avgFrameTime);
    
    lastTime = now;
}

void MTFrameworkAdapter::logMTFrameworkError(const std::string& operation, const std::string& details) {
    m_lastError = "MT Framework " + operation + " failed: " + details;
    m_hasErrors = true;
    
    if (m_debugMode) {
        logDebug(m_lastError, 2);
    }
}

} // namespace MTFramework
} // namespace uevr
