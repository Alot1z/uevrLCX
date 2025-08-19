#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <chrono>

// Forward declarations for VR runtime interfaces
struct VRSystem;
struct VRCompositor;
struct VRSettings;

namespace uevr {

// Forward declarations
class ICrossEngineAdapter;
struct VRCompatibilityScore;

namespace vr {

/**
 * @brief Universal VR System for cross-engine VR implementation
 * 
 * This class provides a unified interface for VR functionality that works
 * across different game engines and graphics APIs. It abstracts away
 * engine-specific details and provides a consistent VR experience.
 */
class UniversalVRSystem {
public:
    /**
     * @brief Supported VR runtimes
     */
    enum class VRRuntime {
        OPENVR,     // SteamVR/OpenVR
        OPENXR,     // OpenXR standard
        OCULUS,     // Oculus SDK
        PICO,       // Pico SDK
        AUTO        // Auto-detect best runtime
    };

    /**
     * @brief Graphics API support
     */
    enum class GraphicsAPI {
        DIRECTX11,
        DIRECTX12,
        OPENGL,
        VULKAN,
        AUTO_DETECT
    };

    /**
     * @brief VR rendering modes
     */
    enum class RenderingMode {
        FORWARD,            // Forward rendering
        DEFERRED,           // Deferred rendering
        FORWARD_PLUS,       // Forward+ rendering
        CLUSTERED,          // Clustered rendering
        RAYTRACED,          // Ray-traced rendering
        HYBRID              // Hybrid techniques
    };

    /**
     * @brief VR comfort settings
     */
    enum class ComfortLevel {
        INTENSE,            // Full VR experience
        COMFORTABLE,        // Moderate comfort features
        GENTLE,            // Maximum comfort features
        CUSTOM             // User-defined settings
    };

    /**
     * @brief VR performance modes
     */
    enum class PerformanceMode {
        QUALITY,           // Maximum quality
        BALANCED,          // Balance quality/performance
        PERFORMANCE,       // Maximum performance
        ADAPTIVE,          // Dynamic adjustment
        BATTERY_SAVER      // Low power consumption
    };

    /**
     * @brief VR system configuration
     */
    struct VRConfig {
        VRRuntime runtime = VRRuntime::AUTO;
        GraphicsAPI graphics_api = GraphicsAPI::AUTO_DETECT;
        RenderingMode rendering_mode = RenderingMode::FORWARD;
        ComfortLevel comfort_level = ComfortLevel::COMFORTABLE;
        PerformanceMode performance_mode = PerformanceMode::BALANCED;
        
        // Display settings
        float render_scale = 1.0f;
        float world_scale = 1.0f;
        bool enable_foveated_rendering = true;
        bool enable_reprojection = true;
        bool enable_eye_tracking = false;
        bool enable_hand_tracking = false;
        
        // Comfort settings
        bool enable_vignetting = true;
        bool enable_snap_turn = false;
        bool enable_smooth_locomotion = true;
        float comfort_vignette_strength = 0.7f;
        float motion_smoothing = 0.3f;
        
        // Performance settings
        int target_framerate = 90;
        float adaptive_quality_min = 0.5f;
        float adaptive_quality_max = 1.5f;
        bool enable_dynamic_resolution = true;
        bool enable_async_timewarp = true;
        
        // Debug settings
        bool show_debug_info = false;
        bool enable_profiling = false;
        bool log_frame_times = false;
    };

    /**
     * @brief VR system state information
     */
    struct VRState {
        bool is_initialized = false;
        bool is_hmd_connected = false;
        bool is_tracking = false;
        bool is_rendering = false;
        VRRuntime active_runtime = VRRuntime::AUTO;
        GraphicsAPI active_graphics_api = GraphicsAPI::AUTO_DETECT;
        
        // Performance metrics
        float current_fps = 0.0f;
        float average_frame_time = 0.0f;
        float render_scale = 1.0f;
        int dropped_frames = 0;
        int total_frames = 0;
        
        // Hardware info
        std::string hmd_name;
        std::string runtime_version;
        std::map<std::string, std::string> device_properties;
        
        // Tracking state
        bool head_tracking = false;
        bool controller_tracking[2] = {false, false};
        bool eye_tracking = false;
        bool hand_tracking = false;
    };

    /**
     * @brief VR pose data
     */
    struct VRPose {
        float position[3] = {0.0f, 0.0f, 0.0f};
        float rotation[4] = {0.0f, 0.0f, 0.0f, 1.0f}; // Quaternion
        float velocity[3] = {0.0f, 0.0f, 0.0f};
        float angular_velocity[3] = {0.0f, 0.0f, 0.0f};
        bool is_valid = false;
        std::chrono::system_clock::time_point timestamp;
    };

    /**
     * @brief Eye tracking data
     */
    struct EyeTrackingData {
        float gaze_direction[2][3]; // Left and right eye gaze directions
        float gaze_origin[2][3];    // Left and right eye origins
        float pupil_diameter[2];    // Left and right pupil diameters
        float convergence_distance; // Distance to convergence point
        bool is_valid = false;
        std::chrono::system_clock::time_point timestamp;
    };

public:
    UniversalVRSystem();
    ~UniversalVRSystem();

    // Disable copy constructor and assignment
    UniversalVRSystem(const UniversalVRSystem&) = delete;
    UniversalVRSystem& operator=(const UniversalVRSystem&) = delete;

    /**
     * @brief Initialize the VR system
     * @param config VR configuration
     * @param adapter Game engine adapter
     * @return true if initialization successful
     */
    bool initialize(const VRConfig& config, ICrossEngineAdapter* adapter);

    /**
     * @brief Shutdown the VR system
     */
    void shutdown();

    /**
     * @brief Check if VR system is ready
     * @return true if initialized and HMD connected
     */
    bool isReady() const;

    /**
     * @brief Enable VR mode
     * @return true if VR enabled successfully
     */
    bool enableVR();

    /**
     * @brief Disable VR mode
     * @return true if VR disabled successfully
     */
    bool disableVR();

    /**
     * @brief Check if VR is currently enabled
     * @return true if VR is active
     */
    bool isVREnabled() const;

    /**
     * @brief Update VR system (call each frame)
     * @param delta_time Time since last update
     */
    void update(float delta_time);

    /**
     * @brief Begin VR frame rendering
     * @param frame_index Current frame index
     * @return true if frame setup successful
     */
    bool beginFrame(uint64_t frame_index);

    /**
     * @brief End VR frame rendering and submit to compositor
     * @return true if frame submission successful
     */
    bool endFrame();

    /**
     * @brief Get VR system state
     * @return Current VR state
     */
    VRState getState() const;

    /**
     * @brief Get VR configuration
     * @return Current VR configuration
     */
    VRConfig getConfig() const;

    /**
     * @brief Update VR configuration
     * @param config New configuration
     * @return true if configuration applied successfully
     */
    bool setConfig(const VRConfig& config);

    /**
     * @brief Get HMD pose
     * @return Head-mounted display pose
     */
    VRPose getHMDPose() const;

    /**
     * @brief Get controller pose
     * @param controller_index Controller index (0 or 1)
     * @return Controller pose
     */
    VRPose getControllerPose(int controller_index) const;

    /**
     * @brief Get eye tracking data
     * @return Eye tracking information
     */
    EyeTrackingData getEyeTrackingData() const;

    /**
     * @brief Get projection matrix for eye
     * @param eye_index Eye index (0 = left, 1 = right)
     * @param near_plane Near clipping plane
     * @param far_plane Far clipping plane
     * @return 4x4 projection matrix
     */
    std::vector<float> getProjectionMatrix(int eye_index, float near_plane, float far_plane) const;

    /**
     * @brief Get view matrix for eye
     * @param eye_index Eye index (0 = left, 1 = right)
     * @return 4x4 view matrix
     */
    std::vector<float> getViewMatrix(int eye_index) const;

    /**
     * @brief Get render target size for eye
     * @param eye_index Eye index (0 = left, 1 = right)
     * @return Width and height in pixels
     */
    std::pair<uint32_t, uint32_t> getRenderTargetSize(int eye_index) const;

    /**
     * @brief Submit rendered frame for eye
     * @param eye_index Eye index (0 = left, 1 = right)
     * @param texture_handle Graphics API texture handle
     * @return true if submission successful
     */
    bool submitFrame(int eye_index, void* texture_handle);

    /**
     * @brief Trigger haptic feedback on controller
     * @param controller_index Controller index (0 or 1)
     * @param duration_seconds Duration of haptic pulse
     * @param frequency Frequency of vibration
     * @param amplitude Amplitude of vibration (0.0-1.0)
     * @return true if haptic triggered successfully
     */
    bool triggerHaptic(int controller_index, float duration_seconds, float frequency, float amplitude);

    /**
     * @brief Register VR event callback
     * @param event_type Type of VR event
     * @param callback Callback function
     */
    void registerEventCallback(const std::string& event_type, std::function<void(const std::map<std::string, std::string>&)> callback);

    /**
     * @brief Unregister VR event callback
     * @param event_type Type of VR event
     */
    void unregisterEventCallback(const std::string& event_type);

    /**
     * @brief Recenter VR tracking
     */
    void recenterTracking();

    /**
     * @brief Reset VR pose to origin
     */
    void resetPose();

    /**
     * @brief Check VR compatibility with current system
     * @return Compatibility score and information
     */
    VRCompatibilityScore checkCompatibility() const;

    /**
     * @brief Get available VR runtimes
     * @return Map of runtime names to availability status
     */
    std::map<std::string, bool> getAvailableRuntimes() const;

    /**
     * @brief Switch VR runtime
     * @param runtime New runtime to use
     * @return true if runtime switched successfully
     */
    bool switchRuntime(VRRuntime runtime);

    /**
     * @brief Get performance statistics
     * @return Map of performance metrics
     */
    std::map<std::string, float> getPerformanceStats() const;

    /**
     * @brief Enable performance profiling
     * @param enable true to enable profiling
     */
    void setProfilingEnabled(bool enable);

    /**
     * @brief Export VR session data
     * @param format Export format ("json", "csv")
     * @return Exported session data
     */
    std::string exportSessionData(const std::string& format = "json") const;

    /**
     * @brief Set VR application properties
     * @param properties Application metadata
     */
    void setApplicationProperties(const std::map<std::string, std::string>& properties);

    /**
     * @brief Get recommended render scale based on performance
     * @return Optimal render scale value
     */
    float getRecommendedRenderScale() const;

    /**
     * @brief Apply comfort settings for motion sickness reduction
     * @param intensity Motion intensity (0.0 = calm, 1.0 = intense)
     */
    void applyComfortSettings(float intensity);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    // Internal VR management
    bool initializeRuntime();
    void shutdownRuntime();
    bool setupRenderTargets();
    void cleanupRenderTargets();
    void updatePoses();
    void updatePerformanceMetrics();
    void processVREvents();
    
    // Advanced pose management
    void updateSimulatedPoses();
    void updatePosesFromOpenVR();
    void updatePosesFromOpenXR();
    void applyWorldTransforms();
    void updatePosePrediction();
    
    // Comfort and accessibility
    void applyVignetting(float intensity);
    void applySmoothLocomotion();
    void adjustWorldScale();
    
    // Performance optimization
    void updateAdaptiveQuality();
    void adjustRenderScale();
    bool shouldDropFrame() const;
    
    // Runtime-specific implementations
    bool initializeOpenVR();
    bool initializeOpenXR();
    void shutdownOpenVR();
    void shutdownOpenXR();
};

} // namespace vr
} // namespace uevr