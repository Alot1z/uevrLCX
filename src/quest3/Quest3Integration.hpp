#pragma once

#include <memory>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <queue>

namespace UEVR {

// Forward declarations
class Logger;
class ConfigurationManager;
class VRSystem;
class PerformanceMonitor;

/**
 * @brief Quest 3 connection state
 */
enum class Quest3ConnectionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    STREAMING,
    ERROR
};

/**
 * @brief Quest 3 streaming quality
 */
enum class Quest3StreamingQuality {
    LOW,        // 1440x1584 per eye, 72Hz
    MEDIUM,     // 1832x1920 per eye, 90Hz
    HIGH,       // 2064x2208 per eye, 90Hz
    ULTRA,      // 2208x2208 per eye, 120Hz
    CUSTOM      // User-defined settings
};

/**
 * @brief Quest 3 controller input
 */
struct Quest3ControllerInput {
    bool connected;
    
    // Buttons
    bool triggerPressed;
    bool gripPressed;
    bool menuPressed;
    bool systemPressed;
    bool aPressed;
    bool bPressed;
    bool xPressed;
    bool yPressed;
    bool thumbstickPressed;
    
    // Analog inputs
    float triggerValue;
    float gripValue;
    std::array<float, 2> thumbstick;
    
    // Hand tracking (if available)
    bool handTrackingActive;
    std::array<std::array<float, 3>, 26> handJoints; // 26 hand joints
    std::array<float, 26> handJointConfidence;
    
    Quest3ControllerInput() : connected(false), triggerPressed(false), gripPressed(false),
                              menuPressed(false), systemPressed(false), aPressed(false),
                              bPressed(false), xPressed(false), yPressed(false),
                              thumbstickPressed(false), triggerValue(0.0f), gripValue(0.0f),
                              handTrackingActive(false) {
        thumbstick.fill(0.0f);
        for (auto& joint : handJoints) {
            joint.fill(0.0f);
        }
        handJointConfidence.fill(0.0f);
    }
};

/**
 * @brief Quest 3 streaming statistics
 */
struct Quest3StreamingStats {
    uint32_t framesEncoded;
    uint32_t framesDropped;
    uint32_t framesSent;
    float averageFrameTime;
    float averageEncodeTime;
    float averageNetworkLatency;
    float averageBitrate;
    uint32_t networkPacketsLost;
    float compressionRatio;
    
    Quest3StreamingStats() : framesEncoded(0), framesDropped(0), framesSent(0),
                            averageFrameTime(0.0f), averageEncodeTime(0.0f),
                            averageNetworkLatency(0.0f), averageBitrate(0.0f),
                            networkPacketsLost(0), compressionRatio(0.0f) {}
};

/**
 * @brief Quest 3 display settings
 */
struct Quest3DisplaySettings {
    uint32_t renderWidth;
    uint32_t renderHeight;
    float refreshRate;
    float fieldOfView;
    float ipd; // Inter-pupillary distance
    bool foveatedRenderingEnabled;
    float foveatedRenderingLevel;
    bool asyncReprojectionEnabled;
    bool motionSmoothingEnabled;
    
    Quest3DisplaySettings() : renderWidth(2208), renderHeight(2208), refreshRate(90.0f),
                             fieldOfView(110.0f), ipd(0.064f), foveatedRenderingEnabled(true),
                             foveatedRenderingLevel(1.0f), asyncReprojectionEnabled(true),
                             motionSmoothingEnabled(true) {}
};

/**
 * @brief Quest 3 integration class
 * 
 * Handles Meta Quest 3 specific integration and streaming
 */
class Quest3Integration {
public:
    /**
     * @brief Constructor
     */
    Quest3Integration();
    
    /**
     * @brief Destructor
     */
    ~Quest3Integration();
    
    /**
     * @brief Initialize Quest 3 integration
     * @param configManager Configuration manager instance
     * @param logger Logger instance
     * @param vrSystem VR system instance
     * @param performanceMonitor Performance monitor instance
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(std::shared_ptr<ConfigurationManager> configManager,
                   std::shared_ptr<Logger> logger,
                   std::shared_ptr<VRSystem> vrSystem,
                   std::shared_ptr<PerformanceMonitor> performanceMonitor);
    
    /**
     * @brief Shutdown Quest 3 integration
     */
    void Shutdown();
    
    /**
     * @brief Check if Quest 3 is connected
     * @return true if Quest 3 is connected, false otherwise
     */
    bool IsConnected() const;
    
    /**
     * @brief Get connection state
     * @return Quest 3 connection state
     */
    Quest3ConnectionState GetConnectionState() const { return m_connectionState; }
    
    /**
     * @brief Start streaming to Quest 3
     * @param quality Streaming quality level
     * @return true if streaming started successfully, false otherwise
     */
    bool StartStreaming(Quest3StreamingQuality quality = Quest3StreamingQuality::HIGH);
    
    /**
     * @brief Stop streaming to Quest 3
     * @return true if streaming stopped successfully, false otherwise
     */
    bool StopStreaming();
    
    /**
     * @brief Check if streaming is active
     * @return true if streaming is active, false otherwise
     */
    bool IsStreaming() const;
    
    /**
     * @brief Update Quest 3 integration (called every frame)
     */
    void Update();
    
    /**
     * @brief Submit frame for streaming
     * @param eye Eye index (0 = left, 1 = right)
     * @param frameData Frame data pointer
     * @param width Frame width
     * @param height Frame height
     * @param format Frame format
     * @return true if frame submitted successfully, false otherwise
     */
    bool SubmitFrame(int eye, void* frameData, uint32_t width, uint32_t height, uint32_t format);
    
    /**
     * @brief Get controller input
     * @param controllerIndex Controller index (0 = left, 1 = right)
     * @return Controller input state
     */
    Quest3ControllerInput GetControllerInput(int controllerIndex) const;
    
    /**
     * @brief Send haptic feedback to controller
     * @param controllerIndex Controller index (0 = left, 1 = right)
     * @param amplitude Haptic amplitude (0.0 - 1.0)
     * @param duration Haptic duration in seconds
     * @return true if haptic feedback sent successfully, false otherwise
     */
    bool SendHapticFeedback(int controllerIndex, float amplitude, float duration);
    
    /**
     * @brief Get streaming statistics
     * @return Streaming statistics
     */
    Quest3StreamingStats GetStreamingStats() const;
    
    /**
     * @brief Get display settings
     * @return Display settings
     */
    Quest3DisplaySettings GetDisplaySettings() const;
    
    /**
     * @brief Set display settings
     * @param settings Display settings
     * @return true if settings applied successfully, false otherwise
     */
    bool SetDisplaySettings(const Quest3DisplaySettings& settings);
    
    /**
     * @brief Set streaming quality
     * @param quality Streaming quality level
     * @return true if quality set successfully, false otherwise
     */
    bool SetStreamingQuality(Quest3StreamingQuality quality);
    
    /**
     * @brief Get streaming quality
     * @return Current streaming quality level
     */
    Quest3StreamingQuality GetStreamingQuality() const { return m_streamingQuality; }
    
    /**
     * @brief Enable/disable foveated rendering
     * @param enabled true to enable, false to disable
     * @param level Foveation level (0.0 - 2.0)
     * @return true if foveated rendering state changed successfully, false otherwise
     */
    bool SetFoveatedRenderingEnabled(bool enabled, float level = 1.0f);
    
    /**
     * @brief Check if foveated rendering is enabled
     * @return true if foveated rendering is enabled, false otherwise
     */
    bool IsFoveatedRenderingEnabled() const;
    
    /**
     * @brief Enable/disable async reprojection
     * @param enabled true to enable, false to disable
     * @return true if async reprojection state changed successfully, false otherwise
     */
    bool SetAsyncReprojectionEnabled(bool enabled);
    
    /**
     * @brief Check if async reprojection is enabled
     * @return true if async reprojection is enabled, false otherwise
     */
    bool IsAsyncReprojectionEnabled() const;
    
    /**
     * @brief Set bitrate for streaming
     * @param bitrate Bitrate in Mbps
     * @return true if bitrate set successfully, false otherwise
     */
    bool SetStreamingBitrate(float bitrate);
    
    /**
     * @brief Get streaming bitrate
     * @return Current streaming bitrate in Mbps
     */
    float GetStreamingBitrate() const;
    
    /**
     * @brief Enable/disable adaptive bitrate
     * @param enabled true to enable, false to disable
     * @return true if adaptive bitrate state changed successfully, false otherwise
     */
    bool SetAdaptiveBitrateEnabled(bool enabled);
    
    /**
     * @brief Check if adaptive bitrate is enabled
     * @return true if adaptive bitrate is enabled, false otherwise
     */
    bool IsAdaptiveBitrateEnabled() const;
    
    /**
     * @brief Get Quest 3 device information
     * @return Device information as JSON string
     */
    std::string GetDeviceInfo() const;
    
    /**
     * @brief Get Quest 3 battery level
     * @return Battery level (0.0 - 1.0)
     */
    float GetBatteryLevel() const;
    
    /**
     * @brief Get Quest 3 temperature
     * @return Temperature in Celsius
     */
    float GetDeviceTemperature() const;
    
    /**
     * @brief Check if hand tracking is supported
     * @return true if hand tracking is supported, false otherwise
     */
    bool IsHandTrackingSupported() const;
    
    /**
     * @brief Enable/disable hand tracking
     * @param enabled true to enable, false to disable
     * @return true if hand tracking state changed successfully, false otherwise
     */
    bool SetHandTrackingEnabled(bool enabled);
    
    /**
     * @brief Check if hand tracking is enabled
     * @return true if hand tracking is enabled, false otherwise
     */
    bool IsHandTrackingEnabled() const;
    
    /**
     * @brief Set connection callback
     * @param callback Callback function for connection state changes
     */
    void SetConnectionCallback(std::function<void(Quest3ConnectionState)> callback) {
        m_connectionCallback = callback;
    }
    
    /**
     * @brief Set frame callback
     * @param callback Callback function for frame events
     */
    void SetFrameCallback(std::function<void(uint32_t, float)> callback) {
        m_frameCallback = callback;
    }

private:
    // Member variables
    std::shared_ptr<ConfigurationManager> m_configManager;
    std::shared_ptr<Logger> m_logger;
    std::shared_ptr<VRSystem> m_vrSystem;
    std::shared_ptr<PerformanceMonitor> m_performanceMonitor;
    std::atomic<bool> m_initialized;
    
    // Connection state
    std::atomic<Quest3ConnectionState> m_connectionState;
    std::atomic<bool> m_streaming;
    Quest3StreamingQuality m_streamingQuality;
    
    // Display settings
    mutable std::mutex m_displayMutex;
    Quest3DisplaySettings m_displaySettings;
    
    // Input state
    mutable std::mutex m_inputMutex;
    std::array<Quest3ControllerInput, 2> m_controllerInput;
    
    // Streaming
    std::thread m_streamingThread;
    std::atomic<bool> m_streamingThreadRunning;
    std::queue<std::pair<int, void*>> m_frameQueue;
    std::mutex m_frameQueueMutex;
    std::condition_variable m_frameCondition;
    
    // Statistics
    mutable std::mutex m_statsMutex;
    Quest3StreamingStats m_streamingStats;
    std::chrono::high_resolution_clock::time_point m_lastStatsUpdate;
    
    // Encoder
    void* m_nvencEncoder; // NVENC encoder instance
    void* m_encoderSession;
    
    // Network
    void* m_networkSocket;
    std::string m_quest3IPAddress;
    uint16_t m_streamingPort;
    
    // Callbacks
    std::function<void(Quest3ConnectionState)> m_connectionCallback;
    std::function<void(uint32_t, float)> m_frameCallback;
    
    // Internal methods
    bool InitializeEncoder();
    void ShutdownEncoder();
    bool InitializeNetwork();
    void ShutdownNetwork();
    void StreamingWorker();
    bool EncodeFrame(void* frameData, uint32_t width, uint32_t height, uint32_t format, std::vector<uint8_t>& encodedData);
    bool SendEncodedFrame(const std::vector<uint8_t>& encodedData, int eye);
    void UpdateControllerInput();
    void UpdateStreamingStats();
    void UpdateConnectionState();
    void LogQuest3Event(const std::string& event, const std::string& details = "");
    
    // Network protocol methods
    bool SendHandshake();
    bool SendDisplaySettings();
    bool ReceiveControllerInput();
    bool HandleNetworkMessage(const std::vector<uint8_t>& message);
    
    // Encoder configuration
    struct EncoderConfig {
        uint32_t width;
        uint32_t height;
        uint32_t bitrate;
        uint32_t fps;
        bool useHardwareEncoding;
        
        EncoderConfig() : width(2208), height(2208), bitrate(100000000), fps(90), useHardwareEncoding(true) {}
    } m_encoderConfig;
    
    // Configuration keys
    static constexpr const char* CONFIG_SECTION = "Quest3Integration";
    static constexpr const char* CONFIG_AUTO_CONNECT = "AutoConnect";
    static constexpr const char* CONFIG_STREAMING_QUALITY = "StreamingQuality";
    static constexpr const char* CONFIG_BITRATE = "Bitrate";
    static constexpr const char* CONFIG_ADAPTIVE_BITRATE = "AdaptiveBitrate";
    static constexpr const char* CONFIG_FOVEATED_RENDERING = "FoveatedRendering";
    static constexpr const char* CONFIG_ASYNC_REPROJECTION = "AsyncReprojection";
    static constexpr const char* CONFIG_HAND_TRACKING = "HandTracking";
    static constexpr const char* CONFIG_QUEST3_IP = "Quest3IPAddress";
    static constexpr const char* CONFIG_STREAMING_PORT = "StreamingPort";
};

} // namespace UEVR
