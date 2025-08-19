#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace uevr {
namespace MTFramework {

/**
 * @brief MT Framework specific types and constants
 * 
 * This header defines types and constants specific to the MT Framework engine
 * that are used throughout the adapter implementation.
 * 
 * @version 1.0.0
 * @author uevr Cross-Engine Adapter Project
 */

// ===== Engine Constants =====

/**
 * @brief MT Framework engine identifier
 */
constexpr const char* MT_FRAMEWORK_ENGINE_ID = "MT-Framework";

/**
 * @brief Supported game identifiers
 */
namespace Games {
    constexpr const char* MONSTER_HUNTER_WORLD = "MonsterHunterWorld";
    constexpr const char* DEVIL_MAY_CRY_5 = "DevilMayCry5";
    constexpr const char* RESIDENT_EVIL_6 = "ResidentEvil6";
    constexpr const char* DRAGONS_DOGMA = "DragonsDogma";
}

// ===== Rendering Constants =====

/**
 * @brief Default VR FOV multiplier
 */
constexpr float DEFAULT_VR_FOV_MULTIPLIER = 1.0f;

/**
 * @brief Default camera offset (eye level height)
 */
constexpr float DEFAULT_CAMERA_OFFSET_Y = 1.6f;

/**
 * @brief Maximum FOV multiplier
 */
constexpr float MAX_FOV_MULTIPLIER = 2.0f;

/**
 * @brief Minimum FOV multiplier
 */
constexpr float MIN_FOV_MULTIPLIER = 0.5f;

// ===== Performance Constants =====

/**
 * @brief Target VR frame rate
 */
constexpr double TARGET_VR_FPS = 90.0;

/**
 * @brief Target frame time in milliseconds
 */
constexpr double TARGET_FRAME_TIME_MS = 1000.0 / TARGET_VR_FPS;

/**
 * @brief Performance warning threshold
 */
constexpr double PERFORMANCE_WARNING_THRESHOLD_MS = 16.67; // 60 FPS

// ===== Hook Constants =====

/**
 * @brief DirectX function names for hooking
 */
namespace DirectXHooks {
    constexpr const char* PRESENT = "Present";
    constexpr const char* RESIZE_BUFFERS = "ResizeBuffers";
    constexpr const char* CREATE_SWAP_CHAIN = "CreateSwapChain";
    constexpr const char* CREATE_DEVICE = "CreateDevice";
}

/**
 * @brief MT Framework function names for hooking
 */
namespace MTFrameworkHooks {
    constexpr const char* RENDER_FRAME = "RenderFrame";
    constexpr const char* UPDATE_CAMERA = "UpdateCamera";
    constexpr const char* PROCESS_INPUT = "ProcessInput";
    constexpr const char* UPDATE_GAME_STATE = "UpdateGameState";
}

// ===== Configuration Keys =====

/**
 * @brief Configuration file keys
 */
namespace ConfigKeys {
    constexpr const char* VR_ENABLED = "vr_enabled";
    constexpr const char* FIRST_PERSON_MODE = "first_person_mode";
    constexpr const char* FOV_MULTIPLIER = "fov_multiplier";
    constexpr const char* CAMERA_OFFSET_X = "camera_offset_x";
    constexpr const char* CAMERA_OFFSET_Y = "camera_offset_y";
    constexpr const char* CAMERA_OFFSET_Z = "camera_offset_z";
    constexpr const char* DEBUG_MODE = "debug_mode";
    constexpr const char* PERFORMANCE_MONITORING = "performance_monitoring";
    constexpr const char* RENDERING_HOOKS = "rendering_hooks";
    constexpr const char* INPUT_HOOKS = "input_hooks";
    constexpr const char* CAMERA_HOOKS = "camera_hooks";
}

// ===== Error Codes =====

/**
 * @brief Error codes for MT Framework operations
 */
enum class MTFrameworkError {
    SUCCESS = 0,
    INITIALIZATION_FAILED = 1,
    MODULE_NOT_FOUND = 2,
    FUNCTION_NOT_FOUND = 3,
    HOOK_INSTALLATION_FAILED = 4,
    RENDERER_NOT_AVAILABLE = 5,
    CAMERA_NOT_AVAILABLE = 6,
    INPUT_NOT_AVAILABLE = 7,
    CONFIGURATION_LOAD_FAILED = 8,
    VR_INITIALIZATION_FAILED = 9,
    PERFORMANCE_CRITICAL = 10,
    UNKNOWN_ERROR = 255
};

/**
 * @brief Convert error code to string
 * @param error Error code
 * @return Error description string
 */
std::string getErrorString(MTFrameworkError error);

// ===== Data Structures =====

/**
 * @brief MT Framework renderer information
 */
struct RendererInfo {
    std::string rendererType;
    std::string apiVersion;
    uint32_t maxTextureSize;
    uint32_t maxAnisotropy;
    bool supportsVR;
    bool supportsStereoRendering;
};

/**
 * @brief MT Framework camera information
 */
struct CameraInfo {
    float position[3];
    float rotation[3];
    float fov;
    float nearPlane;
    float farPlane;
    bool isOrthographic;
    bool isFirstPerson;
};

/**
 * @brief MT Framework input information
 */
struct InputInfo {
    bool keyboardEnabled;
    bool mouseEnabled;
    bool gamepadEnabled;
    bool touchEnabled;
    uint32_t maxGamepads;
    uint32_t maxTouchPoints;
};

/**
 * @brief Performance metrics
 */
struct PerformanceMetrics {
    double frameTime;
    double averageFrameTime;
    double fps;
    double cpuUsage;
    double gpuUsage;
    double memoryUsage;
    uint32_t drawCalls;
    uint32_t triangles;
    uint32_t vertices;
};

/**
 * @brief VR configuration
 */
struct VRConfig {
    bool enabled;
    bool firstPersonMode;
    float fovMultiplier;
    float cameraOffset[3];
    bool hapticFeedback;
    bool motionControllers;
    std::string vrRuntime;
    float ipd;
    float worldScale;
};

// ===== Utility Functions =====

/**
 * @brief Check if a game is supported
 * @param gameName Name of the game to check
 * @return true if the game is supported
 */
bool isGameSupported(const std::string& gameName);

/**
 * @brief Get supported games list
 * @return Vector of supported game names
 */
std::vector<std::string> getSupportedGames();

/**
 * @brief Validate configuration values
 * @param key Configuration key
 * @param value Configuration value
 * @return true if the value is valid
 */
bool validateConfigValue(const std::string& key, const std::string& value);

/**
 * @brief Get default configuration value
 * @param key Configuration key
 * @return Default value for the key
 */
std::string getDefaultConfigValue(const std::string& key);

} // namespace MTFramework
} // namespace uevr
