# RE Engine Adapter

This document provides comprehensive information about UEVR's RE Engine adapter, which enables VR support for Capcom games built with the RE Engine, including Resident Evil 7/8, Devil May Cry 5, and other modern Capcom titles.

## RE Engine Overview

### What is RE Engine?
RE Engine is Capcom's modern, proprietary game engine designed for next-generation gaming experiences. It provides advanced rendering capabilities, improved performance, and enhanced visual fidelity compared to the older MT Framework engine.

### Supported Games
- **Resident Evil Series**
  - Resident Evil 7: Biohazard
  - Resident Evil 2 Remake
  - Resident Evil 3 Remake
  - Resident Evil 4 Remake
  - Resident Evil Village
- **Devil May Cry Series**
  - Devil May Cry 5
  - Devil May Cry 5: Special Edition
- **Other Capcom Titles**
  - Monster Hunter Rise
  - Pragmata (upcoming)
  - Exoprimal

### Engine Characteristics
- **Rendering APIs**: DirectX 11/12, Vulkan
- **Graphics Features**: PBR materials, ray tracing, advanced lighting, volumetric effects
- **Performance**: Optimized for modern hardware, 60+ FPS target
- **Architecture**: Modern component-based design, advanced memory management

## Adapter Implementation

### RE Engine Adapter Class
```cpp
// RE Engine specific adapter implementation
class REEngineAdapter : public IEngineAdapter {
public:
    REEngineAdapter();
    virtual ~REEngineAdapter();
    
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
    
    // RE Engine specific methods
    bool DetectREEngineVersion();
    bool HookREEngineRendering();
    bool HookREEngineInput();
    bool SetupREStereoRendering();
    
private:
    // RE Engine specific members
    REEngineInfo reInfo;
    std::unique_ptr<RERenderHook> renderHook;
    std::unique_ptr<REInputHook> inputHook;
    std::unique_ptr<REStereoRenderer> stereoRenderer;
    std::unique_ptr<REConfiguration> configuration;
    
    // Internal methods
    bool InitializeREHooks();
    bool SetupRERendering();
    bool SetupREInput();
    bool ValidateRESetup();
};

// RE Engine specific information
struct REEngineInfo {
    std::string version;                 // Engine version (e.g., "1.0", "2.0")
    std::string buildType;               // Build type (e.g., "Release", "Debug")
    std::string gameTitle;               // Game title identifier
    std::string executablePath;          // Path to game executable
    
    // Engine capabilities
    bool supportsDirectX11;
    bool supportsDirectX12;
    bool supportsVulkan;
    bool supportsRayTracing;
    
    // Graphics features
    bool hasPBRMaterials;
    bool hasVolumetricEffects;
    bool hasAdvancedLighting;
    bool hasPostProcessing;
    bool hasDynamicShadows;
    bool hasRayTracedShadows;
    
    // Performance characteristics
    uint32_t targetFrameRate;
    uint32_t maxDrawCalls;
    uint32_t maxTextureSize;
    bool supportsMultiThreading;
    bool supportsGPUCompute;
    
    // VR compatibility
    bool hasVRSupport;
    bool supportsStereoRendering;
    bool supportsVRInput;
    bool supportsVRHaptics;
    
    // Timestamp
    std::chrono::system_clock::time_point detectionTime;
};
```

### Engine Detection
```cpp
// RE Engine detection implementation
bool REEngineAdapter::DetectEngine() {
    // Check process name
    std::string processName = GetCurrentProcessName();
    if (processName.find("RE7") != std::string::npos ||
        processName.find("RE2") != std::string::npos ||
        processName.find("RE3") != std::string::npos ||
        processName.find("RE4") != std::string::npos ||
        processName.find("RE8") != std::string::npos ||
        processName.find("DMC5") != std::string::npos ||
        processName.find("MHRise") != std::string::npos) {
        
        // Detect specific game and engine version
        if (DetectREEngineVersion()) {
            return true;
        }
    }
    
    // Check loaded modules
    if (CheckForREEngineModules()) {
        return DetectREEngineVersion();
    }
    
    // Check memory patterns
    if (CheckForREEnginePatterns()) {
        return DetectREEngineVersion();
    }
    
    return false;
}

bool REEngineAdapter::DetectREEngineVersion() {
    // Check for RE Engine DLLs
    std::vector<std::string> reDlls = {
        "REEngine.dll",
        "REEngineCore.dll",
        "REEngineRender.dll",
        "REEngineInput.dll"
    };
    
    for (const auto& dll : reDlls) {
        if (IsModuleLoaded(dll)) {
            // Extract version information
            reInfo.version = ExtractREEngineVersion(dll);
            reInfo.buildType = ExtractBuildType(dll);
            reInfo.gameTitle = ExtractGameTitle();
            reInfo.executablePath = GetExecutablePath();
            
            // Detect capabilities
            DetectRECapabilities();
            
            return true;
        }
    }
    
    return false;
}

void REEngineAdapter::DetectRECapabilities() {
    // Check DirectX support
    reInfo.supportsDirectX11 = CheckDirectX11Support();
    reInfo.supportsDirectX12 = CheckDirectX12Support();
    reInfo.supportsVulkan = CheckVulkanSupport();
    reInfo.supportsRayTracing = CheckRayTracingSupport();
    
    // Check graphics features
    reInfo.hasPBRMaterials = CheckPBRMaterialSupport();
    reInfo.hasVolumetricEffects = CheckVolumetricEffectSupport();
    reInfo.hasAdvancedLighting = CheckAdvancedLightingSupport();
    reInfo.hasPostProcessing = CheckPostProcessingSupport();
    reInfo.hasDynamicShadows = CheckDynamicShadowSupport();
    reInfo.hasRayTracedShadows = CheckRayTracedShadowSupport();
    
    // Check performance characteristics
    reInfo.targetFrameRate = DetectTargetFrameRate();
    reInfo.maxDrawCalls = DetectMaxDrawCalls();
    reInfo.maxTextureSize = DetectMaxTextureSize();
    reInfo.supportsMultiThreading = CheckMultiThreadingSupport();
    reInfo.supportsGPUCompute = CheckGPUComputeSupport();
    
    // Check VR compatibility
    reInfo.hasVRSupport = CheckVRSupport();
    reInfo.supportsStereoRendering = CheckStereoRenderingSupport();
    reInfo.supportsVRInput = CheckVRInputSupport();
    reInfo.supportsVRHaptics = CheckVRHapticSupport();
}
```

### Rendering Hooks
```cpp
// RE Engine rendering hook implementation
class RERenderHook {
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
    bool InterceptRayTracing();
    
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
    static HRESULT HookCreateRayTracingStateObject(ID3D12Device* pDevice, const D3D12_RAYTRACING_STATE_OBJECT_DESC* pDesc, REFIID riid, void** ppStateObject);
    static void HookRenderFrame();
    
    // Internal methods
    bool InstallDirectX11Hooks();
    bool InstallDirectX12Hooks();
    bool InstallVulkanHooks();
    bool InstallRayTracingHooks();
};

// RE Engine stereo renderer
class REStereoRenderer {
public:
    // Stereo rendering setup
    bool InitializeStereoRendering(const StereoRenderingConfig& config);
    bool ShutdownStereoRendering();
    bool IsStereoRenderingInitialized() const;
    
    // Frame rendering
    bool RenderLeftEye();
    bool RenderRightEye();
    bool RenderStereoFrame();
    
    // Advanced rendering features
    bool EnableRayTracing();
    bool DisableRayTracing();
    bool SetRayTracingQuality(const std::string& quality);
    
    // Quality settings
    bool SetRenderScale(float scale);
    bool SetSupersampling(bool enabled);
    bool SetAntiAliasing(const std::string& method);
    bool SetShadowQuality(const std::string& quality);
    
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
    std::unique_ptr<RayTracingResources> rayTracingResources;
    
    // Internal methods
    bool CreateRenderTargets();
    bool SetupDepthBuffer();
    bool SetupRayTracingResources();
    bool ConfigureRenderingPipeline();
    void UpdateMetrics();
};

// Enhanced stereo rendering configuration
struct REStereoRenderingConfig {
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
    std::string rayTracingQuality;
    
    // Performance settings
    bool enableAsyncReprojection;
    bool enableMotionSmoothing;
    bool enablePerformanceOptimizations;
    bool enableDynamicResolution;
    uint32_t maxFrameRate;
    
    // VR settings
    float ipd;                           // Interpupillary distance
    float eyeRelief;                     // Eye relief distance
    bool enableRoomScale;
    bool enableSeatedMode;
    
    // Advanced settings
    bool enableRayTracing;
    bool enableVolumetricEffects;
    bool enableAdvancedLighting;
    bool enableHDR;
};
```

### Input Processing
```cpp
// RE Engine input hook implementation
class REInputHook {
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
    bool InterceptTouchInput();
    
    // VR input processing
    bool ProcessVRInput();
    bool MapVRInputToGameInput();
    bool HandleVRControllerInput();
    bool HandleVRHapticFeedback();
    
private:
    // Hook information
    std::vector<std::unique_ptr<Hook>> inputHooks;
    bool hooksInstalled;
    bool hooksEnabled;
    
    // Hook functions
    static LRESULT HookWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static bool HookDirectInput();
    static bool HookXInput();
    static bool HookTouchInput();
    
    // Internal methods
    bool InstallKeyboardHooks();
    bool InstallMouseHooks();
    bool InstallGamepadHooks();
    bool InstallTouchHooks();
    bool ProcessVRInputData();
    bool ProcessHapticFeedback();
};

// Enhanced VR input mapping for RE Engine
class REVRInputMapper {
public:
    // Input mapping configuration
    bool LoadInputMapping(const std::string& configPath);
    bool SaveInputMapping(const std::string& configPath);
    bool ResetInputMapping();
    
    // Input mapping
    bool MapVRControllerToGameInput(const VRControllerInput& vrInput, GameInput& gameInput);
    bool MapVRHandToGameInput(const VRHandInput& vrHand, GameInput& gameInput);
    bool MapVRHeadsetToGameInput(const VRHeadsetInput& vrHeadset, GameInput& gameInput);
    bool MapVRHapticsToGameFeedback(const VRHapticInput& vrHaptics, GameFeedback& gameFeedback);
    
    // Custom mapping
    bool AddCustomMapping(const std::string& vrAction, const std::string& gameAction);
    bool RemoveCustomMapping(const std::string& vrAction);
    bool ModifyCustomMapping(const std::string& vrAction, const std::string& gameAction);
    
    // Input profiles
    bool LoadInputProfile(const std::string& profileName);
    bool SaveInputProfile(const std::string& profileName);
    std::vector<std::string> GetAvailableProfiles() const;
    
    // Haptic feedback
    bool ConfigureHapticFeedback(const HapticConfig& config);
    bool TestHapticFeedback(const std::string& hapticPattern);
    bool CalibrateHapticFeedback();
    
private:
    // Input mapping data
    std::map<std::string, std::string> inputMappings;
    std::map<std::string, InputProfile> inputProfiles;
    std::string currentProfile;
    HapticConfig hapticConfig;
    
    // Internal methods
    bool ValidateInputMapping(const std::string& vrAction, const std::string& gameAction);
    bool ApplyInputProfile(const InputProfile& profile);
    void UpdateInputMappings();
    bool ProcessHapticPattern(const std::string& pattern);
};

// Enhanced input profile structure
struct REInputProfile {
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
    float aimSensitivity;
    
    // Deadzone settings
    float leftStickDeadzone;
    float rightStickDeadzone;
    float triggerDeadzone;
    float touchpadDeadzone;
    
    // Vibration settings
    bool enableControllerVibration;
    bool enableHapticFeedback;
    float vibrationIntensity;
    std::string hapticPattern;
    
    // Advanced settings
    bool enableTouchInput;
    bool enableGestureRecognition;
    bool enableEyeTracking;
    bool enableFacialTracking;
    
    // Timestamp
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point modified;
};
```

## Game-Specific Configurations

### Resident Evil 7/8
```cpp
// Resident Evil 7/8 specific configuration
struct REConfiguration {
    // Game-specific settings
    bool enableInventoryVR;
    bool enableMapVR;
    bool enableAmmoVR;
    bool enableHealthVR;
    bool enableWeaponVR;
    
    // VR interface
    bool enableVRHUD;
    bool enableVRMenus;
    bool enableVRNotifications;
    bool enableVRTooltips;
    bool enableVRInventory;
    
    // Horror settings
    bool enableComfortMode;
    bool enableJumpScareProtection;
    bool enableBrightnessAdjustment;
    bool enableAudioAdjustment;
    float brightnessLevel;
    float audioLevel;
    
    // Performance settings
    bool enableDynamicResolution;
    bool enableFrameRateOptimization;
    bool enableMemoryOptimization;
    bool enableRayTracingOptimization;
    uint32_t targetFrameRate;
    
    // Quality settings
    std::string textureQuality;
    std::string shadowQuality;
    std::string antiAliasing;
    std::string postProcessing;
    std::string rayTracingQuality;
    
    // VR comfort
    bool enableComfortMode;
    bool enableTeleportMovement;
    bool enableSmoothMovement;
    float movementSpeed;
    bool enableVignette;
};

// Resident Evil adapter
class REAdapter : public REEngineAdapter {
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
    bool EnableWeaponVR();
    
    // Horror game optimization
    bool OptimizeForHorrorGame();
    bool ApplyComfortSettings();
    bool ConfigureHorrorSettings();
    bool OptimizeRayTracing();
    
private:
    REConfiguration reConfig;
    std::unique_ptr<REVRInterface> vrInterface;
    std::unique_ptr<REComfortManager> comfortManager;
    std::unique_ptr<RERayTracingOptimizer> rayTracingOptimizer;
    
    // Internal methods
    bool SetupREHooks();
    bool ConfigureRERendering();
    bool SetupREInputMapping();
    bool OptimizeRERayTracing();
};
```

### Devil May Cry 5
```cpp
// Devil May Cry 5 specific configuration
struct DMC5Configuration {
    // Game-specific settings
    bool enableCombatVR;
    bool enableWeaponVR;
    bool enableStyleVR;
    bool enableComboVR;
    bool enableDevilTriggerVR;
    
    // VR interface
    bool enableVRHUD;
    bool enableVRMenus;
    bool enableVRNotifications;
    bool enableVRTooltips;
    bool enableVRCombatHUD;
    
    // Combat settings
    bool enableVRCombat;
    bool enableWeaponSwitching;
    bool enableStyleSwitching;
    bool enableComboDisplay;
    bool enableDevilTriggerDisplay;
    
    // Performance settings
    bool enableDynamicResolution;
    bool enableFrameRateOptimization;
    bool enableMemoryOptimization;
    bool enableCombatOptimization;
    uint32_t targetFrameRate;
    
    // Quality settings
    std::string textureQuality;
    std::string shadowQuality;
    std::string antiAliasing;
    std::string postProcessing;
    std::string particleQuality;
    
    // VR comfort
    bool enableComfortMode;
    bool enableTeleportMovement;
    bool enableSmoothMovement;
    float movementSpeed;
    bool enableCombatVignette;
};

// Devil May Cry 5 adapter
class DMC5Adapter : public REEngineAdapter {
public:
    // Game-specific initialization
    bool InitializeDMC5();
    bool SetupDMC5VR();
    bool ConfigureDMC5Input();
    
    // Game-specific VR features
    bool EnableCombatVR();
    bool EnableWeaponVR();
    bool EnableStyleVR();
    bool EnableComboVR();
    bool EnableDevilTriggerVR();
    
    // Action game optimization
    bool OptimizeForActionGame();
    bool ApplyCombatSettings();
    bool ConfigureCombatSettings();
    bool OptimizeParticleEffects();
    
private:
    DMC5Configuration dmc5Config;
    std::unique_ptr<DMC5VRInterface> vrInterface;
    std::unique_ptr<DMC5CombatManager> combatManager;
    std::unique_ptr<DMC5ParticleOptimizer> particleOptimizer;
    
    // Internal methods
    bool SetupDMC5Hooks();
    bool ConfigureDMC5Rendering();
    bool SetupDMC5InputMapping();
    bool OptimizeDMC5Particles();
};
```

## Performance Optimization

### RE Engine Specific Optimizations
```cpp
// RE Engine performance optimizer
class REPerformanceOptimizer {
public:
    // Performance optimization
    bool OptimizeRendering();
    bool OptimizeMemory();
    bool OptimizeCPU();
    bool OptimizeGPU();
    bool OptimizeRayTracing();
    
    // Game-specific optimization
    bool OptimizeForHorrorGame();
    bool OptimizeForActionGame();
    bool OptimizeForOpenWorld();
    bool OptimizeForLinearGame();
    
    // Quality vs performance
    bool SetPerformanceMode();
    bool SetBalancedMode();
    bool SetQualityMode();
    bool SetCustomMode(const PerformanceProfile& profile);
    
    // Ray tracing optimization
    bool OptimizeRayTracingPerformance();
    bool SetRayTracingQuality(const std::string& quality);
    bool EnableRayTracingOptimizations();
    
    // Monitoring
    PerformanceMetrics GetPerformanceMetrics() const;
    bool IsPerformanceAcceptable() const;
    std::vector<PerformanceIssue> GetPerformanceIssues() const;
    
private:
    PerformanceProfile currentProfile;
    std::unique_ptr<PerformanceMonitor> monitor;
    std::unique_ptr<PerformanceProfiler> profiler;
    std::unique_ptr<RayTracingOptimizer> rayTracingOptimizer;
    
    // Internal methods
    bool ApplyPerformanceProfile(const PerformanceProfile& profile);
    bool OptimizeRenderingPipeline();
    bool OptimizeMemoryAllocation();
    bool OptimizeCPUScheduling();
    bool OptimizeGPURendering();
    bool OptimizeRayTracingPipeline();
};

// Enhanced performance profile structure
struct REPerformanceProfile {
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
    std::string rayTracingQuality;
    
    // Performance settings
    bool enableDynamicResolution;
    bool enableFrameRateOptimization;
    bool enableMemoryOptimization;
    bool enableGPUOptimization;
    bool enableRayTracingOptimization;
    
    // Target metrics
    uint32_t targetFrameRate;
    float targetLatency;
    uint32_t maxMemoryUsage;
    float maxCPUUsage;
    float maxGPUUsage;
    
    // Advanced settings
    bool enableVolumetricEffects;
    bool enableAdvancedLighting;
    bool enableParticleOptimization;
    bool enableComputeShaders;
    
    // Timestamp
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point modified;
};
```

## Troubleshooting

### Common Issues
1. **Game Not Detected**: Check process name and loaded modules
2. **Rendering Issues**: Verify DirectX 11/12 hooks installation
3. **Ray Tracing Issues**: Check ray tracing support and optimization
4. **Input Problems**: Check input hook installation and mapping
5. **Performance Issues**: Adjust quality settings and optimization
6. **VR Compatibility**: Verify VR runtime and headset support

### Debug Information
```cpp
// Debug information for RE Engine adapter
struct REDebugInfo {
    // Engine detection
    bool engineDetected;
    std::string detectedVersion;
    std::string detectedGame;
    
    // Hook status
    bool renderHooksInstalled;
    bool inputHooksInstalled;
    bool vrHooksInstalled;
    bool rayTracingHooksInstalled;
    
    // VR status
    bool vrEnabled;
    bool stereoRenderingActive;
    bool vrInputActive;
    bool rayTracingActive;
    
    // Performance status
    float currentFrameRate;
    float currentLatency;
    uint64_t currentMemoryUsage;
    float currentGPUUsage;
    
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
- [MT Framework](mt-framework.md) - Capcom engine support
- [Performance Overview](../performance/overview.md) - Performance optimization
- [Testing Framework](../testing/overview.md) - Testing your adapters
- [API Reference](../development/api-reference.md) - Complete API documentation

---

*For RE Engine adapter questions or support, check the GitHub Issues or join the community Discord.*
