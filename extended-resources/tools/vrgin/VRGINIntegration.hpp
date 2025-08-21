#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <spdlog/spdlog.h>

// VRGIN Core Integration
namespace uevrLCX {
namespace VRGIN {

// Core VRGIN Configuration
struct VRGINConfig {
    bool enableVRGIN = true;
    bool enableVRGINNET = true;
    bool enableOpenVR = true;
    bool enableSteamVR = true;
    bool enableOculus = true;
    bool enableOpenXR = true;
    bool enableOpenHMD = true;
    
    // Injection settings
    bool enableDLLInjection = true;
    bool enableMemoryHooking = true;
    bool enableShaderInjection = true;
    bool enableDepthExtraction = true;
    
    // Performance settings
    bool enableFoveatedRendering = true;
    bool enableAsyncTimewarp = true;
    bool enableMultiResShading = true;
    bool enableGPUAcceleration = true;
    
    // Community integration
    bool enableRedditSolutions = true;
    bool enableCommunityProfiles = true;
    bool enableAutoOptimization = true;
    
    // Advanced features
    bool enableNeuralRendering = true;
    bool enableAIOptimization = true;
    bool enableRealCollision = true;
    bool enableAdvancedTracking = true;
};

// VRGIN Metrics
struct VRGINMetrics {
    int injectedGames = 0;
    int successfulConversions = 0;
    int failedConversions = 0;
    float averageFrameRate = 0.0f;
    float averageLatency = 0.0f;
    float compatibilityScore = 0.0f;
    int redditSolutionsApplied = 0;
    int communityProfilesUsed = 0;
    int autoOptimizationsApplied = 0;
    
    // Performance metrics
    float gpuUtilization = 0.0f;
    float cpuUtilization = 0.0f;
    float memoryUsage = 0.0f;
    int droppedFrames = 0;
    float motionToPhotonLatency = 0.0f;
};

// Game Profile for VRGIN
struct VRGINGameProfile {
    std::string gameName;
    std::string engineType;
    std::string engineVersion;
    std::string graphicsAPI;
    float compatibilityScore;
    std::vector<std::string> redditSolutions;
    std::vector<std::string> communityProfiles;
    std::vector<std::string> autoOptimizations;
    bool isVerified;
    std::string lastTested;
};

// VRGIN Event System
struct VRGINEvent {
    enum class Type {
        GameDetected,
        InjectionStarted,
        InjectionCompleted,
        ConversionStarted,
        ConversionCompleted,
        ErrorOccurred,
        PerformanceWarning,
        RedditSolutionApplied,
        CommunityProfileLoaded,
        AutoOptimizationApplied
    };
    
    Type type;
    std::string message;
    std::string gameName;
    float timestamp;
    std::unordered_map<std::string, std::string> data;
};

// Core VRGIN Interface
class IVRGINCore {
public:
    virtual ~IVRGINCore() = default;
    
    virtual bool Initialize(const VRGINConfig& config) = 0;
    virtual bool InjectIntoGame(const std::string& processName) = 0;
    virtual bool ConvertGame(const std::string& gamePath) = 0;
    virtual bool ApplyRedditSolution(const std::string& gameName, const std::string& solutionId) = 0;
    virtual bool LoadCommunityProfile(const std::string& gameName, const std::string& profileId) = 0;
    virtual bool ApplyAutoOptimization(const std::string& gameName) = 0;
    virtual VRGINMetrics GetMetrics() const = 0;
    virtual std::vector<VRGINGameProfile> GetGameProfiles() const = 0;
    virtual void Shutdown() = 0;
    
    // Event system
    using EventCallback = std::function<void(const VRGINEvent&)>;
    virtual void RegisterEventCallback(EventCallback callback) = 0;
    virtual void UnregisterEventCallback() = 0;
};

// VRGIN Renderer Interface
class IVRGINRenderer {
public:
    virtual ~IVRGINRenderer() = default;
    
    virtual bool Initialize(const VRGINConfig& config) = 0;
    virtual bool SetupStereoscopicRendering(const std::string& gameName) = 0;
    virtual bool EnableFoveatedRendering(const std::string& gameName) = 0;
    virtual bool EnableAsyncTimewarp(const std::string& gameName) = 0;
    virtual bool EnableMultiResShading(const std::string& gameName) = 0;
    virtual bool EnableGPUAcceleration(const std::string& gameName) = 0;
    virtual bool RenderFrame(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// VRGIN Input Interface
class IVRGINInput {
public:
    virtual ~IVRGINInput() = default;
    
    virtual bool Initialize(const VRGINConfig& config) = 0;
    virtual bool SetupOpenVR(const std::string& gameName) = 0;
    virtual bool SetupSteamVR(const std::string& gameName) = 0;
    virtual bool SetupOculus(const std::string& gameName) = 0;
    virtual bool SetupOpenXR(const std::string& gameName) = 0;
    virtual bool SetupOpenHMD(const std::string& gameName) = 0;
    virtual bool MapInputs(const std::string& gameName) = 0;
    virtual bool UpdateTracking(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// VRGIN Physics Interface
class IVRGINPhysics {
public:
    virtual ~IVRGINPhysics() = default;
    
    virtual bool Initialize(const VRGINConfig& config) = 0;
    virtual bool SetupBulletPhysics(const std::string& gameName) = 0;
    virtual bool SetupPhysX(const std::string& gameName) = 0;
    virtual bool EnableRealCollision(const std::string& gameName) = 0;
    virtual bool CreatePhysicsObjects(const std::string& gameName) = 0;
    virtual bool UpdatePhysics(const std::string& gameName) = 0;
    virtual bool DetectCollisions(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// VRGIN Optimizer Interface
class IVRGINOptimizer {
public:
    virtual ~IVRGINOptimizer() = default;
    
    virtual bool Initialize(const VRGINConfig& config) = 0;
    virtual bool OptimizePerformance(const std::string& gameName) = 0;
    virtual bool ApplyRedditOptimizations(const std::string& gameName) = 0;
    virtual bool ApplyCommunityOptimizations(const std::string& gameName) = 0;
    virtual bool ApplyAutoOptimizations(const std::string& gameName) = 0;
    virtual bool EnableAIOptimization(const std::string& gameName) = 0;
    virtual bool EnableNeuralRendering(const std::string& gameName) = 0;
    virtual VRGINMetrics GetOptimizationMetrics() const = 0;
    virtual void Shutdown() = 0;
};

// VRGIN Community Integration Interface
class IVRGINCommunity {
public:
    virtual ~IVRGINCommunity() = default;
    
    virtual bool Initialize(const VRGINConfig& config) = 0;
    virtual bool LoadRedditSolutions(const std::string& gameName) = 0;
    virtual bool LoadCommunityProfiles(const std::string& gameName) = 0;
    virtual bool ApplyRedditSolution(const std::string& gameName, const std::string& solutionId) = 0;
    virtual bool ApplyCommunityProfile(const std::string& gameName, const std::string& profileId) = 0;
    virtual bool SubmitCommunityProfile(const std::string& gameName, const VRGINGameProfile& profile) = 0;
    virtual std::vector<std::string> GetAvailableRedditSolutions(const std::string& gameName) = 0;
    virtual std::vector<std::string> GetAvailableCommunityProfiles(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// VRGIN Legacy Support Interface
class IVRGINLegacy {
public:
    virtual ~IVRGINLegacy() = default;
    
    virtual bool Initialize(const VRGINConfig& config) = 0;
    virtual bool Setup3DVision(const std::string& gameName) = 0;
    virtual bool SetupTridef(const std::string& gameName) = 0;
    virtual bool SetupVireio(const std::string& gameName) = 0;
    virtual bool SetupVorpx(const std::string& gameName) = 0;
    virtual bool SetupHelixMod(const std::string& gameName) = 0;
    virtual bool ApplyLegacyTechniques(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// VRGIN Research Integration Interface
class IVRGINResearch {
public:
    virtual ~IVRGINResearch() = default;
    
    virtual bool Initialize(const VRGINConfig& config) = 0;
    virtual bool LoadAcademicPapers(const std::string& gameName) = 0;
    virtual bool LoadIndustryStandards(const std::string& gameName) = 0;
    virtual bool LoadPerformanceStudies(const std::string& gameName) = 0;
    virtual bool ApplyResearchTechniques(const std::string& gameName) = 0;
    virtual bool OptimizeBasedOnResearch(const std::string& gameName) = 0;
    virtual bool ValidateAgainstStandards(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// Main VRGIN Integration Manager
class VRGINIntegrationManager {
private:
    std::unique_ptr<IVRGINCore> m_core;
    std::unique_ptr<IVRGINRenderer> m_renderer;
    std::unique_ptr<IVRGINInput> m_input;
    std::unique_ptr<IVRGINPhysics> m_physics;
    std::unique_ptr<IVRGINOptimizer> m_optimizer;
    std::unique_ptr<IVRGINCommunity> m_community;
    std::unique_ptr<IVRGINLegacy> m_legacy;
    std::unique_ptr<IVRGINResearch> m_research;
    
    VRGINConfig m_config;
    VRGINMetrics m_metrics;
    std::vector<VRGINGameProfile> m_gameProfiles;
    std::vector<VRGINEvent> m_events;
    
    bool m_initialized;
    bool m_running;

public:
    VRGINIntegrationManager();
    ~VRGINIntegrationManager();
    
    bool Initialize(const VRGINConfig& config);
    bool StartIntegration();
    bool StopIntegration();
    bool ConvertGame(const std::string& gamePath);
    bool ApplyAllOptimizations(const std::string& gameName);
    bool ApplyAllCommunitySolutions(const std::string& gameName);
    bool ApplyAllResearchTechniques(const std::string& gameName);
    
    VRGINMetrics GetMetrics() const;
    std::vector<VRGINGameProfile> GetGameProfiles() const;
    std::vector<VRGINEvent> GetEvents() const;
    
    void Shutdown();
    
private:
    void OnVRGINEvent(const VRGINEvent& event);
    void UpdateMetrics();
    void LogIntegrationStatus();
};

// Factory functions
std::unique_ptr<IVRGINCore> CreateVRGINCore();
std::unique_ptr<IVRGINRenderer> CreateVRGINRenderer();
std::unique_ptr<IVRGINInput> CreateVRGINInput();
std::unique_ptr<IVRGINPhysics> CreateVRGINPhysics();
std::unique_ptr<IVRGINOptimizer> CreateVRGINOptimizer();
std::unique_ptr<IVRGINCommunity> CreateVRGINCommunity();
std::unique_ptr<IVRGINLegacy> CreateVRGINLegacy();
std::unique_ptr<IVRGINResearch> CreateVRGINResearch();

} // namespace VRGIN
} // namespace uevrLCX
