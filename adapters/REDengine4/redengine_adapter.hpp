#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// REDengine4 specific includes
#include "uobject_hook.hpp"
#include "redengine_memory.hpp"
#include "redengine_renderer.hpp"
#include "redengine_input.hpp"

namespace uevr {
namespace adapters {
namespace redengine4 {

/**
 * @brief REDengine4 Adapter for Cyberpunk 2077
 * 
 * This adapter provides comprehensive integration with REDengine4,
 * including UObject hooking, memory management, and rendering optimization.
 */
class REDengine4Adapter : public ICrossEngineAdapter {
public:
    REDengine4Adapter();
    virtual ~REDengine4Adapter();

    // ICrossEngineAdapter interface implementation
    bool initialize() override;
    bool shutdown() override;
    bool isInitialized() const override;
    std::string getEngineName() const override;
    std::string getEngineVersion() const override;
    bool supportsFeature(EngineFeature feature) const override;
    
    // REDengine4 specific methods
    bool attachToProcess(uint32_t processId);
    bool detachFromProcess();
    bool isAttached() const;
    
    // UObject hooking system
    bool installUObjectHooks();
    bool uninstallUObjectHooks();
    bool isUObjectHooksInstalled() const;
    
    // Memory management
    bool allocateMemory(size_t size, void** address);
    bool freeMemory(void* address);
    bool readMemory(void* address, void* buffer, size_t size);
    bool writeMemory(void* address, const void* buffer, size_t size);
    
    // Rendering optimization
    bool optimizeRendering();
    bool setRenderingQuality(RenderingQuality quality);
    RenderingQuality getCurrentRenderingQuality() const;
    
    // Input handling
    bool captureInput();
    bool releaseInput();
    bool isInputCaptured() const;
    
    // Performance monitoring
    PerformanceMetrics getPerformanceMetrics() const;
    bool startPerformanceMonitoring();
    bool stopPerformanceMonitoring();
    
    // Configuration
    bool loadConfiguration(const std::string& configPath);
    bool saveConfiguration(const std::string& configPath) const;
    bool setConfigurationValue(const std::string& key, const std::string& value);
    std::string getConfigurationValue(const std::string& key) const;

private:
    // Internal implementation details
    bool initializeUObjectSystem();
    bool initializeMemorySystem();
    bool initializeRenderingSystem();
    bool initializeInputSystem();
    
    void cleanupUObjectSystem();
    void cleanupMemorySystem();
    void cleanupRenderingSystem();
    void cleanupInputSystem();
    
    bool validateProcessCompatibility();
    bool setupExceptionHandling();
    bool setupMemoryProtection();
    
    // Member variables
    std::unique_ptr<UObjectHook> m_uobjectHook;
    std::unique_ptr<REDengineMemory> m_memoryManager;
    std::unique_ptr<REDengineRenderer> m_renderer;
    std::unique_ptr<REDengineInput> m_inputManager;
    
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_attached{false};
    std::atomic<bool> m_uobjectHooksInstalled{false};
    std::atomic<bool> m_inputCaptured{false};
    std::atomic<bool> m_performanceMonitoring{false};
    
    uint32_t m_targetProcessId{0};
    void* m_targetProcessHandle{nullptr};
    
    RenderingQuality m_currentRenderingQuality{RenderingQuality::High};
    PerformanceMetrics m_performanceMetrics;
    
    std::unordered_map<std::string, std::string> m_configuration;
    
    mutable std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
    
    // Thread management
    std::thread m_monitoringThread;
    std::atomic<bool> m_shutdownRequested{false};
    
    // Error handling
    std::string m_lastError;
    std::vector<std::string> m_errorLog;
    
    // Constants
    static constexpr size_t MAX_ERROR_LOG_SIZE = 1000;
    static constexpr uint32_t DEFAULT_TIMEOUT_MS = 5000;
};

} // namespace redengine4
} // namespace adapters
} // namespace uevr
