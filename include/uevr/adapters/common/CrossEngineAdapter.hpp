#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace uevr {
namespace Adapters {

// Forward declarations
class GameProfile;
class EngineHooks;
class VRModManager;

/**
 * @brief Base class for all cross-engine VR adapters
 * 
 * This class provides the interface that all game engine adapters
 * must implement to provide VR functionality.
 */
class CrossEngineAdapter {
public:
    /**
     * @brief Adapter state enumeration
     */
    enum class State {
        UNINITIALIZED,  ///< Adapter has not been initialized
        INITIALIZING,   ///< Adapter is currently initializing
        INITIALIZED,    ///< Adapter has been initialized successfully
        RUNNING,        ///< Adapter is running normally
        PAUSED,         ///< Adapter is paused
        ERROR,          ///< Adapter encountered an error
        SHUTDOWN        ///< Adapter is shutting down
    };

    /**
     * @brief Engine type enumeration
     */
    enum class EngineType {
        UNKNOWN,        ///< Unknown engine type
        UNREAL_ENGINE,  ///< Unreal Engine
        UNITY,          ///< Unity Engine
        MT_FRAMEWORK,   ///< MT Framework (Capcom)
        RE_ENGINE,      ///< RE Engine (Capcom)
        REDENGINE4,     ///< REDengine 4 (CD Projekt Red)
        CUSTOM          ///< Custom engine
    };

    /**
     * @brief VR mode enumeration
     */
    enum class VRMode {
        DISABLED,       ///< VR is disabled
        STEREO,         ///< Stereo rendering
        FULL_VR,        ///< Full VR with controllers
        PASSTHROUGH,    ///< AR passthrough mode
        MIXED_REALITY   ///< Mixed reality mode
    };

    /**
     * @brief Performance profile enumeration
     */
    enum class PerformanceProfile {
        LOW,            ///< Low performance, high quality
        MEDIUM,         ///< Balanced performance/quality
        HIGH,           ///< High performance, lower quality
        ULTRA           ///< Ultra performance, minimal quality
    };

    /**
     * @brief Adapter configuration structure
     */
    struct Config {
        std::string name;                           ///< Adapter name
        std::string version;                        ///< Adapter version
        std::string description;                    ///< Adapter description
        EngineType engine_type;                     ///< Supported engine type
        std::vector<std::string> supported_games;  ///< List of supported games
        bool auto_detect = true;                    ///< Auto-detect game
        bool enable_debug = false;                  ///< Enable debug mode
        PerformanceProfile performance_profile = PerformanceProfile::MEDIUM;
        VRMode default_vr_mode = VRMode::STEREO;
        std::unordered_map<std::string, std::string> engine_specific_settings;
    };

    /**
     * @brief VR statistics structure
     */
    struct VRStats {
        uint64_t total_frames = 0;
        uint64_t vr_frames = 0;
        float average_fps = 0.0f;
        float average_frame_time = 0.0f;
        float cpu_usage = 0.0f;
        float gpu_usage = 0.0f;
        float memory_usage = 0.0f;
        std::chrono::steady_clock::time_point last_update;
    };

    /**
     * @brief Constructor
     * @param config Adapter configuration
     */
    explicit CrossEngineAdapter(const Config& config);
    
    /**
     * @brief Destructor
     */
    virtual ~CrossEngineAdapter();

    // Disable copy constructor and assignment
    CrossEngineAdapter(const CrossEngineAdapter&) = delete;
    CrossEngineAdapter& operator=(const CrossEngineAdapter&) = delete;

    // Allow move constructor and assignment
    CrossEngineAdapter(CrossEngineAdapter&&) noexcept;
    CrossEngineAdapter& operator=(CrossEngineAdapter&&) noexcept;

    // Core lifecycle methods
    /**
     * @brief Initialize the adapter
     * @return true if initialization was successful
     */
    virtual bool initialize();

    /**
     * @brief Start the adapter
     * @return true if start was successful
     */
    virtual bool start();

    /**
     * @brief Stop the adapter
     */
    virtual void stop();

    /**
     * @brief Pause the adapter
     */
    virtual void pause();

    /**
     * @brief Resume the adapter
     */
    virtual void resume();

    /**
     * @brief Shutdown the adapter
     */
    virtual void shutdown();

    /**
     * @brief Cleanup adapter resources
     */
    virtual void cleanup();

    // Frame processing methods
    /**
     * @brief Process frame before rendering
     * @param delta_time Time since last frame
     */
    virtual void onFrameBegin(float delta_time);

    /**
     * @brief Process frame after rendering
     * @param delta_time Time since last frame
     */
    virtual void onFrameEnd(float delta_time);

    /**
     * @brief Process frame before present
     */
    virtual void onPrePresent();

    /**
     * @brief Process frame after present
     */
    virtual void onPostPresent();

    // VR-specific methods
    /**
     * @brief Initialize VR system
     * @return true if VR initialization was successful
     */
    virtual bool initializeVR();

    /**
     * @brief Shutdown VR system
     */
    virtual void shutdownVR();

    /**
     * @brief Update VR state
     * @param delta_time Time since last update
     */
    virtual void updateVR(float delta_time);

    /**
     * @brief Render VR content
     */
    virtual void renderVR();

    /**
     * @brief Handle VR input
     */
    virtual void handleVRInput();

    /**
     * @brief Update VR controllers
     */
    virtual void updateControllers();

    // Engine-specific methods
    /**
     * @brief Detect game engine
     * @return true if engine was detected
     */
    virtual bool detectEngine();

    /**
     * @brief Initialize engine hooks
     * @return true if hooks were initialized
     */
    virtual bool initializeEngineHooks();

    /**
     * @brief Remove engine hooks
     */
    virtual void removeEngineHooks();

    /**
     * @brief Process engine-specific data
     */
    virtual void processEngineData();

    // Game profile methods
    /**
     * @brief Load game profile
     * @param game_name Name of the game
     * @return true if profile was loaded
     */
    virtual bool loadGameProfile(const std::string& game_name);

    /**
     * @brief Save game profile
     * @return true if profile was saved
     */
    virtual bool saveGameProfile();

    /**
     * @brief Get current game profile
     * @return Pointer to current game profile
     */
    virtual GameProfile* getGameProfile() const;

    // Configuration methods
    /**
     * @brief Load configuration
     * @param config_path Path to configuration file
     * @return true if configuration was loaded
     */
    virtual bool loadConfig(const std::string& config_path);

    /**
     * @brief Save configuration
     * @param config_path Path to configuration file
     * @return true if configuration was saved
     */
    virtual bool saveConfig(const std::string& config_path);

    /**
     * @brief Reset configuration to defaults
     */
    virtual void resetConfig();

    /**
     * @brief Reload configuration
     */
    virtual void reloadConfig();

    // Settings management
    template<typename T>
    T getSetting(const std::string& key, const T& default_value = T{}) const;

    template<typename T>
    void setSetting(const std::string& key, const T& value);

    bool hasSetting(const std::string& key) const;
    void removeSetting(const std::string& key);

    // State queries
    State getState() const { return m_state; }
    bool isInitialized() const { return m_state >= State::INITIALIZED; }
    bool isRunning() const { return m_state == State::RUNNING; }
    bool isPaused() const { return m_state == State::PAUSED; }
    bool hasError() const { return m_state == State::ERROR; }
    bool isVRInitialized() const { return m_vr_initialized; }

    // Information queries
    const Config& getConfig() const { return m_config; }
    EngineType getEngineType() const { return m_config.engine_type; }
    VRMode getVRMode() const { return m_vr_mode; }
    PerformanceProfile getPerformanceProfile() const { return m_performance_profile; }
    const VRStats& getVRStats() const { return m_vr_stats; }

    // Engine information
    std::string getEngineName() const;
    std::string getEngineVersion() const;
    std::string getGameName() const;
    std::string getGameVersion() const;

    // Performance methods
    void setPerformanceProfile(PerformanceProfile profile);
    void setVRMode(VRMode mode);
    void enablePerformanceMonitoring(bool enable);
    void resetPerformanceStats();

    // Error handling
    const std::string& getLastError() const { return m_last_error; }
    void setLastError(const std::string& error);
    std::vector<std::string> getErrorHistory() const;

    // Debug methods
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const { return m_debug_mode; }
    void logDebug(const std::string& message);
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);

protected:
    // Protected methods for derived classes
    void setState(State state);
    void setVRInitialized(bool initialized);
    void updateVRStats(float delta_time);
    void updatePerformanceStats();

    // Engine integration
    bool registerWithEngine();
    void unregisterFromEngine();

    // VR integration
    bool registerWithVRSystem();
    void unregisterFromVRSystem();

    // Configuration validation
    bool validateConfig() const;
    bool validateGameProfile() const;

private:
    Config m_config;
    State m_state;
    VRMode m_vr_mode;
    PerformanceProfile m_performance_profile;
    bool m_vr_initialized;
    bool m_debug_mode;
    bool m_performance_monitoring_enabled;
    
    // Components
    std::unique_ptr<GameProfile> m_game_profile;
    std::unique_ptr<EngineHooks> m_engine_hooks;
    std::unique_ptr<VRModManager> m_vr_mod_manager;
    
    // State
    std::string m_last_error;
    std::vector<std::string> m_error_history;
    VRStats m_vr_stats;
    std::chrono::steady_clock::time_point m_start_time;
    std::chrono::steady_clock::time_point m_last_frame_time;
    
    // Internal methods
    void updateState();
    void updateErrorHistory(const std::string& error);
    void initializeDefaultSettings();
    void cleanupResources();
};

/**
 * @brief Factory interface for creating adapters
 */
class AdapterFactory {
public:
    /**
     * @brief Constructor
     */
    AdapterFactory();
    
    /**
     * @brief Destructor
     */
    virtual ~AdapterFactory();

    /**
     * @brief Create adapter for specific game
     * @param game_executable Path to game executable
     * @return Pointer to created adapter, or nullptr if creation failed
     */
    virtual std::shared_ptr<CrossEngineAdapter> createAdapter(const std::string& game_executable) = 0;

    /**
     * @brief Check if factory can create adapter for game
     * @param game_executable Path to game executable
     * @return true if factory can create adapter
     */
    virtual bool canCreateAdapter(const std::string& game_executable) const = 0;

    /**
     * @brief Get supported engine type
     * @return Engine type supported by this factory
     */
    virtual CrossEngineAdapter::EngineType getSupportedEngineType() const = 0;

    /**
     * @brief Get factory name
     * @return Factory name
     */
    virtual std::string getFactoryName() const = 0;

    /**
     * @brief Get factory version
     * @return Factory version
     */
    virtual std::string getFactoryVersion() const = 0;

    /**
     * @brief Get supported games
     * @return List of supported games
     */
    virtual std::vector<std::string> getSupportedGames() const = 0;

    /**
     * @brief Check if game is supported
     * @param game_name Name of the game
     * @return true if game is supported
     */
    virtual bool isGameSupported(const std::string& game_name) const;

    /**
     * @brief Get adapter configuration for game
     * @param game_name Name of the game
     * @return Adapter configuration, or default config if not found
     */
    virtual CrossEngineAdapter::Config getAdapterConfig(const std::string& game_name) const;

protected:
    // Protected methods for derived classes
    virtual CrossEngineAdapter::Config createDefaultConfig() const;
    virtual bool validateGameExecutable(const std::string& game_executable) const;
    virtual std::string detectGameName(const std::string& game_executable) const;
};

// Template implementations
template<typename T>
T CrossEngineAdapter::getSetting(const std::string& key, const T& default_value) const {
    // Implementation would depend on configuration system
    // For now, return default value
    return default_value;
}

template<typename T>
void CrossEngineAdapter::setSetting(const std::string& key, const T& value) {
    // Implementation would depend on configuration system
    // For now, do nothing
}

} // namespace Adapters
} // namespace uevr
