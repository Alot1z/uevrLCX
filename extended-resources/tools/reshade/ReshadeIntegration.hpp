#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <spdlog/spdlog.h>

// Reshade Core Integration
namespace uevrLCX {
namespace Reshade {

// Core Reshade Configuration
struct ReshadeConfig {
    bool enableReshade = true;
    bool enableShaderInjection = true;
    bool enablePostProcessing = true;
    bool enableVRShaders = true;
    bool enableCustomShaders = true;
    
    // Injection settings
    bool enableDLLInjection = true;
    bool enableAPIHooking = true;
    bool enableShaderCompilation = true;
    bool enableRealTimeUpdates = true;
    
    // Shader settings
    bool enableFXAA = true;
    bool enableBloom = true;
    bool enableColorCorrection = true;
    bool enableSharpen = true;
    bool enableDepthOfField = true;
    bool enableMotionBlur = true;
    bool enableAmbientOcclusion = true;
    bool enableGlobalIllumination = true;
    
    // VR-specific settings
    bool enableVRStereoShaders = true;
    bool enableVRDepthShaders = true;
    bool enableVRPerformanceShaders = true;
    bool enableVRQualityShaders = true;
    
    // Community integration
    bool enableRedditShaders = true;
    bool enableCommunityShaders = true;
    bool enableResearchShaders = true;
};

// Reshade Metrics
struct ReshadeMetrics {
    int injectedGames = 0;
    int loadedShaders = 0;
    int compiledShaders = 0;
    int failedCompilations = 0;
    float averageFrameRate = 0.0f;
    float averageLatency = 0.0f;
    float shaderQuality = 0.0f;
    int redditShadersApplied = 0;
    int communityShadersUsed = 0;
    int researchShadersApplied = 0;
    
    // Performance metrics
    float gpuUtilization = 0.0f;
    float shaderCompilationTime = 0.0f;
    float shaderExecutionTime = 0.0f;
    int activeShaders = 0;
    float memoryUsage = 0.0f;
};

// Shader Data Structure
struct ReshadeShader {
    std::string name;
    std::string source;
    std::string type;
    std::string version;
    std::unordered_map<std::string, float> parameters;
    bool isEnabled;
    bool isCompiled;
    float quality;
    std::string author;
    std::string description;
    
    ReshadeShader() : isEnabled(false), isCompiled(false), quality(0.0f) {}
};

// Game Profile for Reshade
struct ReshadeGameProfile {
    std::string gameName;
    std::string engineType;
    std::string graphicsAPI;
    std::vector<std::string> activeShaders;
    std::vector<std::string> redditShaders;
    std::vector<std::string> communityShaders;
    std::vector<std::string> researchShaders;
    float shaderQuality;
    bool isVerified;
    std::string lastTested;
};

// Reshade Event System
struct ReshadeEvent {
    enum class Type {
        ShaderLoaded,
        ShaderCompiled,
        ShaderApplied,
        ShaderFailed,
        ErrorOccurred,
        PerformanceWarning,
        RedditShaderApplied,
        CommunityShaderUsed,
        ResearchShaderApplied
    };
    
    Type type;
    std::string message;
    std::string gameName;
    std::string shaderName;
    float timestamp;
    std::unordered_map<std::string, std::string> data;
};

// Core Reshade Interface
class IReshadeCore {
public:
    virtual ~IReshadeCore() = default;
    
    virtual bool Initialize(const ReshadeConfig& config) = 0;
    virtual bool InjectIntoGame(const std::string& gameName) = 0;
    virtual bool LoadShader(const std::string& shaderPath) = 0;
    virtual bool CompileShader(const std::string& shaderName) = 0;
    virtual bool ApplyShader(const std::string& gameName, const std::string& shaderName) = 0;
    virtual bool ApplyRedditShader(const std::string& gameName, const std::string& shaderId) = 0;
    virtual bool ApplyCommunityShader(const std::string& gameName, const std::string& shaderId) = 0;
    virtual bool ApplyResearchShader(const std::string& gameName, const std::string& shaderId) = 0;
    virtual ReshadeMetrics GetMetrics() const = 0;
    virtual std::vector<ReshadeGameProfile> GetGameProfiles() const = 0;
    virtual std::vector<ReshadeShader> GetShaders() const = 0;
    virtual void Shutdown() = 0;
    
    // Event system
    using EventCallback = std::function<void(const ReshadeEvent&)>;
    virtual void RegisterEventCallback(EventCallback callback) = 0;
    virtual void UnregisterEventCallback() = 0;
};

// Reshade Injector Interface
class IReshadeInjector {
public:
    virtual ~IReshadeInjector() = default;
    
    virtual bool Initialize(const ReshadeConfig& config) = 0;
    virtual bool InjectDLL(const std::string& gameName) = 0;
    virtual bool HookAPI(const std::string& gameName) = 0;
    virtual bool SetupHooks(const std::string& gameName) = 0;
    virtual bool EnableShaderInjection(const std::string& gameName) = 0;
    virtual bool EnablePostProcessing(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// Reshade Shader Interface
class IReshadeShader {
public:
    virtual ~IReshadeShader() = default;
    
    virtual bool Initialize(const ReshadeConfig& config) = 0;
    virtual bool LoadShader(const std::string& shaderPath) = 0;
    virtual bool CompileShader(const std::string& shaderName) = 0;
    virtual bool EnableShader(const std::string& shaderName) = 0;
    virtual bool DisableShader(const std::string& shaderName) = 0;
    virtual bool SetShaderParameter(const std::string& shaderName, const std::string& parameter, float value) = 0;
    virtual bool UpdateShader(const std::string& shaderName) = 0;
    virtual std::vector<ReshadeShader> GetLoadedShaders() const = 0;
    virtual void Shutdown() = 0;
};

// Reshade Renderer Interface
class IReshadeRenderer {
public:
    virtual ~IReshadeRenderer() = default;
    
    virtual bool Initialize(const ReshadeConfig& config) = 0;
    virtual bool SetupRendering(const std::string& gameName) = 0;
    virtual bool RenderFrame(const std::string& gameName) = 0;
    virtual bool EnableVRStereoShaders(const std::string& gameName) = 0;
    virtual bool EnableVRDepthShaders(const std::string& gameName) = 0;
    virtual bool EnableVRPerformanceShaders(const std::string& gameName) = 0;
    virtual bool EnableVRQualityShaders(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// Reshade Optimizer Interface
class IReshadeOptimizer {
public:
    virtual ~IReshadeOptimizer() = default;
    
    virtual bool Initialize(const ReshadeConfig& config) = 0;
    virtual bool OptimizeShaders(const std::string& gameName) = 0;
    virtual bool OptimizePerformance(const std::string& gameName) = 0;
    virtual bool ApplyRedditOptimizations(const std::string& gameName) = 0;
    virtual bool ApplyCommunityOptimizations(const std::string& gameName) = 0;
    virtual bool ApplyResearchOptimizations(const std::string& gameName) = 0;
    virtual ReshadeMetrics GetOptimizationMetrics() const = 0;
    virtual void Shutdown() = 0;
};

// Main Reshade Integration Manager
class ReshadeIntegrationManager {
private:
    std::unique_ptr<IReshadeCore> m_core;
    std::unique_ptr<IReshadeInjector> m_injector;
    std::unique_ptr<IReshadeShader> m_shader;
    std::unique_ptr<IReshadeRenderer> m_renderer;
    std::unique_ptr<IReshadeOptimizer> m_optimizer;
    
    ReshadeConfig m_config;
    ReshadeMetrics m_metrics;
    std::vector<ReshadeGameProfile> m_gameProfiles;
    std::vector<ReshadeEvent> m_events;
    std::vector<ReshadeShader> m_shaders;
    
    bool m_initialized;
    bool m_running;

public:
    ReshadeIntegrationManager();
    ~ReshadeIntegrationManager();
    
    bool Initialize(const ReshadeConfig& config);
    bool StartInjection();
    bool StopInjection();
    bool InjectIntoGame(const std::string& gameName);
    bool LoadAndApplyShaders(const std::string& gameName);
    bool ApplyAllOptimizations(const std::string& gameName);
    
    ReshadeMetrics GetMetrics() const;
    std::vector<ReshadeGameProfile> GetGameProfiles() const;
    std::vector<ReshadeEvent> GetEvents() const;
    std::vector<ReshadeShader> GetShaders() const;
    
    void Shutdown();
    
private:
    void OnReshadeEvent(const ReshadeEvent& event);
    void UpdateMetrics();
    void LogInjectionStatus();
};

// Factory functions
std::unique_ptr<IReshadeCore> CreateReshadeCore();
std::unique_ptr<IReshadeInjector> CreateReshadeInjector();
std::unique_ptr<IReshadeShader> CreateReshadeShader();
std::unique_ptr<IReshadeRenderer> CreateReshadeRenderer();
std::unique_ptr<IReshadeOptimizer> CreateReshadeOptimizer();

} // namespace Reshade
} // namespace uevrLCX
