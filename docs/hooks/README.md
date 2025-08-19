# UEVR Universal Hooking Patterns

This directory contains documentation for universal hooking patterns and safety mechanisms used across all engine types in the UEVR system.

## Universal Hooking Architecture

### Core Principles
- **Safety First**: All hooks implement safety checks and fallbacks
- **Performance**: Minimal overhead hooks with efficient execution
- **Compatibility**: Cross-engine patterns that work with multiple APIs
- **Maintainability**: Clean, documented hook implementations

### Supported APIs
- **DirectX 11**: Swapchain, device context, and resource hooks
- **DirectX 12**: Command list, resource, and swapchain hooks
- **Vulkan**: Command buffer, swapchain, and render pass hooks
- **OpenGL**: Context, framebuffer, and shader program hooks

## Hook Types & Patterns

### 1. Swapchain Hooks
```cpp
// Universal swapchain hook pattern
template<typename T>
class SwapchainHook {
    T* m_originalFunction;
    T* m_hookFunction;
    
public:
    bool install(T* target, T* hook);
    void uninstall();
    T* getOriginal() const { return m_originalFunction; }
};
```

**Common Hook Points:**
- `Present` (D3D11) - Post-render frame injection
- `Present1` (D3D11) - Enhanced present with sync interval
- `ExecuteCommandLists` (D3D12) - Command list execution
- `vkQueuePresentKHR` (Vulkan) - Queue presentation

### 2. Rendering Pipeline Hooks
```cpp
// Universal render hook pattern
class RenderHook {
    std::function<void(void*)> m_preCallback;
    std::function<void(void*)> m_postCallback;
    
public:
    void setPreCallback(std::function<void(void*)> callback);
    void setPostCallback(std::function<void(void*)> callback);
    void execute(void* context);
};
```

**Hook Points:**
- Pre-render: Camera setup, matrix preparation
- Post-render: VR frame injection, stereo rendering
- Frame end: Performance monitoring, cleanup

### 3. Input System Hooks
```cpp
// Universal input hook pattern
class InputHook {
    std::unordered_map<std::string, std::function<bool(void*)>> m_inputHandlers;
    
public:
    void registerHandler(const std::string& input, std::function<bool(void*)> handler);
    bool processInput(const std::string& input, void* data);
};
```

**Hook Points:**
- Mouse input: Camera control, aiming
- Keyboard input: VR menu, system controls
- Gamepad input: Motion controller emulation

## Safety Mechanisms

### 1. Hook Validation
```cpp
class HookValidator {
public:
    static bool validateHookPoint(void* address, size_t size);
    static bool validateFunctionSignature(void* function, const std::string& expected);
    static bool checkMemoryProtection(void* address);
};
```

**Validation Steps:**
- Address range verification
- Function signature validation
- Memory protection checks
- Hook point stability testing

### 2. Error Handling & Recovery
```cpp
class HookErrorHandler {
public:
    enum class ErrorType {
        HOOK_FAILED,
        FUNCTION_NOT_FOUND,
        MEMORY_ACCESS_VIOLATION,
        INVALID_SIGNATURE
    };
    
    void handleError(ErrorType type, const std::string& details);
    bool attemptRecovery(ErrorType type);
    void logError(const std::string& error);
};
```

**Recovery Strategies:**
- Automatic hook retry with exponential backoff
- Fallback to alternative hook points
- Graceful degradation of VR features
- Safe mode activation

### 3. Blacklist & Kill-Switch
```cpp
class SafetyManager {
    std::unordered_set<std::string> m_blacklistedGames;
    std::unordered_set<std::string> m_blacklistedProcesses;
    bool m_killSwitchEnabled;
    
public:
    bool isGameBlacklisted(const std::string& gameName);
    bool isProcessBlacklisted(const std::string& processName);
    void enableKillSwitch();
    void disableKillSwitch();
};
```

**Safety Features:**
- Game-specific blacklists for problematic titles
- Process-level blocking for anti-cheat conflicts
- Global kill-switch for emergency situations
- Offline-only mode for unknown builds

## Engine-Specific Hook Patterns

### RE Engine
```cpp
// RE Engine specific hook pattern
class REEngineHook : public RenderHook {
private:
    uintptr_t m_cameraMatrixAddress;
    uintptr_t m_fovAddress;
    
public:
    bool hookCameraSystem();
    bool hookFOVSystem();
    bool hookRenderingPipeline();
};
```

**Hook Points:**
- Camera matrix extraction from constant buffers
- FOV modification for VR comfort
- Post-process injection for stereo rendering

### REDengine 4
```cpp
// REDengine 4 specific hook pattern
class REDengine4Hook : public RenderHook {
private:
    uintptr_t m_neuralUpscalingAddress;
    uintptr_t m_rayTracingAddress;
    
public:
    bool hookNeuralUpscaling();
    bool hookRayTracing();
    bool hookTemporalUpsampling();
};
```

**Hook Points:**
- Neural network upscaling pipeline
- Ray-tracing to rasterization fallback
- Temporal upsampling integration

### Unreal Engine
```cpp
// Unreal Engine specific hook pattern
class UnrealEngineHook : public RenderHook {
private:
    uintptr_t m_uObjectArray;
    uintptr_t m_renderThread;
    
public:
    bool hookUObjectSystem();
    bool hookRenderThread();
    bool hookBlueprintSystem();
};
```

**Hook Points:**
- UObject array manipulation
- Render thread synchronization
- Blueprint function injection

## Performance Optimization

### 1. Hook Overhead Minimization
```cpp
// Low-overhead hook implementation
class FastHook {
private:
    alignas(64) uint8_t m_trampoline[64];
    
public:
    bool install(void* target, void* hook);
    void* getTrampoline() const { return m_trampoline; }
};
```

**Optimization Techniques:**
- Aligned trampoline memory for cache efficiency
- Minimal instruction overhead
- Branch prediction friendly code paths
- SIMD-optimized matrix operations

### 2. Batch Hook Processing
```cpp
// Batch hook processing for multiple targets
class BatchHookProcessor {
    std::vector<std::pair<void*, void*>> m_hookPairs;
    
public:
    void addHook(void* target, void* hook);
    bool processAllHooks();
    void clearHooks();
};
```

**Benefits:**
- Reduced context switching overhead
- Batch memory allocation
- Optimized validation checks
- Parallel hook installation

## Testing & Validation

### 1. Hook Testing Framework
```cpp
class HookTestFramework {
public:
    bool testHookInstallation(void* target, void* hook);
    bool testHookExecution(void* target, void* testData);
    bool testHookRemoval(void* target);
    bool benchmarkHookPerformance(void* target, void* hook);
};
```

**Test Categories:**
- Installation success rate
- Execution correctness
- Performance impact measurement
- Memory leak detection
- Crash prevention validation

### 2. Automated Validation
```cpp
class AutomatedHookValidator {
public:
    bool validateAllHooks();
    bool runCompatibilityTests();
    bool generateValidationReport();
    bool detectHookConflicts();
};
```

**Validation Steps:**
- Hook point stability testing
- Cross-engine compatibility verification
- Performance regression detection
- Memory usage monitoring

## Best Practices

### 1. Hook Design
- Keep hooks lightweight and focused
- Implement proper error handling
- Use RAII for resource management
- Document all hook behaviors

### 2. Safety Implementation
- Always validate hook points before installation
- Implement graceful fallbacks for failures
- Monitor hook stability during runtime
- Provide user feedback for hook status

### 3. Performance Considerations
- Minimize hook execution overhead
- Use efficient data structures
- Implement caching where appropriate
- Profile and optimize critical paths

## Resources

- [Engine Specifications](../engines/) - Per-engine hook requirements
- [Rendering Documentation](../rendering/) - Graphics API integration
- [Validation Tools](../validation/) - Testing and quality assurance
- [Deployment Guide](../deployment/) - Production deployment guidelines
