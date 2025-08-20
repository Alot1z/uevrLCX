# Cross-Engine Memory Management Protocols

## Overview
This document defines the universal memory management protocols that all cross-engine VR adapters must implement. These protocols ensure safe, stable, and efficient memory hooking across different game engines while maintaining compatibility with uevr's plugin architecture.

## Core Memory Management Principles

### 1. Safety First Protocol
- **No Direct Memory Writes**: All memory modifications must go through validated hook mechanisms
- **Address Validation**: Verify all memory addresses before use
- **Fallback Mechanisms**: Multiple hook strategies for reliability
- **Performance Monitoring**: Track hook performance impact

### 2. Engine-Agnostic Design
- **Common Interface**: Unified memory management across all engines
- **Pattern Recognition**: Engine-specific memory signature detection
- **Resource Pooling**: Efficient resource management across frames
- **Garbage Collection**: Proper cleanup of temporary resources

### 3. uevr Integration
- **Plugin Architecture**: Extends uevr's plugin system
- **Event Hooks**: Integrates with uevr's rendering and device events
- **Resource Management**: Uses uevr's resource handling framework
- **Performance Monitoring**: Leverages uevr's built-in performance tracking

## Memory Hook Architecture

### Primary Hook Types

#### 1. D3D11 Device Hooks
```cpp
// Hook into D3D11CreateDeviceAndSwapChain
typedef HRESULT(WINAPI* D3D11CreateDeviceAndSwapChain_t)(
    IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
    const D3D_FEATURE_LEVEL*, UINT, UINT,
    const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**,
    ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

// Hook into Present calls
typedef HRESULT(STDMETHODCALLTYPE* Present_t)(
    IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
```

**Purpose**: Capture D3D11 device and context for VR rendering
**Integration**: Modify swap chain creation for VR compatibility

#### 2. D3D12 Command List Hooks
```cpp
// Hook into D3D12CreateCommandList
typedef HRESULT(WINAPI* D3D12CreateCommandList_t)(
    ID3D12Device* pDevice,
    D3D12_COMMAND_LIST_TYPE type,
    ID3D12CommandAllocator* pCommandAllocator,
    ID3D12PipelineState* pInitialState,
    REFIID riid,
    void** ppCommandList);

// Hook into ExecuteCommandLists
typedef void(STDMETHODCALLTYPE* ExecuteCommandLists_t)(
    ID3D12CommandQueue* pCommandQueue,
    UINT NumCommandLists,
    ID3D12CommandList* const* ppCommandLists);
```

**Purpose**: Capture D3D12 command lists for VR rendering
**Integration**: Modify command list execution for stereo rendering

#### 3. Camera System Hooks
```cpp
// Hook into engine camera update functions
typedef void(*CameraUpdate_t)(void* camera, float deltaTime);

// Hook into view/projection matrix calculations
typedef void(*MatrixUpdate_t)(void* matrices, void* camera);
```

**Purpose**: Transform engine camera systems for VR
**Integration**: Convert flat screen camera to VR head-tracked camera

### Secondary Hook Types

#### 4. Scene Graph Hooks
```cpp
// Hook into scene traversal
typedef void(*SceneTraverse_t)(void* scene, void* context);

// Hook into rendering pipeline
typedef void(*RenderScene_t)(void* scene, void* context);
```

**Purpose**: Optimize rendering for VR performance
**Integration**: Handle VR-specific culling and LOD

#### 5. Input System Hooks
```cpp
// Hook into input handling
typedef void(*InputProcess_t)(void* input, void* context);

// Hook into controller input
typedef void(*ControllerInput_t)(void* controller, void* input);
```

**Purpose**: Map VR controller inputs to game actions
**Integration**: Implement VR-specific input methods

## Pattern Scanning Strategy

### Memory Signature Recognition

#### 1. Pattern Definition
```cpp
struct MemoryPattern {
    const char* pattern;      // Memory pattern to search for
    const char* mask;         // Pattern mask (x = exact, ? = wildcard)
    size_t offset;            // Offset from pattern match
    size_t size;              // Size of target data
    bool required;            // Whether this pattern is required
};

// Example patterns for different engines
static const MemoryPattern RE_ENGINE_PATTERNS[] = {
    {"\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x00", "xxx????xxx?x", 0, 64, true},
    {"\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x10\x0D", "xxxx????xxxx", 64, 64, true}
};

static const MemoryPattern REDENGINE4_PATTERNS[] = {
    {"\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x00", "xxx????xxx?x", 0, 64, true},
    {"\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x10\x0D", "xxxx????xxxx", 64, 64, true}
};

static const MemoryPattern MT_FRAMEWORK_PATTERNS[] = {
    {"\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x00", "xxx????xxx?x", 0, 64, true},
    {"\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x10\x0D", "xxxx????xxxx", 64, 64, true}
};
```

#### 2. Pattern Scanning Implementation
```cpp
class MemoryScanner {
private:
    std::vector<MemoryPattern> m_patterns;
    std::unordered_map<std::string, uintptr_t> m_addresses;

public:
    // Scan memory for patterns
    bool scanMemory(HANDLE process, uintptr_t start, size_t size) {
        std::vector<uint8_t> buffer(size);
        
        if (!ReadProcessMemory(process, (LPCVOID)start, buffer.data(), size, nullptr)) {
            return false;
        }

        for (const auto& pattern : m_patterns) {
            uintptr_t address = findPattern(buffer, pattern, start);
            if (address != 0) {
                m_addresses[pattern.pattern] = address;
            }
        }

        return true;
    }

    // Find pattern in memory buffer
    uintptr_t findPattern(const std::vector<uint8_t>& buffer, 
                         const MemoryPattern& pattern, 
                         uintptr_t baseAddress) {
        for (size_t i = 0; i <= buffer.size() - pattern.size; i++) {
            bool match = true;
            for (size_t j = 0; j < pattern.size; j++) {
                if (pattern.mask[j] == 'x' && buffer[i + j] != pattern.pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                return baseAddress + i + pattern.offset;
            }
        }
        return 0;
    }

    // Get address for pattern
    uintptr_t getAddress(const std::string& pattern) const {
        auto it = m_addresses.find(pattern);
        return (it != m_addresses.end()) ? it->second : 0;
    }
};
```

## Address Validation System

### Validation Protocols

#### 1. Address Range Validation
```cpp
class AddressValidator {
private:
    uintptr_t m_baseAddress;
    size_t m_moduleSize;
    std::vector<std::pair<uintptr_t, size_t>> m_validRanges;

public:
    // Validate address is within valid ranges
    bool isValidAddress(uintptr_t address) const {
        // Check if address is within module bounds
        if (address < m_baseAddress || address >= m_baseAddress + m_moduleSize) {
            return false;
        }

        // Check if address is within valid memory ranges
        for (const auto& range : m_validRanges) {
            if (address >= range.first && address < range.first + range.second) {
                return true;
            }
        }

        return false;
    }

    // Add valid memory range
    void addValidRange(uintptr_t start, size_t size) {
        m_validRanges.emplace_back(start, size);
    }

    // Validate memory access
    bool canReadMemory(uintptr_t address, size_t size) const {
        return isValidAddress(address) && isValidAddress(address + size - 1);
    }
};
```

#### 2. Memory Access Validation
```cpp
class MemoryAccessValidator {
private:
    AddressValidator m_validator;
    std::unordered_set<uintptr_t> m_accessedAddresses;

public:
    // Validate memory read
    bool validateRead(uintptr_t address, size_t size) {
        if (!m_validator.canReadMemory(address, size)) {
            return false;
        }

        // Track accessed addresses for debugging
        m_accessedAddresses.insert(address);
        return true;
    }

    // Validate memory write
    bool validateWrite(uintptr_t address, size_t size) {
        if (!m_validator.canReadMemory(address, size)) {
            return false;
        }

        // Additional validation for write operations
        if (isReadOnlyMemory(address)) {
            return false;
        }

        m_accessedAddresses.insert(address);
        return true;
    }

    // Check if memory is read-only
    bool isReadOnlyMemory(uintptr_t address) const {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi)) == 0) {
            return true; // Assume read-only if query fails
        }

        return (mbi.Protect & PAGE_READWRITE) == 0;
    }
};
```

## Fallback Hook Mechanisms

### Multiple Hook Strategies

#### 1. Primary Hook Strategy
```cpp
class PrimaryHookStrategy {
private:
    std::vector<std::unique_ptr<IHook>> m_hooks;
    bool m_primaryHooksActive;

public:
    // Install primary hooks
    bool installPrimaryHooks() {
        for (auto& hook : m_hooks) {
            if (!hook->install()) {
                return false;
            }
        }
        m_primaryHooksActive = true;
        return true;
    }

    // Uninstall primary hooks
    void uninstallPrimaryHooks() {
        for (auto& hook : m_hooks) {
            hook->uninstall();
        }
        m_primaryHooksActive = false;
    }

    // Check if primary hooks are active
    bool arePrimaryHooksActive() const {
        return m_primaryHooksActive;
    }
};
```

#### 2. Fallback Hook Strategy
```cpp
class FallbackHookStrategy {
private:
    std::vector<std::unique_ptr<IHook>> m_fallbackHooks;
    bool m_fallbackHooksActive;

public:
    // Install fallback hooks when primary hooks fail
    bool installFallbackHooks() {
        for (auto& hook : m_fallbackHooks) {
            if (!hook->install()) {
                return false;
            }
        }
        m_fallbackHooksActive = true;
        return true;
    }

    // Uninstall fallback hooks
    void uninstallFallbackHooks() {
        for (auto& hook : m_fallbackHooks) {
            hook->uninstall();
        }
        m_fallbackHooksActive = false;
    }

    // Check if fallback hooks are active
    bool areFallbackHooksActive() const {
        return m_fallbackHooksActive;
    }
};
```

#### 3. Hook Strategy Manager
```cpp
class HookStrategyManager {
private:
    std::unique_ptr<PrimaryHookStrategy> m_primaryStrategy;
    std::unique_ptr<FallbackHookStrategy> m_fallbackStrategy;
    bool m_hooksInstalled;

public:
    // Install hooks with fallback
    bool installHooks() {
        // Try primary hooks first
        if (m_primaryStrategy->installPrimaryHooks()) {
            m_hooksInstalled = true;
            return true;
        }

        // If primary hooks fail, try fallback hooks
        if (m_fallbackStrategy->installFallbackHooks()) {
            m_hooksInstalled = true;
            return true;
        }

        return false;
    }

    // Uninstall all hooks
    void uninstallHooks() {
        m_primaryStrategy->uninstallPrimaryHooks();
        m_fallbackStrategy->uninstallFallbackHooks();
        m_hooksInstalled = false;
    }

    // Check if any hooks are active
    bool areHooksActive() const {
        return m_hooksInstalled;
    }
};
```

## Resource Management

### Resource Pooling

#### 1. VR Resource Pool
```cpp
class VRResourcePool {
private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<IVRResource>>> m_resourcePools;
    std::mutex m_poolMutex;

public:
    // Get resource from pool
    std::shared_ptr<IVRResource> getResource(const std::string& type) {
        std::lock_guard<std::mutex> lock(m_poolMutex);
        
        auto& pool = m_resourcePools[type];
        if (!pool.empty()) {
            auto resource = pool.back();
            pool.pop_back();
            return resource;
        }

        // Create new resource if pool is empty
        return createResource(type);
    }

    // Return resource to pool
    void returnResource(const std::string& type, std::shared_ptr<IVRResource> resource) {
        std::lock_guard<std::mutex> lock(m_poolMutex);
        
        if (resource) {
            resource->reset();
            m_resourcePools[type].push_back(resource);
        }
    }

    // Create new resource
    std::shared_ptr<IVRResource> createResource(const std::string& type) {
        // Implementation depends on resource type
        return nullptr;
    }
};
```

#### 2. Memory Mapping
```cpp
class MemoryMapper {
private:
    std::unordered_map<uintptr_t, std::pair<size_t, void*>> m_mappedRegions;
    std::mutex m_mapperMutex;

public:
    // Map memory region
    void* mapMemory(uintptr_t address, size_t size) {
        std::lock_guard<std::mutex> lock(m_mapperMutex);
        
        auto it = m_mappedRegions.find(address);
        if (it != m_mappedRegions.end()) {
            // Return existing mapping
            return it->second.second;
        }

        // Create new mapping
        void* mappedAddress = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (mappedAddress) {
            m_mappedRegions[address] = std::make_pair(size, mappedAddress);
        }

        return mappedAddress;
    }

    // Unmap memory region
    void unmapMemory(uintptr_t address) {
        std::lock_guard<std::mutex> lock(m_mapperMutex);
        
        auto it = m_mappedRegions.find(address);
        if (it != m_mappedRegions.end()) {
            VirtualFree(it->second.second, 0, MEM_RELEASE);
            m_mappedRegions.erase(it);
        }
    }

    // Get mapped address
    void* getMappedAddress(uintptr_t address) const {
        std::lock_guard<std::mutex> lock(m_mapperMutex);
        
        auto it = m_mappedRegions.find(address);
        return (it != m_mappedRegions.end()) ? it->second.second : nullptr;
    }
};
```

## Performance Monitoring

### Hook Performance Tracking

#### 1. Performance Metrics
```cpp
struct HookPerformanceMetrics {
    uint64_t totalCalls;
    uint64_t totalTime;
    uint64_t minTime;
    uint64_t maxTime;
    uint64_t lastCallTime;
    
    // Calculate average time
    double getAverageTime() const {
        return (totalCalls > 0) ? (double)totalTime / totalCalls : 0.0;
    }
    
    // Reset metrics
    void reset() {
        totalCalls = 0;
        totalTime = 0;
        minTime = UINT64_MAX;
        maxTime = 0;
        lastCallTime = 0;
    }
};
```

#### 2. Performance Monitor
```cpp
class PerformanceMonitor {
private:
    std::unordered_map<std::string, HookPerformanceMetrics> m_metrics;
    std::mutex m_monitorMutex;
    LARGE_INTEGER m_frequency;

public:
    PerformanceMonitor() {
        QueryPerformanceFrequency(&m_frequency);
    }

    // Start performance measurement
    uint64_t startMeasurement() {
        LARGE_INTEGER start;
        QueryPerformanceCounter(&start);
        return start.QuadPart;
    }

    // End performance measurement
    void endMeasurement(const std::string& hookName, uint64_t startTime) {
        LARGE_INTEGER end;
        QueryPerformanceCounter(&end);
        
        uint64_t elapsed = end.QuadPart - startTime;
        uint64_t elapsedMicroseconds = (elapsed * 1000000) / m_frequency.QuadPart;

        std::lock_guard<std::mutex> lock(m_monitorMutex);
        
        auto& metrics = m_metrics[hookName];
        metrics.totalCalls++;
        metrics.totalTime += elapsedMicroseconds;
        metrics.minTime = std::min(metrics.minTime, elapsedMicroseconds);
        metrics.maxTime = std::max(metrics.maxTime, elapsedMicroseconds);
        metrics.lastCallTime = elapsedMicroseconds;
    }

    // Get performance metrics
    HookPerformanceMetrics getMetrics(const std::string& hookName) const {
        std::lock_guard<std::mutex> lock(m_monitorMutex);
        
        auto it = m_metrics.find(hookName);
        return (it != m_metrics.end()) ? it->second : HookPerformanceMetrics{};
    }

    // Get all metrics
    std::unordered_map<std::string, HookPerformanceMetrics> getAllMetrics() const {
        std::lock_guard<std::mutex> lock(m_monitorMutex);
        return m_metrics;
    }
};
```

## Integration with uevr

### Plugin Integration

#### 1. uevr Plugin Extension
```cpp
class CrossEngineMemoryManager : public uevr::Plugin {
private:
    std::unique_ptr<MemoryScanner> m_scanner;
    std::unique_ptr<MemoryAccessValidator> m_validator;
    std::unique_ptr<HookStrategyManager> m_hookManager;
    std::unique_ptr<VRResourcePool> m_resourcePool;
    std::unique_ptr<MemoryMapper> m_mapper;
    std::unique_ptr<PerformanceMonitor> m_performanceMonitor;

public:
    CrossEngineMemoryManager() : uevr::Plugin() {
        m_scanner = std::make_unique<MemoryScanner>();
        m_validator = std::make_unique<MemoryAccessValidator>();
        m_hookManager = std::make_unique<HookStrategyManager>();
        m_resourcePool = std::make_unique<VRResourcePool>();
        m_mapper = std::make_unique<MemoryMapper>();
        m_performanceMonitor = std::make_unique<PerformanceMonitor>();
    }

    // Plugin initialization
    virtual void on_initialize() override {
        // Initialize memory management system
        if (!initializeMemoryManagement()) {
            return;
        }

        // Install hooks
        if (!m_hookManager->installHooks()) {
            return;
        }
    }

    // Plugin cleanup
    virtual ~CrossEngineMemoryManager() {
        if (m_hookManager) {
            m_hookManager->uninstallHooks();
        }
    }

private:
    // Initialize memory management
    bool initializeMemoryManagement() {
        // Initialize scanner with engine-specific patterns
        if (!initializeScanner()) {
            return false;
        }

        // Initialize validator with valid memory ranges
        if (!initializeValidator()) {
            return false;
        }

        return true;
    }

    // Initialize scanner with engine patterns
    bool initializeScanner() {
        // Add engine-specific patterns based on detected engine
        if (detectEngine() == "RE_Engine") {
            // Add RE Engine patterns
        } else if (detectEngine() == "REDengine4") {
            // Add REDengine 4 patterns
        } else if (detectEngine() == "MT_Framework") {
            // Add MT Framework patterns
        }

        return true;
    }

    // Initialize validator with memory ranges
    bool initializeValidator() {
        // Add valid memory ranges for detected engine
        return true;
    }

    // Detect game engine
    std::string detectEngine() {
        // Implementation to detect game engine
        return "Unknown";
    }
};
```

## Usage Examples

### Engine-Specific Implementation

#### 1. RE Engine Implementation
```cpp
class RE_Engine_MemoryManager : public CrossEngineMemoryManager {
public:
    RE_Engine_MemoryManager() : CrossEngineMemoryManager() {
        // RE Engine specific initialization
    }

private:
    bool initializeScanner() override {
        // Add RE Engine specific patterns
        return true;
    }
};
```

#### 2. REDengine 4 Implementation
```cpp
class REDengine4_MemoryManager : public CrossEngineMemoryManager {
public:
    REDengine4_MemoryManager() : CrossEngineMemoryManager() {
        // REDengine 4 specific initialization
    }

private:
    bool initializeScanner() override {
        // Add REDengine 4 specific patterns
        return true;
    }
};
```

#### 3. MT Framework Implementation
```cpp
class MT_Framework_MemoryManager : public CrossEngineMemoryManager {
public:
    MT_Framework_MemoryManager() : CrossEngineMemoryManager() {
        // MT Framework specific initialization
    }

private:
    bool initializeScanner() override {
        // Add MT Framework specific patterns
        return true;
    }
};
```

## Best Practices

### 1. Hook Installation
- Always validate addresses before installing hooks
- Use fallback strategies when primary hooks fail
- Monitor hook performance and stability

### 2. Memory Access
- Never write directly to game memory
- Always validate memory access through hooks
- Use proper error handling for memory operations

### 3. Resource Management
- Pool VR resources for efficiency
- Properly clean up resources when not needed
- Monitor memory usage and performance

### 4. Performance
- Track hook performance metrics
- Optimize hook execution time
- Use efficient memory scanning algorithms

## Conclusion

The Cross-Engine Memory Management Protocols provide a robust, safe, and efficient foundation for memory hooking across different game engines. By following these protocols, cross-engine VR adapters can ensure stable performance and compatibility while maintaining the safety and reliability required for production use.

These protocols integrate seamlessly with uevr's plugin architecture and provide the foundation for future cross-engine VR development beyond the three target games.
