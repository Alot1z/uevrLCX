#include "SystemManager.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>

namespace uevrLCX {

SystemManager::SystemManager() 
    : state(SystemState::UNINITIALIZED)
    , shouldShutdown(false)
    , lastMetricsUpdate(std::chrono::steady_clock::now())
    , startTime(std::chrono::steady_clock::now()) {
}

SystemManager::~SystemManager() {
    shutdown();
}

bool SystemManager::initialize() {
    std::lock_guard<std::mutex> lock(threadsMutex);
    
    if (state != SystemState::UNINITIALIZED) {
        return false;
    }
    
    state = SystemState::INITIALIZING;
    
    try {
        // Initialize metrics
        metrics = SystemMetrics{};
        lastMetricsUpdate = std::chrono::steady_clock::now();
        startTime = std::chrono::steady_clock::now();
        
        // Set default configuration
        setConfiguration("max_threads", "16");
        setConfiguration("thread_timeout", "30000");
        setConfiguration("metrics_update_interval", "1000");
        
        state = SystemState::RUNNING;
        return true;
        
    } catch (const std::exception& e) {
        state = SystemState::ERROR;
        if (errorHandler) {
            errorHandler("SystemManager", std::string("Initialization failed: ") + e.what());
        }
        return false;
    }
}

void SystemManager::shutdown() {
    if (state == SystemState::SHUTDOWN) {
        return;
    }
    
    state = SystemState::SHUTTING_DOWN;
    shouldShutdown = true;
    
    // Stop all threads
    {
        std::lock_guard<std::mutex> lock(threadsMutex);
        for (auto& threadInfo : threads) {
            if (threadInfo->thread.joinable()) {
                threadInfo->isRunning = false;
            }
        }
    }
    
    // Wait for threads to finish
    cleanupThreads();
    
    state = SystemState::SHUTDOWN;
}

void SystemManager::update() {
    if (state != SystemState::RUNNING) {
        return;
    }
    
    try {
        updateMetrics();
        cleanupThreads();
        processEvents();
        
    } catch (const std::exception& e) {
        if (errorHandler) {
            errorHandler("SystemManager", std::string("Update failed: ") + e.what());
        }
    }
}

SystemState SystemManager::getState() const {
    return state.load();
}

void SystemManager::setState(SystemState newState) {
    state = newState;
}

bool SystemManager::isRunning() const {
    return state == SystemState::RUNNING;
}

bool SystemManager::isPaused() const {
    return state == SystemState::PAUSED;
}

SystemMetrics SystemManager::getMetrics() const {
    std::lock_guard<std::mutex> lock(threadsMutex);
    return metrics;
}

void SystemManager::resetMetrics() {
    std::lock_guard<std::mutex> lock(threadsMutex);
    metrics = SystemMetrics{};
    lastMetricsUpdate = std::chrono::steady_clock::now();
}

void SystemManager::addThread(std::function<void()> task, SystemPriority priority) {
    if (!task) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(threadsMutex);
    
    auto threadInfo = std::make_unique<ThreadInfo>();
    threadInfo->id = generateThreadId();
    threadInfo->task = std::move(task);
    threadInfo->priority = priority;
    threadInfo->isRunning = true;
    threadInfo->isPaused = false;
    threadInfo->lastActivity = std::chrono::steady_clock::now();
    
    // Start the thread
    threadInfo->thread = std::thread([this, threadInfo = threadInfo.get()]() {
        try {
            while (threadInfo->isRunning && !shouldShutdown) {
                if (!threadInfo->isPaused) {
                    threadInfo->task();
                    threadInfo->lastActivity = std::chrono::steady_clock::now();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        } catch (const std::exception& e) {
            if (errorHandler) {
                errorHandler("SystemManager", std::string("Thread error: ") + e.what());
            }
        }
    });
    
    threads.push_back(std::move(threadInfo));
}

void SystemManager::removeThread(const std::string& threadId) {
    std::lock_guard<std::mutex> lock(threadsMutex);
    
    auto it = std::find_if(threads.begin(), threads.end(),
        [&threadId](const std::unique_ptr<ThreadInfo>& info) {
            return info->id == threadId;
        });
    
    if (it != threads.end()) {
        (*it)->isRunning = false;
        if ((*it)->thread.joinable()) {
            (*it)->thread.join();
        }
        threads.erase(it);
    }
}

void SystemManager::pauseThread(const std::string& threadId) {
    std::lock_guard<std::mutex> lock(threadsMutex);
    
    auto it = std::find_if(threads.begin(), threads.end(),
        [&threadId](const std::unique_ptr<ThreadInfo>& info) {
            return info->id == threadId;
        });
    
    if (it != threads.end()) {
        (*it)->isPaused = true;
    }
}

void SystemManager::resumeThread(const std::string& threadId) {
    std::lock_guard<std::mutex> lock(threadsMutex);
    
    auto it = std::find_if(threads.begin(), threads.end(),
        [&threadId](const std::unique_ptr<ThreadInfo>& info) {
            return info->id == threadId;
        });
    
    if (it != threads.end()) {
        (*it)->isPaused = false;
    }
}

void SystemManager::registerEventCallback(const std::string& eventType, EventCallback callback) {
    std::lock_guard<std::mutex> lock(eventsMutex);
    
    auto it = std::find_if(events.begin(), events.end(),
        [&eventType](const EventInfo& info) {
            return info.type == eventType;
        });
    
    if (it != events.end()) {
        it->callbacks.push_back(std::move(callback));
    } else {
        EventInfo newEvent;
        newEvent.type = eventType;
        newEvent.callbacks.push_back(std::move(callback));
        events.push_back(std::move(newEvent));
    }
}

void SystemManager::unregisterEventCallback(const std::string& eventType) {
    std::lock_guard<std::mutex> lock(eventsMutex);
    
    auto it = std::find_if(events.begin(), events.end(),
        [&eventType](const EventInfo& info) {
            return info.type == eventType;
        });
    
    if (it != events.end()) {
        events.erase(it);
    }
}

void SystemManager::triggerEvent(const std::string& eventType, const void* data) {
    std::lock_guard<std::mutex> lock(eventsMutex);
    
    auto it = std::find_if(events.begin(), events.end(),
        [&eventType](const EventInfo& info) {
            return info.type == eventType;
        });
    
    if (it != events.end()) {
        for (const auto& callback : it->callbacks) {
            try {
                callback(eventType, data);
            } catch (const std::exception& e) {
                if (errorHandler) {
                    errorHandler("SystemManager", std::string("Event callback error: ") + e.what());
                }
            }
        }
    }
}

void SystemManager::setConfiguration(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(configMutex);
    configuration[key] = value;
}

std::string SystemManager::getConfiguration(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    auto it = configuration.find(key);
    if (it != configuration.end()) {
        return it->second;
    }
    return defaultValue;
}

void SystemManager::setErrorHandler(std::function<void(const std::string&, const std::string&)> handler) {
    errorHandler = std::move(handler);
}

void SystemManager::reportError(const std::string& component, const std::string& error) {
    if (errorHandler) {
        errorHandler(component, error);
    }
}

void SystemManager::setLogCallback(std::function<void(const std::string&, const std::string&)> callback) {
    logCallback = std::move(callback);
}

void SystemManager::log(const std::string& level, const std::string& message) {
    if (logCallback) {
        logCallback(level, message);
    }
}

void SystemManager::updateMetrics() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMetricsUpdate).count();
    
    if (elapsed > 1000) { // Update every second
        std::lock_guard<std::mutex> lock(threadsMutex);
        
        // Update frame count and FPS
        metrics.frameCount++;
        metrics.fps = 1000.0 / elapsed;
        
        // Update timestamp
        metrics.lastUpdate = now;
        lastMetricsUpdate = now;
        
        // Simple CPU usage simulation (in real implementation, this would use system APIs)
        static double cpuUsage = 0.0;
        cpuUsage += (std::rand() % 10 - 5) * 0.1;
        cpuUsage = std::max(0.0, std::min(100.0, cpuUsage));
        metrics.cpuUsage = cpuUsage;
        
        // Memory usage simulation
        static double memoryUsage = 50.0;
        memoryUsage += (std::rand() % 6 - 3) * 0.1;
        memoryUsage = std::max(0.0, std::min(100.0, memoryUsage));
        metrics.memoryUsage = memoryUsage;
    }
}

void SystemManager::cleanupThreads() {
    std::lock_guard<std::mutex> lock(threadsMutex);
    
    auto it = threads.begin();
    while (it != threads.end()) {
        if (!(*it)->isRunning && (*it)->thread.joinable()) {
            (*it)->thread.join();
            it = threads.erase(it);
        } else {
            ++it;
        }
    }
}

void SystemManager::processEvents() {
    // Process any pending events
    // This is a placeholder for event processing logic
}

std::string SystemManager::generateThreadId() const {
    static std::atomic<int> counter{0};
    std::ostringstream oss;
    oss << "thread_" << std::setfill('0') << std::setw(4) << counter.fetch_add(1);
    return oss.str();
}

} // namespace uevrLCX
