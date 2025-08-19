# Code Standards

This document defines the coding standards, conventions, and best practices for UEVR development, ensuring code quality, consistency, and maintainability across the project.

## Code Style and Formatting

### General Principles
- **Readability**: Code should be easy to read and understand
- **Consistency**: Follow established patterns throughout the codebase
- **Maintainability**: Write code that is easy to modify and extend
- **Performance**: Consider performance implications of design decisions

### Naming Conventions

#### Classes and Types
```cpp
// Use PascalCase for class names
class VRManager;
class PerformanceMonitor;
class HookManager;

// Use PascalCase for struct names
struct VRConfig;
struct PerformanceMetrics;
struct HookInfo;

// Use PascalCase for enum names
enum class LogLevel;
enum class VRRuntimeType;
enum class HookStatus;

// Use PascalCase for type aliases
using VRFramePtr = std::shared_ptr<VRFrame>;
using ConfigMap = std::unordered_map<std::string, std::any>;
```

#### Functions and Methods
```cpp
// Use PascalCase for function names
bool InitializeVR();
void ShutdownVR();
VRConfig GetConfiguration() const;

// Use PascalCase for method names
class VRManager {
public:
    bool ConnectToRuntime(VRRuntimeType runtimeType);
    void DisconnectFromRuntime();
    bool IsConnected() const;
    
private:
    bool ValidateRuntime(VRRuntimeType runtimeType);
    void LogConnectionStatus(bool connected);
};
```

#### Variables and Members
```cpp
// Use camelCase for variable names
bool isInitialized;
std::string configPath;
uint32_t frameCount;

// Use camelCase for member variables
class VRManager {
private:
    std::unique_ptr<VRRuntime> runtime;
    std::unique_ptr<VRRenderer> renderer;
    bool isConnected;
    VRRuntimeType currentRuntime;
};

// Use UPPER_CASE for constants
const uint32_t MAX_FRAME_RATE = 144;
const std::string DEFAULT_CONFIG_PATH = "config.json";
const float DEFAULT_RENDER_SCALE = 1.0f;
```

#### Namespaces
```cpp
// Use lowercase for namespace names
namespace uevr {
namespace vr {
namespace rendering {
namespace utils {

// Nested namespaces for organization
namespace uevr::core {
    class Framework;
}

namespace uevr::vr::input {
    class InputManager;
}
```

### File Organization

#### Header Files (.h/.hpp)
```cpp
// Standard header guard
#pragma once

// Include order
#include <system_headers>
#include <standard_library>
#include <third_party>
#include "project_headers"

// Forward declarations
namespace uevr {
    class VRManager;
    struct VRConfig;
}

// Class declaration
namespace uevr::vr {
    class VRRenderer {
    public:
        // Public interface
        VRRenderer();
        ~VRRenderer();
        
        bool Initialize(const VRConfig& config);
        void Shutdown();
        bool IsInitialized() const;
        
    private:
        // Private implementation
        bool SetupRendering();
        void CleanupRendering();
        
        // Member variables
        std::unique_ptr<RenderBackend> backend;
        bool isInitialized;
        VRConfig config;
    };
}
```

#### Source Files (.cpp)
```cpp
// Include order
#include "VRRenderer.h"

// Standard library includes
#include <memory>
#include <stdexcept>
#include <chrono>

// Third-party includes
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

// Project includes
#include "RenderBackend.h"
#include "VRConfig.h"
#include "Logger.h"

// Implementation
namespace uevr::vr {

VRRenderer::VRRenderer() : isInitialized(false) {
    // Constructor implementation
}

VRRenderer::~VRRenderer() {
    if (isInitialized) {
        Shutdown();
    }
}

bool VRRenderer::Initialize(const VRConfig& config) {
    try {
        this->config = config;
        
        if (!SetupRendering()) {
            return false;
        }
        
        isInitialized = true;
        Logger::Info("VR renderer initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        Logger::Error("Failed to initialize VR renderer: {}", e.what());
        return false;
    }
}

void VRRenderer::Shutdown() {
    if (isInitialized) {
        CleanupRendering();
        isInitialized = false;
        Logger::Info("VR renderer shut down");
    }
}

bool VRRenderer::IsInitialized() const {
    return isInitialized;
}

bool VRRenderer::SetupRendering() {
    // Implementation details
    return true;
}

void VRRenderer::CleanupRendering() {
    // Cleanup implementation
}

} // namespace uevr::vr
```

## C++ Language Features

### Modern C++ Usage

#### Smart Pointers
```cpp
// Prefer smart pointers over raw pointers
class VRManager {
private:
    // Use unique_ptr for exclusive ownership
    std::unique_ptr<VRRuntime> runtime;
    std::unique_ptr<VRRenderer> renderer;
    
    // Use shared_ptr for shared ownership
    std::shared_ptr<VRInputManager> inputManager;
    
    // Use weak_ptr for non-owning references
    std::weak_ptr<VRMod> currentMod;
};

// Avoid raw pointers for ownership
// ❌ Bad
VRRuntime* runtime = new VRRuntime();

// ✅ Good
auto runtime = std::make_unique<VRRuntime>();
```

#### RAII and Resource Management
```cpp
// Use RAII for resource management
class ScopedLock {
public:
    ScopedLock(std::mutex& mutex) : mutex_(mutex) {
        mutex_.lock();
    }
    
    ~ScopedLock() {
        mutex_.unlock();
    }
    
private:
    std::mutex& mutex_;
};

// Use RAII in functions
void ProcessFrame() {
    ScopedLock lock(frameMutex);
    // Process frame safely
    // Lock automatically released when function exits
}
```

#### Range-based For Loops
```cpp
// Prefer range-based for loops
// ❌ Bad
for (size_t i = 0; i < controllers.size(); ++i) {
    auto& controller = controllers[i];
    // Process controller
}

// ✅ Good
for (const auto& controller : controllers) {
    // Process controller
}

// With index when needed
for (auto [index, controller] : std::views::enumerate(controllers)) {
    // Process controller with index
}
```

#### Structured Bindings
```cpp
// Use structured bindings for multiple return values
auto [success, errorCode] = InitializeVR();
if (!success) {
    HandleError(errorCode);
}

// For map iterations
for (const auto& [key, value] : configMap) {
    ProcessConfig(key, value);
}
```

### Error Handling

#### Exception Safety
```cpp
// Use RAII and smart pointers for exception safety
class SafeResourceManager {
public:
    bool Initialize() {
        try {
            // Allocate resources
            resource1 = std::make_unique<Resource1>();
            resource2 = std::make_unique<Resource2>();
            
            // If any allocation fails, destructors clean up automatically
            return true;
            
        } catch (const std::exception& e) {
            Logger::Error("Initialization failed: {}", e.what());
            return false;
        }
    }
    
private:
    std::unique_ptr<Resource1> resource1;
    std::unique_ptr<Resource2> resource2;
};
```

#### Error Codes vs Exceptions
```cpp
// Use exceptions for exceptional cases
bool InitializeVR() {
    if (!CheckSystemRequirements()) {
        throw std::runtime_error("System requirements not met");
    }
    
    if (!InitializeGraphics()) {
        throw std::runtime_error("Graphics initialization failed");
    }
    
    return true;
}

// Use error codes for expected failures
enum class ConnectionResult {
    Success,
    Timeout,
    InvalidCredentials,
    NetworkError
};

ConnectionResult ConnectToServer(const std::string& address) {
    if (address.empty()) {
        return ConnectionResult::InvalidCredentials;
    }
    
    // Connection logic
    return ConnectionResult::Success;
}
```

### Performance Considerations

#### Move Semantics
```cpp
// Use move semantics to avoid unnecessary copies
class VRFrame {
public:
    // Move constructor
    VRFrame(VRFrame&& other) noexcept
        : data(std::move(other.data))
        , timestamp(other.timestamp) {
        other.timestamp = {};
    }
    
    // Move assignment
    VRFrame& operator=(VRFrame&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            timestamp = other.timestamp;
            other.timestamp = {};
        }
        return *this;
    }
    
private:
    std::vector<uint8_t> data;
    std::chrono::high_resolution_clock::time_point timestamp;
};

// Use std::move when appropriate
VRFrame CreateFrame() {
    VRFrame frame;
    // Fill frame data
    return frame; // RVO/move will be used
}
```

#### Const Correctness
```cpp
// Use const wherever possible
class VRManager {
public:
    // Const methods for non-modifying operations
    bool IsInitialized() const { return isInitialized; }
    const VRConfig& GetConfiguration() const { return config; }
    
    // Non-const methods for modifying operations
    void SetConfiguration(const VRConfig& newConfig) { config = newConfig; }
    
private:
    bool isInitialized;
    VRConfig config;
};

// Use const references for parameters
void ProcessFrame(const VRFrame& frame) {
    // Process frame without modifying it
}

// Use const for local variables that don't change
const auto frameCount = frames.size();
```

## Design Patterns

### Common Patterns

#### Singleton Pattern
```cpp
// Thread-safe singleton implementation
class Framework {
public:
    static Framework& Instance() {
        static Framework instance;
        return instance;
    }
    
    // Delete copy and move operations
    Framework(const Framework&) = delete;
    Framework& operator=(const Framework&) = delete;
    Framework(Framework&&) = delete;
    Framework& operator=(Framework&&) = delete;
    
private:
    Framework() = default;
    ~Framework() = default;
};
```

#### Factory Pattern
```cpp
// Abstract factory for creating renderers
class RendererFactory {
public:
    virtual ~RendererFactory() = default;
    
    virtual std::unique_ptr<IRenderer> CreateRenderer(RendererType type) = 0;
    
    static std::unique_ptr<RendererFactory> CreateFactory();
};

// Concrete factory implementation
class DirectXRendererFactory : public RendererFactory {
public:
    std::unique_ptr<IRenderer> CreateRenderer(RendererType type) override {
        switch (type) {
            case RendererType::DirectX11:
                return std::make_unique<DirectX11Renderer>();
            case RendererType::DirectX12:
                return std::make_unique<DirectX12Renderer>();
            default:
                throw std::invalid_argument("Unsupported renderer type");
        }
    }
};
```

#### Observer Pattern
```cpp
// Event system using observer pattern
class EventDispatcher {
public:
    using EventHandler = std::function<void(const Event&)>;
    
    void RegisterHandler(EventType type, EventHandler handler) {
        handlers[type].push_back(std::move(handler));
    }
    
    void DispatchEvent(const Event& event) {
        auto it = handlers.find(event.type);
        if (it != handlers.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
        }
    }
    
private:
    std::unordered_map<EventType, std::vector<EventHandler>> handlers;
};
```

### Memory Management

#### Memory Pools
```cpp
// Object pool for frequently allocated objects
template<typename T>
class ObjectPool {
public:
    std::unique_ptr<T> Allocate() {
        if (freeObjects.empty()) {
            return std::make_unique<T>();
        }
        
        auto obj = std::move(freeObjects.back());
        freeObjects.pop_back();
        return obj;
    }
    
    void Release(std::unique_ptr<T> obj) {
        if (obj) {
            freeObjects.push_back(std::move(obj));
        }
    }
    
private:
    std::vector<std::unique_ptr<T>> freeObjects;
};
```

#### Resource Management
```cpp
// RAII wrapper for resources
template<typename T, typename Deleter>
class ScopedResource {
public:
    ScopedResource(T resource, Deleter deleter)
        : resource_(resource), deleter_(deleter) {}
    
    ~ScopedResource() {
        if (resource_) {
            deleter_(resource_);
        }
    }
    
    T Get() const { return resource_; }
    T Release() { 
        T temp = resource_;
        resource_ = T{};
        return temp;
    }
    
private:
    T resource_;
    Deleter deleter_;
};

// Usage example
using ScopedVulkanDevice = ScopedResource<VkDevice, decltype(&vkDestroyDevice)>;
```

## Testing Standards

### Unit Testing

#### Test Structure
```cpp
// Use Google Test framework
#include <gtest/gtest.h>
#include "VRManager.h"

class VRManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<VRManager>();
    }
    
    void TearDown() override {
        manager.reset();
    }
    
    std::unique_ptr<VRManager> manager;
};

TEST_F(VRManagerTest, InitializationTest) {
    EXPECT_FALSE(manager->IsInitialized());
    
    EXPECT_TRUE(manager->Initialize());
    EXPECT_TRUE(manager->IsInitialized());
}

TEST_F(VRManagerTest, ShutdownTest) {
    manager->Initialize();
    EXPECT_TRUE(manager->IsInitialized());
    
    manager->Shutdown();
    EXPECT_FALSE(manager->IsInitialized());
}
```

#### Mock Objects
```cpp
// Use Google Mock for mocking
#include <gmock/gmock.h>

class MockVRRuntime : public IVRRuntime {
public:
    MOCK_METHOD(bool, Initialize, (), (override));
    MOCK_METHOD(void, Shutdown, (), (override));
    MOCK_METHOD(bool, IsInitialized, (), (const, override));
};

TEST_F(VRManagerTest, RuntimeIntegration) {
    auto mockRuntime = std::make_shared<MockVRRuntime>();
    
    EXPECT_CALL(*mockRuntime, Initialize())
        .WillOnce(Return(true));
    
    EXPECT_CALL(*mockRuntime, IsInitialized())
        .WillRepeatedly(Return(true));
    
    manager->SetRuntime(mockRuntime);
    EXPECT_TRUE(manager->Initialize());
}
```

### Integration Testing

#### Test Setup
```cpp
class VRIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup complete VR environment
        SetupTestEnvironment();
        
        // Initialize all components
        framework = std::make_unique<Framework>();
        hookManager = std::make_unique<HookManager>();
        vrManager = std::make_unique<VRManager>();
        
        SetupIntegration();
    }
    
    void TearDown() override {
        CleanupIntegration();
        CleanupTestEnvironment();
    }
    
private:
    void SetupTestEnvironment();
    void CleanupTestEnvironment();
    void SetupIntegration();
    void CleanupIntegration();
    
    std::unique_ptr<Framework> framework;
    std::unique_ptr<HookManager> hookManager;
    std::unique_ptr<VRManager> vrManager;
};
```

## Documentation Standards

### Code Comments

#### Function Documentation
```cpp
/**
 * Initializes the VR system with the specified configuration.
 * 
 * This function sets up the VR runtime, renderer, and input systems
 * according to the provided configuration. It performs validation
 * of system requirements and initializes all necessary components.
 * 
 * @param config The VR configuration to use for initialization
 * @return true if initialization was successful, false otherwise
 * 
 * @throws std::runtime_error if system requirements are not met
 * @throws std::invalid_argument if configuration is invalid
 * 
 * @note This function must be called before any VR operations
 * @warning Calling this function multiple times will cause undefined behavior
 * 
 * @see VRConfig, ShutdownVR
 * @since Version 2.0.0
 */
bool InitializeVR(const VRConfig& config);
```

#### Class Documentation
```cpp
/**
 * Manages VR runtime integration and VR-specific functionality.
 * 
 * The VRManager class provides a unified interface for managing
 * VR systems, including runtime connection, rendering, input,
 * and tracking. It supports multiple VR runtimes and provides
 * automatic fallback mechanisms.
 * 
 * @example
 * ```cpp
 * auto vrManager = std::make_unique<VRManager>();
 * if (vrManager->Initialize(config)) {
 *     vrManager->EnableVR();
 *     // Use VR functionality
 *     vrManager->Shutdown();
 * }
 * ```
 * 
 * @thread_safety This class is not thread-safe. All operations
 * must be performed on the same thread.
 * 
 * @see VRRuntime, VRRenderer, VRInputManager
 * @since Version 2.0.0
 */
class VRManager {
    // Implementation
};
```

#### Inline Comments
```cpp
// Use inline comments sparingly and only when necessary
bool ProcessFrame(const VRFrame& frame) {
    // Validate frame data
    if (!ValidateFrame(frame)) {
        return false;
    }
    
    // Apply VR transformations
    auto transformedFrame = ApplyVRTransformations(frame);
    
    // Submit to renderer
    return renderer->SubmitFrame(transformedFrame);
}
```

### API Documentation

#### Public Interface
```cpp
/**
 * @brief VR Management API
 * 
 * This module provides comprehensive VR functionality including:
 * - Runtime management (SteamVR, OpenXR)
 * - Rendering and display
 * - Input handling and mapping
 * - Performance monitoring
 * 
 * @defgroup vr VR Management
 * @{
 */

/**
 * @brief Initialize VR system
 * @param config VR configuration
 * @return true on success
 * @ingroup vr
 */
bool InitializeVR(const VRConfig& config);

/**
 * @brief Shutdown VR system
 * @ingroup vr
 */
void ShutdownVR();

/** @} */
```

## Quality Assurance

### Code Review Checklist

#### Functionality
- [ ] Code implements the intended functionality
- [ ] Error handling is appropriate and complete
- [ ] Edge cases are handled correctly
- [ ] Performance considerations are addressed

#### Code Quality
- [ ] Code follows established conventions
- [ ] Functions are appropriately sized and focused
- [ ] Variable names are clear and descriptive
- [ ] Comments explain why, not what

#### Testing
- [ ] Unit tests cover new functionality
- [ ] Integration tests verify component interaction
- [ ] Performance tests validate performance requirements
- [ ] Edge case tests cover boundary conditions

#### Documentation
- [ ] Public APIs are documented
- [ ] Complex algorithms have explanatory comments
- [ ] Examples demonstrate proper usage
- [ ] Documentation is up to date

### Static Analysis

#### Tools
```bash
# Clang-Tidy for static analysis
clang-tidy src/**/*.cpp -checks=*,-fuchsia-*,-google-*,-zircon-*,-abseil-duration-*,-abseil-time-*,-modernize-use-trailing-return-type

# Cppcheck for additional checks
cppcheck --enable=all --std=c++20 src/ include/

# Clang-format for code formatting
clang-format -i src/**/*.cpp include/**/*.h
```

#### Configuration
```yaml
# .clang-tidy configuration
Checks: >
  *,
  -fuchsia-*,
  -google-*,
  -zircon-*,
  -abseil-duration-*,
  -abseil-time-*,
  -modernize-use-trailing-return-type,
  -llvmlibc-*,
  -altera-*,
  -z3-*,
  -fuchsia-*,
  -google-*,
  -zircon-*,
  -abseil-duration-*,
  -abseil-time-*,
  -modernize-use-trailing-return-type
WarningsAsErrors: ''
HeaderFilterRegex: ''
AnalyzeTemporaryDtors: false
FormatStyle: none
CheckOptions:
  - key: readability-identifier-naming.ClassCase
    value: CamelCase
  - key: readability-identifier-naming.FunctionCase
    value: camelBack
  - key: readability-identifier-naming.VariableCase
    value: camelBack
```

### Performance Guidelines

#### Optimization Principles
```cpp
// 1. Measure before optimizing
// Use profiling tools to identify bottlenecks

// 2. Optimize algorithms first
// Choose efficient algorithms and data structures

// 3. Consider memory layout
// Use cache-friendly data structures

// 4. Profile and iterate
// Measure improvements and continue optimizing

// Example: Optimized vector iteration
class OptimizedProcessor {
public:
    void ProcessFrames(const std::vector<VRFrame>& frames) {
        // Reserve capacity to avoid reallocations
        processedFrames.reserve(frames.size());
        
        // Use range-based for loop for better performance
        for (const auto& frame : frames) {
            if (ShouldProcessFrame(frame)) {
                processedFrames.push_back(ProcessFrame(frame));
            }
        }
    }
    
private:
    std::vector<ProcessedFrame> processedFrames;
    
    bool ShouldProcessFrame(const VRFrame& frame) const {
        // Early exit for invalid frames
        return frame.IsValid() && frame.HasData();
    }
    
    ProcessedFrame ProcessFrame(const VRFrame& frame) {
        // Process frame efficiently
        return ProcessedFrame(frame);
    }
};
```

## Resources

- [UEVR Development Setup](setup.md)
- [Testing Framework](../testing/overview.md)
- [API Reference](api-reference.md)
- [Performance Optimization](../performance/overview.md)
- [Contributing Guidelines](../../CONTRIBUTING.md)

---

*For questions about code standards, consult the development team or check the GitHub repository.*
