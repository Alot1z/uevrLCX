#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

// ============================================================================
// UEVRLCX VR Conversion System Header
// Universal Game-to-VR Converter with Real Collision Detection
// ============================================================================

namespace UEVRLCX {
namespace VR {

// ============================================================================
// Forward Declarations
// ============================================================================

class VRGINInjector;
class Depth3DExtractor;
class ReshadeFramework;
class PhysicsEngine;
class CollisionDetector;
class PerformanceOptimizer;
class EngineAdapter;
class InputMapper;
class ShaderPipeline;
class FoveatedRenderer;

// ============================================================================
// VR Conversion Enums
// ============================================================================

enum class GameEngine {
    UNREAL_ENGINE,
    UNITY,
    GODOT,
    CUSTOM,
    LEGACY,
    UNKNOWN
};

enum class RenderingAPI {
    DIRECTX_11,
    DIRECTX_12,
    VULKAN,
    OPENGL,
    METAL,
    UNKNOWN
};

enum class VRHeadset {
    OCULUS_QUEST_2,
    OCULUS_QUEST_3,
    HTC_VIVE,
    VALVE_INDEX,
    WINDOWS_MR,
    CUSTOM,
    UNKNOWN
};

enum class CollisionType {
    REAL_3D_COLLISION,
    DEPTH_BUFFER_COLLISION,
    HYBRID_COLLISION,
    DISABLED
};

enum class OptimizationLevel {
    NONE,
    BASIC,
    ADVANCED,
    ULTRA
};

// ============================================================================
// VR Conversion Data Structures
// ============================================================================

struct GameInfo {
    std::string name;
    std::string executablePath;
    GameEngine engine;
    RenderingAPI api;
    std::string version;
    bool is64Bit;
    std::vector<std::string> dependencies;
    std::unordered_map<std::string, std::string> metadata;
    
    GameInfo() : engine(GameEngine::UNKNOWN), api(RenderingAPI::UNKNOWN), is64Bit(true) {}
};

struct VRConfig {
    VRHeadset headset;
    double refreshRate;
    double resolution;
    double fieldOfView;
    bool enableMotionControllers;
    bool enableRoomScale;
    bool enableHapticFeedback;
    CollisionType collisionType;
    OptimizationLevel optimizationLevel;
    bool enableFoveatedRendering;
    bool enableAsyncTimewarp;
    
    VRConfig() : headset(VRHeadset::OCULUS_QUEST_2), refreshRate(90.0), 
                 resolution(1.0), fieldOfView(110.0), enableMotionControllers(true),
                 enableRoomScale(true), enableHapticFeedback(true),
                 collisionType(CollisionType::REAL_3D_COLLISION),
                 optimizationLevel(OptimizationLevel::ADVANCED),
                 enableFoveatedRendering(true), enableAsyncTimewarp(true) {}
};

struct ConversionMetrics {
    double frameRate;
    double latency;
    double collisionAccuracy;
    double depthAccuracy;
    double performanceOverhead;
    bool motionSicknessDetected;
    std::chrono::steady_clock::time_point timestamp;
    
    ConversionMetrics() : frameRate(0.0), latency(0.0), collisionAccuracy(0.0),
                         depthAccuracy(0.0), performanceOverhead(0.0),
                         motionSicknessDetected(false) {}
};

struct ConversionStatus {
    enum class State {
        IDLE,
        ANALYZING,
        CONVERTING,
        OPTIMIZING,
        TESTING,
        COMPLETED,
        ERROR
    };
    
    State state;
    double progress;
    std::string currentStep;
    std::vector<std::string> completedSteps;
    std::vector<std::string> errors;
    ConversionMetrics metrics;
    
    ConversionStatus() : state(State::IDLE), progress(0.0) {}
};

// ============================================================================
// VR Conversion System Interface
// ============================================================================

class IVRConversionSystem {
public:
    virtual ~IVRConversionSystem() = default;
    
    // ========================================================================
    // Core Conversion Operations
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual bool AnalyzeGame(const std::string& gamePath) = 0;
    virtual bool ConvertGameToVR(const GameInfo& gameInfo, const VRConfig& vrConfig) = 0;
    virtual bool StartVRConversion() = 0;
    virtual bool StopVRConversion() = 0;
    virtual void Shutdown() = 0;
    
    // ========================================================================
    // Game Analysis and Detection
    // ========================================================================
    
    virtual GameInfo GetGameInfo() const = 0;
    virtual bool DetectGameEngine(const std::string& gamePath) = 0;
    virtual bool DetectRenderingAPI(const std::string& gamePath) = 0;
    virtual std::vector<std::string> GetSupportedGames() const = 0;
    virtual bool IsGameCompatible(const std::string& gamePath) const = 0;
    
    // ========================================================================
    // VR Configuration Management
    // ========================================================================
    
    virtual bool SetVRConfig(const VRConfig& config) = 0;
    virtual VRConfig GetVRConfig() const = 0;
    virtual bool OptimizeForHeadset(VRHeadset headset) = 0;
    virtual bool SetCollisionType(CollisionType type) = 0;
    virtual bool SetOptimizationLevel(OptimizationLevel level) = 0;
    
    // ========================================================================
    // Conversion Status and Monitoring
    // ========================================================================
    
    virtual ConversionStatus GetConversionStatus() const = 0;
    virtual ConversionMetrics GetConversionMetrics() const = 0;
    virtual bool IsConverting() const = 0;
    virtual double GetConversionProgress() const = 0;
    
    // ========================================================================
    // Performance and Optimization
    // ========================================================================
    
    virtual bool OptimizePerformance() = 0;
    virtual bool EnableFoveatedRendering(bool enable) = 0;
    virtual bool EnableAsyncTimewarp(bool enable) = 0;
    virtual bool SetTargetFrameRate(double frameRate) = 0;
    virtual bool SetMaxLatency(double latency) = 0;
    
    // ========================================================================
    // Collision Detection and Physics
    // ========================================================================
    
    virtual bool InitializeCollisionSystem() = 0;
    virtual bool EnableRealCollision(bool enable) = 0;
    virtual bool SetCollisionAccuracy(double accuracy) = 0;
    virtual bool TestCollisionDetection() = 0;
    
    // ========================================================================
    // Engine-Specific Adapters
    // ========================================================================
    
    virtual bool LoadEngineAdapter(GameEngine engine) = 0;
    virtual bool UnloadEngineAdapter(GameEngine engine) = 0;
    virtual std::vector<GameEngine> GetSupportedEngines() const = 0;
    virtual bool IsEngineSupported(GameEngine engine) const = 0;
    
    // ========================================================================
    // Input and Control Mapping
    // ========================================================================
    
    virtual bool InitializeInputSystem() = 0;
    virtual bool MapGameControls() = 0;
    virtual bool EnableMotionControllers(bool enable) = 0;
    virtual bool EnableHapticFeedback(bool enable) = 0;
    
    // ========================================================================
    // Shader and Visual Enhancement
    // ========================================================================
    
    virtual bool InitializeShaderPipeline() = 0;
    virtual bool LoadVRShaders() = 0;
    virtual bool EnableDepthExtraction(bool enable) = 0;
    virtual bool SetVisualQuality(int quality) = 0;
    
    // ========================================================================
    // Testing and Validation
    // ========================================================================
    
    virtual bool RunConversionTests() = 0;
    virtual bool ValidateVRConversion() = 0;
    virtual bool TestPerformance() = 0;
    virtual bool TestCompatibility() = 0;
    
    // ========================================================================
    // Configuration and Persistence
    // ========================================================================
    
    virtual bool LoadConfiguration(const std::string& configPath) = 0;
    virtual bool SaveConfiguration(const std::string& configPath) = 0;
    virtual bool ExportVRProfile(const std::string& profilePath) = 0;
    virtual bool ImportVRProfile(const std::string& profilePath) = 0;
    
    // ========================================================================
    // Event Handling and Callbacks
    // ========================================================================
    
    using ConversionProgressCallback = std::function<void(double progress, const std::string& step)>;
    using ConversionCompleteCallback = std::function<void(bool success, const std::string& message)>;
    using ErrorCallback = std::function<void(const std::string& error)>;
    
    virtual void RegisterProgressCallback(ConversionProgressCallback callback) = 0;
    virtual void RegisterCompleteCallback(ConversionCompleteCallback callback) = 0;
    virtual void RegisterErrorCallback(ErrorCallback callback) = 0;
    
    // ========================================================================
    // Advanced Features
    // ========================================================================
    
    virtual bool EnableFeature(const std::string& featureName) = 0;
    virtual bool DisableFeature(const std::string& featureName) = 0;
    virtual bool IsFeatureEnabled(const std::string& featureName) const = 0;
    virtual std::vector<std::string> GetAvailableFeatures() const = 0;
    
    // ========================================================================
    // Diagnostics and Debugging
    // ========================================================================
    
    virtual bool GenerateDiagnosticReport(const std::string& reportPath) = 0;
    virtual bool EnableDebugMode(bool enable) = 0;
    virtual std::string GetSystemLogs() const = 0;
    virtual bool ClearLogs() = 0;
};

// ============================================================================
// Factory and Creation Functions
// ============================================================================

std::unique_ptr<IVRConversionSystem> CreateVRConversionSystem();
std::unique_ptr<IVRConversionSystem> CreateVRConversionSystemWithConfig(const VRConfig& config);

// ============================================================================
// Utility Functions
// ============================================================================

std::string GameEngineToString(GameEngine engine);
std::string RenderingAPIToString(RenderingAPI api);
std::string VRHeadsetToString(VRHeadset headset);
std::string CollisionTypeToString(CollisionType type);
std::string OptimizationLevelToString(OptimizationLevel level);

GameEngine StringToGameEngine(const std::string& str);
RenderingAPI StringToRenderingAPI(const std::string& str);
VRHeadset StringToVRHeadset(const std::string& str);
CollisionType StringToCollisionType(const std::string& str);
OptimizationLevel StringToOptimizationLevel(const std::string& str);

// ============================================================================
// Constants and Default Values
// ============================================================================

namespace Constants {
    constexpr double DEFAULT_VR_REFRESH_RATE = 90.0;
    constexpr double DEFAULT_VR_FIELD_OF_VIEW = 110.0;
    constexpr double DEFAULT_TARGET_FRAME_RATE = 90.0;
    constexpr double DEFAULT_MAX_LATENCY = 11.0;
    constexpr double DEFAULT_COLLISION_ACCURACY = 0.95;
    constexpr double DEFAULT_DEPTH_ACCURACY = 0.90;
    
    constexpr size_t MAX_CONVERSION_STEPS = 100;
    constexpr size_t MAX_SUPPORTED_GAMES = 1000;
    constexpr size_t MAX_ENGINE_ADAPTERS = 20;
    
    constexpr std::chrono::milliseconds DEFAULT_CONVERSION_TIMEOUT{300000}; // 5 minutes
    constexpr std::chrono::milliseconds DEFAULT_ANALYSIS_TIMEOUT{60000};    // 1 minute
    constexpr std::chrono::milliseconds DEFAULT_OPTIMIZATION_TIMEOUT{120000}; // 2 minutes
}

} // namespace VR
} // namespace UEVRLCX

// ============================================================================
// C API Declarations
// ============================================================================

#ifdef __cplusplus
extern "C" {
#endif

// Core Conversion Functions
bool UEVRLCX_VR_Initialize();
bool UEVRLCX_VR_AnalyzeGame(const char* gamePath);
bool UEVRLCX_VR_ConvertGameToVR(const char* gamePath);
bool UEVRLCX_VR_StartConversion();
bool UEVRLCX_VR_StopConversion();
void UEVRLCX_VR_Shutdown();

// Status Functions
bool UEVRLCX_VR_IsConverting();
double UEVRLCX_VR_GetConversionProgress();
const char* UEVRLCX_VR_GetConversionStatus();
double UEVRLCX_VR_GetFrameRate();
double UEVRLCX_VR_GetLatency();

// Configuration Functions
bool UEVRLCX_VR_SetVRConfig(const char* configJson);
const char* UEVRLCX_VR_GetVRConfig();
bool UEVRLCX_VR_SetCollisionType(int collisionType);
bool UEVRLCX_VR_SetOptimizationLevel(int optimizationLevel);

// Performance Functions
bool UEVRLCX_VR_OptimizePerformance();
bool UEVRLCX_VR_EnableFoveatedRendering(bool enable);
bool UEVRLCX_VR_EnableAsyncTimewarp(bool enable);
bool UEVRLCX_VR_SetTargetFrameRate(double frameRate);

// Testing Functions
bool UEVRLCX_VR_RunConversionTests();
bool UEVRLCX_VR_ValidateConversion();
bool UEVRLCX_VR_TestPerformance();
bool UEVRLCX_VR_TestCompatibility();

// Configuration Functions
bool UEVRLCX_VR_LoadConfiguration(const char* configPath);
bool UEVRLCX_VR_SaveConfiguration(const char* configPath);
bool UEVRLCX_VR_ExportVRProfile(const char* profilePath);
bool UEVRLCX_VR_ImportVRProfile(const char* profilePath);

// Diagnostic Functions
bool UEVRLCX_VR_GenerateDiagnosticReport(const char* reportPath);
bool UEVRLCX_VR_EnableDebugMode(bool enable);
const char* UEVRLCX_VR_GetSystemLogs();
bool UEVRLCX_VR_ClearLogs();

#ifdef __cplusplus
}
#endif
