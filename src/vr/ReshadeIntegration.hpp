#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace uevr {

/**
 * @brief Reshade Integration - Shader Injection Framework
 * 
 * Integrates Reshade (https://github.com/crosire/reshade) into the uevrLCX system.
 * Reshade is a generic post-processing injector that can enhance games with
 * advanced shader effects and VR-specific rendering improvements.
 * 
 * Key Features:
 * - Universal shader injection
 * - VR-optimized post-processing
 * - Real-time shader effects
 * - Performance-optimized rendering
 * - Multi-engine shader support
 */
class ReshadeIntegration {
public:
    ReshadeIntegration();
    ~ReshadeIntegration();

    // Core lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized; }

    // Reshade injection
    bool injectReshade(const std::string& gamePath);
    bool setupReshadeFramework();
    bool configureReshadeSettings();
    
    // Shader management
    bool loadShader(const std::string& shaderPath);
    bool enableShader(const std::string& shaderName, bool enable);
    bool configureShader(const std::string& shaderName, const std::string& parameter, float value);
    bool reloadShaders();
    
    // VR-specific shaders
    bool enableVRStereoShader(bool enable);
    bool enableVRDepthShader(bool enable);
    bool enableVRPerformanceShader(bool enable);
    bool enableVRQualityShader(bool enable);
    
    // Post-processing effects
    bool enablePostProcessing(bool enable);
    bool configurePostProcessing();
    bool optimizePostProcessing();
    bool validatePostProcessing();

    // Configuration
    struct ReshadeSettings {
        // Injection settings
        bool enableInjection = true;
        bool enableShaders = true;
        bool enablePostProcessing = true;
        std::string shaderPath = "shaders/";
        std::string texturePath = "textures/";
        
        // VR-specific settings
        bool enableVRStereo = true;
        bool enableVRDepth = true;
        bool enableVRPerformance = true;
        bool enableVRQuality = true;
        float vrStereoStrength = 1.0f;
        float vrDepthStrength = 1.0f;
        
        // Performance settings
        bool enablePerformanceOptimization = true;
        int maxShaderPasses = 10;
        bool enableAsyncProcessing = true;
        float performanceScale = 1.0f;
        
        // Quality settings
        bool enableHighQuality = true;
        int textureQuality = 1; // 0 = low, 1 = medium, 2 = high
        bool enableAnisotropicFiltering = true;
        int anisotropicLevel = 16;
        
        // Advanced settings
        bool debugMode = false;
        bool enableShaderLogging = true;
        std::string logLevel = "info";
    };

    void setReshadeSettings(const ReshadeSettings& settings);
    ReshadeSettings getReshadeSettings() const;

    // Status and monitoring
    struct ReshadeStatus {
        bool isInjected = false;
        bool isActive = false;
        bool shadersLoaded = false;
        bool postProcessingActive = false;
        int activeShaders = 0;
        float processingTime = 0.0f;
        std::string lastError;
    };

    ReshadeStatus getStatus() const;
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

    // Shader information
    struct ShaderInfo {
        std::string name;
        std::string path;
        bool enabled = false;
        std::unordered_map<std::string, float> parameters;
        std::vector<std::string> techniques;
        std::string description;
    };

    std::vector<ShaderInfo> getLoadedShaders() const;
    ShaderInfo getShaderInfo(const std::string& shaderName) const;
    bool updateShaderParameters(const std::string& shaderName, const std::unordered_map<std::string, float>& parameters);

    // Performance monitoring
    struct ShaderPerformance {
        std::string shaderName;
        float frameTime = 0.0f;
        float gpuTime = 0.0f;
        int drawCalls = 0;
        int textureSwitches = 0;
    };

    std::vector<ShaderPerformance> getShaderPerformance() const;
    float getTotalProcessingTime() const;

private:
    // Reshade framework integration
    struct ReshadeFramework;
    std::unique_ptr<ReshadeFramework> m_reshadeFramework;
    
    // Settings and state
    ReshadeSettings m_settings;
    ReshadeStatus m_status;
    bool m_initialized = false;
    
    // Shader management
    std::unordered_map<std::string, ShaderInfo> m_loadedShaders;
    std::vector<ShaderPerformance> m_shaderPerformance;
    
    // Error tracking
    std::vector<std::string> m_errors;
    
    // Internal methods
    bool loadReshadeLibrary();
    bool initializeReshadeFramework();
    bool setupReshadeCallbacks();
    void updateStatus();
    void logError(const std::string& error);
    bool validateShaderFile(const std::string& shaderPath);
    bool optimizeShaderPerformance();
    bool validateShaderCompatibility(const std::string& shaderName);
    void updateShaderPerformance();
};

} // namespace uevr
