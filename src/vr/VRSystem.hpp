#pragma once

#include <memory>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <mutex>
#include <atomic>
#include <chrono>

namespace UEVR {

// Forward declarations
class Logger;
class ConfigurationManager;

/**
 * @brief VR system types
 */
enum class VRSystemType {
    NONE,
    OPENVR,
    OPENXR,
    OCULUS,
    PICO,
    META_QUEST
};

/**
 * @brief VR pose structure
 */
struct VRPose {
    std::array<std::array<float, 4>, 3> deviceToAbsoluteTracking;
    std::array<float, 3> velocity;
    std::array<float, 3> angularVelocity;
    bool poseIsValid;
    bool deviceIsConnected;
    
    VRPose() : poseIsValid(false), deviceIsConnected(false) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j) {
                deviceToAbsoluteTracking[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
        velocity.fill(0.0f);
        angularVelocity.fill(0.0f);
    }
};

/**
 * @brief VR render target structure
 */
struct VRRenderTarget {
    uint32_t width;
    uint32_t height;
    void* colorTexture;
    void* depthTexture;
    uint32_t colorFormat;
    uint32_t depthFormat;
    
    VRRenderTarget() : width(0), height(0), colorTexture(nullptr), depthTexture(nullptr),
                       colorFormat(0), depthFormat(0) {}
};

/**
 * @brief VR system class
 */
class VRSystem {
public:
    VRSystem();
    ~VRSystem();
    
    bool Initialize(std::shared_ptr<ConfigurationManager> configManager, std::shared_ptr<Logger> logger);
    void Shutdown();
    
    bool IsVRAvailable() const;
    bool IsHMDConnected() const;
    VRSystemType GetSystemType() const { return m_systemType; }
    std::string GetSystemName() const;
    
    std::pair<uint32_t, uint32_t> GetRecommendedRenderTargetSize(int eye = 0) const;
    
    void Update();
    std::vector<VRPose> GetDevicePoses() const;
    VRPose GetHMDPose() const;
    
    bool SubmitFrame(int eye, const VRRenderTarget& renderTarget);
    bool WaitForVRCompositor();
    
    std::string GetVRStatistics() const;

private:
    std::shared_ptr<ConfigurationManager> m_configManager;
    std::shared_ptr<Logger> m_logger;
    std::atomic<bool> m_initialized;
    VRSystemType m_systemType;
    
    void* m_vrSystem; // Platform-specific VR system
    std::chrono::high_resolution_clock::time_point m_lastUpdate;
    
    bool InitializeOpenVR();
    void ShutdownOpenVR();
    void LogVREvent(const std::string& event, const std::string& details = "");
};

} // namespace UEVR