#include "PerformanceMonitor.h"
#include "LoggingSystem.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <numeric>

namespace uevrLCX {
namespace core {

class PerformanceMonitorImpl {
private:
    std::shared_ptr<LoggingSystem> logger;
    bool isInitialized;
    std::atomic<bool> isRunning;
    std::thread monitorThread;
    
    // Performance metrics
    std::atomic<double> currentFPS;
    std::atomic<double> averageFPS;
    std::atomic<double> frameTime;
    std::atomic<double> latency;
    std::atomic<double> cpuUsage;
    std::atomic<double> gpuUsage;
    std::atomic<double> memoryUsage;
    
    // Historical data
    std::vector<double> fpsHistory;
    std::vector<double> frameTimeHistory;
    std::vector<double> latencyHistory;
    std::vector<double> cpuHistory;
    std::vector<double> gpuHistory;
    std::vector<double> memoryHistory;
    
    // Configuration
    size_t maxHistorySize;
    std::chrono::milliseconds updateInterval;

public:
    PerformanceMonitorImpl() : isInitialized(false), isRunning(false), 
                              currentFPS(0.0), averageFPS(0.0), frameTime(0.0), 
                              latency(0.0), cpuUsage(0.0), gpuUsage(0.0), memoryUsage(0.0),
                              maxHistorySize(1000), updateInterval(std::chrono::milliseconds(100)) {
        logger = std::make_shared<LoggingSystem>();
        logger->log(LogLevel::INFO, "PerformanceMonitor initialized");
    }

    bool initialize() {
        try {
            if (isInitialized) {
                logger->log(LogLevel::WARNING, "PerformanceMonitor already initialized");
                return true;
            }

            logger->log(LogLevel::INFO, "Initializing PerformanceMonitor");

            // Initialize performance monitoring libraries if needed
            if (!initializePerformanceLibraries()) {
                logger->log(LogLevel::ERROR, "Failed to initialize performance libraries");
                return false;
            }

            // Initialize system monitoring
            if (!initializeSystemMonitoring()) {
                logger->log(LogLevel::ERROR, "Failed to initialize system monitoring");
                return false;
            }

            isInitialized = true;
            logger->log(LogLevel::INFO, "PerformanceMonitor initialized successfully");
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error initializing PerformanceMonitor: " + std::string(e.what()));
            return false;
        }
    }

    bool start() {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::ERROR, "PerformanceMonitor not initialized");
                return false;
            }

            if (isRunning) {
                logger->log(LogLevel::WARNING, "PerformanceMonitor already running");
                return true;
            }

            logger->log(LogLevel::INFO, "Starting PerformanceMonitor");

            isRunning = true;

            // Start monitoring thread
            monitorThread = std::thread(&PerformanceMonitorImpl::monitoringLoop, this);
            logger->log(LogLevel::INFO, "Performance monitoring thread started");

            logger->log(LogLevel::INFO, "PerformanceMonitor started successfully");
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error starting PerformanceMonitor: " + std::string(e.what()));
            return false;
        }
    }

    bool stop() {
        try {
            if (!isRunning) {
                logger->log(LogLevel::WARNING, "PerformanceMonitor not running");
                return true;
            }

            logger->log(LogLevel::INFO, "Stopping PerformanceMonitor");

            isRunning = false;

            // Wait for thread to finish
            if (monitorThread.joinable()) {
                monitorThread.join();
                logger->log(LogLevel::INFO, "Performance monitoring thread stopped");
            }

            logger->log(LogLevel::INFO, "PerformanceMonitor stopped successfully");
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error stopping PerformanceMonitor: " + std::string(e.what()));
            return false;
        }
    }

    void updateFPS(double fps) {
        currentFPS = fps;
        
        // Update average FPS
        fpsHistory.push_back(fps);
        if (fpsHistory.size() > maxHistorySize) {
            fpsHistory.erase(fpsHistory.begin());
        }
        
        if (!fpsHistory.empty()) {
            double sum = std::accumulate(fpsHistory.begin(), fpsHistory.end(), 0.0);
            averageFPS = sum / fpsHistory.size();
        }
    }

    void updateFrameTime(double time) {
        frameTime = time;
        
        frameTimeHistory.push_back(time);
        if (frameTimeHistory.size() > maxHistorySize) {
            frameTimeHistory.erase(frameTimeHistory.begin());
        }
    }

    void updateLatency(double lat) {
        latency = lat;
        
        latencyHistory.push_back(lat);
        if (latencyHistory.size() > maxHistorySize) {
            latencyHistory.erase(latencyHistory.begin());
        }
    }

    double getCurrentFPS() const {
        return currentFPS;
    }

    double getAverageFPS() const {
        return averageFPS;
    }

    double getFrameTime() const {
        return frameTime;
    }

    double getLatency() const {
        return latency;
    }

    double getCPUUsage() const {
        return cpuUsage;
    }

    double getGPUUsage() const {
        return gpuUsage;
    }

    double getMemoryUsage() const {
        return memoryUsage;
    }

    std::vector<double> getFPSHistory() const {
        return fpsHistory;
    }

    std::vector<double> getFrameTimeHistory() const {
        return frameTimeHistory;
    }

    std::vector<double> getLatencyHistory() const {
        return latencyHistory;
    }

    std::vector<double> getCPUHistory() const {
        return cpuHistory;
    }

    std::vector<double> getGPUHistory() const {
        return gpuHistory;
    }

    std::vector<double> getMemoryHistory() const {
        return memoryHistory;
    }

    PerformanceStats getPerformanceStats() const {
        PerformanceStats stats;
        stats.currentFPS = currentFPS;
        stats.averageFPS = averageFPS;
        stats.frameTime = frameTime;
        stats.latency = latency;
        stats.cpuUsage = cpuUsage;
        stats.gpuUsage = gpuUsage;
        stats.memoryUsage = memoryUsage;
        
        // Calculate min/max values
        if (!fpsHistory.empty()) {
            stats.minFPS = *std::min_element(fpsHistory.begin(), fpsHistory.end());
            stats.maxFPS = *std::max_element(fpsHistory.begin(), fpsHistory.end());
        }
        
        if (!frameTimeHistory.empty()) {
            stats.minFrameTime = *std::min_element(frameTimeHistory.begin(), frameTimeHistory.end());
            stats.maxFrameTime = *std::max_element(frameTimeHistory.begin(), frameTimeHistory.end());
        }
        
        if (!latencyHistory.empty()) {
            stats.minLatency = *std::min_element(latencyHistory.begin(), latencyHistory.end());
            stats.maxLatency = *std::max_element(latencyHistory.begin(), latencyHistory.end());
        }
        
        return stats;
    }

    std::string getPerformanceReport() const {
        std::string report = "Performance Report:\n";
        report += "  Current FPS: " + std::to_string(currentFPS) + "\n";
        report += "  Average FPS: " + std::to_string(averageFPS) + "\n";
        report += "  Frame Time: " + std::to_string(frameTime) + " ms\n";
        report += "  Latency: " + std::to_string(latency) + " ms\n";
        report += "  CPU Usage: " + std::to_string(cpuUsage) + "%\n";
        report += "  GPU Usage: " + std::to_string(gpuUsage) + "%\n";
        report += "  Memory Usage: " + std::to_string(memoryUsage) + "%\n";
        
        if (!fpsHistory.empty()) {
            double minFPS = *std::min_element(fpsHistory.begin(), fpsHistory.end());
            double maxFPS = *std::max_element(fpsHistory.begin(), fpsHistory.end());
            report += "  FPS Range: " + std::to_string(minFPS) + " - " + std::to_string(maxFPS) + "\n";
        }
        
        return report;
    }

    void setMaxHistorySize(size_t size) {
        maxHistorySize = size;
        logger->log(LogLevel::INFO, "Set max history size to: " + std::to_string(size));
    }

    void setUpdateInterval(std::chrono::milliseconds interval) {
        updateInterval = interval;
        logger->log(LogLevel::INFO, "Set update interval to: " + std::to_string(interval.count()) + "ms");
    }

    void clearHistory() {
        fpsHistory.clear();
        frameTimeHistory.clear();
        latencyHistory.clear();
        cpuHistory.clear();
        gpuHistory.clear();
        memoryHistory.clear();
        
        logger->log(LogLevel::INFO, "Performance history cleared");
    }

    bool shutdown() {
        try {
            if (isInitialized) {
                logger->log(LogLevel::INFO, "Shutting down PerformanceMonitor");

                // Stop if running
                if (isRunning) {
                    stop();
                }

                // Shutdown performance libraries
                shutdownPerformanceLibraries();

                // Shutdown system monitoring
                shutdownSystemMonitoring();

                isInitialized = false;
                logger->log(LogLevel::INFO, "PerformanceMonitor shut down successfully");
            }

            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error shutting down PerformanceMonitor: " + std::string(e.what()));
            return false;
        }
    }

private:
    void monitoringLoop() {
        try {
            logger->log(LogLevel::INFO, "Performance monitoring loop started");

            while (isRunning) {
                // Update system metrics
                updateSystemMetrics();

                // Sleep for update interval
                std::this_thread::sleep_for(updateInterval);
            }

            logger->log(LogLevel::INFO, "Performance monitoring loop stopped");

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error in performance monitoring loop: " + std::string(e.what()));
        }
    }

    void updateSystemMetrics() {
        try {
            // Update CPU usage
            double cpu = getCurrentCPUUsage();
            cpuUsage = cpu;
            cpuHistory.push_back(cpu);
            if (cpuHistory.size() > maxHistorySize) {
                cpuHistory.erase(cpuHistory.begin());
            }

            // Update GPU usage
            double gpu = getCurrentGPUUsage();
            gpuUsage = gpu;
            gpuHistory.push_back(gpu);
            if (gpuHistory.size() > maxHistorySize) {
                gpuHistory.erase(gpuHistory.begin());
            }

            // Update memory usage
            double memory = getCurrentMemoryUsage();
            memoryUsage = memory;
            memoryHistory.push_back(memory);
            if (memoryHistory.size() > maxHistorySize) {
                memoryHistory.erase(memoryHistory.begin());
            }

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error updating system metrics: " + std::string(e.what()));
        }
    }

    bool initializePerformanceLibraries() {
        try {
            logger->log(LogLevel::INFO, "Initializing performance libraries");
            // Performance library initialization code would go here
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error initializing performance libraries: " + std::string(e.what()));
            return false;
        }
    }

    bool initializeSystemMonitoring() {
        try {
            logger->log(LogLevel::INFO, "Initializing system monitoring");
            // System monitoring initialization code would go here
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error initializing system monitoring: " + std::string(e.what()));
            return false;
        }
    }

    double getCurrentCPUUsage() {
        try {
            // CPU usage measurement code would go here
            // This is a placeholder implementation
            return 0.0;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error getting CPU usage: " + std::string(e.what()));
            return 0.0;
        }
    }

    double getCurrentGPUUsage() {
        try {
            // GPU usage measurement code would go here
            // This is a placeholder implementation
            return 0.0;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error getting GPU usage: " + std::string(e.what()));
            return 0.0;
        }
    }

    double getCurrentMemoryUsage() {
        try {
            // Memory usage measurement code would go here
            // This is a placeholder implementation
            return 0.0;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error getting memory usage: " + std::string(e.what()));
            return 0.0;
        }
    }

    void shutdownPerformanceLibraries() {
        try {
            logger->log(LogLevel::INFO, "Shutting down performance libraries");
            // Performance library shutdown code would go here

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error shutting down performance libraries: " + std::string(e.what()));
        }
    }

    void shutdownSystemMonitoring() {
        try {
            logger->log(LogLevel::INFO, "Shutting down system monitoring");
            // System monitoring shutdown code would go here

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error shutting down system monitoring: " + std::string(e.what()));
        }
    }
};

// PerformanceMonitor implementation
PerformanceMonitor::PerformanceMonitor() : impl(std::make_unique<PerformanceMonitorImpl>()) {}

PerformanceMonitor::~PerformanceMonitor() = default;

bool PerformanceMonitor::initialize() {
    return impl->initialize();
}

bool PerformanceMonitor::start() {
    return impl->start();
}

bool PerformanceMonitor::stop() {
    return impl->stop();
}

void PerformanceMonitor::updateFPS(double fps) {
    impl->updateFPS(fps);
}

void PerformanceMonitor::updateFrameTime(double time) {
    impl->updateFrameTime(time);
}

void PerformanceMonitor::updateLatency(double latency) {
    impl->updateLatency(latency);
}

double PerformanceMonitor::getCurrentFPS() const {
    return impl->getCurrentFPS();
}

double PerformanceMonitor::getAverageFPS() const {
    return impl->getAverageFPS();
}

double PerformanceMonitor::getFrameTime() const {
    return impl->getFrameTime();
}

double PerformanceMonitor::getLatency() const {
    return impl->getLatency();
}

double PerformanceMonitor::getCPUUsage() const {
    return impl->getCPUUsage();
}

double PerformanceMonitor::getGPUUsage() const {
    return impl->getGPUUsage();
}

double PerformanceMonitor::getMemoryUsage() const {
    return impl->getMemoryUsage();
}

std::vector<double> PerformanceMonitor::getFPSHistory() const {
    return impl->getFPSHistory();
}

std::vector<double> PerformanceMonitor::getFrameTimeHistory() const {
    return impl->getFrameTimeHistory();
}

std::vector<double> PerformanceMonitor::getLatencyHistory() const {
    return impl->getLatencyHistory();
}

std::vector<double> PerformanceMonitor::getCPUHistory() const {
    return impl->getCPUHistory();
}

std::vector<double> PerformanceMonitor::getGPUHistory() const {
    return impl->getGPUHistory();
}

std::vector<double> PerformanceMonitor::getMemoryHistory() const {
    return impl->getMemoryHistory();
}

PerformanceStats PerformanceMonitor::getPerformanceStats() const {
    return impl->getPerformanceStats();
}

std::string PerformanceMonitor::getPerformanceReport() const {
    return impl->getPerformanceReport();
}

void PerformanceMonitor::setMaxHistorySize(size_t size) {
    impl->setMaxHistorySize(size);
}

void PerformanceMonitor::setUpdateInterval(std::chrono::milliseconds interval) {
    impl->setUpdateInterval(interval);
}

void PerformanceMonitor::clearHistory() {
    impl->clearHistory();
}

bool PerformanceMonitor::shutdown() {
    return impl->shutdown();
}

} // namespace core
} // namespace uevrLCX
