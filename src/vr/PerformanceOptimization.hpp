#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace uevr {

/**
 * @brief Performance Optimization - VR Performance Enhancement
 * 
 * Implements advanced VR performance optimization techniques including
 * foveated rendering, async timewarp, multi-resolution shading, and
 * other VR-specific optimizations for the uevrLCX system.
 * 
 * Key Features:
 * - Foveated rendering (eye-tracking optimization)
 * - Async timewarp (frame optimization)
 * - Multi-resolution shading
 * - Dynamic quality scaling
 * - VR-specific performance optimization
 */
class PerformanceOptimization {
public:
    PerformanceOptimization();
    ~PerformanceOptimization();

    // Core lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized; }

    // Foveated rendering
    bool enableFoveatedRendering(bool enable);
    bool setupFoveatedRendering();
    bool configureFoveatedRendering();
    bool optimizeFoveatedRendering();
    
    // Async timewarp
    bool enableAsyncTimewarp(bool enable);
    bool setupAsyncTimewarp();
    bool configureAsyncTimewarp();
    bool optimizeAsyncTimewarp();
    
    // Multi-resolution shading
    bool enableMultiResolutionShading(bool enable);
    bool setupMultiResolutionShading();
    bool configureMultiResolutionShading();
    bool optimizeMultiResolutionShading();
    
    // Dynamic quality scaling
    bool enableDynamicQualityScaling(bool enable);
    bool setupDynamicQualityScaling();
    bool configureDynamicQualityScaling();
    bool optimizeDynamicQualityScaling();
    
    // VR-specific optimizations
    bool enableVROptimizations(bool enable);
    bool setupVROptimizations();
    bool configureVROptimizations();
    bool optimizeVROptimizations();

    // Configuration
    struct PerformanceSettings {
        // Foveated rendering settings
        bool enableFoveatedRendering = true;
        float foveatedRadius = 0.3f; // 30% of screen radius
        float foveatedFalloff = 0.5f; // 50% falloff
        int foveatedQuality = 2; // 0 = low, 1 = medium, 2 = high
        bool enableEyeTracking = true;
        
        // Async timewarp settings
        bool enableAsyncTimewarp = true;
        float timewarpStrength = 1.0f;
        bool enableReprojection = true;
        bool enableMotionPrediction = true;
        int timewarpBufferSize = 3;
        
        // Multi-resolution shading settings
        bool enableMultiResolutionShading = true;
        float centerQuality = 1.0f;
        float peripheryQuality = 0.5f;
        int shadingSamples = 4;
        bool enableAdaptiveShading = true;
        
        // Dynamic quality scaling settings
        bool enableDynamicQualityScaling = true;
        float targetFrameRate = 90.0f;
        float minQuality = 0.5f;
        float maxQuality = 1.0f;
        float qualityStep = 0.1f;
        int adaptationSpeed = 10; // frames
        
        // VR-specific settings
        bool enableVROptimizations = true;
        bool enableSinglePassRendering = true;
        bool enableInstancedRendering = true;
        bool enableOcclusionCulling = true;
        bool enableLODOptimization = true;
        
        // Performance settings
        bool enablePerformanceMonitoring = true;
        bool enableAutoOptimization = true;
        float performanceThreshold = 0.8f; // 80% target performance
        int optimizationInterval = 60; // frames
        
        // Advanced settings
        bool debugMode = false;
        bool enablePerformanceLogging = true;
        std::string logLevel = "info";
    };

    void setPerformanceSettings(const PerformanceSettings& settings);
    PerformanceSettings getPerformanceSettings() const;

    // Status and monitoring
    struct PerformanceStatus {
        bool foveatedRenderingActive = false;
        bool asyncTimewarpActive = false;
        bool multiResolutionShadingActive = false;
        bool dynamicQualityScalingActive = false;
        bool vrOptimizationsActive = false;
        float currentQuality = 1.0f;
        float currentFrameRate = 0.0f;
        float optimizationLevel = 0.0f;
        std::string lastError;
    };

    PerformanceStatus getStatus() const;
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

    // Performance metrics
    struct PerformanceMetrics {
        float frameRate = 0.0f;
        float latency = 0.0f;
        float gpuTime = 0.0f;
        float cpuTime = 0.0f;
        float memoryUsage = 0.0f;
        int droppedFrames = 0;
        float qualityScore = 0.0f;
        float optimizationScore = 0.0f;
    };

    PerformanceMetrics getPerformanceMetrics() const;
    bool updatePerformanceMetrics();
    bool validatePerformanceMetrics(const PerformanceMetrics& metrics);

    // Optimization control
    bool optimizeFrame();
    bool optimizeQuality();
    bool optimizePerformance();
    bool resetOptimizations();
    
    // Real-time optimization
    bool enableRealTimeOptimization(bool enable);
    bool setupRealTimeOptimization();
    bool configureRealTimeOptimization();
    bool processRealTimeOptimization();

    // Eye tracking integration
    struct EyeTrackingData {
        std::vector<float> leftEyePosition;
        std::vector<float> rightEyePosition;
        std::vector<float> gazeDirection;
        float eyeSeparation = 0.064f;
        bool isTracking = false;
    };

    EyeTrackingData getEyeTrackingData() const;
    bool updateEyeTrackingData();
    bool validateEyeTrackingData(const EyeTrackingData& data);

    // Performance profiling
    struct PerformanceProfile {
        std::string profileName;
        PerformanceMetrics metrics;
        PerformanceSettings settings;
        float profileScore = 0.0f;
        std::string timestamp;
    };

    std::vector<PerformanceProfile> getPerformanceProfiles() const;
    bool savePerformanceProfile(const std::string& profileName);
    bool loadPerformanceProfile(const std::string& profileName);
    bool deletePerformanceProfile(const std::string& profileName);

private:
    // Performance optimization implementations
    struct FoveatedRenderingEngine;
    struct AsyncTimewarpEngine;
    struct MultiResolutionEngine;
    struct DynamicQualityEngine;
    struct VROptimizationEngine;
    
    std::unique_ptr<FoveatedRenderingEngine> m_foveatedEngine;
    std::unique_ptr<AsyncTimewarpEngine> m_timewarpEngine;
    std::unique_ptr<MultiResolutionEngine> m_multiResEngine;
    std::unique_ptr<DynamicQualityEngine> m_qualityEngine;
    std::unique_ptr<VROptimizationEngine> m_vrOptimizationEngine;
    
    // Settings and state
    PerformanceSettings m_settings;
    PerformanceStatus m_status;
    bool m_initialized = false;
    
    // Current data
    PerformanceMetrics m_currentMetrics;
    EyeTrackingData m_eyeTrackingData;
    std::vector<PerformanceProfile> m_performanceProfiles;
    
    // Error tracking
    std::vector<std::string> m_errors;
    
    // Internal methods
    bool initializeFoveatedRendering();
    bool initializeAsyncTimewarp();
    bool initializeMultiResolutionShading();
    bool initializeDynamicQualityScaling();
    bool initializeVROptimizations();
    bool setupPerformanceCallbacks();
    void updateStatus();
    void logError(const std::string& error);
    bool validatePerformanceSettings(const PerformanceSettings& settings);
    bool optimizePerformanceAlgorithms();
    bool validateEyeTrackingData();
    void updatePerformanceProfiles();
};

} // namespace uevr
