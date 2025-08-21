#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <mutex>
#include <atomic>
#include <filesystem>

// VR system includes
#include "../vr/VRSystem.hpp"
#include "../core/Framework.hpp"

namespace UEVR {
namespace Adapters {

// Forward declarations
class EngineDetector;
class HookManager;
class RenderingManager;
class InputManager;
class AudioManager;

// Engine types
enum class EngineType {
    UNKNOWN,
    UNREAL_ENGINE_4,
    UNREAL_ENGINE_5,
    UNITY,
    CRYENGINE,
    LUMBERYARD,
    GODOT,
    CUSTOM_ENGINE
};

// Engine versions
enum class EngineVersion {
    UNKNOWN_VERSION,
    UE4_20, UE4_21, UE4_22, UE4_23, UE4_24, UE4_25, UE4_26, UE4_27,
    UE5_0, UE5_1, UE5_2, UE5_3, UE5_4, UE5_5,
    UNITY_2019, UNITY_2020, UNITY_2021, UNITY_2022, UNITY_2023,
    CRYENGINE_5, CRYENGINE_6,
    LUMBERYARD_1_0, LUMBERYARD_1_1,
    GODOT_3, GODOT_4
};

// Engine capabilities
enum class EngineCapability {
    D3D11_RENDERING,
    D3D12_RENDERING,
    VULKAN_RENDERING,
    OPENGL_RENDERING,
    METAL_RENDERING,
    SOFTWARE_RENDERING,
    STEREO_RENDERING,
    VR_RENDERING,
    RAY_TRACING,
    COMPUTE_SHADERS,
    MULTI_THREADED_RENDERING,
    INSTANCED_RENDERING,
    INDIRECT_RENDERING,
    MESH_SHADERS,
    VARIABLE_RATE_SHADING
};

// Engine configuration
struct EngineConfig {
    EngineType engine_type;
    EngineVersion engine_version;
    std::string engine_name;
    std::string engine_path;
    std::string game_executable;
    std::string game_data_path;
    std::vector<EngineCapability> capabilities;
    std::map<std::string, std::string> engine_specific_settings;
    bool enable_debug_mode;
    bool enable_profiling;
    std::string log_level;
};

// Adapter configuration
struct AdapterConfig {
    std::string adapter_name;
    std::string adapter_version;
    std::string description;
    std::string author;
    std::string license;
    bool enable_vr_mode;
    bool enable_physics_integration;
    bool enable_audio_integration;
    bool enable_input_integration;
    bool enable_haptic_feedback;
    std::map<std::string, std::string> custom_settings;
};

// Adapter state
enum class AdapterState {
    UNINITIALIZED,
    INITIALIZING,
    INITIALIZED,
    RUNNING,
    PAUSED,
    SHUTTING_DOWN,
    ERROR
};

// Adapter performance metrics
struct AdapterPerformanceMetrics {
    float frame_time_ms;
    float cpu_time_ms;
    float gpu_time_ms;
    float vr_render_time_ms;
    float physics_update_time_ms;
    float input_processing_time_ms;
    float audio_processing_time_ms;
    uint64_t frame_count;
    uint64_t dropped_frames;
    float frame_rate;
    float target_frame_rate;
    std::chrono::steady_clock::time_point last_update;
};

// Adapter event types
enum class AdapterEventType {
    AdapterInitialized,
    AdapterStarted,
    AdapterStopped,
    AdapterPaused,
    AdapterResumed,
    EngineDetected,
    EngineInitialized,
    VRModeEnabled,
    VRModeDisabled,
    PerformanceAlert,
    ErrorOccurred,
    WarningIssued
};

// Adapter event data
struct AdapterEvent {
    AdapterEventType type;
    std::string source;
    std::string message;
    std::map<std::string, std::string> data;
    std::chrono::steady_clock::time_point timestamp;
    int severity_level;
};

// Adapter event listener
using AdapterEventListener = std::function<void(const AdapterEvent&)>;

// Base engine adapter class
class EngineAdapter {
public:
    // Constructor and destructor
    EngineAdapter(const std::string& name, const std::string& version);
    virtual ~EngineAdapter();
    
    // Lifecycle management
    virtual bool initialize(const EngineConfig& engine_config, 
                          const AdapterConfig& adapter_config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    virtual bool isRunning() const = 0;
    
    // Configuration
    virtual void setEngineConfig(const EngineConfig& config) = 0;
    virtual EngineConfig getEngineConfig() const = 0;
    virtual void setAdapterConfig(const AdapterConfig& config) = 0;
    virtual AdapterConfig getAdapterConfig() const = 0;
    
    // Engine detection and initialization
    virtual bool detectEngine() = 0;
    virtual bool initializeEngine() = 0;
    virtual bool isEngineDetected() const = 0;
    virtual bool isEngineInitialized() const = 0;
    virtual EngineType getDetectedEngineType() const = 0;
    virtual EngineVersion getDetectedEngineVersion() const = 0;
    
    // VR integration
    virtual bool enableVRMode() = 0;
    virtual bool disableVRMode() = 0;
    virtual bool isVRModeEnabled() const = 0;
    virtual bool initializeVRSystem() = 0;
    virtual bool shutdownVRSystem() = 0;
    
    // Rendering integration
    virtual bool initializeRendering() = 0;
    virtual bool shutdownRendering() = 0;
    virtual bool isRenderingInitialized() const = 0;
    virtual bool beginFrame() = 0;
    virtual bool endFrame() = 0;
    virtual bool presentFrame() = 0;
    
    // Physics integration
    virtual bool initializePhysics() = 0;
    virtual bool shutdownPhysics() = 0;
    virtual bool isPhysicsInitialized() const = 0;
    virtual bool updatePhysics(float delta_time) = 0;
    virtual bool syncPhysicsWithVR() = 0;
    
    // Input integration
    virtual bool initializeInput() = 0;
    virtual bool shutdownInput() = 0;
    virtual bool isInputInitialized() const = 0;
    virtual bool processInput() = 0;
    virtual bool syncInputWithVR() = 0;
    
    // Audio integration
    virtual bool initializeAudio() = 0;
    virtual bool shutdownAudio() = 0;
    virtual bool isAudioInitialized() const = 0;
    virtual bool updateAudio(float delta_time) = 0;
    virtual bool syncAudioWithVR() = 0;
    
    // Haptic feedback
    virtual bool initializeHaptics() = 0;
    virtual bool shutdownHaptics() = 0;
    virtual bool isHapticsInitialized() const = 0;
    virtual bool triggerHapticFeedback(const std::string& device_id, 
                                     float intensity, 
                                     float duration_ms) = 0;
    
    // System control
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void reset() = 0;
    
    // Update and rendering
    virtual void update(float delta_time) = 0;
    virtual void render() = 0;
    virtual void present() = 0;
    
    // Performance monitoring
    virtual void enablePerformanceMonitoring(bool enable) = 0;
    virtual bool isPerformanceMonitoringEnabled() const = 0;
    virtual AdapterPerformanceMetrics getPerformanceMetrics() const = 0;
    virtual void resetPerformanceMetrics() = 0;
    
    // Event system
    virtual void addEventListener(AdapterEventType event_type, AdapterEventListener listener) = 0;
    virtual void removeEventListener(AdapterEventType event_type, AdapterEventListener listener) = 0;
    virtual void dispatchEvent(const AdapterEvent& event) = 0;
    
    // Error handling
    virtual std::string getLastError() const = 0;
    virtual void clearErrors() = 0;
    virtual std::vector<std::string> getErrorHistory() const = 0;
    
    // Utility functions
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getLicense() const = 0;
    
    // Debug and profiling
    virtual void enableDebugMode(bool enable) = 0;
    virtual void enableProfiling(bool enable) = 0;
    virtual void setLogLevel(const std::string& level) = 0;
    
    // Configuration management
    virtual bool loadConfiguration(const std::string& config_file) = 0;
    virtual bool saveConfiguration(const std::string& config_file) = 0;
    virtual bool resetConfiguration() = 0;
    
    // Hot-reload support
    virtual bool supportsHotReload() const = 0;
    virtual bool reloadConfiguration() = 0;
    virtual bool reloadShaders() = 0;
    virtual bool reloadTextures() = 0;
    
    // Multi-threading support
    virtual bool supportsMultiThreading() const = 0;
    virtual void setThreadingMode(bool enable) = 0;
    virtual bool isMultiThreadingEnabled() const = 0;
    
    // Platform-specific functions
    virtual bool isPlatformSupported() const = 0;
    virtual std::string getPlatformInfo() const = 0;
    virtual bool initializePlatformSpecific() = 0;
    
protected:
    // Common adapter functionality
    bool initializeCommon(const EngineConfig& engine_config, 
                         const AdapterConfig& adapter_config);
    void shutdownCommon();
    void updateCommon(float delta_time);
    void logEvent(const AdapterEvent& event);
    void logError(const std::string& error);
    void updatePerformanceMetrics();
    
    // Configuration validation
    bool validateEngineConfig(const EngineConfig& config) const;
    bool validateAdapterConfig(const AdapterConfig& config) const;
    
    // Event dispatching
    void dispatchEventInternal(const AdapterEvent& event);
    
    // Performance tracking
    void recordFrameTime(float frame_time);
    void recordCPUTime(float cpu_time);
    void recordGPUTime(float gpu_time);
    
    // Internal state
    std::string m_name;
    std::string m_version;
    std::string m_description;
    std::string m_author;
    std::string m_license;
    
    AdapterState m_state;
    EngineConfig m_engine_config;
    AdapterConfig m_adapter_config;
    std::atomic<bool> m_initialized;
    std::atomic<bool> m_running;
    
    // Subsystems
    std::unique_ptr<EngineDetector> m_engine_detector;
    std::unique_ptr<HookManager> m_hook_manager;
    std::unique_ptr<RenderingManager> m_rendering_manager;
    std::unique_ptr<InputManager> m_input_manager;
    std::unique_ptr<AudioManager> m_audio_manager;
    
    // VR system integration
    std::shared_ptr<VR::VRSystem> m_vr_system;
    bool m_vr_mode_enabled;
    
    // Performance monitoring
    AdapterPerformanceMetrics m_performance_metrics;
    std::chrono::steady_clock::time_point m_last_frame_time;
    std::chrono::steady_clock::time_point m_last_performance_update;
    bool m_performance_monitoring_enabled;
    
    // Event system
    std::map<AdapterEventType, std::vector<AdapterEventListener>> m_event_listeners;
    
    // Error handling
    std::string m_last_error;
    std::vector<std::string> m_error_history;
    
    // Thread safety
    mutable std::mutex m_adapter_mutex;
    mutable std::mutex m_config_mutex;
    mutable std::mutex m_events_mutex;
    
    // Debug and profiling
    bool m_debug_mode_enabled;
    bool m_profiling_enabled;
    std::string m_log_level;
    
    // Helper methods
    virtual bool initializeSubsystems() = 0;
    virtual void shutdownSubsystems() = 0;
    virtual void updateSubsystems(float delta_time) = 0;
    virtual bool checkSystemRequirements() = 0;
    virtual void handleSystemEvents() = 0;
    
    // Engine-specific methods (to be implemented by derived classes)
    virtual bool detectEngineInternal() = 0;
    virtual bool initializeEngineInternal() = 0;
    virtual bool initializeRenderingInternal() = 0;
    virtual bool initializePhysicsInternal() = 0;
    virtual bool initializeInputInternal() = 0;
    virtual bool initializeAudioInternal() = 0;
    virtual bool initializeHapticsInternal() = 0;
    
    // Configuration helpers
    virtual bool loadEngineSpecificConfiguration() = 0;
    virtual bool saveEngineSpecificConfiguration() = 0;
    virtual bool validateEngineSpecificRequirements() = 0;
    
    // Performance helpers
    virtual void optimizeEnginePerformance() = 0;
    virtual void monitorEngineResources() = 0;
    virtual void adjustEngineParameters() = 0;
    
    // Error handling helpers
    virtual void handleEngineErrors() = 0;
    virtual void recoverFromErrors() = 0;
    virtual void reportEngineStatus() = 0;
};

// Engine adapter factory
class EngineAdapterFactory {
public:
    using AdapterCreator = std::function<std::unique_ptr<EngineAdapter>()>;
    
    static EngineAdapterFactory& getInstance();
    
    bool registerAdapter(const std::string& name, AdapterCreator creator);
    std::unique_ptr<EngineAdapter> createAdapter(const std::string& name);
    std::vector<std::string> getAvailableAdapters() const;
    bool isAdapterAvailable(const std::string& name) const;
    
private:
    EngineAdapterFactory() = default;
    ~EngineAdapterFactory() = default;
    EngineAdapterFactory(const EngineAdapterFactory&) = delete;
    EngineAdapterFactory& operator=(const EngineAdapterFactory&) = delete;
    
    std::map<std::string, AdapterCreator> m_adapters;
    mutable std::mutex m_factory_mutex;
};

// Adapter registration macro
#define REGISTER_ENGINE_ADAPTER(AdapterClass, AdapterName) \
    static bool g_##AdapterClass##_registered = \
        UEVR::Adapters::EngineAdapterFactory::getInstance().registerAdapter( \
            AdapterName, \
            []() -> std::unique_ptr<UEVR::Adapters::EngineAdapter> { \
                return std::make_unique<AdapterClass>(); \
            } \
        );

} // namespace Adapters
} // namespace UEVR
