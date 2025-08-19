#pragma once

#include <uevr/ICrossEngineAdapter.hpp>
#include <string>
#include <memory>
#include <unordered_map>

namespace uevr {
namespace MTFramework {

/**
 * @brief MT Framework VR Adapter for Monster Hunter World
 * 
 * This adapter provides VR support for games built with Capcom's MT Framework engine,
 * specifically targeting Monster Hunter World. It implements the ICrossEngineAdapter
 * interface and provides MT Framework-specific VR integration features.
 * 
 * @version 1.0.0
 * @author uevr Cross-Engine Adapter Project
 */
class MTFrameworkAdapter : public ICrossEngineAdapter {
public:
    MTFrameworkAdapter();
    virtual ~MTFrameworkAdapter();

    // ===== ICrossEngineAdapter Implementation =====
    
    // Core Identification
    std::string getName() const override;
    std::string getEngineType() const override;
    std::string getGameExecutable() const override;
    std::string getVersion() const override;

    // Lifecycle Management
    bool initialize() override;
    void shutdown() override;
    bool isReady() const override;

    // VR Integration
    bool enableVR() override;
    void disableVR() override;
    bool isVREnabled() const override;

    // Rendering Hooks
    bool installRenderingHooks() override;
    void removeRenderingHooks() override;
    bool areRenderingHooksActive() const override;

    // Camera Management
    bool convertCameraToVR() override;
    bool updateCameraMatrices(float* viewMatrix, float* projectionMatrix) override;
    bool getCameraOffset(float* offset) override;
    float getFOVMultiplier() const override;
    bool isFirstPersonMode() const override;

    // Engine-Specific Handles
    void* getRenderer() const override;
    void* getCamera() const override;
    void* getInput() const override;

    // Input Handling
    bool processVRInput(const void* leftControllerData, const void* rightControllerData) override;
    bool mapVRInputToGameActions() override;

    // Performance & Monitoring
    double getFrameTime() const override;
    double getAverageFrameTime() const override;
    double getFPS() const override;

    // Configuration
    bool loadConfiguration(const std::string& configPath) override;
    bool saveConfiguration(const std::string& configPath) override;
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "") const override;
    bool setConfigValue(const std::string& key, const std::string& value) override;

    // Error Handling
    std::string getLastError() const override;
    bool hasErrors() const override;
    void clearErrors() override;

    // Debug & Logging
    void setDebugMode(bool enabled) override;
    bool isDebugModeEnabled() const override;
    void logDebug(const std::string& message, int level = 0) override;

    // ===== MT Framework Specific Features =====
    
    /**
     * @brief Enable/disable first-person mode
     * @param enabled true to enable, false to disable
     * @return true if mode change successful
     */
    bool setFirstPersonMode(bool enabled);

    /**
     * @brief Set camera offset for VR positioning
     * @param offset Input array for [x, y, z] offset values
     */
    void setCameraOffset(const float offset[3]);

    /**
     * @brief Set FOV multiplier for VR
     * @param multiplier FOV multiplier value
     */
    void setFOVMultiplier(float multiplier);

private:
    // MT Framework specific members
    void* m_mtFrameworkModule;
    void* m_renderer;
    void* m_camera;
    void* m_input;

    // VR state
    bool m_vrEnabled;
    bool m_firstPersonMode;
    float m_fovMultiplier;
    float m_cameraOffset[3];

    // Performance tracking
    double m_lastFrameTime;
    double m_avgFrameTime;

    // Configuration
    std::unordered_map<std::string, std::string> m_config;
    bool m_debugMode;

    // Error handling
    std::string m_lastError;
    bool m_hasErrors;

    // Internal helper methods
    bool initializeMTFramework();
    bool installDirectXHooks();
    bool installCameraHooks();
    bool installInputHooks();
    void updatePerformanceMetrics();
    void logMTFrameworkError(const std::string& operation, const std::string& details);
};

} // namespace MTFramework
} // namespace uevr
