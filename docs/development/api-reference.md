# API Reference

This document provides comprehensive API reference documentation for UEVR, including all public interfaces, classes, methods, and data structures.

## Core Framework API

### Framework Class
The main entry point for UEVR functionality.

```cpp
class Framework {
public:
    // Singleton access
    static Framework& Instance();
    
    // Core lifecycle
    bool Initialize(const FrameworkConfig& config = FrameworkConfig{});
    void Shutdown();
    bool IsInitialized() const;
    
    // Configuration management
    bool LoadConfiguration(const std::string& configPath);
    bool SaveConfiguration(const std::string& configPath) const;
    const FrameworkConfig& GetConfiguration() const;
    
    // Component access
    HookManager* GetHookManager() const;
    ModManager* GetModManager() const;
    VRManager* GetVRManager() const;
    PerformanceMonitor* GetPerformanceMonitor() const;
    
    // Event system
    EventDispatcher& GetEventDispatcher();
    void RegisterEventHandler(EventType type, EventHandler handler);
    void UnregisterEventHandler(EventType type, EventHandler handler);
    
    // Utility methods
    std::string GetVersion() const;
    std::string GetBuildInfo() const;
    void LogMessage(LogLevel level, const std::string& message);
    
private:
    Framework() = default;
    ~Framework() = default;
    Framework(const Framework&) = delete;
    Framework& operator=(const Framework&) = delete;
    
    std::unique_ptr<HookManager> hookManager;
    std::unique_ptr<ModManager> modManager;
    std::unique_ptr<VRManager> vrManager;
    std::unique_ptr<PerformanceMonitor> performanceMonitor;
    EventDispatcher eventDispatcher;
    FrameworkConfig config;
};
```

### Framework Configuration
Configuration structure for framework initialization.

```cpp
struct FrameworkConfig {
    // Logging configuration
    LogLevel logLevel = LogLevel::Info;
    std::string logFilePath = "uevr.log";
    bool enableConsoleLogging = true;
    bool enableFileLogging = true;
    
    // Performance configuration
    bool enablePerformanceMonitoring = true;
    bool enableProfiling = false;
    float targetFrameRate = 90.0f;
    
    // VR configuration
    bool enableVR = true;
    VRConfig vrConfig;
    
    // Hook configuration
    bool enableHooking = true;
    HookConfig hookConfig;
    
    // Mod configuration
    bool enableMods = true;
    std::string modsDirectory = "mods/";
    
    // Threading configuration
    uint32_t maxWorkerThreads = 4;
    bool enableMultithreading = true;
};
```

## Hook Management API

### Hook Manager
Manages the installation and management of function hooks.

```cpp
class HookManager {
public:
    // Hook installation
    bool InstallHook(const std::string& functionName, void* targetFunction, void* hookFunction);
    bool InstallHook(const std::string& functionName, const HookSignature& signature);
    bool RemoveHook(const std::string& functionName);
    bool RemoveAllHooks();
    
    // Hook management
    bool IsHookInstalled(const std::string& functionName) const;
    std::vector<std::string> GetInstalledHooks() const;
    HookStatus GetHookStatus(const std::string& functionName) const;
    
    // Hook execution
    bool EnableHook(const std::string& functionName);
    bool DisableHook(const std::string& functionName);
    bool IsHookEnabled(const std::string& functionName) const;
    
    // Hook information
    HookInfo GetHookInfo(const std::string& functionName) const;
    std::vector<HookInfo> GetAllHookInfo() const;
    
    // Safety features
    bool ValidateHook(const std::string& functionName) const;
    bool BackupOriginalFunction(const std::string& functionName);
    bool RestoreOriginalFunction(const std::string& functionName);
    
private:
    std::unordered_map<std::string, std::unique_ptr<Hook>> hooks;
    std::unordered_map<std::string, void*> originalFunctions;
    std::mutex hooksMutex;
};
```

### Hook Class
Represents an individual function hook.

```cpp
class Hook {
public:
    // Construction
    Hook(const std::string& name, void* target, void* hook);
    ~Hook();
    
    // Hook management
    bool Install();
    bool Remove();
    bool IsInstalled() const;
    
    // Hook execution
    bool Enable();
    bool Disable();
    bool IsEnabled() const;
    
    // Hook information
    std::string GetName() const;
    void* GetTargetFunction() const;
    void* GetHookFunction() const;
    HookStatus GetStatus() const;
    
    // Safety features
    bool Validate() const;
    bool BackupOriginal();
    bool RestoreOriginal();
    
private:
    std::string name;
    void* targetFunction;
    void* hookFunction;
    void* originalFunction;
    bool installed;
    bool enabled;
    HookStatus status;
};
```

### Hook Information Structures
```cpp
struct HookSignature {
    std::string functionName;
    std::string moduleName;
    std::string functionSignature;
    HookType type;
    uint32_t priority;
};

struct HookInfo {
    std::string name;
    std::string targetModule;
    std::string targetFunction;
    HookStatus status;
    bool enabled;
    std::chrono::system_clock::time_point installTime;
    uint32_t callCount;
    std::chrono::microseconds totalExecutionTime;
};

enum class HookStatus {
    NotInstalled,
    Installing,
    Installed,
    Failed,
    Removed
};

enum class HookType {
    PreHook,      // Execute before original function
    PostHook,     // Execute after original function
    ReplaceHook,  // Replace original function entirely
    ConditionalHook // Execute based on conditions
};
```

## Mod System API

### Mod Manager
Manages the loading and execution of UEVR mods.

```cpp
class ModManager {
public:
    // Mod loading
    bool LoadMod(const std::string& modPath);
    bool LoadModsFromDirectory(const std::string& directory);
    bool UnloadMod(const std::string& modName);
    bool UnloadAllMods();
    
    // Mod management
    bool EnableMod(const std::string& modName);
    bool DisableMod(const std::string& modName);
    bool IsModEnabled(const std::string& modName) const;
    bool IsModLoaded(const std::string& modName) const;
    
    // Mod information
    std::vector<std::string> GetLoadedMods() const;
    std::vector<std::string> GetEnabledMods() const;
    ModInfo GetModInfo(const std::string& modName) const;
    
    // Mod execution
    bool ExecuteModFunction(const std::string& modName, const std::string& functionName);
    bool ExecuteModFunction(const std::string& modName, const std::string& functionName, const ModParameters& params);
    
    // Mod lifecycle
    bool InitializeMod(const std::string& modName);
    bool ShutdownMod(const std::string& modName);
    bool ReloadMod(const std::string& modName);
    
    // Mod dependencies
    bool ResolveModDependencies(const std::string& modName);
    std::vector<std::string> GetModDependencies(const std::string& modName) const;
    std::vector<std::string> GetModDependents(const std::string& modName) const;
    
private:
    std::unordered_map<std::string, std::unique_ptr<Mod>> mods;
    std::unordered_map<std::string, ModState> modStates;
    std::mutex modsMutex;
};
```

### Mod Class
Base class for all UEVR mods.

```cpp
class Mod {
public:
    // Construction and destruction
    Mod(const std::string& name, const std::string& version);
    virtual ~Mod() = default;
    
    // Mod lifecycle
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // Mod information
    std::string GetName() const;
    std::string GetVersion() const;
    std::string GetDescription() const;
    std::string GetAuthor() const;
    std::vector<std::string> GetDependencies() const;
    
    // Mod execution
    virtual bool Execute(const ModParameters& params = ModParameters{}) = 0;
    virtual bool CanExecute() const = 0;
    
    // Mod configuration
    virtual bool LoadConfiguration(const Config& config) = 0;
    virtual bool SaveConfiguration(Config& config) const = 0;
    virtual Config GetDefaultConfiguration() const = 0;
    
    // Mod events
    virtual void OnModLoaded() {}
    virtual void OnModUnloaded() {}
    virtual void OnModEnabled() {}
    virtual void OnModDisabled() {}
    virtual void OnGameStarted() {}
    virtual void OnGameStopped() {}
    
    // Mod utilities
    bool RegisterHook(const std::string& functionName, void* hookFunction);
    bool UnregisterHook(const std::string& functionName);
    bool RegisterEventHandler(EventType type, EventHandler handler);
    bool UnregisterEventHandler(EventType type, EventHandler handler);
    
protected:
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    bool initialized;
    Config config;
};
```

### Mod Information Structures
```cpp
struct ModInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    ModState state;
    std::chrono::system_clock::time_point loadTime;
    uint32_t executionCount;
    std::chrono::microseconds totalExecutionTime;
};

struct ModParameters {
    std::unordered_map<std::string, std::any> parameters;
    
    template<typename T>
    T Get(const std::string& key, const T& defaultValue = T{}) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    template<typename T>
    void Set(const std::string& key, const T& value) {
        parameters[key] = value;
    }
};

enum class ModState {
    NotLoaded,
    Loading,
    Loaded,
    Initializing,
    Initialized,
    Failed,
    Unloading
};
```

## VR Management API

### VR Manager
Manages VR runtime integration and VR-specific functionality.

```cpp
class VRManager {
public:
    // VR initialization
    bool Initialize(const VRConfig& config);
    void Shutdown();
    bool IsInitialized() const;
    
    // VR runtime management
    bool ConnectToRuntime(VRRuntimeType runtimeType);
    bool DisconnectFromRuntime();
    bool IsConnected() const;
    VRRuntimeType GetConnectedRuntime() const;
    
    // VR state management
    bool EnableVR();
    bool DisableVR();
    bool IsVREnabled() const;
    
    // VR rendering
    bool StartRendering();
    bool StopRendering();
    bool IsRendering() const;
    bool SubmitFrame(const VRFrame& frame);
    
    // VR input
    bool InitializeInput();
    bool ProcessInput();
    VRInputState GetInputState() const;
    bool SetInputMapping(const InputMapping& mapping);
    
    // VR tracking
    bool InitializeTracking();
    bool UpdateTracking();
    VRTrackingState GetTrackingState() const;
    bool CalibrateTracking();
    
    // VR configuration
    bool LoadVRConfiguration(const std::string& configPath);
    bool SaveVRConfiguration(const std::string& configPath) const;
    VRConfig GetConfiguration() const;
    
    // VR information
    VRSystemInfo GetSystemInfo() const;
    VRDisplayInfo GetDisplayInfo() const;
    std::vector<VRControllerInfo> GetControllerInfo() const;
    
private:
    std::unique_ptr<VRRuntime> runtime;
    std::unique_ptr<VRRenderer> renderer;
    std::unique_ptr<VRInputManager> inputManager;
    std::unique_ptr<VRTrackingManager> trackingManager;
    VRConfig config;
    bool initialized;
    bool vrEnabled;
};
```

### VR Configuration Structures
```cpp
struct VRConfig {
    // Runtime configuration
    VRRuntimeType preferredRuntime = VRRuntimeType::OpenXR;
    std::string customRuntimePath;
    
    // Display configuration
    uint32_t renderWidth = 1920;
    uint32_t renderHeight = 1080;
    float renderScale = 1.0f;
    bool enableDynamicResolution = true;
    
    // Performance configuration
    float targetFrameRate = 90.0f;
    bool enableAlternateFrameRendering = true;
    bool enableAsyncReprojection = true;
    
    // Input configuration
    bool enableHapticFeedback = true;
    float inputSensitivity = 1.0f;
    bool enableInputRemapping = true;
    
    // Tracking configuration
    bool enableRoomScaleTracking = true;
    float trackingUpdateRate = 144.0f;
    bool enablePredictiveTracking = true;
    
    // Advanced configuration
    bool enableDebugMode = false;
    bool enableProfiling = false;
    std::string logLevel = "Info";
};

enum class VRRuntimeType {
    OpenVR,     // SteamVR
    OpenXR,     // Windows Mixed Reality, Oculus
    Custom      // Custom runtime
};
```

### VR Data Structures
```cpp
struct VRFrame {
    uint32_t frameIndex;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::vector<VRView> views;
    VRFrameData frameData;
    
    // Frame metadata
    uint32_t width;
    uint32_t height;
    VRPixelFormat pixelFormat;
    bool isStereo;
};

struct VRView {
    uint32_t viewIndex;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    VRPose pose;
    VRFov fov;
};

struct VRInputState {
    VRPose headPose;
    std::vector<VRControllerState> controllers;
    std::vector<VRButtonState> buttons;
    std::vector<VRAxisState> axes;
    std::vector<VRHapticState> haptics;
};

struct VRTrackingState {
    VRPose headPose;
    VRPose leftHandPose;
    VRPose rightHandPose;
    std::vector<VRTrackerPose> additionalTrackers;
    VRTrackingQuality quality;
    bool isTracking;
};
```

## Rendering API

### VR Renderer
Manages VR-specific rendering operations.

```cpp
class VRRenderer {
public:
    // Initialization
    bool Initialize(const RendererConfig& config);
    void Shutdown();
    bool IsInitialized() const;
    
    // Rendering lifecycle
    bool StartRendering();
    bool StopRendering();
    bool IsRendering() const;
    
    // Frame rendering
    bool BeginFrame();
    bool RenderFrame(const VRFrame& frame);
    bool EndFrame();
    bool SubmitFrame();
    
    // Render target management
    bool CreateRenderTargets(uint32_t width, uint32_t height);
    bool DestroyRenderTargets();
    bool ResizeRenderTargets(uint32_t width, uint32_t height);
    
    // Shader management
    bool LoadShaders(const std::string& shaderPath);
    bool CompileShader(const std::string& source, ShaderType type);
    bool UseShader(const std::string& shaderName);
    
    // Texture management
    bool CreateTexture(const TextureInfo& info);
    bool UpdateTexture(uint32_t textureId, const void* data);
    bool BindTexture(uint32_t textureId, uint32_t slot);
    
    // Performance optimization
    bool EnableAlternateFrameRendering(bool enable);
    bool EnableDynamicResolution(bool enable);
    bool SetRenderScale(float scale);
    
    // Configuration
    bool LoadConfiguration(const RendererConfig& config);
    RendererConfig GetConfiguration() const;
    
private:
    std::unique_ptr<RenderBackend> backend;
    std::vector<std::unique_ptr<RenderTarget>> renderTargets;
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    std::unordered_map<uint32_t, std::unique_ptr<Texture>> textures;
    RendererConfig config;
    bool initialized;
    bool rendering;
};
```

### Renderer Configuration
```cpp
struct RendererConfig {
    // Backend configuration
    RenderBackendType backendType = RenderBackendType::DirectX11;
    std::string customBackendPath;
    
    // Quality settings
    uint32_t renderWidth = 1920;
    uint32_t renderHeight = 1080;
    float renderScale = 1.0f;
    uint32_t msaaSamples = 4;
    
    // Performance settings
    bool enableAlternateFrameRendering = true;
    bool enableDynamicResolution = true;
    bool enableAsyncRendering = true;
    uint32_t maxFramesInFlight = 3;
    
    // Advanced settings
    bool enableDebugRendering = false;
    bool enableProfiling = false;
    bool enableValidationLayers = false;
    
    // Shader settings
    std::string shaderDirectory = "shaders/";
    bool enableShaderHotReload = true;
    bool enableShaderOptimization = true;
};

enum class RenderBackendType {
    DirectX11,
    DirectX12,
    Vulkan,
    OpenGL
};
```

## Input Management API

### VR Input Manager
Manages VR input devices and input processing.

```cpp
class VRInputManager {
public:
    // Initialization
    bool Initialize(const InputConfig& config);
    void Shutdown();
    bool IsInitialized() const;
    
    // Input device management
    bool InitializeControllers();
    bool InitializeTrackers();
    bool InitializeHapticDevices();
    
    // Input processing
    bool ProcessInput();
    bool UpdateInputState();
    VRInputState GetInputState() const;
    
    // Input mapping
    bool SetInputMapping(const InputMapping& mapping);
    bool LoadInputMapping(const std::string& filePath);
    bool SaveInputMapping(const std::string& filePath) const;
    
    // Haptic feedback
    bool SendHapticFeedback(uint32_t deviceId, const HapticFeedback& feedback);
    bool StopHapticFeedback(uint32_t deviceId);
    
    // Input events
    void RegisterInputHandler(InputEventType type, InputEventHandler handler);
    void UnregisterInputHandler(InputEventType type, InputEventHandler handler);
    
    // Device information
    std::vector<InputDeviceInfo> GetInputDevices() const;
    InputDeviceInfo GetDeviceInfo(uint32_t deviceId) const;
    
private:
    std::vector<std::unique_ptr<InputDevice>> devices;
    std::unordered_map<InputEventType, std::vector<InputEventHandler>> eventHandlers;
    InputMapping inputMapping;
    InputConfig config;
    bool initialized;
};
```

### Input Configuration Structures
```cpp
struct InputConfig {
    // Device configuration
    bool enableControllers = true;
    bool enableTrackers = true;
    bool enableHaptics = true;
    
    // Input processing
    float inputSensitivity = 1.0f;
    float deadzone = 0.1f;
    bool enableInputRemapping = true;
    
    // Haptic configuration
    bool enableHapticFeedback = true;
    float hapticIntensity = 1.0f;
    uint32_t hapticUpdateRate = 1000;
    
    // Advanced configuration
    bool enableInputProfiling = false;
    bool enableInputLogging = false;
};

struct InputMapping {
    std::unordered_map<std::string, InputAction> actions;
    std::unordered_map<std::string, InputAxis> axes;
    std::unordered_map<std::string, InputButton> buttons;
    
    // Mapping utilities
    bool AddAction(const std::string& name, const InputAction& action);
    bool AddAxis(const std::string& name, const InputAxis& axis);
    bool AddButton(const std::string& name, const InputButton& button);
    
    bool RemoveAction(const std::string& name);
    bool RemoveAxis(const std::string& name);
    bool RemoveButton(const std::string& name);
    
    InputAction* GetAction(const std::string& name);
    InputAxis* GetAxis(const std::string& name);
    InputButton* GetButton(const std::string& name);
};
```

## Performance Monitoring API

### Performance Monitor
Monitors and analyzes system performance.

```cpp
class PerformanceMonitor {
public:
    // Initialization
    bool Initialize(const PerformanceConfig& config);
    void Shutdown();
    bool IsInitialized() const;
    
    // Monitoring control
    bool StartMonitoring();
    bool StopMonitoring();
    bool IsMonitoring() const;
    
    // Performance metrics
    void BeginFrame();
    void EndFrame();
    void BeginSection(const std::string& sectionName);
    void EndSection(const std::string& sectionName);
    
    // Metric collection
    void RecordMetric(const std::string& name, float value);
    void RecordMetric(const std::string& name, uint64_t value);
    void RecordMetric(const std::string& name, const std::string& value);
    
    // Performance analysis
    PerformanceMetrics GetCurrentMetrics() const;
    PerformanceMetrics GetAverageMetrics(uint32_t frameCount) const;
    PerformanceReport GenerateReport() const;
    
    // Performance alerts
    void SetThreshold(const std::string& metricName, float threshold);
    void RegisterAlertHandler(PerformanceAlertHandler handler);
    void UnregisterAlertHandler(PerformanceAlertHandler handler);
    
    // Configuration
    bool LoadConfiguration(const PerformanceConfig& config);
    PerformanceConfig GetConfiguration() const;
    
private:
    std::vector<PerformanceFrame> frames;
    std::unordered_map<std::string, std::vector<float>> metrics;
    std::unordered_map<std::string, float> thresholds;
    std::vector<PerformanceAlertHandler> alertHandlers;
    PerformanceConfig config;
    bool initialized;
    bool monitoring;
    uint32_t currentFrame;
};
```

### Performance Data Structures
```cpp
struct PerformanceMetrics {
    // Frame metrics
    float frameRate;
    float frameTime;
    float frameTimeVariance;
    
    // VR-specific metrics
    float vrLatency;
    float vrFrameTime;
    float vrReprojectionRate;
    
    // System metrics
    float cpuUsage;
    float gpuUsage;
    size_t memoryUsage;
    size_t peakMemoryUsage;
    
    // Custom metrics
    std::unordered_map<std::string, float> customMetrics;
    
    // Timestamp
    std::chrono::high_resolution_clock::time_point timestamp;
};

struct PerformanceReport {
    std::string reportName;
    std::chrono::system_clock::time_point generationTime;
    std::chrono::milliseconds reportPeriod;
    
    // Summary statistics
    PerformanceMetrics minMetrics;
    PerformanceMetrics maxMetrics;
    PerformanceMetrics averageMetrics;
    
    // Detailed data
    std::vector<PerformanceMetrics> frameMetrics;
    std::vector<PerformanceSection> sectionMetrics;
    
    // Analysis
    std::vector<PerformanceIssue> issues;
    std::vector<PerformanceRecommendation> recommendations;
};

struct PerformanceSection {
    std::string name;
    float totalTime;
    float averageTime;
    uint32_t callCount;
    float percentageOfFrame;
};
```

## Event System API

### Event Dispatcher
Manages event distribution and handling.

```cpp
class EventDispatcher {
public:
    // Event registration
    void RegisterHandler(EventType type, EventHandler handler);
    void UnregisterHandler(EventType type, EventHandler handler);
    void UnregisterAllHandlers();
    
    // Event dispatch
    void DispatchEvent(const Event& event);
    void DispatchEvent(EventType type, const EventData& data = EventData{});
    
    // Event queuing
    void QueueEvent(const Event& event);
    void QueueEvent(EventType type, const EventData& data = EventData{});
    void ProcessEventQueue();
    
    // Event filtering
    void SetEventFilter(EventFilter filter);
    void ClearEventFilter();
    
    // Event statistics
    uint32_t GetEventCount(EventType type) const;
    uint32_t GetTotalEventCount() const;
    EventStatistics GetStatistics() const;
    
private:
    std::unordered_map<EventType, std::vector<EventHandler>> handlers;
    std::queue<Event> eventQueue;
    std::optional<EventFilter> eventFilter;
    EventStatistics statistics;
    std::mutex eventMutex;
};
```

### Event Structures
```cpp
struct Event {
    EventType type;
    EventData data;
    std::chrono::high_resolution_clock::time_point timestamp;
    uint32_t sequenceNumber;
    std::string source;
    
    // Event utilities
    template<typename T>
    T GetData() const {
        try {
            return std::any_cast<T>(data);
        } catch (...) {
            return T{};
        }
    }
    
    template<typename T>
    void SetData(const T& value) {
        data = value;
    }
};

struct EventData {
    std::any data;
    std::unordered_map<std::string, std::any> additionalData;
    
    template<typename T>
    T Get(const std::string& key, const T& defaultValue = T{}) const {
        auto it = additionalData.find(key);
        if (it != additionalData.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    template<typename T>
    void Set(const std::string& key, const T& value) {
        additionalData[key] = value;
    }
};

enum class EventType {
    // System events
    SystemInitialized,
    SystemShutdown,
    SystemError,
    
    // VR events
    VREnabled,
    VRDisabled,
    VRFrameStarted,
    VRFrameEnded,
    
    // Input events
    InputDeviceConnected,
    InputDeviceDisconnected,
    InputActionTriggered,
    InputAxisChanged,
    
    // Performance events
    PerformanceThresholdExceeded,
    PerformanceIssueDetected,
    
    // Mod events
    ModLoaded,
    ModUnloaded,
    ModEnabled,
    ModDisabled,
    
    // Custom events
    Custom
};
```

## Configuration API

### Configuration Manager
Manages application configuration and settings.

```cpp
class Config {
public:
    // Value access
    template<typename T>
    T Get(const std::string& key, const T& defaultValue = T{}) const;
    
    template<typename T>
    void Set(const std::string& key, const T& value);
    
    // Section management
    bool HasSection(const std::string& section) const;
    Config GetSection(const std::string& section) const;
    Config GetOrCreateSection(const std::string& section);
    
    // Configuration persistence
    bool LoadFromFile(const std::string& filePath);
    bool SaveToFile(const std::string& filePath) const;
    bool LoadFromString(const std::string& configString);
    std::string SaveToString() const;
    
    // Configuration validation
    bool Validate(const ConfigSchema& schema) const;
    std::vector<std::string> GetValidationErrors() const;
    
    // Configuration utilities
    bool HasKey(const std::string& key) const;
    std::vector<std::string> GetKeys() const;
    void RemoveKey(const std::string& key);
    void Clear();
    
    // Type checking
    bool IsBool(const std::string& key) const;
    bool IsInt(const std::string& key) const;
    bool IsFloat(const std::string& key) const;
    bool IsString(const std::string& key) const;
    
private:
    std::unordered_map<std::string, std::any> values;
    std::unordered_map<std::string, Config> sections;
    std::string name;
};
```

### Configuration Schema
```cpp
struct ConfigSchema {
    std::string name;
    std::string description;
    std::vector<ConfigField> fields;
    std::vector<ConfigValidationRule> validationRules;
    
    // Schema utilities
    bool ValidateField(const std::string& fieldName, const std::any& value) const;
    std::vector<std::string> ValidateConfig(const Config& config) const;
    Config GetDefaultConfig() const;
};

struct ConfigField {
    std::string name;
    std::string description;
    ConfigFieldType type;
    std::any defaultValue;
    bool required;
    std::vector<std::any> allowedValues;
    std::string validationPattern;
    
    // Field validation
    bool ValidateValue(const std::any& value) const;
    std::string GetValidationError(const std::any& value) const;
};

enum class ConfigFieldType {
    Bool,
    Int,
    Float,
    String,
    Section
};
```

## Logging API

### Logger
Provides logging functionality for UEVR.

```cpp
class Logger {
public:
    // Singleton access
    static Logger& Instance();
    
    // Logging methods
    void Log(LogLevel level, const std::string& message);
    void Log(LogLevel level, const std::string& message, const std::string& source);
    
    // Convenience methods
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Critical(const std::string& message);
    
    // Logging configuration
    void SetLogLevel(LogLevel level);
    LogLevel GetLogLevel() const;
    void EnableConsoleLogging(bool enable);
    void EnableFileLogging(bool enable, const std::string& filePath = "");
    
    // Log formatting
    void SetLogFormat(const std::string& format);
    void SetTimestampFormat(const std::string& format);
    
    // Log filtering
    void SetSourceFilter(const std::string& source);
    void ClearSourceFilter();
    
    // Log statistics
    uint32_t GetLogCount(LogLevel level) const;
    uint32_t GetTotalLogCount() const;
    LogStatistics GetStatistics() const;
    
private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    LogLevel currentLevel;
    bool consoleLogging;
    bool fileLogging;
    std::ofstream logFile;
    std::string logFormat;
    std::string timestampFormat;
    std::optional<std::string> sourceFilter;
    LogStatistics statistics;
    std::mutex logMutex;
};
```

### Logging Structures
```cpp
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

struct LogEntry {
    LogLevel level;
    std::string message;
    std::string source;
    std::chrono::system_clock::time_point timestamp;
    uint32_t sequenceNumber;
    std::thread::id threadId;
};

struct LogStatistics {
    std::unordered_map<LogLevel, uint32_t> logCounts;
    uint32_t totalLogs;
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
    std::chrono::milliseconds totalLoggingTime;
};
```

## Utility API

### Math Utilities
```cpp
namespace Math {
    // Vector operations
    template<typename T, size_t N>
    T Dot(const std::array<T, N>& a, const std::array<T, N>& b);
    
    template<typename T, size_t N>
    std::array<T, N> Cross(const std::array<T, N>& a, const std::array<T, N>& b);
    
    template<typename T, size_t N>
    T Length(const std::array<T, N>& v);
    
    template<typename T, size_t N>
    std::array<T, N> Normalize(const std::array<T, N>& v);
    
    // Matrix operations
    template<typename T, size_t R, size_t C>
    std::array<std::array<T, C>, R> Multiply(const std::array<std::array<T, C>, R>& a, 
                                             const std::array<std::array<T, R>, C>& b);
    
    template<typename T, size_t N>
    std::array<std::array<T, N>, N> Inverse(const std::array<std::array<T, N>, N>& m);
    
    template<typename T, size_t N>
    std::array<std::array<T, N>, N> Transpose(const std::array<std::array<T, N>, N>& m);
    
    // Quaternion operations
    template<typename T>
    std::array<T, 4> QuaternionMultiply(const std::array<T, 4>& a, const std::array<T, 4>& b);
    
    template<typename T>
    std::array<T, 4> QuaternionFromEuler(T pitch, T yaw, T roll);
    
    template<typename T>
    std::array<T, 3> EulerFromQuaternion(const std::array<T, 4>& q);
}
```

### String Utilities
```cpp
namespace String {
    // String manipulation
    std::string ToLower(const std::string& str);
    std::string ToUpper(const std::string& str);
    std::string Trim(const std::string& str);
    std::string TrimLeft(const std::string& str);
    std::string TrimRight(const std::string& str);
    
    // String splitting and joining
    std::vector<std::string> Split(const std::string& str, char delimiter);
    std::vector<std::string> Split(const std::string& str, const std::string& delimiter);
    std::string Join(const std::vector<std::string>& strings, const std::string& separator);
    
    // String formatting
    std::string Format(const std::string& format, ...);
    std::string FormatV(const std::string& format, va_list args);
    
    // String validation
    bool IsEmpty(const std::string& str);
    bool IsNumeric(const std::string& str);
    bool IsAlpha(const std::string& str);
    bool IsAlphanumeric(const std::string& str);
    
    // String conversion
    template<typename T>
    T ToNumber(const std::string& str, T defaultValue = T{});
    
    template<typename T>
    std::string FromNumber(T value);
}
```

### File Utilities
```cpp
namespace File {
    // File operations
    bool Exists(const std::string& path);
    bool IsFile(const std::string& path);
    bool IsDirectory(const std::string& path);
    bool CreateDirectory(const std::string& path);
    bool DeleteFile(const std::string& path);
    bool DeleteDirectory(const std::string& path);
    
    // File reading and writing
    std::string ReadTextFile(const std::string& path);
    bool WriteTextFile(const std::string& path, const std::string& content);
    std::vector<uint8_t> ReadBinaryFile(const std::string& path);
    bool WriteBinaryFile(const std::string& path, const std::vector<uint8_t>& data);
    
    // File information
    size_t GetFileSize(const std::string& path);
    std::chrono::system_clock::time_point GetLastModified(const std::string& path);
    std::string GetFileExtension(const std::string& path);
    std::string GetFileName(const std::string& path);
    std::string GetDirectoryPath(const std::string& path);
    
    // Path utilities
    std::string Combine(const std::string& path1, const std::string& path2);
    std::string GetAbsolutePath(const std::string& path);
    std::string GetRelativePath(const std::string& basePath, const std::string& targetPath);
    std::string NormalizePath(const std::string& path);
}
```

## Error Handling API

### Error Handler
Manages error handling and recovery.

```cpp
class ErrorHandler {
public:
    // Singleton access
    static ErrorHandler& Instance();
    
    // Error handling
    void HandleError(const Error& error);
    void HandleException(const std::exception& exception);
    void HandleUnknownException();
    
    // Error recovery
    bool AttemptRecovery(const Error& error);
    bool AttemptRecovery(const std::string& errorType);
    
    // Error reporting
    void ReportError(const Error& error);
    void ReportError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error);
    
    // Error configuration
    void SetErrorCallback(ErrorCallback callback);
    void SetRecoveryCallback(RecoveryCallback callback);
    void SetLoggingEnabled(bool enabled);
    
    // Error statistics
    uint32_t GetErrorCount(ErrorSeverity severity) const;
    uint32_t GetTotalErrorCount() const;
    ErrorStatistics GetStatistics() const;
    
private:
    ErrorHandler() = default;
    ~ErrorHandler() = default;
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    std::vector<ErrorCallback> errorCallbacks;
    std::vector<RecoveryCallback> recoveryCallbacks;
    bool loggingEnabled;
    ErrorStatistics statistics;
    std::mutex errorMutex;
};
```

### Error Structures
```cpp
struct Error {
    std::string message;
    std::string type;
    ErrorSeverity severity;
    std::string source;
    std::string function;
    uint32_t line;
    std::chrono::system_clock::time_point timestamp;
    std::string stackTrace;
    
    // Error utilities
    std::string GetFullMessage() const;
    bool IsRecoverable() const;
    bool RequiresImmediateAction() const;
};

enum class ErrorSeverity {
    Info = 0,
    Warning = 1,
    Error = 2,
    Critical = 3,
    Fatal = 4
};

struct ErrorStatistics {
    std::unordered_map<ErrorSeverity, uint32_t> errorCounts;
    uint32_t totalErrors;
    uint32_t recoveredErrors;
    uint32_t fatalErrors;
    std::chrono::system_clock::time_point firstError;
    std::chrono::system_clock::time_point lastError;
    std::chrono::milliseconds totalRecoveryTime;
};
```

## Resources

- [UEVR Architecture Overview](../architecture/system-overview.md)
- [Core Components Documentation](../architecture/core-components.md)
- [Development Setup Guide](setup.md)
- [Testing Framework](../testing/overview.md)
- [Performance Optimization](../performance/overview.md)

---

*For API support and questions, check the GitHub Issues or join the community Discord.*
