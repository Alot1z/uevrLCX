/*
 * MT Framework Cross-Engine VR Adapter Header
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 * Based on uevr Plugin System (MIT License)
 */

#pragma once

#include <uevr/ICrossEngineAdapter.hpp>
#include <windows.h>
#include <memory>
#include <string>

// Forward declarations for MT Framework
namespace MTFramework {
    class Renderer;
    class Camera;
    class Input;
}

namespace uevr {
namespace MTFramework {

/**
 * @brief Monster Hunter World Cross-Engine VR Adapter
 * 
 * This adapter integrates Monster Hunter World (MT Framework engine) with uevr's
 * cross-engine framework to provide full VR support including third-person to
 * first-person camera conversion, VR controller integration, and performance optimization.
 */
class MonsterHunterWorldAdapter : public ICrossEngineAdapter {
private:
    // MT Framework specific handles
    HMODULE m_mtFrameworkModule;
    MTFramework::Renderer* m_renderer;
    MTFramework::Camera* m_camera;
    MTFramework::Input* m_input;
    
    // uevr cross-engine integration
    std::unique_ptr<uevr::API> m_uevrAPI;
    std::unique_ptr<uevr::Plugin> m_uevrPlugin;
    
    // VR state management
    bool m_vrEnabled;
    bool m_firstPersonMode;
    float m_cameraOffset[3];
    float m_fovMultiplier;
    
    // Performance tracking
    double m_lastFrameTime;
    double m_avgFrameTime;
    
public:
    MonsterHunterWorldAdapter();
    virtual ~MonsterHunterWorldAdapter();
    
    // ICrossEngineAdapter implementation
    bool initialize() override;
    void cleanup() override;
    bool isVREnabled() const override;
    void update() override;
    void render() override;
    
    // Adapter information
    std::string getName() const override;
    std::string getEngineType() const override;
    std::string getGameExecutable() const override;
    std::string getVersion() const override;
    
    // Monster Hunter World specific methods
    void enableFirstPersonMode(bool enable);
    void setCameraOffset(float x, float y, float z);
    void setFOVMultiplier(float multiplier);
    
private:
    // Core integration methods
    bool hookRenderingPipeline();
    bool hookMTFrameworkFunctions();
    bool initializeCameraSystem();
    bool initializeInputSystem();
    
    // VR system methods
    void initializeVRCameraSettings();
    void initializeVRInputMapping();
    void updateCameraSystem();
    void updateInputSystem();
    
    // Camera transformation methods
    void applyHeadsetTracking(const uevr::Pose* headsetPose);
    void applyFirstPersonCameraSettings();
    void updateCameraPosition();
    void updateCameraFOV();
    
    // Input mapping methods
    void updateLeftControllerInput(const uevr::Pose* controller);
    void updateRightControllerInput(const uevr::Pose* controller);
    
    // Rendering methods
    void renderEye(uevr::Eye eye);
    
    // Performance methods
    void updatePerformanceMetrics();
};

} // namespace MTFramework
} // namespace uevr
