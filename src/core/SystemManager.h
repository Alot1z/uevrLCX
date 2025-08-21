#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

namespace uevrLCX {

enum class SystemState {
    UNINITIALIZED,
    INITIALIZING,
    RUNNING,
    PAUSED,
    SHUTTING_DOWN,
    ERROR,
    SHUTDOWN
};

enum class SystemPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

struct SystemMetrics {
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    double gpuUsage = 0.0;
    uint64_t frameCount = 0;
    double fps = 0.0;
    double latency = 0.0;
    std::chrono::steady_clock::time_point lastUpdate;
};

class SystemManager {
public:
    SystemManager();
    ~SystemManager();

    // Core lifecycle
    bool initialize();
    void shutdown();
    void update();
    
    // State management
    SystemState getState() const;
    void setState(SystemState state);
    bool isRunning() const;
    bool isPaused() const;
    
    // Performance monitoring
    SystemMetrics getMetrics() const;
    void resetMetrics();
    
    // Thread management
    void addThread(std::function<void()> task, SystemPriority priority = SystemPriority::NORMAL);
    void removeThread(const std::string& threadId);
    void pauseThread(const std::string& threadId);
    void resumeThread(const std::string& threadId);
    
    // Event system
    using EventCallback = std::function<void(const std::string&, const void*)>;
    void registerEventCallback(const std::string& eventType, EventCallback callback);
    void unregisterEventCallback(const std::string& eventType);
    void triggerEvent(const std::string& eventType, const void* data = nullptr);
    
    // Configuration
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key, const std::string& defaultValue = "") const;
    
    // Error handling
    void setErrorHandler(std::function<void(const std::string&, const std::string&)> handler);
    void reportError(const std::string& component, const std::string& error);
    
    // Logging
    void setLogCallback(std::function<void(const std::string&, const std::string&)> callback);
    void log(const std::string& level, const std::string& message);

private:
    struct ThreadInfo {
        std::string id;
        std::thread thread;
        std::function<void()> task;
        SystemPriority priority;
        bool isRunning;
        bool isPaused;
        std::chrono::steady_clock::time_point lastActivity;
    };

    struct EventInfo {
        std::string type;
        std::vector<EventCallback> callbacks;
    };

    // Core members
    std::atomic<SystemState> state;
    std::atomic<bool> shouldShutdown;
    SystemMetrics metrics;
    
    // Thread management
    std::vector<std::unique_ptr<ThreadInfo>> threads;
    std::mutex threadsMutex;
    
    // Event system
    std::vector<EventInfo> events;
    std::mutex eventsMutex;
    
    // Configuration
    std::unordered_map<std::string, std::string> configuration;
    mutable std::mutex configMutex;
    
    // Callbacks
    std::function<void(const std::string&, const std::string&)> errorHandler;
    std::function<void(const std::string&, const std::string&)> logCallback;
    
    // Internal methods
    void updateMetrics();
    void cleanupThreads();
    void processEvents();
    std::string generateThreadId() const;
    
    // Performance monitoring
    std::chrono::steady_clock::time_point lastMetricsUpdate;
    std::chrono::steady_clock::time_point startTime;
};

} // namespace uevrLCX
