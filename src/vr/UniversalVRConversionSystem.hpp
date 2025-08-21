#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <spdlog/spdlog.h>

// Extended Resources Integration
#include "../../extended-resources/tools/vrgin/VRGINIntegration.hpp"
#include "../../extended-resources/tools/depth3d/Depth3DIntegration.hpp"
#include "../../extended-resources/tools/reshade/ReshadeIntegration.hpp"
#include "../../extended-resources/tools/physics/PhysicsIntegration.hpp"

// Core uevrLCX Components
#include "GameAnalyzer.hpp"
#include "EngineAdapters.hpp"
#include "PerformanceOptimizer.hpp"
#include "CompatibilityManager.hpp"

namespace uevrLCX {

// Universal VR Conversion Configuration
struct UniversalVRConfig {
    // Core system settings
    bool enableUniversalConversion = true;
    bool enableRealCollision = true;
    bool enableRedditIntegration = true;
    bool enableCommunityIntegration = true;
    bool enableResearchIntegration = true;
    
    // Component settings
    bool enableVRGIN = true;
    bool enableDepth3D = true;
    bool enableReshade = true;
    bool enablePhysics = true;
    bool enableGameAnalyzer = true;
    bool enableEngineAdapters = true;
    bool enablePerformanceOptimizer = true;
    bool enableCompatibilityManager = true;
    
    // Advanced features
    bool enableNeuralRendering = true;
    bool enableAIOptimization = true;
    bool enableAdvancedTracking = true;
    bool enableHapticFeedback = true;
    
    // Performance settings
    bool enableFoveatedRendering = true;
    bool enableAsyncTimewarp = true;
    bool enableMultiResShading = true;
    bool enableGPUAcceleration = true;
    
    // Community settings
    bool enableRedditSolutions = true;
    bool enableCommunityProfiles = true;
    bool enableAutoOptimization = true;
    bool enableResearchAlgorithms = true;
};

// Universal VR Conversion Metrics
struct UniversalVRMetrics {
    // Core metrics
    int convertedGames = 0;
    int successfulConversions = 0;
    int failedConversions = 0;
    float averageCompatibilityScore = 0.0f;
    float averagePerformanceScore = 0.0f;
    float averageQualityScore = 0.0f;
    
    // Component metrics
    VRGIN::VRGINMetrics vrginMetrics;
    Depth3D::Depth3DMetrics depth3dMetrics;
    Reshade::ReshadeMetrics reshadeMetrics;
    Physics::PhysicsMetrics physicsMetrics;
    
    // Community metrics
    int redditSolutionsApplied = 0;
    int communityProfilesUsed = 0;
    int researchAlgorithmsApplied = 0;
    int autoOptimizationsApplied = 0;
    
    // Performance metrics
    float averageFrameRate = 0.0f;
    float averageLatency = 0.0f;
    float gpuUtilization = 0.0f;
    float cpuUtilization = 0.0f;
    float memoryUsage = 0.0f;
    int droppedFrames = 0;
    float motionToPhotonLatency = 0.0f;
};

// Game Profile for Universal VR Conversion
struct GameProfile {
    std::string gameName;
    std::string engineType;
    std::string engineVersion;
    std::string graphicsAPI;
    float compatibilityScore;
    float performanceScore;
    float qualityScore;
    
    // Component profiles
    VRGIN::VRGINGameProfile vrginProfile;
    Depth3D::Depth3DGameProfile depth3dProfile;
    Reshade::ReshadeGameProfile reshadeProfile;
    Physics::PhysicsGameProfile physicsProfile;
    
    // Community solutions
    std::vector<std::string> redditSolutions;
    std::vector<std::string> communityProfiles;
    std::vector<std::string> researchAlgorithms;
    std::vector<std::string> autoOptimizations;
    
    // Status
    bool isConverted;
    bool isVerified;
    std::string lastTested;
    std::string conversionStatus;
    
    GameProfile() : compatibilityScore(0.0f), performanceScore(0.0f), qualityScore(0.0f),
                    isConverted(false), isVerified(false) {}
};

// Conversion Result
struct ConversionResult {
    bool success;
    std::string gameName;
    std::string errorMessage;
    float compatibilityScore;
    float performanceScore;
    float qualityScore;
    std::vector<std::string> appliedSolutions;
    std::vector<std::string> appliedOptimizations;
    std::vector<std::string> appliedAlgorithms;
    
    ConversionResult() : success(false), compatibilityScore(0.0f), 
                         performanceScore(0.0f), qualityScore(0.0f) {}
};

// Universal VR Conversion Event
struct UniversalVREvent {
    enum class Type {
        GameDetected,
        ConversionStarted,
        ConversionCompleted,
        ConversionFailed,
        ComponentInitialized,
        ComponentFailed,
        RedditSolutionApplied,
        CommunityProfileUsed,
        ResearchAlgorithmApplied,
        AutoOptimizationApplied,
        PerformanceWarning,
        ErrorOccurred
    };
    
    Type type;
    std::string message;
    std::string gameName;
    std::string componentName;
    float timestamp;
    std::unordered_map<std::string, std::string> data;
};

// Main Universal VR Conversion Interface
class IUniversalVRConverter {
public:
    virtual ~IUniversalVRConverter() = default;
    
    // Core functionality
    virtual bool Initialize(const UniversalVRConfig& config) = 0;
    virtual bool ConvertGame(const std::string& gamePath) = 0;
    virtual bool ConvertGameByName(const std::string& gameName) = 0;
    virtual bool ConvertAllDetectedGames() = 0;
    virtual ConversionResult GetConversionResult(const std::string& gameName) const = 0;
    
    // Component management
    virtual bool InitializeComponents() = 0;
    virtual bool StartComponents() = 0;
    virtual bool StopComponents() = 0;
    virtual bool ShutdownComponents() = 0;
    
    // Community integration
    virtual bool ApplyRedditSolution(const std::string& gameName, const std::string& solutionId) = 0;
    virtual bool ApplyCommunityProfile(const std::string& gameName, const std::string& profileId) = 0;
    virtual bool ApplyResearchAlgorithm(const std::string& gameName, const std::string& algorithmId) = 0;
    virtual bool ApplyAutoOptimization(const std::string& gameName) = 0;
    
    // Performance optimization
    virtual bool OptimizePerformance(const std::string& gameName) = 0;
    virtual bool EnableFoveatedRendering(const std::string& gameName) = 0;
    virtual bool EnableAsyncTimewarp(const std::string& gameName) = 0;
    virtual bool EnableMultiResShading(const std::string& gameName) = 0;
    virtual bool EnableGPUAcceleration(const std::string& gameName) = 0;
    
    // Real collision detection
    virtual bool EnableRealCollision(const std::string& gameName) = 0;
    virtual bool SetupPhysicsWorld(const std::string& gameName) = 0;
    virtual bool AddPhysicsObjects(const std::string& gameName) = 0;
    virtual bool StartPhysicsSimulation(const std::string& gameName) = 0;
    
    // Metrics and monitoring
    virtual UniversalVRMetrics GetMetrics() const = 0;
    virtual std::vector<GameProfile> GetGameProfiles() const = 0;
    virtual GameProfile GetGameProfile(const std::string& gameName) const = 0;
    
    // Event system
    using EventCallback = std::function<void(const UniversalVREvent&)>;
    virtual void RegisterEventCallback(EventCallback callback) = 0;
    virtual void UnregisterEventCallback() = 0;
    
    // Status queries
    virtual bool IsInitialized() const = 0;
    virtual bool IsRunning() const = 0;
    virtual bool IsGameConverted(const std::string& gameName) const = 0;
    virtual std::string GetStatus() const = 0;
    
    // Shutdown
    virtual void Shutdown() = 0;
};

// Main Universal VR Conversion System Implementation
class UniversalVRConverter : public IUniversalVRConverter {
private:
    // Configuration and state
    UniversalVRConfig m_config;
    UniversalVRMetrics m_metrics;
    std::vector<GameProfile> m_gameProfiles;
    std::vector<UniversalVREvent> m_events;
    
    // Component managers
    std::unique_ptr<VRGIN::VRGINIntegrationManager> m_vrginManager;
    std::unique_ptr<Depth3D::Depth3DIntegrationManager> m_depth3dManager;
    std::unique_ptr<Reshade::ReshadeIntegrationManager> m_reshadeManager;
    std::unique_ptr<Physics::PhysicsIntegrationManager> m_physicsManager;
    
    // Core components
    std::unique_ptr<GameAnalyzer> m_gameAnalyzer;
    std::unique_ptr<EngineAdapter> m_engineAdapter;
    std::unique_ptr<PerformanceOptimizer> m_performanceOptimizer;
    std::unique_ptr<CompatibilityManager> m_compatibilityManager;
    
    // State management
    bool m_initialized;
    bool m_running;
    std::unordered_map<std::string, ConversionResult> m_conversionResults;
    
    // Event callback
    EventCallback m_eventCallback;

public:
    UniversalVRConverter();
    ~UniversalVRConverter();
    
    // Core functionality
    bool Initialize(const UniversalVRConfig& config) override;
    bool ConvertGame(const std::string& gamePath) override;
    bool ConvertGameByName(const std::string& gameName) override;
    bool ConvertAllDetectedGames() override;
    ConversionResult GetConversionResult(const std::string& gameName) const override;
    
    // Component management
    bool InitializeComponents() override;
    bool StartComponents() override;
    bool StopComponents() override;
    bool ShutdownComponents() override;
    
    // Community integration
    bool ApplyRedditSolution(const std::string& gameName, const std::string& solutionId) override;
    bool ApplyCommunityProfile(const std::string& gameName, const std::string& profileId) override;
    bool ApplyResearchAlgorithm(const std::string& gameName, const std::string& algorithmId) override;
    bool ApplyAutoOptimization(const std::string& gameName) override;
    
    // Performance optimization
    bool OptimizePerformance(const std::string& gameName) override;
    bool EnableFoveatedRendering(const std::string& gameName) override;
    bool EnableAsyncTimewarp(const std::string& gameName) override;
    bool EnableMultiResShading(const std::string& gameName) override;
    bool EnableGPUAcceleration(const std::string& gameName) override;
    
    // Real collision detection
    bool EnableRealCollision(const std::string& gameName) override;
    bool SetupPhysicsWorld(const std::string& gameName) override;
    bool AddPhysicsObjects(const std::string& gameName) override;
    bool StartPhysicsSimulation(const std::string& gameName) override;
    
    // Metrics and monitoring
    UniversalVRMetrics GetMetrics() const override;
    std::vector<GameProfile> GetGameProfiles() const override;
    GameProfile GetGameProfile(const std::string& gameName) const override;
    
    // Event system
    void RegisterEventCallback(EventCallback callback) override;
    void UnregisterEventCallback() override;
    
    // Status queries
    bool IsInitialized() const override;
    bool IsRunning() const override;
    bool IsGameConverted(const std::string& gameName) const override;
    std::string GetStatus() const override;
    
    // Shutdown
    void Shutdown() override;

private:
    // Internal helper methods
    void OnUniversalVREvent(const UniversalVREvent& event);
    void UpdateMetrics();
    void LogConversionStatus();
    
    // Component event handlers
    void OnVRGINEvent(const VRGIN::VRGINEvent& event);
    void OnDepth3DEvent(const Depth3D::Depth3DEvent& event);
    void OnReshadeEvent(const Reshade::ReshadeEvent& event);
    void OnPhysicsEvent(const Physics::PhysicsEvent& event);
    
    // Conversion pipeline
    bool AnalyzeGame(const std::string& gamePath, GameProfile& profile);
    bool AdaptEngine(const std::string& gameName, GameProfile& profile);
    bool ApplyVRGIN(const std::string& gameName, GameProfile& profile);
    bool ApplyDepth3D(const std::string& gameName, GameProfile& profile);
    bool ApplyReshade(const std::string& gameName, GameProfile& profile);
    bool ApplyPhysics(const std::string& gameName, GameProfile& profile);
    bool OptimizePerformance(const std::string& gameName, GameProfile& profile);
    bool ApplyCommunitySolutions(const std::string& gameName, GameProfile& profile);
    
    // Utility methods
    void CalculateConversionScores(GameProfile& profile);
    void StoreConversionResult(const std::string& gameName, const ConversionResult& result);
    void ValidateConversion(const std::string& gameName, GameProfile& profile);
};

// Factory function
std::unique_ptr<IUniversalVRConverter> CreateUniversalVRConverter();

} // namespace uevrLCX
