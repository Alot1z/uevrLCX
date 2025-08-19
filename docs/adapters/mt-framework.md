# MT Framework Adapter

This document provides comprehensive information about UEVR's MT Framework adapter, which enables VR support for Capcom games built with the MT Framework engine.

## MT Framework Overview

### What is MT Framework?
MT Framework is Capcom's proprietary game engine used for many of their titles, including the Monster Hunter series, Resident Evil series, and other popular games. The engine provides a robust foundation for action games with advanced rendering capabilities.

### Supported Games
- **Monster Hunter Series**
  - Monster Hunter: World
  - Monster Hunter: Rise
  - Monster Hunter Generations Ultimate
- **Resident Evil Series**
  - Resident Evil 5
  - Resident Evil 6
  - Resident Evil: Revelations
  - Resident Evil: Revelations 2
- **Other Capcom Titles**
  - Devil May Cry 4
  - Lost Planet series
  - Dead Rising series

### Engine Characteristics
- **Rendering APIs**: DirectX 9/11, OpenGL
- **Graphics Features**: Advanced lighting, particle systems, post-processing
- **Performance**: Optimized for action games, 60 FPS target
- **Architecture**: Component-based, modular design

## Adapter Implementation

### MT Framework Adapter Class
```cpp
// MT Framework specific adapter implementation
class MTFrameworkAdapter : public IEngineAdapter {
public:
    MTFrameworkAdapter();
    virtual ~MTFrameworkAdapter();
    
    // Engine detection and initialization
    virtual bool DetectEngine() override;
    virtual bool Initialize() override;
    virtual bool Shutdown() override;
    virtual EngineInfo GetEngineInfo() const override;
    
    // Core VR functionality
    virtual bool EnableVR() override;
    virtual bool DisableVR() override;
    virtual bool IsVREnabled() const override;
    
    // Rendering hooks
    virtual bool HookRenderFrame() override;
    virtual bool UnhookRenderFrame() override;
    virtual bool IsRenderFrameHooked() const override;
    
    // Input processing
    virtual bool HookInputProcessing() override;
    virtual bool UnhookInputProcessing() override;
    virtual bool IsInputProcessingHooked() const override;
    
    // VR rendering
    virtual bool SetupVRStereoRendering() override;
    virtual bool RenderVRStereoFrame() override;
    virtual bool CleanupVRStereoRendering() override;
    
    // Configuration
    virtual bool LoadConfiguration(const std::string& configPath) override;
    virtual bool SaveConfiguration(const std::string& configPath) override;
    virtual EngineConfiguration GetConfiguration() const override;
    
    // Error handling
    virtual std::vector<EngineError> GetErrors() const override;
    virtual bool HasErrors() const override;
    virtual bool ClearErrors() override;
    
    // Performance monitoring
    virtual EnginePerformanceMetrics GetPerformanceMetrics() const override;
    virtual bool IsPerformanceAcceptable() const override;
    
    // Status information
    virtual EngineStatus GetStatus() const override;
    virtual std::string GetStatusString() const override;
    
    // MT Framework specific methods
    bool DetectMTFrameworkVersion();
    bool HookMTFrameworkRendering();
    bool HookMTFrameworkInput();
    bool SetupMTStereoRendering();
    
private:
    // MT Framework specific members
    MTFrameworkInfo mtInfo;
    std::unique_ptr<MTRenderHook> renderHook;
    std::unique_ptr<MTInputHook> inputHook;
    std::unique_ptr<MTStereoRenderer> stereoRenderer;
    std::unique_ptr<MTConfiguration> configuration;
    
    // Internal methods
    bool InitializeMTHooks();
    bool SetupMTRendering();
    bool SetupMTInput();
    bool ValidateMTSetup();
};

// MT Framework specific information
struct MTFrameworkInfo {
    std::string version;                 // Engine version (e.g., "2.0", "3.0")
    std::string buildType;               // Build type (e.g., "Release", "Debug")
    std::string gameTitle;               // Game title identifier
    std::string executablePath;          // Path to game executable
    
    // Engine capabilities
    bool supportsDirectX9;
    bool supportsDirectX11;
    bool supportsOpenGL;
    bool supportsVulkan;
    
    // Graphics features
    bool hasAdvancedLighting;
    bool hasParticleSystems;
    bool hasPostProcessing;
    bool hasDynamicShadows;
    
    // Performance characteristics
    uint32_t targetFrameRate;
    uint32_t maxDrawCalls;
    uint32_t maxTextureSize;
    bool supportsMultiThreading;
    
    // VR compatibility
    bool hasVRSupport;
    bool supportsStereoRendering;
    bool supportsVRInput;
    
    // Timestamp
    std::chrono::system_clock::time_point detectionTime;
};
```

### Engine Detection
```cpp
// MT Framework detection implementation
bool MTFrameworkAdapter::DetectEngine() {
    // Check process name
    std::string processName = GetCurrentProcessName();
    if (processName.find("MonsterHunter") != std::string::npos ||
        processName.find("RE5") != std::string::npos ||
        processName.find("RE6") != std::string::npos ||
        processName.find("DMC4") != std::string::npos) {
        
        // Detect specific game and engine version
        if (DetectMTFrameworkVersion()) {
            return true;
        }
    }
    
    // Check loaded modules
    if (CheckForMTFrameworkModules()) {
        return DetectMTFrameworkVersion();
    }
    
    // Check memory patterns
    if (CheckForMTFrameworkPatterns()) {
        return DetectMTFrameworkVersion();
    }
    
    return false;
}

bool MTFrameworkAdapter::DetectMTFrameworkVersion() {
    // Check for MT Framework DLLs
    std::vector<std::string> mtDlls = {
        "MTFramework.dll",
        "MTFrameworkCore.dll",
        "MTFrameworkRender.dll"
    };
    
    for (const auto& dll : mtDlls) {
        if (IsModuleLoaded(dll)) {
            // Extract version information
            mtInfo.version = ExtractMTFrameworkVersion(dll);
            mtInfo.buildType = ExtractBuildType(dll);
            mtInfo.gameTitle = ExtractGameTitle();
            mtInfo.executablePath = GetExecutablePath();
            
            // Detect capabilities
            DetectMTCapabilities();
            
            return true;
        }
    }
    
    return false;
}

void MTFrameworkAdapter::DetectMTCapabilities() {
    // Check DirectX support
    mtInfo.supportsDirectX9 = CheckDirectX9Support();
    mtInfo.supportsDirectX11 = CheckDirectX11Support();
    mtInfo.supportsOpenGL = CheckOpenGLSupport();
    mtInfo.supportsVulkan = CheckVulkanSupport();
    
    // Check graphics features
    mtInfo.hasAdvancedLighting = CheckAdvancedLightingSupport();
    mtInfo.hasParticleSystems = CheckParticleSystemSupport();
    mtInfo.hasPostProcessing = CheckPostProcessingSupport();
    mtInfo.hasDynamicShadows = CheckDynamicShadowSupport();
    
    // Check performance characteristics
    mtInfo.targetFrameRate = DetectTargetFrameRate();
    mtInfo.maxDrawCalls = DetectMaxDrawCalls();
    mtInfo.maxTextureSize = DetectMaxTextureSize();
    mtInfo.supportsMultiThreading = CheckMultiThreadingSupport();
    
    // Check VR compatibility
    mtInfo.hasVRSupport = CheckVRSupport();
    mtInfo.supportsStereoRendering = CheckStereoRenderingSupport();
    mtInfo.supportsVRInput = CheckVRInputSupport();
}
```

### Rendering Hooks
```cpp
// MT Framework rendering hook implementation
class MTRenderHook {
public:
    // Hook installation
    bool InstallRenderHooks();
    bool UninstallRenderHooks();
    bool AreRenderHooksInstalled() const;
    
    // Hook management
    bool EnableRenderHooks();
    bool DisableRenderHooks();
    bool IsRenderHookEnabled() const;
    
    // Rendering interception
    bool InterceptRenderFrame();
    bool InterceptPresent();
    bool InterceptSwapChain();
    
    // VR rendering setup
    bool SetupVRStereoRendering();
    bool RenderVRStereoFrame();
    bool CleanupVRStereoRendering();
    
private:
    // Hook information
    std::vector<std::unique_ptr<Hook>> renderHooks;
    bool hooksInstalled;
    bool hooksEnabled;
    
    // Hook functions
    static HRESULT HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    static HRESULT HookResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    static void HookRenderFrame();
    
    // Internal methods
    bool InstallDirectXHooks();
    bool InstallOpenGLHooks();
    bool InstallVulkanHooks();
};

// MT Framework stereo renderer
class MTStereoRenderer {
public:
    // Stereo rendering setup
    bool InitializeStereoRendering(const StereoRenderingConfig& config);
    bool ShutdownStereoRendering();
    bool IsStereoRenderingInitialized() const;
    
    // Frame rendering
    bool RenderLeftEye();
    bool RenderRightEye();
    bool RenderStereoFrame();
    
    // Quality settings
    bool SetRenderScale(float scale);
    bool SetSupersampling(bool enabled);
    bool SetAntiAliasing(const std::string& method);
    
    // Performance optimization
    bool EnablePerformanceOptimizations();
    bool DisablePerformanceOptimizations();
    StereoRenderingMetrics GetMetrics() const;
    
private:
    // Stereo rendering state
    StereoRenderingConfig config;
    bool initialized;
    StereoRenderingMetrics metrics;
    
    // Rendering resources
    std::unique_ptr<RenderTarget> leftEyeTarget;
    std::unique_ptr<RenderTarget> rightEyeTarget;
    std::unique_ptr<DepthBuffer> depthBuffer;
    
    // Internal methods
    bool CreateRenderTargets();
    bool SetupDepthBuffer();
    bool ConfigureRenderingPipeline();
    void UpdateMetrics();
};

// Stereo rendering configuration
struct StereoRenderingConfig {
    // Display settings
    uint32_t renderWidth;
    uint32_t renderHeight;
    float renderScale;
    bool enableSupersampling;
    
    // Quality settings
    std::string antiAliasing;
    std::string textureQuality;
    std::string shadowQuality;
    std::string postProcessing;
    
    // Performance settings
    bool enableAsyncReprojection;
    bool enableMotionSmoothing;
    bool enablePerformanceOptimizations;
    uint32_t maxFrameRate;
    
    // VR settings
    float ipd;                           // Interpupillary distance
    float eyeRelief;                     // Eye relief distance
    bool enableRoomScale;
    bool enableSeatedMode;
};
```

### Input Processing
```cpp
// MT Framework input hook implementation
class MTInputHook {
public:
    // Hook installation
    bool InstallInputHooks();
    bool UninstallInputHooks();
    bool AreInputHooksInstalled() const;
    
    // Hook management
    bool EnableInputHooks();
    bool DisableInputHooks();
    bool IsInputHookEnabled() const;
    
    // Input interception
    bool InterceptKeyboardInput();
    bool InterceptMouseInput();
    bool InterceptGamepadInput();
    
    // VR input processing
    bool ProcessVRInput();
    bool MapVRInputToGameInput();
    bool HandleVRControllerInput();
    
private:
    // Hook information
    std::vector<std::unique_ptr<Hook>> inputHooks;
    bool hooksInstalled;
    bool hooksEnabled;
    
    // Hook functions
    static LRESULT HookWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static bool HookDirectInput();
    static bool HookXInput();
    
    // Internal methods
    bool InstallKeyboardHooks();
    bool InstallMouseHooks();
    bool InstallGamepadHooks();
    bool ProcessVRInputData();
};

// VR input mapping for MT Framework
class VRInputMapper {
public:
    // Input mapping configuration
    bool LoadInputMapping(const std::string& configPath);
    bool SaveInputMapping(const std::string& configPath);
    bool ResetInputMapping();
    
    // Input mapping
    bool MapVRControllerToGameInput(const VRControllerInput& vrInput, GameInput& gameInput);
    bool MapVRHandToGameInput(const VRHandInput& vrHand, GameInput& gameInput);
    bool MapVRHeadsetToGameInput(const VRHeadsetInput& vrHeadset, GameInput& gameInput);
    
    // Custom mapping
    bool AddCustomMapping(const std::string& vrAction, const std::string& gameAction);
    bool RemoveCustomMapping(const std::string& vrAction);
    bool ModifyCustomMapping(const std::string& vrAction, const std::string& gameAction);
    
    // Input profiles
    bool LoadInputProfile(const std::string& profileName);
    bool SaveInputProfile(const std::string& profileName);
    std::vector<std::string> GetAvailableProfiles() const;
    
private:
    // Input mapping data
    std::map<std::string, std::string> inputMappings;
    std::map<std::string, InputProfile> inputProfiles;
    std::string currentProfile;
    
    // Internal methods
    bool ValidateInputMapping(const std::string& vrAction, const std::string& gameAction);
    bool ApplyInputProfile(const InputProfile& profile);
    void UpdateInputMappings();
};

// Input profile structure
struct InputProfile {
    std::string name;
    std::string description;
    std::string gameTitle;
    std::string engineVersion;
    
    // Input mappings
    std::map<std::string, std::string> vrToGameMappings;
    std::map<std::string, std::string> gameToVRMappings;
    
    // Sensitivity settings
    float movementSensitivity;
    float rotationSensitivity;
    float actionSensitivity;
    
    // Deadzone settings
    float leftStickDeadzone;
    float rightStickDeadzone;
    float triggerDeadzone;
    
    // Vibration settings
    bool enableControllerVibration;
    bool enableHapticFeedback;
    float vibrationIntensity;
    
    // Timestamp
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point modified;
};
```

## Game-Specific Configurations

### Monster Hunter: World
```cpp
// Monster Hunter: World specific configuration
struct MHWConfiguration {
    // Game-specific settings
    bool enableMonsterTrackingVR;
    bool enableWeaponVR;
    bool enableInventoryVR;
    bool enableMapVR;
    
    // VR interface
    bool enableVRHUD;
    bool enableVRMenus;
    bool enableVRNotifications;
    bool enableVRTooltips;
    
    // Performance settings
    bool enableDynamicResolution;
    bool enableFrameRateOptimization;
    bool enableMemoryOptimization;
    uint32_t targetFrameRate;
    
    // Quality settings
    std::string textureQuality;
    std::string shadowQuality;
    std::string antiAliasing;
    std::string postProcessing;
    
    // VR comfort
    bool enableComfortMode;
    bool enableTeleportMovement;
    bool enableSmoothMovement;
    float movementSpeed;
};

// Monster Hunter: World adapter
class MHWAdapter : public MTFrameworkAdapter {
public:
    // Game-specific initialization
    bool InitializeMHW();
    bool SetupMHWVR();
    bool ConfigureMHWInput();
    
    // Game-specific VR features
    bool EnableMonsterTrackingVR();
    bool EnableWeaponVR();
    bool EnableInventoryVR();
    bool EnableMapVR();
    
    // Game-specific optimization
    bool OptimizeForMHW();
    bool ApplyMHWPerformanceSettings();
    bool ConfigureMHWQualitySettings();
    
private:
    MHWConfiguration mhwConfig;
    std::unique_ptr<MHWVRInterface> vrInterface;
    std::unique_ptr<MHWPerformanceOptimizer> performanceOptimizer;
    
    // Internal methods
    bool SetupMHWHooks();
    bool ConfigureMHWRendering();
    bool SetupMHWInputMapping();
};
```

### Resident Evil 5/6
```cpp
// Resident Evil 5/6 specific configuration
struct REConfiguration {
    // Game-specific settings
    bool enableInventoryVR;
    bool enableMapVR;
    bool enableAmmoVR;
    bool enableHealthVR;
    
    // VR interface
    bool enableVRHUD;
    bool enableVRMenus;
    bool enableVRNotifications;
    bool enableVRTooltips;
    
    // Horror settings
    bool enableComfortMode;
    bool enableJumpScareProtection;
    bool enableBrightnessAdjustment;
    float brightnessLevel;
    
    // Performance settings
    bool enableDynamicResolution;
    bool enableFrameRateOptimization;
    bool enableMemoryOptimization;
    uint32_t targetFrameRate;
    
    // Quality settings
    std::string textureQuality;
    std::string shadowQuality;
    std::string antiAliasing;
    std::string postProcessing;
};

// Resident Evil adapter
class REAdapter : public MTFrameworkAdapter {
public:
    // Game-specific initialization
    bool InitializeRE();
    bool SetupREVR();
    bool ConfigureREInput();
    
    // Game-specific VR features
    bool EnableInventoryVR();
    bool EnableMapVR();
    bool EnableAmmoVR();
    bool EnableHealthVR();
    
    // Horror game optimization
    bool OptimizeForHorrorGame();
    bool ApplyComfortSettings();
    bool ConfigureHorrorSettings();
    
private:
    REConfiguration reConfig;
    std::unique_ptr<REVRInterface> vrInterface;
    std::unique_ptr<REComfortManager> comfortManager;
    
    // Internal methods
    bool SetupREHooks();
    bool ConfigureRERendering();
    bool SetupREInputMapping();
};
```

## Performance Optimization

### MT Framework Specific Optimizations
```cpp
// MT Framework performance optimizer
class MTPerformanceOptimizer {
public:
    // Performance optimization
    bool OptimizeRendering();
    bool OptimizeMemory();
    bool OptimizeCPU();
    bool OptimizeGPU();
    
    // Game-specific optimization
    bool OptimizeForActionGame();
    bool OptimizeForOpenWorld();
    bool OptimizeForLinearGame();
    
    // Quality vs performance
    bool SetPerformanceMode();
    bool SetBalancedMode();
    bool SetQualityMode();
    bool SetCustomMode(const PerformanceProfile& profile);
    
    // Monitoring
    PerformanceMetrics GetPerformanceMetrics() const;
    bool IsPerformanceAcceptable() const;
    std::vector<PerformanceIssue> GetPerformanceIssues() const;
    
private:
    PerformanceProfile currentProfile;
    std::unique_ptr<PerformanceMonitor> monitor;
    std::unique_ptr<PerformanceProfiler> profiler;
    
    // Internal methods
    bool ApplyPerformanceProfile(const PerformanceProfile& profile);
    bool OptimizeRenderingPipeline();
    bool OptimizeMemoryAllocation();
    bool OptimizeCPUScheduling();
    bool OptimizeGPURendering();
};

// Performance profile structure
struct PerformanceProfile {
    std::string name;
    std::string description;
    PerformanceMode mode;
    
    // Rendering settings
    uint32_t renderWidth;
    uint32_t renderHeight;
    float renderScale;
    bool enableSupersampling;
    
    // Quality settings
    std::string textureQuality;
    std::string shadowQuality;
    std::string antiAliasing;
    std::string postProcessing;
    
    // Performance settings
    bool enableDynamicResolution;
    bool enableFrameRateOptimization;
    bool enableMemoryOptimization;
    bool enableGPUOptimization;
    
    // Target metrics
    uint32_t targetFrameRate;
    float targetLatency;
    uint32_t maxMemoryUsage;
    float maxCPUUsage;
    
    // Timestamp
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point modified;
};
```

## Troubleshooting

### Common Issues
1. **Game Not Detected**: Check process name and loaded modules
2. **Rendering Issues**: Verify DirectX/OpenGL hooks installation
3. **Input Problems**: Check input hook installation and mapping
4. **Performance Issues**: Adjust quality settings and optimization
5. **VR Compatibility**: Verify VR runtime and headset support

### Debug Information
```cpp
// Debug information for MT Framework adapter
struct MTDebugInfo {
    // Engine detection
    bool engineDetected;
    std::string detectedVersion;
    std::string detectedGame;
    
    // Hook status
    bool renderHooksInstalled;
    bool inputHooksInstalled;
    bool vrHooksInstalled;
    
    // VR status
    bool vrEnabled;
    bool stereoRenderingActive;
    bool vrInputActive;
    
    // Performance status
    float currentFrameRate;
    float currentLatency;
    uint64_t currentMemoryUsage;
    
    // Error information
    std::vector<EngineError> errors;
    std::vector<EngineWarning> warnings;
    
    // Timestamp
    std::chrono::system_clock::time_point timestamp;
};
```

## Resources

- [Engine Adapters Overview](overview.md) - General adapter information
- [Custom Adapters](custom-adapters.md) - Creating custom adapters
- [RE Engine](re-engine.md) - Resident Evil engine support
- [Performance Overview](../performance/overview.md) - Performance optimization
- [Testing Framework](../testing/overview.md) - Testing your adapters
- [API Reference](../development/api-reference.md) - Complete API documentation

---

*For MT Framework adapter questions or support, check the GitHub Issues or join the community Discord.*
