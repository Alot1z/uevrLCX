#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_map>

namespace uevrLCX {

enum class VRState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    CALIBRATING,
    READY,
    RUNNING,
    PAUSED,
    ERROR,
    DISCONNECTING,
    SHUTDOWN
};

enum class VRHeadsetType {
    UNKNOWN,
    OCULUS_QUEST,
    OCULUS_QUEST_2,
    OCULUS_QUEST_3,
    OCULUS_RIFT,
    OCULUS_RIFT_S,
    HTC_VIVE,
    HTC_VIVE_PRO,
    HTC_VIVE_COSMOS,
    VALVE_INDEX,
    WINDOWS_MR,
    PICO_4,
    PICO_NEO_3,
    CUSTOM
};

enum class VRControllerType {
    UNKNOWN,
    OCULUS_TOUCH,
    HTC_VIVE_WAND,
    VALVE_INDEX_CONTROLLER,
    WINDOWS_MR_CONTROLLER,
    PICO_CONTROLLER,
    CUSTOM
};

struct VRDisplayInfo {
    uint32_t width;
    uint32_t height;
    float refreshRate;
    float fieldOfView;
    float ipd;
    bool isConnected;
};

struct VRControllerInfo {
    VRControllerType type;
    bool isConnected;
    bool hasHaptics;
    bool hasTouchpad;
    bool hasJoystick;
    bool hasGrip;
    bool hasTrigger;
    bool hasMenu;
    bool hasSystem;
};

struct VRTrackingInfo {
    float position[3];
    float rotation[4];
    float velocity[3];
    float angularVelocity[3];
    bool isTracked;
    float confidence;
};

struct VRPerformanceMetrics {
    double frameRate;
    double latency;
    double droppedFrames;
    double reprojectionRate;
    double gpuTime;
    double cpuTime;
    double totalTime;
};

class VRSystem {
public:
    VRSystem();
    ~VRSystem();

    // Core lifecycle
    bool initialize();
    void shutdown();
    void update();
    
    // State management
    VRState getState() const;
    void setState(VRState state);
    bool isConnected() const;
    bool isReady() const;
    
    // Headset management
    VRHeadsetType getHeadsetType() const;
    VRDisplayInfo getDisplayInfo() const;
    bool setIPD(float ipd);
    float getIPD() const;
    bool setRefreshRate(float refreshRate);
    float getRefreshRate() const;
    
    // Controller management
    std::vector<VRControllerInfo> getControllers() const;
    VRControllerInfo getControllerInfo(int controllerIndex) const;
    VRTrackingInfo getControllerTracking(int controllerIndex) const;
    bool setControllerHaptics(int controllerIndex, float frequency, float amplitude, float duration);
    
    // Tracking
    VRTrackingInfo getHeadTracking() const;
    VRTrackingInfo getEyeTracking() const;
    bool isEyeTrackingAvailable() const;
    bool isFullBodyTrackingAvailable() const;
    
    // Rendering
    bool beginFrame();
    bool endFrame();
    bool submitFrame();
    bool setRenderTarget(void* leftEye, void* rightEye);
    bool setDepthBuffer(void* depthBuffer);
    
    // Performance
    VRPerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    bool setPerformanceLevel(int level);
    int getPerformanceLevel() const;
    
    // Configuration
    bool loadConfiguration(const std::string& filename);
    bool saveConfiguration(const std::string& filename);
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key, const std::string& defaultValue = "") const;
    
    // Calibration
    bool startCalibration();
    bool stopCalibration();
    bool isCalibrating() const;
    float getCalibrationProgress() const;
    
    // Comfort features
    bool enableComfortMode(bool enable);
    bool isComfortModeEnabled() const;
    bool setComfortSettings(float vignette, float fadeDistance);
    
    // Event handling
    using VREventCallback = std::function<void(const std::string&, const void*)>;
    void registerEventCallback(const std::string& eventType, VREventCallback callback);
    void unregisterEventCallback(const std::string& eventType);
    
    // Error handling
    void setErrorHandler(std::function<void(const std::string&, const std::string&)> handler);
    void reportError(const std::string& component, const std::string& error);
    
    // Logging
    void setLogCallback(std::function<void(const std::string&, const std::string&)> callback);
    void log(const std::string& level, const std::string& message);

private:
    // Core members
    std::atomic<VRState> state;
    VRHeadsetType headsetType;
    VRDisplayInfo displayInfo;
    std::vector<VRControllerInfo> controllers;
    VRPerformanceMetrics performanceMetrics;
    
    // Configuration
    std::unordered_map<std::string, std::string> configuration;
    mutable std::mutex configMutex;
    
    // Calibration
    bool calibrating;
    float calibrationProgress;
    
    // Comfort
    bool comfortModeEnabled;
    float comfortVignette;
    float comfortFadeDistance;
    
    // Performance
    int performanceLevel;
    
    // Callbacks
    std::unordered_map<std::string, std::vector<VREventCallback>> eventCallbacks;
    std::function<void(const std::string&, const std::string&)> errorHandler;
    std::function<void(const std::string&, const std::string&)> logCallback;
    
    // Thread safety
    mutable std::mutex vrMutex;
    
    // Internal methods
    bool initializeOpenVR();
    bool initializeOculus();
    bool initializeSteamVR();
    void updateTracking();
    void updatePerformance();
    void processEvents();
    void notifyEvent(const std::string& eventType, const void* data);
    
    // Platform-specific implementations
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace uevrLCX


