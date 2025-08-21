#pragma once

#include <chrono>
#include <vector>
#include <string>

namespace UEVRLCX {

struct PerformanceMetrics {
    // Frame Rate Metrics
    double frameRate;
    double targetFrameRate;
    double frameTime;
    double frameTimeVariance;
    
    // Latency Metrics
    double totalLatency;
    double renderLatency;
    double inputLatency;
    double audioLatency;
    double networkLatency;
    
    // Resource Usage
    double cpuUsage;
    double gpuUsage;
    double memoryUsage;
    double diskUsage;
    double networkUsage;
    
    // Performance Scores
    double overallScore;
    double renderingScore;
    double inputScore;
    double audioScore;
    double optimizationScore;
    
    // AI Optimization
    double improvementFactor;
    double optimizationEfficiency;
    double learningProgress;
    
    // Timestamps
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::steady_clock::time_point lastOptimization;
    
    // Historical Data
    std::vector<double> frameRateHistory;
    std::vector<double> latencyHistory;
    std::vector<double> cpuUsageHistory;
    std::vector<double> gpuUsageHistory;
    
    PerformanceMetrics() : frameRate(0.0), targetFrameRate(90.0),
                          frameTime(0.0), frameTimeVariance(0.0),
                          totalLatency(0.0), renderLatency(0.0),
                          inputLatency(0.0), audioLatency(0.0),
                          networkLatency(0.0), cpuUsage(0.0),
                          gpuUsage(0.0), memoryUsage(0.0),
                          diskUsage(0.0), networkUsage(0.0),
                          overallScore(100.0), renderingScore(100.0),
                          inputScore(100.0), audioScore(100.0),
                          optimizationScore(100.0), improvementFactor(1.0),
                          optimizationEfficiency(0.0), learningProgress(0.0) {
        timestamp = std::chrono::steady_clock::now();
        lastOptimization = std::chrono::steady_clock::now();
    }
    
    void UpdateTimestamp() {
        timestamp = std::chrono::steady_clock::now();
    }
    
    void AddFrameRateData(double fps) {
        frameRateHistory.push_back(fps);
        if (frameRateHistory.size() > 1000) {
            frameRateHistory.erase(frameRateHistory.begin());
        }
    }
    
    void AddLatencyData(double latency) {
        latencyHistory.push_back(latency);
        if (latencyHistory.size() > 1000) {
            latencyHistory.erase(latencyHistory.begin());
        }
    }
    
    double GetAverageFrameRate() const {
        if (frameRateHistory.empty()) return 0.0;
        double sum = 0.0;
        for (double fps : frameRateHistory) {
            sum += fps;
        }
        return sum / frameRateHistory.size();
    }
    
    double GetAverageLatency() const {
        if (latencyHistory.empty()) return 0.0;
        double sum = 0.0;
        for (double latency : latencyHistory) {
            sum += latency;
        }
        return sum / latencyHistory.size();
    }
};

struct PerformanceTargets {
    double minFrameRate;
    double maxLatency;
    double maxCpuUsage;
    double maxGpuUsage;
    double maxMemoryUsage;
    double targetOptimizationFactor;
    
    PerformanceTargets() : minFrameRate(60.0), maxLatency(16.67),
                          maxCpuUsage(80.0), maxGpuUsage(90.0),
                          maxMemoryUsage(85.0), targetOptimizationFactor(20.0) {}
    
    bool IsFrameRateAcceptable(double currentFPS) const {
        return currentFPS >= minFrameRate;
    }
    
    bool IsLatencyAcceptable(double currentLatency) const {
        return currentLatency <= maxLatency;
    }
    
    bool IsCpuUsageAcceptable(double currentUsage) const {
        return currentUsage <= maxCpuUsage;
    }
    
    bool IsGpuUsageAcceptable(double currentUsage) const {
        return currentUsage <= maxGpuUsage;
    }
    
    bool IsMemoryUsageAcceptable(double currentUsage) const {
        return currentUsage <= maxMemoryUsage;
    }
};

struct OptimizationResult {
    bool success;
    double improvementFactor;
    std::string optimizationType;
    std::string description;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<std::string> appliedOptimizations;
    
    OptimizationResult() : success(false), improvementFactor(1.0) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    OptimizationResult(bool s, double factor, const std::string& type, const std::string& desc)
        : success(s), improvementFactor(factor), optimizationType(type), description(desc) {
        timestamp = std::chrono::steady_clock::now();
    }
};

} // namespace UEVRLCX
