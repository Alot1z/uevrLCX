#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>

// OpenXR headers
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

namespace uevr {

/**
 * @brief OpenXR Integration for uevr
 * 
 * Provides comprehensive OpenXR support including:
 * - VR session management
 * - Motion controller input
 * - Haptic feedback
 * - Stereo rendering
 * - Performance optimization
 */
class OpenXRIntegration {
public:
    // Callback types
    using SessionStateCallback = std::function<void(XrSessionState)>;
    using ControllerInputCallback = std::function<void(uint32_t controllerId, const XrActionStateVector2f& thumbstick, const XrActionStateBoolean& trigger, const XrActionStateBoolean& grip)>;
    using HapticFeedbackCallback = std::function<void(uint32_t controllerId, float intensity, float duration)>;
    
    // VR device information
    struct VRDeviceInfo {
        std::string deviceName;
        std::string manufacturer;
        std::string serialNumber;
        XrVersion apiVersion;
        bool supportsHaptics;
        bool supportsEyeTracking;
        float displayRefreshRate;
        uint32_t displayWidth;
        uint32_t displayHeight;
    };
    
    // Motion controller state
    struct ControllerState {
        uint32_t controllerId;
        bool isConnected;
        XrPosef pose;
        XrVector2f thumbstick;
        float triggerValue;
        float gripValue;
        bool triggerPressed;
        bool gripPressed;
        bool menuPressed;
        bool systemPressed;
    };
    
    // VR rendering configuration
    struct VRRenderConfig {
        bool enableStereoRendering;
        bool enableAsyncReprojection;
        bool enableMotionVectorPass;
        float ipd;
        float nearPlane;
        float farPlane;
        uint32_t msaaSamples;
        bool enableHDR;
    };
    
    OpenXRIntegration();
    ~OpenXRIntegration();
    
    // Core OpenXR lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized; }
    
    // Session management
    bool createSession();
    bool destroySession();
    bool beginSession();
    bool endSession();
    bool isSessionActive() const { return m_sessionActive; }
    
    // Device information
    VRDeviceInfo getDeviceInfo() const { return m_deviceInfo; }
    std::vector<ControllerState> getControllerStates() const { return m_controllerStates; }
    
    // Input handling
    bool initializeInput();
    void updateInput();
    void registerControllerInputCallback(ControllerInputCallback callback);
    
    // Haptic feedback
    bool initializeHaptics();
    bool triggerHapticFeedback(uint32_t controllerId, float intensity, float duration);
    void registerHapticFeedbackCallback(HapticFeedbackCallback callback);
    
    // Rendering
    bool initializeRendering();
    bool beginFrame();
    bool endFrame();
    bool submitFrame();
    
    // Configuration
    void setRenderConfig(const VRRenderConfig& config) { m_renderConfig = config; }
    VRRenderConfig getRenderConfig() const { return m_renderConfig; }
    
    // Performance monitoring
    struct PerformanceMetrics {
        double frameTime;
        double cpuTime;
        double gpuTime;
        uint32_t droppedFrames;
        double reprojectionRatio;
    };
    
    PerformanceMetrics getPerformanceMetrics() const { return m_performanceMetrics; }
    
    // Error handling
    std::string getLastError() const { return m_lastError; }
    void clearErrors();

private:
    // OpenXR instances and handles
    XrInstance m_xrInstance;
    XrSession m_xrSession;
    XrSpace m_viewSpace;
    XrSpace m_localSpace;
    
    // Input actions and spaces
    XrActionSet m_actionSet;
    XrAction m_thumbstickAction;
    XrAction m_triggerAction;
    XrAction m_gripAction;
    XrAction m_menuAction;
    XrAction m_systemAction;
    XrAction m_hapticAction;
    
    // Controller paths
    XrPath m_leftHandPath;
    XrPath m_rightHandPath;
    
    // Rendering
    std::vector<XrSwapchain> m_swapchains;
    std::vector<XrSwapchainImageBaseHeader*> m_swapchainImages;
    XrViewConfigurationView m_viewConfigViews[2];
    
    // State tracking
    bool m_initialized;
    bool m_sessionActive;
    VRDeviceInfo m_deviceInfo;
    std::vector<ControllerState> m_controllerStates;
    VRRenderConfig m_renderConfig;
    PerformanceMetrics m_performanceMetrics;
    
    // Callbacks
    std::vector<ControllerInputCallback> m_controllerInputCallbacks;
    std::vector<HapticFeedbackCallback> m_hapticFeedbackCallbacks;
    
    // Error handling
    std::string m_lastError;
    
    // Private methods
    bool createInstance();
    bool createSession();
    bool createSwapchains();
    bool createActions();
    bool createActionSpaces();
    
    void updateControllerStates();
    void updatePerformanceMetrics();
    
    bool checkXRResult(XrResult result, const std::string& operation);
    void logError(const std::string& error);
    
    // Frame timing
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    std::chrono::high_resolution_clock::time_point m_sessionStartTime;
};

} // namespace uevr
