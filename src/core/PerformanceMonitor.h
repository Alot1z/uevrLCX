#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace uevrLCX {
namespace core {

struct PerformanceStats {
    double currentFPS;
    double averageFPS;
    double minFPS;
    double maxFPS;
    double frameTime;
    double minFrameTime;
    double maxFrameTime;
    double latency;
    double minLatency;
    double maxLatency;
    double cpuUsage;
    double gpuUsage;
    double memoryUsage;
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();

    // Core functionality
    bool initialize();
    bool start();
    bool stop();
    
    // Performance updates
    void updateFPS(double fps);
    void updateFrameTime(double time);
    void updateLatency(double latency);
    
    // Current metrics
    double getCurrentFPS() const;
    double getAverageFPS() const;
    double getFrameTime() const;
    double getLatency() const;
    double getCPUUsage() const;
    double getGPUUsage() const;
    double getMemoryUsage() const;
    
    // Historical data
    std::vector<double> getFPSHistory() const;
    std::vector<double> getFrameTimeHistory() const;
    std::vector<double> getLatencyHistory() const;
    std::vector<double> getCPUHistory() const;
    std::vector<double> getGPUHistory() const;
    std::vector<double> getMemoryHistory() const;
    
    // Statistics and reporting
    PerformanceStats getPerformanceStats() const;
    std::string getPerformanceReport() const;
    
    // Configuration
    void setMaxHistorySize(size_t size);
    void setUpdateInterval(std::chrono::milliseconds interval);
    void clearHistory();
    
    // Cleanup
    bool shutdown();

private:
    class PerformanceMonitorImpl;
    std::unique_ptr<PerformanceMonitorImpl> impl;
};

} // namespace core
} // namespace uevrLCX
