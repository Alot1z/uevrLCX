#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>

namespace UEVR {

// Forward declarations
class Logger;
class ConfigurationManager;
class HookManager;
class VRSystem;

/**
 * @brief Unreal Engine version
 */
enum class UnrealEngineVersion {
    UNKNOWN,
    UE4_25,
    UE4_26,
    UE4_27,
    UE5_0,
    UE5_1,
    UE5_2,
    UE5_3,
    UE5_4
};

/**
 * @brief UObject information structure
 */
struct UObjectInfo {
    void* objectPtr;
    std::string name;
    std::string className;
    std::string packageName;
    bool isValid;
    
    UObjectInfo() : objectPtr(nullptr), isValid(false) {}
};

/**
 * @brief Camera information structure
 */
struct CameraInfo {
    std::array<float, 3> location;
    std::array<float, 3> rotation;
    float fieldOfView;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    bool isActive;
    
    CameraInfo() : fieldOfView(90.0f), aspectRatio(16.0f/9.0f), nearPlane(0.1f), farPlane(10000.0f), isActive(false) {
        location.fill(0.0f);
        rotation.fill(0.0f);
    }
};

/**
 * @brief Rendering information structure
 */
struct RenderingInfo {
    void* renderTargetPool;
    void* sceneRenderer;
    void* viewFamily;
    void* viewport;
    uint32_t renderWidth;
    uint32_t renderHeight;
    bool stereoRenderingEnabled;
    
    RenderingInfo() : renderTargetPool(nullptr), sceneRenderer(nullptr), viewFamily(nullptr),
                      viewport(nullptr), renderWidth(1920), renderHeight(1080), stereoRenderingEnabled(false) {}
};

/**
 * @brief Unreal Engine adapter class
 * 
 * Handles integration with Unreal Engine games
 */
class UnrealEngineAdapter {
public:
    /**
     * @brief Constructor
     */
    UnrealEngineAdapter();
    
    /**
     * @brief Destructor
     */
    ~UnrealEngineAdapter();
    
    /**
     * @brief Initialize the adapter
     * @param configManager Configuration manager instance
     * @param logger Logger instance
     * @param hookManager Hook manager instance
     * @param vrSystem VR system instance
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(std::shared_ptr<ConfigurationManager> configManager, 
                   std::shared_ptr<Logger> logger,
                   std::shared_ptr<HookManager> hookManager,
                   std::shared_ptr<VRSystem> vrSystem);
    
    /**
     * @brief Shutdown the adapter
     */
    void Shutdown();
    
    /**
     * @brief Check if Unreal Engine is detected
     * @return true if engine detected, false otherwise
     */
    bool IsEngineDetected() const;
    
    /**
     * @brief Get detected engine version
     * @return Unreal Engine version
     */
    UnrealEngineVersion GetEngineVersion() const { return m_engineVersion; }
    
    /**
     * @brief Get engine version as string
     * @return Engine version string
     */
    std::string GetEngineVersionString() const;
    
    /**
     * @brief Install engine hooks
     * @return true if hooks installed successfully, false otherwise
     */
    bool InstallHooks();
    
    /**
     * @brief Remove engine hooks
     */
    void RemoveHooks();
    
    /**
     * @brief Update adapter (called every frame)
     */
    void Update();
    
    /**
     * @brief Get current camera information
     * @return Camera information
     */
    CameraInfo GetCameraInfo() const;
    
    /**
     * @brief Set camera information
     * @param info Camera information
     * @return true if camera info set successfully, false otherwise
     */
    bool SetCameraInfo(const CameraInfo& info);
    
    /**
     * @brief Get rendering information
     * @return Rendering information
     */
    RenderingInfo GetRenderingInfo() const;
    
    /**
     * @brief Enable VR rendering
     * @param enabled true to enable VR rendering, false to disable
     * @return true if VR rendering state changed successfully, false otherwise
     */
    bool SetVRRenderingEnabled(bool enabled);
    
    /**
     * @brief Check if VR rendering is enabled
     * @return true if VR rendering is enabled, false otherwise
     */
    bool IsVRRenderingEnabled() const;
    
    /**
     * @brief Get UObject by name
     * @param name Object name
     * @return UObject information or nullptr if not found
     */
    std::shared_ptr<UObjectInfo> FindUObject(const std::string& name) const;
    
    /**
     * @brief Get UObjects by class name
     * @param className Class name
     * @return Vector of UObject information
     */
    std::vector<std::shared_ptr<UObjectInfo>> FindUObjectsByClass(const std::string& className) const;
    
    /**
     * @brief Get all UObjects
     * @return Vector of all UObject information
     */
    std::vector<std::shared_ptr<UObjectInfo>> GetAllUObjects() const;
    
    /**
     * @brief Execute console command
     * @param command Console command
     * @return true if command executed successfully, false otherwise
     */
    bool ExecuteConsoleCommand(const std::string& command);
    
    /**
     * @brief Get console variable value
     * @param varName Variable name
     * @return Variable value as string
     */
    std::string GetConsoleVariable(const std::string& varName) const;
    
    /**
     * @brief Set console variable value
     * @param varName Variable name
     * @param value Variable value
     * @return true if variable set successfully, false otherwise
     */
    bool SetConsoleVariable(const std::string& varName, const std::string& value);
    
    /**
     * @brief Get world context
     * @return World context pointer
     */
    void* GetWorldContext() const;
    
    /**
     * @brief Get game instance
     * @return Game instance pointer
     */
    void* GetGameInstance() const;
    
    /**
     * @brief Get player controller
     * @param playerIndex Player index (default 0)
     * @return Player controller pointer
     */
    void* GetPlayerController(int playerIndex = 0) const;
    
    /**
     * @brief Get player pawn
     * @param playerIndex Player index (default 0)
     * @return Player pawn pointer
     */
    void* GetPlayerPawn(int playerIndex = 0) const;
    
    /**
     * @brief Get game viewport client
     * @return Game viewport client pointer
     */
    void* GetGameViewportClient() const;
    
    /**
     * @brief Get engine instance
     * @return Engine instance pointer
     */
    void* GetEngineInstance() const;
    
    /**
     * @brief Hook UObject constructor
     * @param callback Callback function
     * @return true if hook installed successfully, false otherwise
     */
    bool HookUObjectConstructor(std::function<void(void*)> callback);
    
    /**
     * @brief Hook UObject destructor
     * @param callback Callback function
     * @return true if hook installed successfully, false otherwise
     */
    bool HookUObjectDestructor(std::function<void(void*)> callback);
    
    /**
     * @brief Hook render thread
     * @param callback Callback function
     * @return true if hook installed successfully, false otherwise
     */
    bool HookRenderThread(std::function<void()> callback);
    
    /**
     * @brief Hook game thread tick
     * @param callback Callback function
     * @return true if hook installed successfully, false otherwise
     */
    bool HookGameThreadTick(std::function<void(float)> callback);
    
    /**
     * @brief Hook viewport draw
     * @param callback Callback function
     * @return true if hook installed successfully, false otherwise
     */
    bool HookViewportDraw(std::function<void(void*)> callback);
    
    /**
     * @brief Get adapter statistics
     * @return Statistics as JSON string
     */
    std::string GetStatistics() const;
    
    /**
     * @brief Get adapter configuration
     * @return Configuration as JSON string
     */
    std::string GetConfiguration() const;
    
    /**
     * @brief Set adapter configuration
     * @param config Configuration as JSON string
     * @return true if configuration set successfully, false otherwise
     */
    bool SetConfiguration(const std::string& config);

private:
    // Member variables
    std::shared_ptr<ConfigurationManager> m_configManager;
    std::shared_ptr<Logger> m_logger;
    std::shared_ptr<HookManager> m_hookManager;
    std::shared_ptr<VRSystem> m_vrSystem;
    std::atomic<bool> m_initialized;
    UnrealEngineVersion m_engineVersion;
    
    // Engine detection
    std::atomic<bool> m_engineDetected;
    std::string m_enginePath;
    std::string m_gameExecutable;
    
    // Hook states
    std::atomic<bool> m_hooksInstalled;
    std::atomic<bool> m_vrRenderingEnabled;
    
    // Engine pointers
    void* m_gEngine;
    void* m_gWorld;
    void* m_gUObjectArray;
    void* m_gNames;
    
    // Rendering state
    mutable std::mutex m_renderingMutex;
    RenderingInfo m_renderingInfo;
    CameraInfo m_cameraInfo;
    
    // Callbacks
    std::function<void(void*)> m_uobjectConstructorCallback;
    std::function<void(void*)> m_uobjectDestructorCallback;
    std::function<void()> m_renderThreadCallback;
    std::function<void(float)> m_gameThreadTickCallback;
    std::function<void(void*)> m_viewportDrawCallback;
    
    // Internal methods
    bool DetectEngine();
    UnrealEngineVersion DetectEngineVersion();
    bool FindEnginePointers();
    bool InstallCoreHooks();
    bool InstallRenderingHooks();
    bool InstallUObjectHooks();
    void RemoveCoreHooks();
    void RemoveRenderingHooks();
    void RemoveUObjectHooks();
    void UpdateCameraInfo();
    void UpdateRenderingInfo();
    void LogAdapterEvent(const std::string& event, const std::string& details = "");
    
    // Hook functions (static)
    static void* UObjectConstructorHook(void* object, size_t size);
    static void UObjectDestructorHook(void* object);
    static void RenderThreadHook();
    static void GameThreadTickHook(float deltaTime);
    static void ViewportDrawHook(void* viewport);
    static void BeginRenderingHook(void* renderingParams);
    static void EndRenderingHook();
    
    // Original function pointers
    static void* (*OriginalUObjectConstructor)(void*, size_t);
    static void (*OriginalUObjectDestructor)(void*);
    static void (*OriginalRenderThread)();
    static void (*OriginalGameThreadTick)(float);
    static void (*OriginalViewportDraw)(void*);
    static void (*OriginalBeginRendering)(void*);
    static void (*OriginalEndRendering)();
    
    // Static instance for hook callbacks
    static UnrealEngineAdapter* s_instance;
    
    // Engine-specific helper methods
    std::string GetUObjectName(void* object) const;
    std::string GetUObjectClassName(void* object) const;
    bool IsValidUObject(void* object) const;
    void* FindUObjectGlobals() const;
    void* FindGNamesArray() const;
    void* FindGWorldPointer() const;
    void* FindGEnginePointer() const;
    
    // Pattern scanning for engine detection
    void* ScanForPattern(const std::vector<uint8_t>& pattern, const std::string& mask) const;
    void* ScanModuleForPattern(const std::string& moduleName, const std::vector<uint8_t>& pattern, const std::string& mask) const;
    
    // Configuration keys
    static constexpr const char* CONFIG_SECTION = "UnrealEngineAdapter";
    static constexpr const char* CONFIG_AUTO_DETECT = "AutoDetect";
    static constexpr const char* CONFIG_HOOK_UOBJECTS = "HookUObjects";
    static constexpr const char* CONFIG_HOOK_RENDERING = "HookRendering";
    static constexpr const char* CONFIG_ENABLE_VR_RENDERING = "EnableVRRendering";
    static constexpr const char* CONFIG_CAMERA_OVERRIDE = "CameraOverride";
};

} // namespace UEVR
