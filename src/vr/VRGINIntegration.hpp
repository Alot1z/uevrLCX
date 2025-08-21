#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace uevr {

/**
 * @brief VRGIN Integration - Universal VR Injector
 * 
 * Integrates VRGIN (https://github.com/Eusth/VRGIN) into the uevrLCX system.
 * VRGIN is a universal VR injector that can add VR support to any Unity game.
 * 
 * Key Features:
 * - Universal Unity game VR injection
 * - Stereoscopic rendering
 * - Motion controller support
 * - Room-scale tracking
 * - Performance optimization
 */
class VRGINIntegration {
public:
    VRGINIntegration();
    ~VRGINIntegration();

    // Core lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized; }

    // VRGIN-specific functionality
    bool injectIntoUnityGame(const std::string& gamePath);
    bool setupVRGINFramework();
    bool configureVRGINSettings();
    
    // VR rendering
    bool enableStereoscopicRendering(bool enable);
    bool setInterpupillaryDistance(float ipd);
    bool setEyeSeparation(float separation);
    
    // Motion controllers
    bool enableMotionControllers(bool enable);
    bool mapControllerInputs();
    bool configureHapticFeedback();
    
    // Room-scale tracking
    bool enableRoomScaleTracking(bool enable);
    bool setPlayAreaSize(float width, float height);
    bool configureBoundarySystem();
    
    // Performance optimization
    bool enablePerformanceOptimization(bool enable);
    bool setTargetFrameRate(int frameRate);
    bool configureQualitySettings();

    // Configuration
    struct VRGINSettings {
        // Rendering settings
        bool stereoscopicRendering = true;
        float interpupillaryDistance = 0.064f; // 64mm default
        float eyeSeparation = 0.064f;
        bool enableMSAA = true;
        int msaaSamples = 4;
        
        // Controller settings
        bool motionControllers = true;
        bool hapticFeedback = true;
        float hapticIntensity = 1.0f;
        
        // Tracking settings
        bool roomScaleTracking = true;
        float playAreaWidth = 3.0f;  // meters
        float playAreaHeight = 3.0f; // meters
        bool boundarySystem = true;
        
        // Performance settings
        bool performanceOptimization = true;
        int targetFrameRate = 90;
        bool adaptiveQuality = true;
        float qualityScale = 1.0f;
        
        // Advanced settings
        bool debugMode = false;
        bool enableLogging = true;
        std::string logLevel = "info";
    };

    void setVRGINSettings(const VRGINSettings& settings);
    VRGINSettings getVRGINSettings() const;

    // Status and monitoring
    struct VRGINStatus {
        bool isInjected = false;
        bool isRendering = false;
        bool controllersConnected = false;
        bool trackingActive = false;
        float currentFrameRate = 0.0f;
        float latency = 0.0f;
        std::string lastError;
    };

    VRGINStatus getStatus() const;
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

private:
    // VRGIN framework integration
    struct VRGINFramework;
    std::unique_ptr<VRGINFramework> m_vrginFramework;
    
    // Settings and state
    VRGINSettings m_settings;
    VRGINStatus m_status;
    bool m_initialized = false;
    
    // Error tracking
    std::vector<std::string> m_errors;
    
    // Internal methods
    bool loadVRGINLibrary();
    bool initializeVRGINFramework();
    bool setupVRGINCallbacks();
    void updateStatus();
    void logError(const std::string& error);
    bool validateGameCompatibility(const std::string& gamePath);
};

} // namespace uevr
