# Custom Engine Adapters

This guide explains how to create custom engine adapters for UEVR, allowing you to add support for new game engines or extend existing functionality.

## Overview

Engine adapters are the core components that enable UEVR to work with different game engines. They handle engine-specific initialization, hooking, and VR integration.

## Adapter Architecture

### Base Interface
```cpp
class IEngineAdapter {
public:
    virtual ~IEngineAdapter() = default;
    
    // Core initialization
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    // Engine detection
    virtual bool IsCompatible() = 0;
    virtual std::string GetEngineName() const = 0;
    virtual std::string GetEngineVersion() const = 0;
    
    // VR integration
    virtual bool SetupVRRendering() = 0;
    virtual bool SetupVRInput() = 0;
    virtual bool SetupVRTracking() = 0;
    
    // Hooking
    virtual bool InstallHooks() = 0;
    virtual void RemoveHooks() = 0;
    
    // Configuration
    virtual void LoadConfiguration(const Config& config) = 0;
    virtual void SaveConfiguration(Config& config) const = 0;
};
```

### Adapter Factory
```cpp
class AdapterFactory {
public:
    static std::unique_ptr<IEngineAdapter> CreateAdapter(
        const std::string& engineName,
        const std::string& engineVersion
    );
    
    static void RegisterAdapter(
        const std::string& engineName,
        std::function<std::unique_ptr<IEngineAdapter>()> factory
    );
    
private:
    static std::unordered_map<std::string, 
        std::function<std::unique_ptr<IEngineAdapter>()>> adapters;
};
```

## Creating a Custom Adapter

### Step 1: Define Your Adapter Class
```cpp
class MyCustomEngineAdapter : public IEngineAdapter {
public:
    MyCustomEngineAdapter();
    ~MyCustomEngineAdapter() override;
    
    // Implement required interface methods
    bool Initialize() override;
    void Shutdown() override;
    bool IsCompatible() override;
    std::string GetEngineName() const override;
    std::string GetEngineVersion() const override;
    
    // Implement VR integration methods
    bool SetupVRRendering() override;
    bool SetupVRInput() override;
    bool SetupVRTracking() override;
    
    // Implement hooking methods
    bool InstallHooks() override;
    void RemoveHooks() override;
    
    // Implement configuration methods
    void LoadConfiguration(const Config& config) override;
    void SaveConfiguration(Config& config) const override;
    
private:
    // Engine-specific members
    void* engineInstance;
    std::vector<std::unique_ptr<Hook>> hooks;
    std::unique_ptr<VRRenderer> vrRenderer;
    std::unique_ptr<VRInputManager> vrInput;
    
    // Helper methods
    bool DetectEngine();
    bool InitializeEngineHooks();
    bool SetupEngineVR();
};
```

### Step 2: Implement Engine Detection
```cpp
bool MyCustomEngineAdapter::IsCompatible() {
    // Check if the target engine is loaded
    HMODULE engineModule = GetModuleHandle(L"myengine.dll");
    if (!engineModule) {
        return false;
    }
    
    // Check engine version
    auto versionFunc = reinterpret_cast<const char*(*)()>(
        GetProcAddress(engineModule, "GetEngineVersion")
    );
    
    if (versionFunc) {
        std::string version = versionFunc();
        return IsVersionSupported(version);
    }
    
    return false;
}

std::string MyCustomEngineAdapter::GetEngineName() const {
    return "MyCustomEngine";
}

std::string MyCustomEngineAdapter::GetEngineVersion() const {
    return engineVersion;
}
```

### Step 3: Implement Initialization
```cpp
bool MyCustomEngineAdapter::Initialize() {
    try {
        // Detect the engine
        if (!DetectEngine()) {
            UEVR_LOG_ERROR("Failed to detect MyCustomEngine");
            return false;
        }
        
        // Initialize engine hooks
        if (!InitializeEngineHooks()) {
            UEVR_LOG_ERROR("Failed to initialize engine hooks");
            return false;
        }
        
        // Setup VR systems
        if (!SetupVRRendering()) {
            UEVR_LOG_ERROR("Failed to setup VR rendering");
            return false;
        }
        
        if (!SetupVRInput()) {
            UEVR_LOG_ERROR("Failed to setup VR input");
            return false;
        }
        
        if (!SetupVRTracking()) {
            UEVR_LOG_ERROR("Failed to setup VR tracking");
            return false;
        }
        
        UEVR_LOG_INFO("MyCustomEngine adapter initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        UEVR_LOG_ERROR("Exception during initialization: {}", e.what());
        return false;
    }
}
```

### Step 4: Implement Engine Hooking
```cpp
bool MyCustomEngineAdapter::InstallHooks() {
    try {
        // Hook the main render function
        auto renderFunc = GetProcAddress(engineModule, "RenderFrame");
        if (renderFunc) {
            auto hook = std::make_unique<Hook>(
                renderFunc,
                &MyCustomEngineAdapter::OnRenderFrame,
                this
            );
            
            if (hook->Install()) {
                hooks.push_back(std::move(hook));
            }
        }
        
        // Hook input processing
        auto inputFunc = GetProcAddress(engineModule, "ProcessInput");
        if (inputFunc) {
            auto hook = std::make_unique<Hook>(
                inputFunc,
                &MyCustomEngineAdapter::OnProcessInput,
                this
            );
            
            if (hook->Install()) {
                hooks.push_back(std::move(hook));
            }
        }
        
        return !hooks.empty();
        
    } catch (const std::exception& e) {
        UEVR_LOG_ERROR("Failed to install hooks: {}", e.what());
        return false;
    }
}

void MyCustomEngineAdapter::RemoveHooks() {
    for (auto& hook : hooks) {
        hook->Remove();
    }
    hooks.clear();
}
```

### Step 5: Implement VR Rendering
```cpp
bool MyCustomEngineAdapter::SetupVRRendering() {
    try {
        // Create VR renderer
        vrRenderer = std::make_unique<VRRenderer>();
        
        // Setup stereo rendering
        if (!vrRenderer->InitializeStereoRendering()) {
            return false;
        }
        
        // Setup render targets
        if (!vrRenderer->SetupRenderTargets()) {
            return false;
        }
        
        // Configure VR-specific rendering settings
        vrRenderer->SetRenderScale(0.85f);
        vrRenderer->EnableDynamicResolution(true);
        
        return true;
        
    } catch (const std::exception& e) {
        UEVR_LOG_ERROR("Failed to setup VR rendering: {}", e.what());
        return false;
    }
}
```

### Step 6: Implement VR Input
```cpp
bool MyCustomEngineAdapter::SetupVRInput() {
    try {
        // Create VR input manager
        vrInput = std::make_unique<VRInputManager>();
        
        // Initialize VR controllers
        if (!vrInput->InitializeControllers()) {
            return false;
        }
        
        // Setup input mapping
        if (!vrInput->SetupInputMapping()) {
            return false;
        }
        
        // Configure haptic feedback
        vrInput->EnableHaptics(true);
        
        return true;
        
    } catch (const std::exception& e) {
        UEVR_LOG_ERROR("Failed to setup VR input: {}", e.what());
        return false;
    }
}
```

## Hook Implementation

### Render Frame Hook
```cpp
void MyCustomEngineAdapter::OnRenderFrame(void* context) {
    try {
        // Get frame data from engine
        FrameData frameData = ExtractFrameData(context);
        
        // Apply VR rendering
        if (vrRenderer) {
            vrRenderer->RenderFrame(frameData);
        }
        
        // Call original function
        // Note: This depends on your hooking library
        CallOriginalFunction(context);
        
    } catch (const std::exception& e) {
        UEVR_LOG_ERROR("Error in render frame hook: {}", e.what());
    }
}
```

### Input Processing Hook
```cpp
void MyCustomEngineAdapter::OnProcessInput(void* context) {
    try {
        // Get input data from engine
        InputData inputData = ExtractInputData(context);
        
        // Process VR input
        if (vrInput) {
            VRInputData vrInputData = vrInput->ProcessInput(inputData);
            
            // Inject VR input into engine
            InjectVRInput(context, vrInputData);
        }
        
        // Call original function
        CallOriginalFunction(context);
        
    } catch (const std::exception& e) {
        UEVR_LOG_ERROR("Error in input processing hook: {}", e.what());
    }
}
```

## Configuration Management

### Loading Configuration
```cpp
void MyCustomEngineAdapter::LoadConfiguration(const Config& config) {
    try {
        // Load engine-specific settings
        auto engineSection = config.GetSection("MyCustomEngine");
        
        if (engineSection) {
            // Load VR settings
            if (auto vrSection = engineSection->GetSection("VR")) {
                vrRenderer->SetRenderScale(
                    vrSection->GetFloat("RenderScale", 0.85f)
                );
                vrRenderer->EnableDynamicResolution(
                    vrSection->GetBool("DynamicResolution", true)
                );
            }
            
            // Load input settings
            if (auto inputSection = engineSection->GetSection("Input")) {
                vrInput->SetInputSensitivity(
                    inputSection->GetFloat("Sensitivity", 1.0f)
                );
                vrInput->EnableHaptics(
                    inputSection->GetBool("Haptics", true)
                );
            }
        }
        
    } catch (const std::exception& e) {
        UEVR_LOG_ERROR("Failed to load configuration: {}", e.what());
    }
}
```

### Saving Configuration
```cpp
void MyCustomEngineAdapter::SaveConfiguration(Config& config) const {
    try {
        // Create or get engine section
        auto engineSection = config.GetOrCreateSection("MyCustomEngine");
        
        // Save VR settings
        auto vrSection = engineSection->GetOrCreateSection("VR");
        vrSection->SetFloat("RenderScale", vrRenderer->GetRenderScale());
        vrSection->SetBool("DynamicResolution", vrRenderer->IsDynamicResolutionEnabled());
        
        // Save input settings
        auto inputSection = engineSection->GetOrCreateSection("Input");
        inputSection->SetFloat("Sensitivity", vrInput->GetInputSensitivity());
        inputSection->SetBool("Haptics", vrInput->IsHapticsEnabled());
        
    } catch (const std::exception& e) {
        UEVR_LOG_ERROR("Failed to save configuration: {}", e.what());
    }
}
```

## Error Handling

### Exception Safety
```cpp
class SafeAdapterWrapper {
public:
    template<typename Func>
    auto SafeExecute(Func&& func, const std::string& operation) {
        try {
            return func();
        } catch (const std::exception& e) {
            UEVR_LOG_ERROR("Exception in {}: {}", operation, e.what());
            return std::nullopt;
        } catch (...) {
            UEVR_LOG_ERROR("Unknown exception in {}", operation);
            return std::nullopt;
        }
    }
    
    template<typename Func>
    bool SafeExecuteBool(Func&& func, const std::string& operation) {
        auto result = SafeExecute(std::forward<Func>(func), operation);
        return result.has_value() && result.value();
    }
};
```

### Error Recovery
```cpp
class ErrorRecovery {
public:
    bool AttemptRecovery(const std::string& errorType) {
        if (errorType == "HookFailure") {
            return RetryHookInstallation();
        } else if (errorType == "VRInitFailure") {
            return RetryVRInitialization();
        } else if (errorType == "InputFailure") {
            return RetryInputSetup();
        }
        return false;
    }
    
private:
    bool RetryHookInstallation();
    bool RetryVRInitialization();
    bool RetryInputSetup();
};
```

## Testing Your Adapter

### Unit Tests
```cpp
class MyCustomEngineAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        adapter = std::make_unique<MyCustomEngineAdapter>();
    }
    
    void TearDown() override {
        adapter.reset();
    }
    
    std::unique_ptr<MyCustomEngineAdapter> adapter;
};

TEST_F(MyCustomEngineAdapterTest, InitializationTest) {
    EXPECT_TRUE(adapter->Initialize());
    EXPECT_EQ(adapter->GetEngineName(), "MyCustomEngine");
}

TEST_F(MyCustomEngineAdapterTest, CompatibilityTest) {
    EXPECT_TRUE(adapter->IsCompatible());
}

TEST_F(MyCustomEngineAdapterTest, VRSetupTest) {
    adapter->Initialize();
    EXPECT_TRUE(adapter->SetupVRRendering());
    EXPECT_TRUE(adapter->SetupVRInput());
}
```

### Integration Tests
```cpp
class IntegrationTest {
public:
    bool TestFullWorkflow() {
        // Test complete adapter workflow
        if (!adapter->Initialize()) {
            return false;
        }
        
        if (!adapter->InstallHooks()) {
            return false;
        }
        
        if (!adapter->SetupVRRendering()) {
            return false;
        }
        
        if (!adapter->SetupVRInput()) {
            return false;
        }
        
        // Test VR functionality
        return TestVRFunctionality();
    }
    
private:
    bool TestVRFunctionality();
    std::unique_ptr<MyCustomEngineAdapter> adapter;
};
```

## Performance Considerations

### Optimization Techniques
```cpp
class PerformanceOptimizer {
public:
    void OptimizeAdapter(MyCustomEngineAdapter& adapter) {
        // Enable SIMD optimizations
        EnableSIMDOptimizations();
        
        // Setup memory pooling
        SetupMemoryPools();
        
        // Configure threading
        ConfigureThreading();
    }
    
private:
    void EnableSIMDOptimizations();
    void SetupMemoryPools();
    void ConfigureThreading();
};
```

### Memory Management
```cpp
class MemoryManager {
public:
    template<typename T>
    T* AllocateOptimized() {
        // Use memory pools for frequently allocated objects
        if constexpr (std::is_base_of_v<FrameData, T>) {
            return frameDataPool.Allocate();
        } else if constexpr (std::is_base_of_v<InputData, T>) {
            return inputDataPool.Allocate();
        } else {
            return new T();
        }
    }
    
private:
    ObjectPool<FrameData> frameDataPool;
    ObjectPool<InputData> inputDataPool;
};
```

## Best Practices

### Code Organization
1. **Separation of Concerns**: Keep engine-specific logic separate from VR logic
2. **Error Handling**: Implement comprehensive error handling and recovery
3. **Resource Management**: Use RAII and smart pointers for resource management
4. **Thread Safety**: Ensure thread safety for multi-threaded operations
5. **Performance**: Profile and optimize critical paths

### Testing Strategy
1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test complete workflows
3. **Performance Tests**: Measure performance impact
4. **Compatibility Tests**: Test with different engine versions
5. **Stress Tests**: Test under heavy load conditions

### Documentation
1. **Code Comments**: Document complex logic and algorithms
2. **API Documentation**: Document public interfaces
3. **Examples**: Provide usage examples
4. **Troubleshooting**: Document common issues and solutions

## Example: Unreal Engine Adapter

### Basic Structure
```cpp
class UnrealEngineAdapter : public IEngineAdapter {
public:
    bool Initialize() override {
        // Detect Unreal Engine version
        if (!DetectUnrealEngine()) {
            return false;
        }
        
        // Initialize version-specific components
        return InitializeVersionSpecificComponents();
    }
    
private:
    bool DetectUnrealEngine();
    bool InitializeVersionSpecificComponents();
    
    std::unique_ptr<UE4Adapter> ue4Adapter;
    std::unique_ptr<UE5Adapter> ue5Adapter;
};
```

### Version Detection
```cpp
bool UnrealEngineAdapter::DetectUnrealEngine() {
    // Check for UE4
    if (IsUE4Running()) {
        ue4Adapter = std::make_unique<UE4Adapter>();
        return true;
    }
    
    // Check for UE5
    if (IsUE5Running()) {
        ue5Adapter = std::make_unique<UE5Adapter>();
        return true;
    }
    
    return false;
}
```

## Resources

- [UEVR Architecture Overview](../architecture/system-overview.md)
- [Core Components Documentation](../architecture/core-components.md)
- [Performance Optimization Guide](../performance/overview.md)
- [Testing Framework Documentation](../testing/overview.md)
- [API Reference](../development/api-reference.md)

---

*For additional support and examples, join the UEVR community Discord or check the GitHub repository.*
