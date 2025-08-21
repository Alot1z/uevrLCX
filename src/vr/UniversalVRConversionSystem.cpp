#include "UniversalVRConversionSystem.hpp"
#include <spdlog/spdlog.h>
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <filesystem>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <algorithm>

namespace uevrLCX {

// UniversalVRConverter Implementation
UniversalVRConverter::UniversalVRConverter() 
    : m_initialized(false), m_running(false) {
    spdlog::info("UniversalVRConverter: Initializing");
}

UniversalVRConverter::~UniversalVRConverter() {
    Shutdown();
}

bool UniversalVRConverter::Initialize(const UniversalVRConfig& config) {
    m_config = config;
    
    spdlog::info("UniversalVRConverter: Initializing with extended resources integration");
    
    // Initialize component managers
    if (m_config.enableVRGIN) {
        m_vrginManager = std::make_unique<VRGIN::VRGINIntegrationManager>();
        VRGIN::VRGINConfig vrginConfig;
        if (!m_vrginManager->Initialize(vrginConfig)) {
            spdlog::error("UniversalVRConverter: Failed to initialize VRGIN manager");
            return false;
        }
    }
    
    if (m_config.enableDepth3D) {
        m_depth3dManager = std::make_unique<Depth3D::Depth3DIntegrationManager>();
        Depth3D::Depth3DConfig depth3dConfig;
        if (!m_depth3dManager->Initialize(depth3dConfig)) {
            spdlog::error("UniversalVRConverter: Failed to initialize Depth3D manager");
            return false;
        }
    }
    
    if (m_config.enableReshade) {
        m_reshadeManager = std::make_unique<Reshade::ReshadeIntegrationManager>();
        Reshade::ReshadeConfig reshadeConfig;
        if (!m_reshadeManager->Initialize(reshadeConfig)) {
            spdlog::error("UniversalVRConverter: Failed to initialize Reshade manager");
            return false;
        }
    }
    
    if (m_config.enablePhysics) {
        m_physicsManager = std::make_unique<Physics::PhysicsIntegrationManager>();
        Physics::PhysicsConfig physicsConfig;
        if (!m_physicsManager->Initialize(physicsConfig)) {
            spdlog::error("UniversalVRConverter: Failed to initialize Physics manager");
            return false;
        }
    }
    
    // Initialize core components
    if (m_config.enableGameAnalyzer) {
        m_gameAnalyzer = std::make_unique<GameAnalyzer>();
        GameAnalyzerConfig analyzerConfig;
        if (!m_gameAnalyzer->Initialize(analyzerConfig)) {
            spdlog::error("UniversalVRConverter: Failed to initialize Game Analyzer");
            return false;
        }
    }
    
    if (m_config.enableEngineAdapters) {
        m_engineAdapter = std::make_unique<EngineAdapter>();
        EngineAdapterConfig adapterConfig;
        if (!m_engineAdapter->Initialize(adapterConfig)) {
            spdlog::error("UniversalVRConverter: Failed to initialize Engine Adapter");
            return false;
        }
    }
    
    if (m_config.enablePerformanceOptimizer) {
        m_performanceOptimizer = std::make_unique<PerformanceOptimizer>();
        PerformanceOptimizerConfig optimizerConfig;
        if (!m_performanceOptimizer->Initialize(optimizerConfig)) {
            spdlog::error("UniversalVRConverter: Failed to initialize Performance Optimizer");
            return false;
        }
    }
    
    if (m_config.enableCompatibilityManager) {
        m_compatibilityManager = std::make_unique<CompatibilityManager>();
        CompatibilityManagerConfig compatConfig;
        if (!m_compatibilityManager->Initialize(compatConfig)) {
            spdlog::error("UniversalVRConverter: Failed to initialize Compatibility Manager");
            return false;
        }
    }
    
    m_initialized = true;
    spdlog::info("UniversalVRConverter: Initialized successfully with all extended resources");
    return true;
}

bool UniversalVRConverter::ConvertGame(const std::string& gamePath) {
    if (!m_initialized) {
        spdlog::error("UniversalVRConverter: Not initialized");
        return false;
    }
    
    spdlog::info("UniversalVRConverter: Starting conversion for game: {}", gamePath);
    
    // Create game profile
    GameProfile profile;
    profile.gameName = std::filesystem::path(gamePath).stem().string();
    
    // Step 1: Analyze game
    if (!AnalyzeGame(gamePath, profile)) {
        spdlog::error("UniversalVRConverter: Failed to analyze game: {}", gamePath);
        return false;
    }
    
    // Step 2: Adapt engine
    if (!AdaptEngine(profile.gameName, profile)) {
        spdlog::error("UniversalVRConverter: Failed to adapt engine for: {}", profile.gameName);
        return false;
    }
    
    // Step 3: Apply VRGIN
    if (!ApplyVRGIN(profile.gameName, profile)) {
        spdlog::error("UniversalVRConverter: Failed to apply VRGIN for: {}", profile.gameName);
        return false;
    }
    
    // Step 4: Apply Depth3D
    if (!ApplyDepth3D(profile.gameName, profile)) {
        spdlog::error("UniversalVRConverter: Failed to apply Depth3D for: {}", profile.gameName);
        return false;
    }
    
    // Step 5: Apply Reshade
    if (!ApplyReshade(profile.gameName, profile)) {
        spdlog::error("UniversalVRConverter: Failed to apply Reshade for: {}", profile.gameName);
        return false;
    }
    
    // Step 6: Apply Physics
    if (!ApplyPhysics(profile.gameName, profile)) {
        spdlog::error("UniversalVRConverter: Failed to apply Physics for: {}", profile.gameName);
        return false;
    }
    
    // Step 7: Optimize performance
    if (!OptimizePerformance(profile.gameName, profile)) {
        spdlog::error("UniversalVRConverter: Failed to optimize performance for: {}", profile.gameName);
        return false;
    }
    
    // Step 8: Apply community solutions
    if (!ApplyCommunitySolutions(profile.gameName, profile)) {
        spdlog::error("UniversalVRConverter: Failed to apply community solutions for: {}", profile.gameName);
        return false;
    }
    
    // Calculate final scores
    CalculateConversionScores(profile);
    
    // Store result
    ConversionResult result;
    result.success = true;
    result.gameName = profile.gameName;
    result.compatibilityScore = profile.compatibilityScore;
    result.performanceScore = profile.performanceScore;
    result.qualityScore = profile.qualityScore;
    StoreConversionResult(profile.gameName, result);
    
    // Add to game profiles
    m_gameProfiles.push_back(profile);
    
    // Update metrics
    m_metrics.convertedGames++;
    m_metrics.successfulConversions++;
    
    spdlog::info("UniversalVRConverter: Successfully converted game: {} (Compatibility: {:.2f}, Performance: {:.2f}, Quality: {:.2f})", 
                  profile.gameName, profile.compatibilityScore, profile.performanceScore, profile.qualityScore);
    
    return true;
}

bool UniversalVRConverter::AnalyzeGame(const std::string& gamePath, GameProfile& profile) {
    if (!m_gameAnalyzer) {
        spdlog::error("UniversalVRConverter: Game Analyzer not available");
        return false;
    }
    
    spdlog::info("UniversalVRConverter: Analyzing game: {}", gamePath);
    
    // Analyze game using GameAnalyzer
    if (!m_gameAnalyzer->AnalyzeGame(gamePath)) {
        spdlog::error("UniversalVRConverter: Game analysis failed");
        return false;
    }
    
    // Get analysis results
    auto gameProfile = m_gameAnalyzer->GetGameProfile(gamePath);
    profile.engineType = gameProfile.engineType;
    profile.engineVersion = gameProfile.engineVersion;
    profile.graphicsAPI = gameProfile.graphicsAPI;
    profile.redditSolutions = gameProfile.redditSolutions;
    profile.communityProfiles = gameProfile.communityProfiles;
    
    spdlog::info("UniversalVRConverter: Game analysis completed - Engine: {}, API: {}", 
                  profile.engineType, profile.graphicsAPI);
    
    return true;
}

bool UniversalVRConverter::ApplyVRGIN(const std::string& gameName, GameProfile& profile) {
    if (!m_vrginManager) {
        spdlog::error("UniversalVRConverter: VRGIN manager not available");
        return false;
    }
    
    spdlog::info("UniversalVRConverter: Applying VRGIN for: {}", gameName);
    
    // Start VRGIN integration
    if (!m_vrginManager->StartIntegration()) {
        spdlog::error("UniversalVRConverter: Failed to start VRGIN integration");
        return false;
    }
    
    // Convert game using VRGIN
    if (!m_vrginManager->ConvertGame(gameName)) {
        spdlog::error("UniversalVRConverter: VRGIN conversion failed");
        return false;
    }
    
    // Apply Reddit solutions
    if (m_config.enableRedditSolutions) {
        for (const auto& solution : profile.redditSolutions) {
            m_vrginManager->ApplyRedditSolution(gameName, solution);
        }
    }
    
    // Get VRGIN metrics
    auto vrginMetrics = m_vrginManager->GetMetrics();
    profile.vrginProfile.compatibilityScore = vrginMetrics.compatibilityScore;
    
    spdlog::info("UniversalVRConverter: VRGIN applied successfully for: {}", gameName);
    
    return true;
}

bool UniversalVRConverter::ApplyDepth3D(const std::string& gameName, GameProfile& profile) {
    if (!m_depth3dManager) {
        spdlog::error("UniversalVRConverter: Depth3D manager not available");
        return false;
    }
    
    spdlog::info("UniversalVRConverter: Applying Depth3D for: {}", gameName);
    
    // Start depth extraction
    if (!m_depth3dManager->StartExtraction()) {
        spdlog::error("UniversalVRConverter: Failed to start Depth3D extraction");
        return false;
    }
    
    // Extract depth buffer
    if (!m_depth3dManager->ExtractDepthBuffer(gameName)) {
        spdlog::error("UniversalVRConverter: Depth3D extraction failed");
        return false;
    }
    
    // Get Depth3D metrics
    auto depth3dMetrics = m_depth3dManager->GetMetrics();
    profile.depth3dProfile.extractionSuccessRate = depth3dMetrics.extractionSuccessRate;
    
    spdlog::info("UniversalVRConverter: Depth3D applied successfully for: {}", gameName);
    
    return true;
}

bool UniversalVRConverter::ApplyReshade(const std::string& gameName, GameProfile& profile) {
    if (!m_reshadeManager) {
        spdlog::error("UniversalVRConverter: Reshade manager not available");
        return false;
    }
    
    spdlog::info("UniversalVRConverter: Applying Reshade for: {}", gameName);
    
    // Start shader injection
    if (!m_reshadeManager->StartInjection()) {
        spdlog::error("UniversalVRConverter: Failed to start Reshade injection");
        return false;
    }
    
    // Inject into game
    if (!m_reshadeManager->InjectIntoGame(gameName)) {
        spdlog::error("UniversalVRConverter: Reshade injection failed");
        return false;
    }
    
    // Load and apply shaders
    if (!m_reshadeManager->LoadAndApplyShaders(gameName)) {
        spdlog::error("UniversalVRConverter: Reshade shader application failed");
        return false;
    }
    
    // Get Reshade metrics
    auto reshadeMetrics = m_reshadeManager->GetMetrics();
    profile.reshadeProfile.shaderQuality = reshadeMetrics.shaderQuality;
    
    spdlog::info("UniversalVRConverter: Reshade applied successfully for: {}", gameName);
    
    return true;
}

bool UniversalVRConverter::ApplyPhysics(const std::string& gameName, GameProfile& profile) {
    if (!m_physicsManager) {
        spdlog::error("UniversalVRConverter: Physics manager not available");
        return false;
    }
    
    spdlog::info("UniversalVRConverter: Applying Physics for: {}", gameName);
    
    // Start physics simulation
    if (!m_physicsManager->StartPhysics()) {
        spdlog::error("UniversalVRConverter: Failed to start physics simulation");
        return false;
    }
    
    // Create physics world
    if (!m_physicsManager->CreatePhysicsWorld(gameName)) {
        spdlog::error("UniversalVRConverter: Failed to create physics world");
        return false;
    }
    
    // Enable real collision
    if (!m_physicsManager->ApplyAllPhysics(gameName)) {
        spdlog::error("UniversalVRConverter: Failed to apply physics");
        return false;
    }
    
    // Get Physics metrics
    auto physicsMetrics = m_physicsManager->GetMetrics();
    profile.physicsProfile.physicsQuality = physicsMetrics.physicsPerformance;
    
    spdlog::info("UniversalVRConverter: Physics applied successfully for: {}", gameName);
    
    return true;
}

bool UniversalVRConverter::OptimizePerformance(const std::string& gameName, GameProfile& profile) {
    if (!m_performanceOptimizer) {
        spdlog::error("UniversalVRConverter: Performance optimizer not available");
        return false;
    }
    
    spdlog::info("UniversalVRConverter: Optimizing performance for: {}", gameName);
    
    // Enable foveated rendering
    if (m_config.enableFoveatedRendering) {
        m_performanceOptimizer->EnableFoveatedRendering(gameName);
    }
    
    // Enable async timewarp
    if (m_config.enableAsyncTimewarp) {
        m_performanceOptimizer->EnableAsyncTimewarp(gameName);
    }
    
    // Enable multi-res shading
    if (m_config.enableMultiResShading) {
        m_performanceOptimizer->EnableMultiResShading(gameName);
    }
    
    // Enable GPU acceleration
    if (m_config.enableGPUAcceleration) {
        m_performanceOptimizer->EnableGPUAcceleration(gameName);
    }
    
    // Get performance metrics
    auto perfMetrics = m_performanceOptimizer->GetMetrics();
    profile.performanceScore = perfMetrics.averageFrameRate / 90.0f; // Normalize to 90 FPS target
    
    spdlog::info("UniversalVRConverter: Performance optimization completed for: {}", gameName);
    
    return true;
}

bool UniversalVRConverter::ApplyCommunitySolutions(const std::string& gameName, GameProfile& profile) {
    if (!m_compatibilityManager) {
        spdlog::error("UniversalVRConverter: Compatibility manager not available");
        return false;
    }
    
    spdlog::info("UniversalVRConverter: Applying community solutions for: {}", gameName);
    
    // Assess compatibility
    if (!m_compatibilityManager->AssessCompatibility(gameName)) {
        spdlog::error("UniversalVRConverter: Failed to assess compatibility");
        return false;
    }
    
    // Apply Reddit solutions
    for (const auto& solution : profile.redditSolutions) {
        m_compatibilityManager->ApplyCommunitySolution(gameName, solution);
    }
    
    // Apply community profiles
    for (const auto& profile : profile.communityProfiles) {
        m_compatibilityManager->ApplyCommunitySolution(gameName, profile);
    }
    
    // Get compatibility result
    auto compatResult = m_compatibilityManager->GetCompatibilityResult(gameName);
    profile.compatibilityScore = compatResult.compatibilityScore;
    
    spdlog::info("UniversalVRConverter: Community solutions applied for: {}", gameName);
    
    return true;
}

void UniversalVRConverter::CalculateConversionScores(GameProfile& profile) {
    // Calculate compatibility score (average of all component scores)
    float compatScore = (profile.vrginProfile.compatibilityScore + 
                        profile.depth3dProfile.extractionSuccessRate + 
                        profile.reshadeProfile.shaderQuality + 
                        profile.physicsProfile.physicsQuality) / 4.0f;
    
    // Calculate quality score (based on depth and shader quality)
    float qualityScore = (profile.depth3dProfile.extractionSuccessRate + 
                         profile.reshadeProfile.shaderQuality) / 2.0f;
    
    profile.compatibilityScore = compatScore;
    profile.qualityScore = qualityScore;
    
    spdlog::info("UniversalVRConverter: Calculated scores for {} - Compatibility: {:.2f}, Performance: {:.2f}, Quality: {:.2f}", 
                  profile.gameName, profile.compatibilityScore, profile.performanceScore, profile.qualityScore);
}

void UniversalVRConverter::StoreConversionResult(const std::string& gameName, const ConversionResult& result) {
    m_conversionResults[gameName] = result;
}

bool UniversalVRConverter::IsInitialized() const {
    return m_initialized;
}

bool UniversalVRConverter::IsRunning() const {
    return m_running;
}

bool UniversalVRConverter::IsGameConverted(const std::string& gameName) const {
    return m_conversionResults.find(gameName) != m_conversionResults.end();
}

std::string UniversalVRConverter::GetStatus() const {
    if (!m_initialized) return "Not Initialized";
    if (!m_running) return "Initialized but not running";
    return "Running - " + std::to_string(m_metrics.convertedGames) + " games converted";
}

void UniversalVRConverter::Shutdown() {
    spdlog::info("UniversalVRConverter: Shutting down");
    
    // Stop all components
    if (m_vrginManager) m_vrginManager->StopIntegration();
    if (m_depth3dManager) m_depth3dManager->StopExtraction();
    if (m_reshadeManager) m_reshadeManager->StopInjection();
    if (m_physicsManager) m_physicsManager->StopPhysics();
    
    // Shutdown all components
    if (m_vrginManager) m_vrginManager->Shutdown();
    if (m_depth3dManager) m_depth3dManager->Shutdown();
    if (m_reshadeManager) m_reshadeManager->Shutdown();
    if (m_physicsManager) m_physicsManager->Shutdown();
    
    if (m_gameAnalyzer) m_gameAnalyzer->Shutdown();
    if (m_engineAdapter) m_engineAdapter->Shutdown();
    if (m_performanceOptimizer) m_performanceOptimizer->Shutdown();
    if (m_compatibilityManager) m_compatibilityManager->Shutdown();
    
    m_initialized = false;
    m_running = false;
    
    spdlog::info("UniversalVRConverter: Shutdown completed");
}

// Factory function
std::unique_ptr<IUniversalVRConverter> CreateUniversalVRConverter() {
    return std::make_unique<UniversalVRConverter>();
}

} // namespace uevrLCX
