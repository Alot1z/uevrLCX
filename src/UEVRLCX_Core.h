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
// UEVRLCX Core System Header
// ============================================================================

namespace UEVRLCX {

// ============================================================================
// Forward Declarations
// ============================================================================

class Context7AIDatabase;
class DeepWikiConnector;
class VRSystem;
class HooksManager;
class ModManager;
class PerformanceOptimizer;
class SecurityFramework;
class TestingFramework;

// ============================================================================
// System State Enums
// ============================================================================

enum class SystemState {
    CREATED,
    INITIALIZING,
    RUNNING,
    STOPPED,
    ERROR,
    SHUTDOWN
};

enum class SecurityLevel {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

enum class PerformanceLevel {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA
};

enum class VRComfortLevel {
    COMFORTABLE,
    MODERATE,
    INTENSE,
    EXTREME
};

// ============================================================================
// Core Data Structures
// ============================================================================

struct PerformanceMetrics {
    double frameRate;
    double latency;
    double throughput;
    double cpuUsage;
    double gpuUsage;
    double memoryUsage;
    double improvementFactor;
    std::chrono::steady_clock::time_point timestamp;
    
    PerformanceMetrics() : frameRate(0.0), latency(0.0), throughput(0.0),
                          cpuUsage(0.0), gpuUsage(0.0), memoryUsage(0.0),
                          improvementFactor(1.0) {}
};

struct SecurityStatus {
    SecurityLevel currentLevel;
    bool isSecure;
    std::vector<std::string> activeThreats;
    std::vector<std::string> securityEvents;
    std::chrono::steady_clock::time_point lastScan;
    
    SecurityStatus() : currentLevel(SecurityLevel::HIGH), isSecure(true) {}
};

struct VRMetrics {
    double refreshRate;
    double resolution;
    double fieldOfView;
    VRComfortLevel comfortLevel;
    bool motionSicknessDetected;
    std::vector<double> trackingAccuracy;
    
    VRMetrics() : refreshRate(90.0), resolution(1.0), fieldOfView(110.0),
                  comfortLevel(VRComfortLevel::COMFORTABLE), motionSicknessDetected(false) {}
};

struct AIMetrics {
    double modelAccuracy;
    double inferenceTime;
    double trainingProgress;
    std::vector<std::string> activeModels;
    std::vector<std::string> optimizationResults;
    
    AIMetrics() : modelAccuracy(0.0), inferenceTime(0.0), trainingProgress(0.0) {}
};

// ============================================================================
// Configuration Structures
// ============================================================================

struct SystemConfig {
    // Performance Settings
    PerformanceLevel targetPerformanceLevel;
    double targetFrameRate;
    double maxLatency;
    bool enableAIOptimization;
    bool enableGPUAcceleration;
    
    // Security Settings
    SecurityLevel securityLevel;
    bool enableRealTimeScanning;
    bool enableThreatPrevention;
    std::vector<std::string> allowedProcesses;
    
    // VR Settings
    VRComfortLevel targetComfortLevel;
    bool enableMotionSicknessPrevention;
    bool enableComfortTunneling;
    double maxFieldOfView;
    
    // AI Settings
    bool enableContext7Integration;
    bool enableDeepWikiIntegration;
    bool enableRealTimeLearning;
    std::string aiModelPath;
    
    SystemConfig() : targetPerformanceLevel(PerformanceLevel::HIGH),
                     targetFrameRate(90.0), maxLatency(11.0),
                     enableAIOptimization(true), enableGPUAcceleration(true),
                     securityLevel(SecurityLevel::HIGH),
                     enableRealTimeScanning(true), enableThreatPrevention(true),
                     targetComfortLevel(VRComfortLevel::COMFORTABLE),
                     enableMotionSicknessPrevention(true),
                     enableComfortTunneling(true), maxFieldOfView(120.0),
                     enableContext7Integration(true),
                     enableDeepWikiIntegration(true),
                     enableRealTimeLearning(true) {}
};

// ============================================================================
// Event and Callback Structures
// ============================================================================

struct SystemEvent {
    enum class Type {
        INITIALIZATION_STARTED,
        INITIALIZATION_COMPLETED,
        SYSTEM_STARTED,
        SYSTEM_STOPPED,
        PERFORMANCE_OPTIMIZED,
        SECURITY_THREAT_DETECTED,
        VR_COMFORT_ALERT,
        AI_MODEL_UPDATED,
        KNOWLEDGE_BASE_UPDATED,
        ERROR_OCCURRED
    };
    
    Type type;
    std::string message;
    std::chrono::steady_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    SystemEvent(Type t, const std::string& msg) : type(t), message(msg) {
        timestamp = std::chrono::steady_clock::now();
    }
};

using SystemEventHandler = std::function<void(const SystemEvent&)>;
using PerformanceCallback = std::function<void(const PerformanceMetrics&)>;
using SecurityCallback = std::function<void(const SecurityStatus&)>;
using VRCallback = std::function<void(const VRMetrics&)>;
using AICallback = std::function<void(const AIMetrics&)>;

// ============================================================================
// Core System Interface
// ============================================================================

class IUEVRLCXCore {
public:
    virtual ~IUEVRLCXCore() = default;
    
    // ========================================================================
    // Core System Operations
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual void Shutdown() = 0;
    
    // ========================================================================
    // System Status
    // ========================================================================
    
    virtual SystemState GetSystemState() const = 0;
    virtual bool IsInitialized() const = 0;
    virtual bool IsRunning() const = 0;
    
    // ========================================================================
    // Performance and Optimization
    // ========================================================================
    
    virtual bool OptimizePerformance() = 0;
    virtual const PerformanceMetrics& GetPerformanceMetrics() const = 0;
    virtual bool SetPerformanceTarget(PerformanceLevel level) = 0;
    
    // ========================================================================
    // Security and Safety
    // ========================================================================
    
    virtual const SecurityStatus& GetSecurityStatus() const = 0;
    virtual bool SetSecurityLevel(SecurityLevel level) = 0;
    virtual bool PerformSecurityScan() = 0;
    
    // ========================================================================
    // VR System Management
    // ========================================================================
    
    virtual const VRMetrics& GetVRMetrics() const = 0;
    virtual bool SetVRComfortLevel(VRComfortLevel level) = 0;
    virtual bool EnableMotionSicknessPrevention(bool enable) = 0;
    
    // ========================================================================
    // AI and Knowledge Integration
    // ========================================================================
    
    virtual bool UpdateKnowledgeBase() = 0;
    virtual const AIMetrics& GetAIMetrics() const = 0;
    virtual bool TrainAIModel(const std::string& modelPath) = 0;
    
    // ========================================================================
    // Configuration Management
    // ========================================================================
    
    virtual bool LoadConfiguration(const std::string& configPath) = 0;
    virtual bool SaveConfiguration(const std::string& configPath) = 0;
    virtual const SystemConfig& GetConfiguration() const = 0;
    virtual bool UpdateConfiguration(const SystemConfig& config) = 0;
    
    // ========================================================================
    // Event Handling
    // ========================================================================
    
    virtual void RegisterEventHandler(SystemEvent::Type type, SystemEventHandler handler) = 0;
    virtual void UnregisterEventHandler(SystemEvent::Type type, SystemEventHandler handler) = 0;
    
    virtual void RegisterPerformanceCallback(PerformanceCallback callback) = 0;
    virtual void RegisterSecurityCallback(SecurityCallback callback) = 0;
    virtual void RegisterVRCallback(VRCallback callback) = 0;
    virtual void RegisterAICallback(AICallback callback) = 0;
    
    // ========================================================================
    // Advanced Features
    // ========================================================================
    
    virtual bool EnableFeature(const std::string& featureName) = 0;
    virtual bool DisableFeature(const std::string& featureName) = 0;
    virtual bool IsFeatureEnabled(const std::string& featureName) const = 0;
    
    virtual std::vector<std::string> GetAvailableFeatures() const = 0;
    virtual std::vector<std::string> GetEnabledFeatures() const = 0;
    
    // ========================================================================
    // Monitoring and Diagnostics
    // ========================================================================
    
    virtual bool StartMonitoring() = 0;
    virtual bool StopMonitoring() = 0;
    virtual bool IsMonitoring() const = 0;
    
    virtual bool GenerateDiagnosticReport(const std::string& reportPath) = 0;
    virtual std::string GetSystemHealth() const = 0;
    
    // ========================================================================
    // Plugin and Extension Management
    // ========================================================================
    
    virtual bool LoadPlugin(const std::string& pluginPath) = 0;
    virtual bool UnloadPlugin(const std::string& pluginName) = 0;
    virtual std::vector<std::string> GetLoadedPlugins() const = 0;
    
    virtual bool RegisterExtension(const std::string& name, void* extension) = 0;
    virtual void* GetExtension(const std::string& name) const = 0;
};

// ============================================================================
// Factory and Creation Functions
// ============================================================================

std::unique_ptr<IUEVRLCXCore> CreateUEVRLCXCore();
std::unique_ptr<IUEVRLCXCore> CreateUEVRLCXCoreWithConfig(const SystemConfig& config);

// ============================================================================
// Utility Functions
// ============================================================================

std::string SystemStateToString(SystemState state);
std::string SecurityLevelToString(SecurityLevel level);
std::string PerformanceLevelToString(PerformanceLevel level);
std::string VRComfortLevelToString(VRComfortLevel level);

SystemState StringToSystemState(const std::string& str);
SecurityLevel StringToSecurityLevel(const std::string& str);
PerformanceLevel StringToPerformanceLevel(const std::string& str);
VRComfortLevel StringToVRComfortLevel(const std::string& str);

// ============================================================================
// Constants and Default Values
// ============================================================================

namespace Constants {
    constexpr double DEFAULT_TARGET_FRAME_RATE = 90.0;
    constexpr double DEFAULT_MAX_LATENCY = 11.0;
    constexpr double DEFAULT_FIELD_OF_VIEW = 110.0;
    constexpr double DEFAULT_REFRESH_RATE = 90.0;
    
    constexpr size_t MAX_EVENT_HANDLERS = 100;
    constexpr size_t MAX_PLUGINS = 50;
    constexpr size_t MAX_EXTENSIONS = 100;
    
    constexpr std::chrono::milliseconds DEFAULT_MONITORING_INTERVAL{100};
    constexpr std::chrono::milliseconds DEFAULT_OPTIMIZATION_INTERVAL{1000};
    constexpr std::chrono::milliseconds DEFAULT_SECURITY_SCAN_INTERVAL{5000};
}

} // namespace UEVRLCX

// ============================================================================
// C API Declarations
// ============================================================================

#ifdef __cplusplus
extern "C" {
#endif

// Core System Functions
bool UEVRLCX_Initialize();
bool UEVRLCX_Start();
bool UEVRLCX_Stop();
void UEVRLCX_Shutdown();
bool UEVRLCX_IsRunning();

// Performance Functions
bool UEVRLCX_OptimizePerformance();
double UEVRLCX_GetFrameRate();
double UEVRLCX_GetLatency();
double UEVRLCX_GetPerformanceImprovement();

// Security Functions
bool UEVRLCX_PerformSecurityScan();
bool UEVRLCX_IsSecure();
const char* UEVRLCX_GetSecurityStatus();

// VR Functions
double UEVRLCX_GetVRRefreshRate();
bool UEVRLCX_IsMotionSicknessDetected();
const char* UEVRLCX_GetVRComfortLevel();

// AI Functions
bool UEVRLCX_UpdateKnowledgeBase();
double UEVRLCX_GetAIModelAccuracy();
const char* UEVRLCX_GetAIOptimizationResults();

// Configuration Functions
bool UEVRLCX_LoadConfiguration(const char* configPath);
bool UEVRLCX_SaveConfiguration(const char* configPath);
bool UEVRLCX_SetPerformanceTarget(int level);
bool UEVRLCX_SetSecurityLevel(int level);
bool UEVRLCX_SetVRComfortLevel(int level);

// Monitoring Functions
bool UEVRLCX_StartMonitoring();
bool UEVRLCX_StopMonitoring();
bool UEVRLCX_IsMonitoring();
bool UEVRLCX_GenerateDiagnosticReport(const char* reportPath);
const char* UEVRLCX_GetSystemHealth();

// Plugin Functions
bool UEVRLCX_LoadPlugin(const char* pluginPath);
bool UEVRLCX_UnloadPlugin(const char* pluginName);
const char* UEVRLCX_GetLoadedPlugins();

#ifdef __cplusplus
}
#endif
