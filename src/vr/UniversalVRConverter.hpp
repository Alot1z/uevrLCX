#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Core system includes
#include "../core/Framework.hpp"
#include "../core/PluginManager.hpp"
#include "../core/HookManager.hpp"

// VR system includes
#include "VRSystem.hpp"
#include "OpenXRIntegration.hpp"
#include "UniversalVRSystem.hpp"
#include "EngineAgnosticRenderer.hpp"
#include "FullPhysicsIntegration.hpp"
#include "FullAestheticCollisionEngine.hpp"

// Engine detection and adapters
#include "../core/engine_detection.hpp"
#include "../core/adapter_loader.hpp"
#include "../adapters/EngineAdapter.hpp"

    // Context7 AI system
    #include "../context7/Context7AISystem.hpp"
    #include "../context7/GameAnalyzer.hpp"
    #include "../context7/PerformanceOptimizer.hpp"
    
    // Advanced AI and Neural systems
    #include "../ai/AIPoweredVROptimizer.hpp"
    #include "../neural/NeuralRenderingSystem.hpp"
    
    // Advanced engine adapters
    #include "../adapters/AdvancedEngineAdapters.hpp"
    
    // Community learning system
    #include "../community/CommunityLearningSystem.hpp"
    
    // Reddit Community Integration
    RedditCommunityIntegration& m_redditCommunity;

    // External VR tools integration
    #include "VRGINIntegration.hpp"
    #include "Depth3DIntegration.hpp"
    #include "ReshadeIntegration.hpp"
    #include "PhysicsEngineIntegration.hpp"
    #include "PerformanceOptimization.hpp"
    #include "AdvancedPhysicsIntegration.hpp"
    
    // Reddit Community Integration
    #include "RedditCommunityIntegration.hpp"

namespace uevr {

/**
 * @brief Universal VR Conversion System - Main Controller
 * 
 * This is the core system that transforms any flat PC game into a fully functional VR experience.
 * It integrates all existing VR tools (VRGIN, Depth3D, Reshade, physics engines) into one unified system.
 * 
 * Key Features:
 * - Universal game compatibility (UE, Unity, custom, legacy engines)
 * - Real collision detection (not fake depth)
 * - Performance optimization (foveated rendering, async timewarp)
 * - AI-powered game analysis and optimization
 * - Community-driven compatibility database
 */
class UniversalVRConverter {
public:
    // Singleton pattern for global access
    static UniversalVRConverter& getInstance();
    
    // Disable copy and assignment
    UniversalVRConverter(const UniversalVRConverter&) = delete;
    UniversalVRConverter& operator=(const UniversalVRConverter&) = delete;

    // Core system lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }

    // Game conversion interface
    bool convertGameToVR(const std::string& gamePath, const std::string& gameName);
    bool injectIntoRunningGame(const std::string& processName);
    bool detectAndConvertGame();
    
    // Real-time conversion control
    void startConversion();
    void stopConversion();
    void pauseConversion();
    void resumeConversion();
    bool isConverting() const { return m_converting.load(); }

    // Configuration and settings
    struct ConversionSettings {
        // VR rendering settings
        bool enableStereoscopicRendering = true;
        bool enableDepthBufferExtraction = true;
        bool enableRealCollisionDetection = true;
        bool enableFoveatedRendering = true;
        bool enableAsyncTimewarp = true;
        
        // Performance settings
        int targetFrameRate = 90;
        float performanceScale = 1.0f;
        bool enablePerformanceOptimization = true;
        
        // Input settings
        bool enableMotionControllers = true;
        bool enableRoomScaleTracking = true;
        bool enableHapticFeedback = true;
        
        // AI settings
        bool enableAIGameAnalysis = true;
        bool enableAutoOptimization = true;
        bool enableCommunityLearning = true;
        
        // Engine-specific settings
        std::string preferredEngine = "auto";
        bool forceEngineDetection = false;
        
        // Advanced settings
        bool enableDebugMode = false;
        bool enableProfiling = false;
        std::string logLevel = "info";
    };

    void setConversionSettings(const ConversionSettings& settings);
    ConversionSettings getConversionSettings() const;

    // Engine detection and adapter management
    std::string detectGameEngine(const std::string& gamePath);
    bool loadEngineAdapter(const std::string& engineName);
    std::vector<std::string> getSupportedEngines() const;

    // VR system access
    VRSystem* getVRSystem() { return m_vrSystem.get(); }
    OpenXRIntegration* getOpenXRIntegration() { return m_openxrIntegration.get(); }
    UniversalVRSystem* getUniversalVRSystem() { return m_universalVRSystem.get(); }

    // Performance monitoring
    struct PerformanceMetrics {
        float currentFrameRate;
        float averageFrameRate;
        float latency;
        float cpuUsage;
        float gpuUsage;
        float memoryUsage;
        int droppedFrames;
        float vrPerformanceScore;
    };

    PerformanceMetrics getPerformanceMetrics() const;
    void enablePerformanceMonitoring(bool enable);

    // Community integration
    bool submitGameProfile(const std::string& gameName, const std::string& profileData);
    bool loadCommunityProfile(const std::string& gameName);
    std::vector<std::string> getCommunityProfiles() const;

    // Error handling and logging
    struct ConversionError {
        std::string errorCode;
        std::string errorMessage;
        std::string gameName;
        std::string engineName;
        std::string timestamp;
        std::string suggestedFix;
    };

    std::vector<ConversionError> getRecentErrors() const;
    void clearErrors();
    bool hasErrors() const;

    // Plugin system
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    std::vector<std::string> getLoadedPlugins() const;

    // Advanced features
    bool enableNeuralRendering(bool enable);
    bool enableAICollisionPrediction(bool enable);
    bool enableDynamicOptimization(bool enable);
    
    // Reddit Community Integration
    bool integrateRedditCommunitySolutions();
    std::vector<RedditCommunityIntegration::CommunitySolution> getCommunitySolutionsForGame(const std::string& gameName);
    bool applyRedditSolution(const std::string& solutionId, const std::string& gamePath);
    bool searchAndApplyCommunitySolutions(const std::string& gameName, const std::string& engineType);

private:
    UniversalVRConverter();
    ~UniversalVRConverter();

    // Core system components
    std::unique_ptr<Framework> m_framework;
    std::unique_ptr<PluginManager> m_pluginManager;
    std::unique_ptr<HookManager> m_hookManager;
    
    // VR system components
    std::unique_ptr<VRSystem> m_vrSystem;
    std::unique_ptr<OpenXRIntegration> m_openxrIntegration;
    std::unique_ptr<UniversalVRSystem> m_universalVRSystem;
    std::unique_ptr<EngineAgnosticRenderer> m_renderer;
    std::unique_ptr<FullPhysicsIntegration> m_physicsIntegration;
    std::unique_ptr<FullAestheticCollisionEngine> m_collisionEngine;

    // Engine detection and adapters
    std::unique_ptr<EngineDetector> m_engineDetector;
    std::unique_ptr<AdapterLoader> m_adapterLoader;
    std::unique_ptr<EngineAdapter> m_currentAdapter;

    // Context7 AI system
    std::unique_ptr<Context7AISystem> m_context7AI;
    std::unique_ptr<GameAnalyzer> m_gameAnalyzer;
    std::unique_ptr<PerformanceOptimizer> m_performanceOptimizer;

    // External VR tools integration
    std::unique_ptr<VRGINIntegration> m_vrginIntegration;
    std::unique_ptr<Depth3DIntegration> m_depth3DIntegration;
    std::unique_ptr<ReshadeIntegration> m_reshadeIntegration;
    std::unique_ptr<PhysicsEngineIntegration> m_physicsEngineIntegration;
    std::unique_ptr<PerformanceOptimization> m_performanceOptimization;

    // Advanced systems
    std::unique_ptr<AdvancedPhysicsIntegration> m_advancedPhysics;
    std::unique_ptr<AIPoweredVROptimizer> m_aiOptimizer;
    std::unique_ptr<NeuralRenderingSystem> m_neuralRendering;
    std::unique_ptr<AdvancedEngineAdapters> m_engineAdapters;
    std::unique_ptr<CommunityLearningSystem> m_communityLearning;

    // System state
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_converting{false};
    std::atomic<bool> m_paused{false};
    std::atomic<bool> m_shutdown{false};

    // Configuration
    ConversionSettings m_settings;
    mutable std::mutex m_settingsMutex;

    // Performance monitoring
    PerformanceMetrics m_performanceMetrics;
    mutable std::mutex m_metricsMutex;
    bool m_performanceMonitoringEnabled{false};
    std::thread m_performanceThread;

    // Error handling
    std::vector<ConversionError> m_recentErrors;
    mutable std::mutex m_errorsMutex;
    static constexpr size_t MAX_ERRORS = 100;

    // Community integration
    std::unordered_map<std::string, std::string> m_communityProfiles;
    mutable std::mutex m_profilesMutex;
    
    // Reddit Community Integration
    RedditCommunityIntegration& m_redditCommunity;

    // Plugin system
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> m_loadedPlugins;
    mutable std::mutex m_pluginsMutex;

    // Conversion thread
    std::thread m_conversionThread;
    std::condition_variable m_conversionCV;
    std::mutex m_conversionMutex;

    // Internal methods
    void conversionLoop();
    void performanceMonitoringLoop();
    bool initializeVRSystems();
    bool initializeAISystems();
    bool initializeExternalTools();
    bool detectAndLoadEngineAdapter();
    void updatePerformanceMetrics();
    void logError(const std::string& errorCode, const std::string& errorMessage, 
                  const std::string& gameName = "", const std::string& engineName = "");
    void loadCommunityProfiles();
    void saveCommunityProfiles();
    bool validateGamePath(const std::string& gamePath);
    bool validateEngineSupport(const std::string& engineName);
    void cleanupResources();
};

} // namespace uevr
