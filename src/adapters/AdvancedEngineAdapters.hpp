#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>

// Engine-specific includes
#include <UnrealEngine/Engine.h>
#include <Unity/UnityInterface.h>
#include <Godot/godot.h>

// VR-specific includes
#include "../vr/VRSystem.hpp"
#include "../vr/OpenXRIntegration.hpp"
#include "../vr/AdvancedPhysicsIntegration.hpp"
#include "../ai/AIPoweredVROptimizer.hpp"
#include "../neural/NeuralRenderingSystem.hpp"

namespace uevr {

enum class EngineType {
    UNREAL_ENGINE_4,
    UNREAL_ENGINE_5,
    UNITY_2019,
    UNITY_2020,
    UNITY_2021,
    UNITY_2022,
    GODOT_3,
    GODOT_4,
    CUSTOM_ENGINE,
    LEGACY_ENGINE,
    UNKNOWN_ENGINE
};

enum class AdapterCapability {
    STEREO_RENDERING,
    DEPTH_BUFFER_EXTRACTION,
    MOTION_CONTROLLERS,
    ROOM_SCALE_TRACKING,
    HAPTIC_FEEDBACK,
    EYE_TRACKING,
    FOVEATED_RENDERING,
    ASYNC_TIMEWARP,
    MULTI_RESOLUTION_SHADING,
    DYNAMIC_QUALITY_SCALING,
    NEURAL_RENDERING,
    AI_OPTIMIZATION,
    PHYSICS_INTEGRATION,
    COLLISION_DETECTION,
    INPUT_MAPPING,
    AUDIO_SPATIALIZATION
};

struct EngineSignature {
    std::string engineName;
    EngineType engineType;
    std::string version;
    std::string buildNumber;
    std::string rendererType;
    std::string graphicsAPI;
    
    // Detection patterns
    std::vector<std::string> executablePatterns;
    std::vector<std::string> dllPatterns;
    std::vector<std::string> registryKeys;
    std::vector<std::string> fileSignatures;
    
    // Capabilities
    std::vector<AdapterCapability> supportedCapabilities;
    float compatibilityScore;
    bool isVerified;
};

struct AdapterSettings {
    EngineType targetEngine;
    bool enableAutoDetection;
    bool enableFallbackMode;
    bool enablePerformanceMode;
    bool enableQualityMode;
    bool enableDebugMode;
    
    // Engine-specific settings
    struct UnrealSettings {
        bool enableBlueprintSupport;
        bool enableCppSupport;
        bool enablePluginSystem;
        bool enableCustomShaders;
        bool enableAdvancedRendering;
        std::string projectPath;
        std::string enginePath;
    } unrealSettings;
    
    struct UnitySettings {
        bool enableXRInteractionToolkit;
        bool enableSteamVRPlugin;
        bool enableOculusIntegration;
        bool enableCustomRenderPipeline;
        bool enableScriptableRenderPipeline;
        std::string projectPath;
        std::string packagesPath;
    } unitySettings;
    
    struct GodotSettings {
        bool enableOpenXRSupport;
        bool enableCustomRendering;
        bool enableAdvancedPhysics;
        bool enablePluginSystem;
        std::string projectPath;
        std::string enginePath;
    } godotSettings;
    
    struct LegacySettings {
        bool enable3DVisionSupport;
        bool enableTridefSupport;
        bool enableVorpxSupport;
        bool enableVireioSupport;
        bool enableCustomWrappers;
        std::string wrapperPath;
        std::string configPath;
    } legacySettings;
};

struct AdapterMetrics {
    float detectionTime;
    float initializationTime;
    float conversionTime;
    float performanceOverhead;
    float compatibilityScore;
    float stabilityScore;
    int successfulConversions;
    int failedConversions;
    float averageFrameRate;
    float averageLatency;
    
    // Engine-specific metrics
    std::unordered_map<std::string, float> engineMetrics;
    std::vector<std::string> detectedIssues;
    std::vector<std::string> appliedOptimizations;
};

class AdvancedEngineAdapters {
public:
    AdvancedEngineAdapters();
    ~AdvancedEngineAdapters();
    
    // Initialization and configuration
    bool initialize(const AdapterSettings& settings = AdapterSettings{});
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }
    
    void setAdapterSettings(const AdapterSettings& settings);
    AdapterSettings getAdapterSettings() const;
    
    // Engine detection and analysis
    EngineSignature detectEngine(const std::string& gamePath);
    std::vector<EngineSignature> detectAllEngines(const std::string& directory);
    bool validateEngineSignature(const EngineSignature& signature);
    float calculateCompatibilityScore(const EngineSignature& signature);
    
    // Adapter management
    bool loadEngineAdapter(const EngineSignature& signature);
    bool unloadEngineAdapter(const std::string& engineName);
    bool switchEngineAdapter(EngineType engineType);
    std::vector<EngineType> getSupportedEngines() const;
    
    // Engine-specific operations
    bool initializeUnrealEngine(const std::string& projectPath);
    bool initializeUnityEngine(const std::string& projectPath);
    bool initializeGodotEngine(const std::string& projectPath);
    bool initializeLegacyEngine(const std::string& wrapperPath);
    
    // VR conversion operations
    bool convertToVR(const std::string& gamePath, const EngineSignature& signature);
    bool injectVRSupport(const std::string& processName, const EngineSignature& signature);
    bool applyVROptimizations(const std::string& gamePath, const EngineSignature& signature);
    
    // Capability management
    std::vector<AdapterCapability> getSupportedCapabilities(const EngineSignature& signature);
    bool enableCapability(const std::string& engineName, AdapterCapability capability);
    bool disableCapability(const std::string& engineName, AdapterCapability capability);
    bool isCapabilitySupported(const std::string& engineName, AdapterCapability capability);
    
    // Performance optimization
    bool optimizeForEngine(const std::string& engineName);
    bool applyEngineSpecificOptimizations(const EngineSignature& signature);
    bool tunePerformanceSettings(const std::string& engineName, const std::string& setting, float value);
    
    // Real-time monitoring
    void startEngineMonitoring();
    void stopEngineMonitoring();
    bool isMonitoring() const { return m_monitoring.load(); }
    
    void updateMonitoring();
    void processEngineEvent(const std::string& eventType, const std::string& eventData);
    AdapterMetrics getEngineMetrics(const std::string& engineName) const;
    
    // Advanced features
    bool enableAutoOptimization(bool enable);
    bool enableAdaptiveScaling(bool enable);
    bool enableDynamicCapabilityManagement(bool enable);
    
    // Plugin and extension management
    bool loadEnginePlugin(const std::string& engineName, const std::string& pluginPath);
    bool unloadEnginePlugin(const std::string& engineName, const std::string& pluginName);
    std::vector<std::string> getLoadedPlugins(const std::string& engineName) const;
    
    // Configuration management
    bool saveEngineConfiguration(const std::string& engineName, const std::string& configPath);
    bool loadEngineConfiguration(const std::string& engineName, const std::string& configPath);
    bool exportEngineProfile(const std::string& engineName, const std::string& profilePath);
    bool importEngineProfile(const std::string& engineName, const std::string& profilePath);
    
    // Debug and diagnostics
    void enableDebugMode(bool enable);
    void setDebugCallback(std::function<void(const std::string&, const std::string&)> callback);
    std::vector<std::string> getDebugInfo(const std::string& engineName) const;
    
    // Performance monitoring
    struct AdapterPerformanceMetrics {
        float detectionAccuracy;
        float conversionSuccessRate;
        float performanceImprovement;
        float stabilityScore;
        float compatibilityScore;
        int totalConversions;
        int successfulConversions;
        float averageProcessingTime;
        float memoryUsage;
        float cpuUsage;
        float gpuUsage;
    };
    
    AdapterPerformanceMetrics getPerformanceMetrics() const;
    void enablePerformanceMonitoring(bool enable);
    void setPerformanceCallback(std::function<void(const AdapterPerformanceMetrics&)> callback);

private:
    // Engine-specific adapter implementations
    class UnrealEngineAdapter;
    std::unique_ptr<UnrealEngineAdapter> m_unrealAdapter;
    
    class UnityEngineAdapter;
    std::unique_ptr<UnityEngineAdapter> m_unityAdapter;
    
    class GodotEngineAdapter;
    std::unique_ptr<GodotEngineAdapter> m_godotAdapter;
    
    class LegacyEngineAdapter;
    std::unique_ptr<LegacyEngineAdapter> m_legacyAdapter;
    
    class CustomEngineAdapter;
    std::unique_ptr<CustomEngineAdapter> m_customAdapter;
    
    // Common members
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_monitoring{false};
    std::atomic<bool> m_shutdown{false};
    
    AdapterSettings m_settings;
    mutable std::mutex m_settingsMutex;
    
    std::unordered_map<std::string, EngineSignature> m_detectedEngines;
    mutable std::mutex m_enginesMutex;
    
    std::unordered_map<std::string, AdapterMetrics> m_engineMetrics;
    mutable std::mutex m_metricsMutex;
    
    std::unordered_map<std::string, std::vector<AdapterCapability>> m_engineCapabilities;
    mutable std::mutex m_capabilitiesMutex;
    
    std::unordered_map<std::string, std::vector<std::string>> m_loadedPlugins;
    mutable std::mutex m_pluginsMutex;
    
    AdapterPerformanceMetrics m_performanceMetrics;
    mutable std::mutex m_performanceMutex;
    bool m_performanceMonitoringEnabled{false};
    std::function<void(const AdapterPerformanceMetrics&)> m_performanceCallback;
    
    std::thread m_monitoringThread;
    std::condition_variable m_monitoringCV;
    std::mutex m_monitoringMutex;
    
    bool m_debugModeEnabled{false};
    std::function<void(const std::string&, const std::string&)> m_debugCallback;
    std::unordered_map<std::string, std::vector<std::string>> m_debugInfo;
    mutable std::mutex m_debugMutex;
    
    // Advanced features
    bool m_autoOptimizationEnabled{true};
    bool m_adaptiveScalingEnabled{true};
    bool m_dynamicCapabilityManagementEnabled{true};
    
    // Private methods
    void monitoringLoop();
    void updateMetrics();
    void processEngineDetection();
    void updateCapabilities();
    void optimizeEngines();
    bool initializeEngineAdapters();
    void cleanupEngineAdapters();
    void validateEngines();
    void updateEngineSignatures();
    void logAdapterError(const std::string& error);
    void saveEngineData();
    void loadEngineData();
    void clearEngineCache();
    void optimizeMemoryUsage();
};

} // namespace uevr
