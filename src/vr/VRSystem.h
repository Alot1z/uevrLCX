#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <functional>
#include "../core/SystemState.hpp"

namespace UEVRLCX {

// ============================================================================
// Forward Declarations
// ============================================================================

class VRRenderer;
class VRInput;
class VRAudio;
class VRComfort;
class VRPlatform;

// ============================================================================
// VR Data Structures
// ============================================================================

struct VRMetrics {
    // Display Metrics
    double refreshRate;
    double resolution;
    double fieldOfView;
    double pixelDensity;
    
    // Performance Metrics
    double frameRate;
    double latency;
    double reprojectionRate;
    double droppedFrames;
    
    // Comfort Metrics
    VRComfortLevel comfortLevel;
    bool motionSicknessDetected;
    double comfortScore;
    std::vector<double> trackingAccuracy;
    
    // Timestamps
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::steady_clock::time_point lastComfortCheck;
    
    VRMetrics() : refreshRate(90.0), resolution(1.0), fieldOfView(110.0),
                  pixelDensity(0.0), frameRate(0.0), latency(0.0),
                  reprojectionRate(0.0), droppedFrames(0.0),
                  comfortLevel(VRComfortLevel::COMFORTABLE), motionSicknessDetected(false),
                  comfortScore(100.0) {
        timestamp = std::chrono::steady_clock::now();
        lastComfortCheck = std::chrono::steady_clock::now();
    }
};

struct VRDevice {
    std::string name;
    std::string type;
    std::string platform;
    bool isConnected;
    bool isActive;
    std::chrono::steady_clock::time_point lastSeen;
    
    VRDevice() : isConnected(false), isActive(false) {
        lastSeen = std::chrono::steady_clock::now();
    }
    
    VRDevice(const std::string& n, const std::string& t, const std::string& p)
        : name(n), type(t), platform(p), isConnected(false), isActive(false) {
        lastSeen = std::chrono::steady_clock::now();
    }
};

struct VRComfortSettings {
    VRComfortLevel targetLevel;
    bool enableMotionSicknessPrevention;
    bool enableComfortTunneling;
    bool enableEyeTracking;
    bool enableHandTracking;
    double maxFieldOfView;
    double maxMovementSpeed;
    
    VRComfortSettings() : targetLevel(VRComfortLevel::COMFORTABLE),
                          enableMotionSicknessPrevention(true),
                          enableComfortTunneling(true), enableEyeTracking(true),
                          enableHandTracking(true), maxFieldOfView(120.0),
                          maxMovementSpeed(10.0) {}
};

// ============================================================================
// VR System Interface
// ============================================================================

class IVRSystem {
public:
    virtual ~IVRSystem() = default;
    
    // ========================================================================
    // Initialization and Management
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ========================================================================
    // VR Device Management
    // ========================================================================
    
    virtual bool ConnectDevice(const std::string& deviceName) = 0;
    virtual bool DisconnectDevice(const std::string& deviceName) = 0;
    virtual bool IsDeviceConnected(const std::string& deviceName) const = 0;
    virtual std::vector<VRDevice> GetConnectedDevices() const = 0;
    virtual VRDevice* GetDevice(const std::string& deviceName) = 0;
    
    // ========================================================================
    // VR Rendering
    // ========================================================================
    
    virtual bool StartRendering() = 0;
    virtual bool StopRendering() = 0;
    virtual bool IsRendering() const = 0;
    virtual bool SetRenderTarget(void* target) = 0;
    virtual bool RenderFrame() = 0;
    
    // ========================================================================
    // VR Input
    // ========================================================================
    
    virtual bool EnableInput(bool enable) = 0;
    virtual bool IsInputEnabled() const = 0;
    virtual bool ProcessInput() = 0;
    virtual std::vector<double> GetInputData() const = 0;
    
    // ========================================================================
    // VR Audio
    // ========================================================================
    
    virtual bool EnableAudio(bool enable) = 0;
    virtual bool IsAudioEnabled() const = 0;
    virtual bool ProcessAudio() = 0;
    virtual bool SetAudioSource(const std::string& source) = 0;
    
    // ========================================================================
    // VR Comfort and Safety
    // ========================================================================
    
    virtual bool EnableComfortFeatures(bool enable) = 0;
    virtual bool AreComfortFeaturesEnabled() const = 0;
    virtual bool CheckComfortLevel() = 0;
    virtual VRComfortLevel GetCurrentComfortLevel() const = 0;
    virtual bool IsMotionSicknessDetected() const = 0;
    
    // ========================================================================
    // Configuration and Settings
    // ========================================================================
    
    virtual bool SetComfortSettings(const VRComfortSettings& settings) = 0;
    virtual VRComfortSettings GetComfortSettings() const = 0;
    virtual bool SetFieldOfView(double fov) = 0;
    virtual double GetFieldOfView() const = 0;
    
    // ========================================================================
    // Performance and Monitoring
    // ========================================================================
    
    virtual const VRMetrics& GetVRMetrics() const = 0;
    virtual bool UpdateMetrics() = 0;
    virtual double GetFrameRate() const = 0;
    virtual double GetLatency() const = 0;
    virtual std::string GetSystemHealth() const = 0;
};

// ============================================================================
// VR System Implementation
// ============================================================================

class VRSystem : public IVRSystem {
private:
    // Core Components
    std::unique_ptr<VRRenderer> m_renderer;
    std::unique_ptr<VRInput> m_input;
    std::unique_ptr<VRAudio> m_audio;
    std::unique_ptr<VRComfort> m_comfort;
    std::unique_ptr<VRPlatform> m_platform;
    
    // Device Management
    std::unordered_map<std::string, VRDevice> m_devices;
    std::vector<std::string> m_connectedDevices;
    
    // System State
    bool m_isInitialized;
    bool m_isRendering;
    bool m_inputEnabled;
    bool m_audioEnabled;
    bool m_comfortEnabled;
    
    // Configuration
    VRComfortSettings m_comfortSettings;
    double m_fieldOfView;
    
    // Metrics
    VRMetrics m_metrics;
    std::chrono::steady_clock::time_point m_lastMetricsUpdate;
    
public:
    VRSystem();
    ~VRSystem();
    
    // ========================================================================
    // IVRSystem Implementation
    // ========================================================================
    
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_isInitialized; }
    
    bool ConnectDevice(const std::string& deviceName) override;
    bool DisconnectDevice(const std::string& deviceName) override;
    bool IsDeviceConnected(const std::string& deviceName) const override;
    std::vector<VRDevice> GetConnectedDevices() const override;
    VRDevice* GetDevice(const std::string& deviceName) override;
    
    bool StartRendering() override;
    bool StopRendering() override;
    bool IsRendering() const override { return m_isRendering; }
    bool SetRenderTarget(void* target) override;
    bool RenderFrame() override;
    
    bool EnableInput(bool enable) override;
    bool IsInputEnabled() const override { return m_inputEnabled; }
    bool ProcessInput() override;
    std::vector<double> GetInputData() const override;
    
    bool EnableAudio(bool enable) override;
    bool IsAudioEnabled() const override { return m_audioEnabled; }
    bool ProcessAudio() override;
    bool SetAudioSource(const std::string& source) override;
    
    bool EnableComfortFeatures(bool enable) override;
    bool AreComfortFeaturesEnabled() const override { return m_comfortEnabled; }
    bool CheckComfortLevel() override;
    VRComfortLevel GetCurrentComfortLevel() const override;
    bool IsMotionSicknessDetected() const override;
    
    bool SetComfortSettings(const VRComfortSettings& settings) override;
    VRComfortSettings GetComfortSettings() const override { return m_comfortSettings; }
    bool SetFieldOfView(double fov) override;
    double GetFieldOfView() const override { return m_fieldOfView; }
    
    const VRMetrics& GetVRMetrics() const override { return m_metrics; }
    bool UpdateMetrics() override;
    double GetFrameRate() const override;
    double GetLatency() const override;
    std::string GetSystemHealth() const override;
    
private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================
    
    bool InitializeRenderer();
    bool InitializeInput();
    bool InitializeAudio();
    bool InitializeComfort();
    bool InitializePlatform();
    
    bool ValidateDevice(const std::string& deviceName);
    bool AuthenticateDevice(VRDevice& device);
    void UpdateDeviceStatus(const std::string& deviceName);
    
    bool ValidateRenderTarget(void* target);
    bool SetupRenderingPipeline();
    void UpdateRenderingMetrics();
    
    bool ValidateInputData(const std::vector<double>& data);
    void ProcessInputEvents();
    void UpdateInputMetrics();
    
    bool ValidateAudioSource(const std::string& source);
    void ProcessAudioStream();
    void UpdateAudioMetrics();
    
    bool ValidateComfortSettings(const VRComfortSettings& settings);
    void ApplyComfortSettings();
    void UpdateComfortMetrics();
    
    void LogDeviceOperation(const std::string& operation, const std::string& deviceName);
    void LogRenderingOperation(const std::string& operation);
    void LogInputOperation(const std::string& operation);
    void LogAudioOperation(const std::string& operation);
    void LogComfortOperation(const std::string& operation);
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<IVRSystem> CreateVRSystem();
std::unique_ptr<IVRSystem> CreateVRSystemWithConfig(const std::string& configPath);

} // namespace UEVRLCX


