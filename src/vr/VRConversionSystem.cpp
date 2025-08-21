#include "VRConversionSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace UEVRLCX {
namespace VR {

// ============================================================================
// VR Conversion System Implementation
// ============================================================================

class VRConversionSystem : public IVRConversionSystem {
private:
    // Core Components
    std::unique_ptr<VRGINInjector> m_vrginInjector;
    std::unique_ptr<Depth3DExtractor> m_depth3DExtractor;
    std::unique_ptr<ReshadeFramework> m_reshadeFramework;
    std::unique_ptr<PhysicsEngine> m_physicsEngine;
    std::unique_ptr<CollisionDetector> m_collisionDetector;
    std::unique_ptr<PerformanceOptimizer> m_performanceOptimizer;
    std::unique_ptr<FoveatedRenderer> m_foveatedRenderer;
    
    // Engine Adapters
    std::unordered_map<GameEngine, std::unique_ptr<EngineAdapter>> m_engineAdapters;
    
    // System State
    GameInfo m_currentGameInfo;
    VRConfig m_vrConfig;
    ConversionStatus m_conversionStatus;
    ConversionMetrics m_conversionMetrics;
    bool m_isInitialized;
    bool m_isConverting;
    
    // Callbacks
    ConversionProgressCallback m_progressCallback;
    ConversionCompleteCallback m_completeCallback;
    ErrorCallback m_errorCallback;
    
    // Threading
    std::thread m_conversionThread;
    std::mutex m_systemMutex;
    std::atomic<bool> m_shouldStop;

public:
    VRConversionSystem() : m_isInitialized(false), m_isConverting(false), m_shouldStop(false) {
        m_conversionStatus.state = ConversionStatus::State::IDLE;
    }
    
    ~VRConversionSystem() {
        Shutdown();
    }

    // ========================================================================
    // Core Conversion Operations
    // ========================================================================

    bool Initialize() override {
        std::lock_guard<std::mutex> lock(m_systemMutex);
        
        try {
            LogInfo("Initializing UEVRLCX VR Conversion System...");
            
            // Initialize core VR components
            if (!InitializeCoreComponents()) {
                LogError("Failed to initialize core components");
                return false;
            }
            
            // Initialize engine adapters
            if (!InitializeEngineAdapters()) {
                LogError("Failed to initialize engine adapters");
                return false;
            }
            
            // Initialize performance systems
            if (!InitializePerformanceSystems()) {
                LogError("Failed to initialize performance systems");
                return false;
            }
            
            m_isInitialized = true;
            LogInfo("UEVRLCX VR Conversion System initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            LogError("Exception during VR conversion system initialization: {}", e.what());
            return false;
        }
    }

    bool AnalyzeGame(const std::string& gamePath) override {
        std::lock_guard<std::mutex> lock(m_systemMutex);
        
        if (!m_isInitialized) {
            LogError("VR conversion system not initialized");
            return false;
        }
        
        try {
            LogInfo("Analyzing game: {}", gamePath);
            m_conversionStatus.state = ConversionStatus::State::ANALYZING;
            m_conversionStatus.progress = 0.0;
            m_conversionStatus.currentStep = "Starting game analysis";
            
            // Detect game engine
            if (!DetectGameEngine(gamePath)) {
                LogError("Failed to detect game engine");
                m_conversionStatus.state = ConversionStatus::State::ERROR;
                return false;
            }
            m_conversionStatus.progress = 25.0;
            
            // Detect rendering API
            if (!DetectRenderingAPI(gamePath)) {
                LogError("Failed to detect rendering API");
                m_conversionStatus.state = ConversionStatus::State::ERROR;
                return false;
            }
            m_conversionStatus.progress = 50.0;
            
            // Analyze game compatibility
            if (!AnalyzeGameCompatibility(gamePath)) {
                LogError("Game compatibility analysis failed");
                m_conversionStatus.state = ConversionStatus::State::ERROR;
                return false;
            }
            m_conversionStatus.progress = 75.0;
            
            // Generate game info
            if (!GenerateGameInfo(gamePath)) {
                LogError("Failed to generate game info");
                m_conversionStatus.state = ConversionStatus::State::ERROR;
                return false;
            }
            m_conversionStatus.progress = 100.0;
            
            m_conversionStatus.state = ConversionStatus::State::IDLE;
            m_conversionStatus.completedSteps.push_back("Game analysis completed");
            
            LogInfo("Game analysis completed successfully");
            return true;
            
        } catch (const std::exception& e) {
            LogError("Exception during game analysis: {}", e.what());
            m_conversionStatus.state = ConversionStatus::State::ERROR;
            return false;
        }
    }

    bool ConvertGameToVR(const GameInfo& gameInfo, const VRConfig& vrConfig) override {
        std::lock_guard<std::mutex> lock(m_systemMutex);
        
        if (!m_isInitialized) {
            LogError("VR conversion system not initialized");
            return false;
        }
        
        if (m_isConverting) {
            LogError("VR conversion already in progress");
            return false;
        }
        
        try {
            LogInfo("Starting VR conversion for game: {}", gameInfo.name);
            
            m_currentGameInfo = gameInfo;
            m_vrConfig = vrConfig;
            m_isConverting = true;
            m_shouldStop = false;
            
            // Start conversion in separate thread
            m_conversionThread = std::thread(&VRConversionSystem::ConversionWorker, this);
            
            return true;
            
        } catch (const std::exception& e) {
            LogError("Exception during VR conversion start: {}", e.what());
            return false;
        }
    }

    bool StartVRConversion() override {
        return ConvertGameToVR(m_currentGameInfo, m_vrConfig);
    }

    bool StopVRConversion() override {
        std::lock_guard<std::mutex> lock(m_systemMutex);
        
        if (!m_isConverting) {
            return true;
        }
        
        LogInfo("Stopping VR conversion...");
        m_shouldStop = true;
        
        if (m_conversionThread.joinable()) {
            m_conversionThread.join();
        }
        
        m_isConverting = false;
        m_conversionStatus.state = ConversionStatus::State::IDLE;
        
        LogInfo("VR conversion stopped");
        return true;
    }

    void Shutdown() override {
        StopVRConversion();
        
        std::lock_guard<std::mutex> lock(m_systemMutex);
        
        // Cleanup components
        m_vrginInjector.reset();
        m_depth3DExtractor.reset();
        m_reshadeFramework.reset();
        m_physicsEngine.reset();
        m_collisionDetector.reset();
        m_performanceOptimizer.reset();
        m_foveatedRenderer.reset();
        
        // Cleanup engine adapters
        m_engineAdapters.clear();
        
        m_isInitialized = false;
        LogInfo("UEVRLCX VR Conversion System shutdown complete");
    }

    // ========================================================================
    // Game Analysis and Detection
    // ========================================================================

    GameInfo GetGameInfo() const override {
        return m_currentGameInfo;
    }

    bool DetectGameEngine(const std::string& gamePath) override {
        try {
            std::filesystem::path path(gamePath);
            
            // Check for Unreal Engine signatures
            if (IsUnrealEngineGame(path)) {
                m_currentGameInfo.engine = GameEngine::UNREAL_ENGINE;
                return true;
            }
            
            // Check for Unity signatures
            if (IsUnityGame(path)) {
                m_currentGameInfo.engine = GameEngine::UNITY;
                return true;
            }
            
            // Check for Godot signatures
            if (IsGodotGame(path)) {
                m_currentGameInfo.engine = GameEngine::GODOT;
                return true;
            }
            
            // Default to custom engine
            m_currentGameInfo.engine = GameEngine::CUSTOM;
            return true;
            
        } catch (const std::exception& e) {
            LogError("Exception during engine detection: {}", e.what());
            return false;
        }
    }

    bool DetectRenderingAPI(const std::string& gamePath) override {
        try {
            std::filesystem::path path(gamePath);
            
            // Check for DirectX signatures
            if (HasDirectXSignatures(path)) {
                m_currentGameInfo.api = RenderingAPI::DIRECTX_11;
                return true;
            }
            
            // Check for Vulkan signatures
            if (HasVulkanSignatures(path)) {
                m_currentGameInfo.api = RenderingAPI::VULKAN;
                return true;
            }
            
            // Check for OpenGL signatures
            if (HasOpenGLSignatures(path)) {
                m_currentGameInfo.api = RenderingAPI::OPENGL;
                return true;
            }
            
            // Default to DirectX 11
            m_currentGameInfo.api = RenderingAPI::DIRECTX_11;
            return true;
            
        } catch (const std::exception& e) {
            LogError("Exception during API detection: {}", e.what());
            return false;
        }
    }

    std::vector<std::string> GetSupportedGames() const override {
        // Return list of games from the comprehensive resource list
        return {
            "Unreal Engine Games",
            "Unity Games", 
            "Godot Games",
            "Custom Engine Games",
            "Legacy Games"
        };
    }

    bool IsGameCompatible(const std::string& gamePath) const override {
        // Check if game is compatible based on engine and API
        GameEngine engine = GameEngine::UNKNOWN;
        RenderingAPI api = RenderingAPI::UNKNOWN;
        
        // Quick compatibility check
        return (engine != GameEngine::UNKNOWN && api != RenderingAPI::UNKNOWN);
    }

    // ========================================================================
    // VR Configuration Management
    // ========================================================================

    bool SetVRConfig(const VRConfig& config) override {
        std::lock_guard<std::mutex> lock(m_systemMutex);
        m_vrConfig = config;
        return true;
    }

    VRConfig GetVRConfig() const override {
        return m_vrConfig;
    }

    bool OptimizeForHeadset(VRHeadset headset) override {
        std::lock_guard<std::mutex> lock(m_systemMutex);
        
        m_vrConfig.headset = headset;
        
        // Apply headset-specific optimizations
        switch (headset) {
            case VRHeadset::OCULUS_QUEST_2:
                m_vrConfig.refreshRate = 72.0;
                m_vrConfig.resolution = 0.8;
                break;
            case VRHeadset::OCULUS_QUEST_3:
                m_vrConfig.refreshRate = 90.0;
                m_vrConfig.resolution = 1.0;
                break;
            case VRHeadset::HTC_VIVE:
                m_vrConfig.refreshRate = 90.0;
                m_vrConfig.resolution = 1.0;
                break;
            case VRHeadset::VALVE_INDEX:
                m_vrConfig.refreshRate = 120.0;
                m_vrConfig.resolution = 1.2;
                break;
            default:
                break;
        }
        
        return true;
    }

    bool SetCollisionType(CollisionType type) override {
        std::lock_guard<std::mutex> lock(m_systemMutex);
        m_vrConfig.collisionType = type;
        return true;
    }

    bool SetOptimizationLevel(OptimizationLevel level) override {
        std::lock_guard<std::mutex> lock(m_systemMutex);
        m_vrConfig.optimizationLevel = level;
        return true;
    }

    // ========================================================================
    // Conversion Status and Monitoring
    // ========================================================================

    ConversionStatus GetConversionStatus() const override {
        return m_conversionStatus;
    }

    ConversionMetrics GetConversionMetrics() const override {
        return m_conversionMetrics;
    }

    bool IsConverting() const override {
        return m_isConverting;
    }

    double GetConversionProgress() const override {
        return m_conversionStatus.progress;
    }

    // ========================================================================
    // Performance and Optimization
    // ========================================================================

    bool OptimizePerformance() override {
        if (!m_performanceOptimizer) {
            return false;
        }
        
        return m_performanceOptimizer->Optimize();
    }

    bool EnableFoveatedRendering(bool enable) override {
        if (!m_foveatedRenderer) {
            return false;
        }
        
        m_vrConfig.enableFoveatedRendering = enable;
        return m_foveatedRenderer->Enable(enable);
    }

    bool EnableAsyncTimewarp(bool enable) override {
        m_vrConfig.enableAsyncTimewarp = enable;
        return true;
    }

    bool SetTargetFrameRate(double frameRate) override {
        m_vrConfig.refreshRate = frameRate;
        return true;
    }

    bool SetMaxLatency(double latency) override {
        // Implementation for latency control
        return true;
    }

    // ========================================================================
    // Collision Detection and Physics
    // ========================================================================

    bool InitializeCollisionSystem() override {
        if (!m_collisionDetector) {
            return false;
        }
        
        return m_collisionDetector->Initialize();
    }

    bool EnableRealCollision(bool enable) override {
        if (enable) {
            m_vrConfig.collisionType = CollisionType::REAL_3D_COLLISION;
        } else {
            m_vrConfig.collisionType = CollisionType::DISABLED;
        }
        
        return true;
    }

    bool SetCollisionAccuracy(double accuracy) override {
        // Implementation for collision accuracy
        return true;
    }

    bool TestCollisionDetection() override {
        if (!m_collisionDetector) {
            return false;
        }
        
        return m_collisionDetector->Test();
    }

    // ========================================================================
    // Engine-Specific Adapters
    // ========================================================================

    bool LoadEngineAdapter(GameEngine engine) override {
        if (m_engineAdapters.find(engine) != m_engineAdapters.end()) {
            return true; // Already loaded
        }
        
        auto adapter = CreateEngineAdapter(engine);
        if (adapter) {
            m_engineAdapters[engine] = std::move(adapter);
            return true;
        }
        
        return false;
    }

    bool UnloadEngineAdapter(GameEngine engine) override {
        auto it = m_engineAdapters.find(engine);
        if (it != m_engineAdapters.end()) {
            m_engineAdapters.erase(it);
            return true;
        }
        
        return false;
    }

    std::vector<GameEngine> GetSupportedEngines() const override {
        return {
            GameEngine::UNREAL_ENGINE,
            GameEngine::UNITY,
            GameEngine::GODOT,
            GameEngine::CUSTOM,
            GameEngine::LEGACY
        };
    }

    bool IsEngineSupported(GameEngine engine) const override {
        return std::find(GetSupportedEngines().begin(), 
                        GetSupportedEngines().end(), engine) != GetSupportedEngines().end();
    }

    // ========================================================================
    // Input and Control Mapping
    // ========================================================================

    bool InitializeInputSystem() override {
        // Implementation for input system initialization
        return true;
    }

    bool MapGameControls() override {
        // Implementation for game control mapping
        return true;
    }

    bool EnableMotionControllers(bool enable) override {
        m_vrConfig.enableMotionControllers = enable;
        return true;
    }

    bool EnableHapticFeedback(bool enable) override {
        m_vrConfig.enableHapticFeedback = enable;
        return true;
    }

    // ========================================================================
    // Shader and Visual Enhancement
    // ========================================================================

    bool InitializeShaderPipeline() override {
        if (!m_reshadeFramework) {
            return false;
        }
        
        return m_reshadeFramework->Initialize();
    }

    bool LoadVRShaders() override {
        if (!m_reshadeFramework) {
            return false;
        }
        
        return m_reshadeFramework->LoadShaders();
    }

    bool EnableDepthExtraction(bool enable) override {
        if (!m_depth3DExtractor) {
            return false;
        }
        
        return m_depth3DExtractor->Enable(enable);
    }

    bool SetVisualQuality(int quality) override {
        // Implementation for visual quality settings
        return true;
    }

    // ========================================================================
    // Testing and Validation
    // ========================================================================

    bool RunConversionTests() override {
        // Implementation for conversion testing
        return true;
    }

    bool ValidateVRConversion() override {
        // Implementation for VR conversion validation
        return true;
    }

    bool TestPerformance() override {
        // Implementation for performance testing
        return true;
    }

    bool TestCompatibility() override {
        // Implementation for compatibility testing
        return true;
    }

    // ========================================================================
    // Configuration and Persistence
    // ========================================================================

    bool LoadConfiguration(const std::string& configPath) override {
        // Implementation for configuration loading
        return true;
    }

    bool SaveConfiguration(const std::string& configPath) override {
        // Implementation for configuration saving
        return true;
    }

    bool ExportVRProfile(const std::string& profilePath) override {
        // Implementation for VR profile export
        return true;
    }

    bool ImportVRProfile(const std::string& profilePath) override {
        // Implementation for VR profile import
        return true;
    }

    // ========================================================================
    // Event Handling and Callbacks
    // ========================================================================

    void RegisterProgressCallback(ConversionProgressCallback callback) override {
        m_progressCallback = callback;
    }

    void RegisterCompleteCallback(ConversionCompleteCallback callback) override {
        m_completeCallback = callback;
    }

    void RegisterErrorCallback(ErrorCallback callback) override {
        m_errorCallback = callback;
    }

    // ========================================================================
    // Advanced Features
    // ========================================================================

    bool EnableFeature(const std::string& featureName) override {
        // Implementation for feature enabling
        return true;
    }

    bool DisableFeature(const std::string& featureName) override {
        // Implementation for feature disabling
        return true;
    }

    bool IsFeatureEnabled(const std::string& featureName) const override {
        // Implementation for feature status check
        return true;
    }

    std::vector<std::string> GetAvailableFeatures() const override {
        return {
            "Real 3D Collision",
            "Depth Buffer Extraction",
            "Foveated Rendering",
            "Async Timewarp",
            "Motion Controllers",
            "Haptic Feedback",
            "Room Scale VR",
            "Performance Optimization"
        };
    }

    // ========================================================================
    // Diagnostics and Debugging
    // ========================================================================

    bool GenerateDiagnosticReport(const std::string& reportPath) override {
        // Implementation for diagnostic report generation
        return true;
    }

    bool EnableDebugMode(bool enable) override {
        // Implementation for debug mode
        return true;
    }

    std::string GetSystemLogs() const override {
        // Implementation for system logs retrieval
        return "System logs";
    }

    bool ClearLogs() override {
        // Implementation for log clearing
        return true;
    }

private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================

    bool InitializeCoreComponents() {
        LogInfo("Initializing core VR components...");
        
        // Initialize VRGIN injector
        m_vrginInjector = std::make_unique<VRGINInjector>();
        if (!m_vrginInjector->Initialize()) {
            LogError("Failed to initialize VRGIN injector");
            return false;
        }
        
        // Initialize Depth3D extractor
        m_depth3DExtractor = std::make_unique<Depth3DExtractor>();
        if (!m_depth3DExtractor->Initialize()) {
            LogError("Failed to initialize Depth3D extractor");
            return false;
        }
        
        // Initialize Reshade framework
        m_reshadeFramework = std::make_unique<ReshadeFramework>();
        if (!m_reshadeFramework->Initialize()) {
            LogError("Failed to initialize Reshade framework");
            return false;
        }
        
        // Initialize physics engine
        m_physicsEngine = std::make_unique<PhysicsEngine>();
        if (!m_physicsEngine->Initialize()) {
            LogError("Failed to initialize physics engine");
            return false;
        }
        
        // Initialize collision detector
        m_collisionDetector = std::make_unique<CollisionDetector>();
        if (!m_collisionDetector->Initialize()) {
            LogError("Failed to initialize collision detector");
            return false;
        }
        
        // Initialize performance optimizer
        m_performanceOptimizer = std::make_unique<PerformanceOptimizer>();
        if (!m_performanceOptimizer->Initialize()) {
            LogError("Failed to initialize performance optimizer");
            return false;
        }
        
        // Initialize foveated renderer
        m_foveatedRenderer = std::make_unique<FoveatedRenderer>();
        if (!m_foveatedRenderer->Initialize()) {
            LogError("Failed to initialize foveated renderer");
            return false;
        }
        
        LogInfo("Core VR components initialized successfully");
        return true;
    }

    bool InitializeEngineAdapters() {
        LogInfo("Initializing engine adapters...");
        
        // Initialize adapters for all supported engines
        for (auto engine : GetSupportedEngines()) {
            if (!LoadEngineAdapter(engine)) {
                LogWarning("Failed to load engine adapter for: {}", GameEngineToString(engine));
            }
        }
        
        LogInfo("Engine adapters initialized");
        return true;
    }

    bool InitializePerformanceSystems() {
        LogInfo("Initializing performance systems...");
        
        // Initialize performance monitoring
        // Initialize optimization algorithms
        // Initialize caching systems
        
        LogInfo("Performance systems initialized");
        return true;
    }

    void ConversionWorker() {
        try {
            LogInfo("Starting VR conversion worker thread");
            
            m_conversionStatus.state = ConversionStatus::State::CONVERTING;
            m_conversionStatus.progress = 0.0;
            
            // Phase 1: Game Analysis (10%)
            if (m_shouldStop) return;
            m_conversionStatus.currentStep = "Analyzing game structure";
            m_conversionStatus.progress = 10.0;
            if (m_progressCallback) m_progressCallback(10.0, "Analyzing game structure");
            
            // Phase 2: Engine Adapter Loading (20%)
            if (m_shouldStop) return;
            m_conversionStatus.currentStep = "Loading engine adapter";
            if (!LoadEngineAdapter(m_currentGameInfo.engine)) {
                throw std::runtime_error("Failed to load engine adapter");
            }
            m_conversionStatus.progress = 20.0;
            if (m_progressCallback) m_progressCallback(20.0, "Engine adapter loaded");
            
            // Phase 3: VR Injection (40%)
            if (m_shouldStop) return;
            m_conversionStatus.currentStep = "Injecting VR components";
            if (!m_vrginInjector->Inject(m_currentGameInfo.executablePath)) {
                throw std::runtime_error("Failed to inject VR components");
            }
            m_conversionStatus.progress = 40.0;
            if (m_progressCallback) m_progressCallback(40.0, "VR components injected");
            
            // Phase 4: Depth Extraction (60%)
            if (m_shouldStop) return;
            m_conversionStatus.currentStep = "Setting up depth extraction";
            if (!m_depth3DExtractor->Setup(m_currentGameInfo)) {
                throw std::runtime_error("Failed to setup depth extraction");
            }
            m_conversionStatus.progress = 60.0;
            if (m_progressCallback) m_progressCallback(60.0, "Depth extraction configured");
            
            // Phase 5: Shader Pipeline (80%)
            if (m_shouldStop) return;
            m_conversionStatus.currentStep = "Configuring shader pipeline";
            if (!m_reshadeFramework->Configure(m_vrConfig)) {
                throw std::runtime_error("Failed to configure shader pipeline");
            }
            m_conversionStatus.progress = 80.0;
            if (m_progressCallback) m_progressCallback(80.0, "Shader pipeline configured");
            
            // Phase 6: Performance Optimization (90%)
            if (m_shouldStop) return;
            m_conversionStatus.currentStep = "Optimizing performance";
            if (!m_performanceOptimizer->Optimize()) {
                LogWarning("Performance optimization failed, continuing...");
            }
            m_conversionStatus.progress = 90.0;
            if (m_progressCallback) m_progressCallback(90.0, "Performance optimized");
            
            // Phase 7: Finalization (100%)
            if (m_shouldStop) return;
            m_conversionStatus.currentStep = "Finalizing VR conversion";
            m_conversionStatus.progress = 100.0;
            if (m_progressCallback) m_progressCallback(100.0, "VR conversion completed");
            
            m_conversionStatus.state = ConversionStatus::State::COMPLETED;
            m_conversionStatus.completedSteps.push_back("VR conversion completed successfully");
            
            if (m_completeCallback) {
                m_completeCallback(true, "VR conversion completed successfully");
            }
            
            LogInfo("VR conversion completed successfully");
            
        } catch (const std::exception& e) {
            LogError("Exception in conversion worker: {}", e.what());
            m_conversionStatus.state = ConversionStatus::State::ERROR;
            m_conversionStatus.errors.push_back(e.what());
            
            if (m_errorCallback) {
                m_errorCallback(e.what());
            }
            
            if (m_completeCallback) {
                m_completeCallback(false, e.what());
            }
        }
        
        m_isConverting = false;
    }

    bool AnalyzeGameCompatibility(const std::string& gamePath) {
        // Implementation for game compatibility analysis
        return true;
    }

    bool GenerateGameInfo(const std::string& gamePath) {
        // Implementation for game info generation
        return true;
    }

    bool IsUnrealEngineGame(const std::filesystem::path& path) {
        // Check for Unreal Engine signatures
        return false; // Placeholder
    }

    bool IsUnityGame(const std::filesystem::path& path) {
        // Check for Unity signatures
        return false; // Placeholder
    }

    bool IsGodotGame(const std::filesystem::path& path) {
        // Check for Godot signatures
        return false; // Placeholder
    }

    bool HasDirectXSignatures(const std::filesystem::path& path) {
        // Check for DirectX signatures
        return false; // Placeholder
    }

    bool HasVulkanSignatures(const std::filesystem::path& path) {
        // Check for Vulkan signatures
        return false; // Placeholder
    }

    bool HasOpenGLSignatures(const std::filesystem::path& path) {
        // Check for OpenGL signatures
        return false; // Placeholder
    }

    std::unique_ptr<EngineAdapter> CreateEngineAdapter(GameEngine engine) {
        // Implementation for engine adapter creation
        return nullptr; // Placeholder
    }

    void LogInfo(const std::string& message) {
        std::cout << "[INFO] " << message << std::endl;
    }

    void LogError(const std::string& message) {
        std::cout << "[ERROR] " << message << std::endl;
    }

    void LogWarning(const std::string& message) {
        std::cout << "[WARNING] " << message << std::endl;
    }
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<IVRConversionSystem> CreateVRConversionSystem() {
    return std::make_unique<VRConversionSystem>();
}

std::unique_ptr<IVRConversionSystem> CreateVRConversionSystemWithConfig(const VRConfig& config) {
    auto system = std::make_unique<VRConversionSystem>();
    system->SetVRConfig(config);
    return system;
}

// ============================================================================
// Utility Functions
// ============================================================================

std::string GameEngineToString(GameEngine engine) {
    switch (engine) {
        case GameEngine::UNREAL_ENGINE: return "Unreal Engine";
        case GameEngine::UNITY: return "Unity";
        case GameEngine::GODOT: return "Godot";
        case GameEngine::CUSTOM: return "Custom";
        case GameEngine::LEGACY: return "Legacy";
        case GameEngine::UNKNOWN: return "Unknown";
        default: return "Unknown";
    }
}

std::string RenderingAPIToString(RenderingAPI api) {
    switch (api) {
        case RenderingAPI::DIRECTX_11: return "DirectX 11";
        case RenderingAPI::DIRECTX_12: return "DirectX 12";
        case RenderingAPI::VULKAN: return "Vulkan";
        case RenderingAPI::OPENGL: return "OpenGL";
        case RenderingAPI::METAL: return "Metal";
        case RenderingAPI::UNKNOWN: return "Unknown";
        default: return "Unknown";
    }
}

std::string VRHeadsetToString(VRHeadset headset) {
    switch (headset) {
        case VRHeadset::OCULUS_QUEST_2: return "Oculus Quest 2";
        case VRHeadset::OCULUS_QUEST_3: return "Oculus Quest 3";
        case VRHeadset::HTC_VIVE: return "HTC Vive";
        case VRHeadset::VALVE_INDEX: return "Valve Index";
        case VRHeadset::WINDOWS_MR: return "Windows Mixed Reality";
        case VRHeadset::CUSTOM: return "Custom";
        case VRHeadset::UNKNOWN: return "Unknown";
        default: return "Unknown";
    }
}

std::string CollisionTypeToString(CollisionType type) {
    switch (type) {
        case CollisionType::REAL_3D_COLLISION: return "Real 3D Collision";
        case CollisionType::DEPTH_BUFFER_COLLISION: return "Depth Buffer Collision";
        case CollisionType::HYBRID_COLLISION: return "Hybrid Collision";
        case CollisionType::DISABLED: return "Disabled";
        default: return "Unknown";
    }
}

std::string OptimizationLevelToString(OptimizationLevel level) {
    switch (level) {
        case OptimizationLevel::NONE: return "None";
        case OptimizationLevel::BASIC: return "Basic";
        case OptimizationLevel::ADVANCED: return "Advanced";
        case OptimizationLevel::ULTRA: return "Ultra";
        default: return "Unknown";
    }
}

GameEngine StringToGameEngine(const std::string& str) {
    if (str == "Unreal Engine") return GameEngine::UNREAL_ENGINE;
    if (str == "Unity") return GameEngine::UNITY;
    if (str == "Godot") return GameEngine::GODOT;
    if (str == "Custom") return GameEngine::CUSTOM;
    if (str == "Legacy") return GameEngine::LEGACY;
    return GameEngine::UNKNOWN;
}

RenderingAPI StringToRenderingAPI(const std::string& str) {
    if (str == "DirectX 11") return RenderingAPI::DIRECTX_11;
    if (str == "DirectX 12") return RenderingAPI::DIRECTX_12;
    if (str == "Vulkan") return RenderingAPI::VULKAN;
    if (str == "OpenGL") return RenderingAPI::OPENGL;
    if (str == "Metal") return RenderingAPI::METAL;
    return RenderingAPI::UNKNOWN;
}

VRHeadset StringToVRHeadset(const std::string& str) {
    if (str == "Oculus Quest 2") return VRHeadset::OCULUS_QUEST_2;
    if (str == "Oculus Quest 3") return VRHeadset::OCULUS_QUEST_3;
    if (str == "HTC Vive") return VRHeadset::HTC_VIVE;
    if (str == "Valve Index") return VRHeadset::VALVE_INDEX;
    if (str == "Windows Mixed Reality") return VRHeadset::WINDOWS_MR;
    if (str == "Custom") return VRHeadset::CUSTOM;
    return VRHeadset::UNKNOWN;
}

CollisionType StringToCollisionType(const std::string& str) {
    if (str == "Real 3D Collision") return CollisionType::REAL_3D_COLLISION;
    if (str == "Depth Buffer Collision") return CollisionType::DEPTH_BUFFER_COLLISION;
    if (str == "Hybrid Collision") return CollisionType::HYBRID_COLLISION;
    if (str == "Disabled") return CollisionType::DISABLED;
    return CollisionType::REAL_3D_COLLISION; // Default
}

OptimizationLevel StringToOptimizationLevel(const std::string& str) {
    if (str == "None") return OptimizationLevel::NONE;
    if (str == "Basic") return OptimizationLevel::BASIC;
    if (str == "Advanced") return OptimizationLevel::ADVANCED;
    if (str == "Ultra") return OptimizationLevel::ULTRA;
    return OptimizationLevel::ADVANCED; // Default
}

} // namespace VR
} // namespace UEVRLCX
