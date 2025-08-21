#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Core system includes
#include "../core/Framework.hpp"
#include "../core/PluginManager.hpp"

namespace uevr {

/**
 * @brief Stereoscopic Rendering System
 * 
 * Converts 2D games into 3D stereoscopic VR experiences:
 * - Depth buffer extraction and analysis
 * - Stereoscopic rendering pipeline
 * - VR viewport management
 * - Performance optimization
 * - Quality control and comfort settings
 */
class StereoscopicRenderer {
public:
    // Singleton pattern
    static StereoscopicRenderer& getInstance();
    
    // Disable copy and assignment
    StereoscopicRenderer(const StereoscopicRenderer&) = delete;
    StereoscopicRenderer& operator=(const StereoscopicRenderer&) = delete;

    // Core system lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }

    // Rendering configuration
    struct RenderConfig {
        // Stereoscopic settings
        float interocularDistance = 0.064f; // 64mm default
        float convergenceDistance = 2.0f;   // 2 meters default
        float depthScale = 1.0f;
        float depthOffset = 0.0f;
        
        // Quality settings
        int renderResolution = 1; // 1x, 2x, etc.
        bool enableMSAA = true;
        int msaaSamples = 4;
        bool enableAnisotropicFiltering = true;
        
        // Performance settings
        bool enableFoveatedRendering = true;
        bool enableAsyncTimewarp = true;
        bool enableDynamicResolution = true;
        float performanceScale = 1.0f;
        
        // Comfort settings
        bool enableComfortMode = true;
        float comfortRadius = 0.5f;
        bool enableMotionBlur = true;
        float motionBlurStrength = 0.5f;
        
        // Advanced settings
        bool enableDepthPrepass = true;
        bool enableOcclusionCulling = true;
        bool enableInstancing = true;
        bool enableTessellation = true;
    };

    void setRenderConfig(const RenderConfig& config);
    RenderConfig getRenderConfig() const;

    // Viewport management
    struct VRViewport {
        int x, y, width, height;
        float fovX, fovY;
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
        bool isLeftEye;
        
        VRViewport() : x(0), y(0), width(0), height(0), fovX(90.0f), fovY(90.0f), isLeftEye(true) {}
    };

    bool setupVRViewports(const VRViewport& leftEye, const VRViewport& rightEye);
    VRViewport getLeftEyeViewport() const;
    VRViewport getRightEyeViewport() const;
    bool updateViewports(const glm::mat4& leftView, const glm::mat4& rightView);

    // Depth buffer processing
    struct DepthInfo {
        void* depthBuffer;
        size_t bufferSize;
        int width, height;
        float* depthData;
        float minDepth, maxDepth;
        float averageDepth;
        std::vector<float> depthHistogram;
        
        DepthInfo() : depthBuffer(nullptr), bufferSize(0), width(0), height(0),
                     depthData(nullptr), minDepth(0.0f), maxDepth(1.0f), averageDepth(0.5f) {}
    };

    bool extractDepthBuffer(void* renderTarget, DepthInfo& depthInfo);
    bool analyzeDepthBuffer(DepthInfo& depthInfo);
    bool optimizeDepthBuffer(DepthInfo& depthInfo);
    std::vector<float> getDepthHistogram() const;

    // Stereoscopic rendering
    struct StereoFrame {
        void* leftEyeTexture;
        void* rightEyeTexture;
        int width, height;
        bool isComplete;
        double timestamp;
        
        StereoFrame() : leftEyeTexture(nullptr), rightEyeTexture(nullptr),
                       width(0), height(0), isComplete(false), timestamp(0.0) {}
    };

    bool renderStereoscopic(void* sourceTexture, StereoFrame& stereoFrame);
    bool renderLeftEye(void* sourceTexture, void* targetTexture);
    bool renderRightEye(void* sourceTexture, void* targetTexture);
    bool compositeStereoFrame(const StereoFrame& stereoFrame, void* outputTexture);

    // Shader management
    struct ShaderProgram {
        std::string name;
        void* vertexShader;
        void* pixelShader;
        void* computeShader;
        std::unordered_map<std::string, void*> constantBuffers;
        std::unordered_map<std::string, void*> textures;
        bool isCompiled;
        
        ShaderProgram() : vertexShader(nullptr), pixelShader(nullptr), 
                         computeShader(nullptr), isCompiled(false) {}
    };

    bool compileShader(const std::string& shaderName, const std::string& vertexSource, 
                      const std::string& pixelSource);
    bool loadShader(const std::string& shaderName, const std::string& filePath);
    bool useShader(const std::string& shaderName);
    bool setShaderParameter(const std::string& shaderName, const std::string& paramName, 
                           const void* data, size_t size);

    // VR-specific shaders
    bool compileStereoscopicShader();
    bool compileDepthExtractionShader();
    bool compileVRPostProcessShader();
    bool compileFoveatedRenderingShader();
    bool compileComfortModeShader();

    // Performance optimization
    struct PerformanceMetrics {
        double frameTime;
        double renderTime;
        double depthExtractionTime;
        double stereoRenderTime;
        double postProcessTime;
        int drawCalls;
        int triangles;
        double gpuTime;
        double cpuTime;
        double memoryUsage;
        
        PerformanceMetrics() : frameTime(0.0), renderTime(0.0), depthExtractionTime(0.0),
                             stereoRenderTime(0.0), postProcessTime(0.0), drawCalls(0),
                             triangles(0), gpuTime(0.0), cpuTime(0.0), memoryUsage(0.0) {}
    };

    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    bool optimizePerformance();

    // Quality control
    struct QualitySettings {
        float overallQuality;
        float textureQuality;
        float shadowQuality;
        float lightingQuality;
        float postProcessQuality;
        float antiAliasingQuality;
        
        QualitySettings() : overallQuality(1.0f), textureQuality(1.0f), shadowQuality(1.0f),
                          lightingQuality(1.0f), postProcessQuality(1.0f), antiAliasingQuality(1.0f) {}
    };

    void setQualitySettings(const QualitySettings& settings);
    QualitySettings getQualitySettings() const;
    bool adjustQualityForPerformance();

    // Comfort and safety
    struct ComfortSettings {
        bool enableComfortMode;
        float comfortRadius;
        bool enableMotionBlur;
        float motionBlurStrength;
        bool enableChromaticAberration;
        float chromaticAberrationStrength;
        bool enableVignette;
        float vignetteStrength;
        
        ComfortSettings() : enableComfortMode(true), comfortRadius(0.5f), enableMotionBlur(true),
                          motionBlurStrength(0.5f), enableChromaticAberration(false),
                          chromaticAberrationStrength(0.0f), enableVignette(true), vignetteStrength(0.3f) {}
    };

    void setComfortSettings(const ComfortSettings& settings);
    ComfortSettings getComfortSettings() const;
    bool applyComfortMode(void* texture, int width, int height);

    // Error handling
    struct RenderError {
        std::string errorCode;
        std::string errorMessage;
        std::string shaderName;
        std::string timestamp;
        std::string suggestedFix;
        
        RenderError() {}
    };

    std::vector<RenderError> getRecentErrors() const;
    void clearErrors();
    bool hasErrors() const;

    // Advanced features
    bool enableFoveatedRendering(bool enable);
    bool enableAsyncTimewarp(bool enable);
    bool enableDynamicResolution(bool enable);
    bool enableMotionReprojection(bool enable);

    // Utility functions
    glm::mat4 createProjectionMatrix(float fovX, float fovY, float nearPlane, float farPlane);
    glm::mat4 createViewMatrix(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
    glm::mat4 createStereoViewMatrix(const glm::mat4& baseView, float eyeOffset, bool isLeftEye);

private:
    StereoscopicRenderer();
    ~StereoscopicRenderer();

    // Internal data structures
    std::atomic<bool> m_initialized{false};
    std::mutex m_mutex;
    std::condition_variable m_cv;
    
    // Core components
    std::unique_ptr<Framework> m_framework;
    std::unique_ptr<PluginManager> m_pluginManager;
    
    // Configuration
    RenderConfig m_renderConfig;
    QualitySettings m_qualitySettings;
    ComfortSettings m_comfortSettings;
    
    // Viewports
    VRViewport m_leftEyeViewport;
    VRViewport m_rightEyeViewport;
    
    // Shaders
    std::unordered_map<std::string, ShaderProgram> m_shaders;
    std::string m_currentShader;
    
    // Performance tracking
    PerformanceMetrics m_performanceMetrics;
    mutable std::mutex m_metricsMutex;
    
    // Error handling
    std::vector<RenderError> m_recentErrors;
    mutable std::mutex m_errorsMutex;
    
    // Internal methods
    bool initializeShaders();
    bool initializeViewports();
    bool initializePerformanceMonitoring();
    
    // Shader implementations
    bool compileVertexShader(const std::string& source, void** shader);
    bool compilePixelShader(const std::string& source, void** shader);
    bool compileComputeShader(const std::string& source, void** shader);
    bool createConstantBuffer(size_t size, void** buffer);
    bool createTexture(int width, int height, void** texture);
    
    // Rendering implementations
    bool renderEye(void* sourceTexture, void* targetTexture, const VRViewport& viewport, bool isLeftEye);
    bool applyPostProcessing(void* texture, int width, int height);
    bool applyComfortMode(void* texture, int width, int height);
    bool applyFoveatedRendering(void* texture, int width, int height);
    
    // Performance implementations
    void updatePerformanceMetrics(const std::string& operation, double time);
    bool optimizeForPerformance();
    bool adjustQualitySettings();
    
    // Error handling
    void logError(const std::string& errorCode, const std::string& errorMessage, 
                  const std::string& shaderName = "");
    
    // Utility methods
    glm::vec3 calculateEyeOffset(float interocularDistance, bool isLeftEye);
    float calculateConvergenceDepth(const DepthInfo& depthInfo);
    void cleanupResources();
};

} // namespace uevr
