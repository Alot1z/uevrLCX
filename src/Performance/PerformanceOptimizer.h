#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>
#include "../core/PerformanceMetrics.hpp"

namespace UEVRLCX {

// ============================================================================
// Forward Declarations
// ============================================================================

class GPUAccelerator;
class MemoryOptimizer;
class ParallelProcessor;
class AIPerformanceAnalyzer;
class OptimizationEngine;

// ============================================================================
// Performance Optimization Structures
// ============================================================================

enum class OptimizationType {
    GPU_ACCELERATION,
    MEMORY_OPTIMIZATION,
    PARALLEL_PROCESSING,
    AI_OPTIMIZATION,
    PLATFORM_SPECIFIC,
    CUSTOM
};

enum class OptimizationPriority {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

struct OptimizationTask {
    std::string name;
    OptimizationType type;
    OptimizationPriority priority;
    std::string description;
    std::function<bool()> executeFunction;
    std::function<void()> rollbackFunction;
    bool isExecuted;
    bool isSuccessful;
    double expectedImprovement;
    std::chrono::steady_clock::time_point executionTime;
    
    OptimizationTask() : type(OptimizationType::CUSTOM), priority(OptimizationPriority::MEDIUM),
                         isExecuted(false), isSuccessful(false), expectedImprovement(0.0) {
        executionTime = std::chrono::steady_clock::now();
    }
    
    OptimizationTask(const std::string& n, OptimizationType t, OptimizationPriority p,
                    const std::string& desc, double improvement)
        : name(n), type(t), priority(p), description(desc), expectedImprovement(improvement),
          isExecuted(false), isSuccessful(false) {
        executionTime = std::chrono::steady_clock::now();
    }
};

struct OptimizationResult {
    bool success;
    double actualImprovement;
    double expectedImprovement;
    std::string optimizationType;
    std::string description;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<std::string> appliedOptimizations;
    std::vector<std::string> failedOptimizations;
    
    OptimizationResult() : success(false), actualImprovement(0.0), expectedImprovement(0.0) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    OptimizationResult(bool s, double actual, double expected, const std::string& type, const std::string& desc)
        : success(s), actualImprovement(actual), expectedImprovement(expected),
          optimizationType(type), description(desc) {
        timestamp = std::chrono::steady_clock::now();
    }
};

struct PerformanceProfile {
    std::string name;
    std::string description;
    std::vector<OptimizationTask> tasks;
    bool isActive;
    double totalExpectedImprovement;
    std::chrono::steady_clock::time_point lastApplied;
    
    PerformanceProfile() : isActive(false), totalExpectedImprovement(0.0) {
        lastApplied = std::chrono::steady_clock::now();
    }
    
    PerformanceProfile(const std::string& n, const std::string& desc)
        : name(n), description(desc), isActive(false), totalExpectedImprovement(0.0) {
        lastApplied = std::chrono::steady_clock::now();
    }
    
    void AddTask(const OptimizationTask& task) {
        tasks.push_back(task);
        totalExpectedImprovement += task.expectedImprovement;
    }
    
    void RemoveTask(const std::string& taskName) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->name == taskName) {
                totalExpectedImprovement -= it->expectedImprovement;
                tasks.erase(it);
                break;
            }
        }
    }
};

// ============================================================================
// Performance Optimizer Interface
// ============================================================================

class IPerformanceOptimizer {
public:
    virtual ~IPerformanceOptimizer() = default;
    
    // ========================================================================
    // Initialization and Management
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ========================================================================
    // Performance Analysis
    // ========================================================================
    
    virtual PerformanceAnalysis AnalyzePerformance(const PerformanceMetrics& metrics) = 0;
    virtual std::vector<OptimizationTask> GenerateOptimizationTasks(const PerformanceAnalysis& analysis) = 0;
    virtual bool ExecuteOptimizationTask(const std::string& taskName) = 0;
    virtual bool ExecuteAllOptimizationTasks() = 0;
    
    // ========================================================================
    // AI-Powered Optimization
    // ========================================================================
    
    virtual bool ApplyAIOptimizations(const std::vector<double>& aiRecommendations) = 0;
    virtual std::vector<double> GetAIOptimizationRecommendations() = 0;
    virtual bool IsAIOptimizationEnabled() const = 0;
    virtual bool EnableAIOptimization(bool enable) = 0;
    
    // ========================================================================
    // GPU Acceleration
    // ========================================================================
    
    virtual bool EnableGPUAcceleration(bool enable) = 0;
    virtual bool IsGPUAccelerationEnabled() const = 0;
    virtual bool OptimizeGPUUsage() = 0;
    virtual double GetGPUOptimizationImprovement() const = 0;
    
    // ========================================================================
    // Memory Optimization
    // ========================================================================
    
    virtual bool OptimizeMemoryUsage() = 0;
    virtual bool IsMemoryOptimizationEnabled() const = 0;
    virtual double GetMemoryOptimizationImprovement() const = 0;
    virtual bool SetMemoryOptimizationLevel(int level) = 0;
    
    // ========================================================================
    // Parallel Processing
    // ========================================================================
    
    virtual bool EnableParallelProcessing(bool enable) = 0;
    virtual bool IsParallelProcessingEnabled() const = 0;
    virtual bool OptimizeParallelExecution() = 0;
    virtual double GetParallelProcessingImprovement() const = 0;
    
    // ========================================================================
    // Performance Profiles
    // ========================================================================
    
    virtual bool CreatePerformanceProfile(const std::string& name, const std::string& description) = 0;
    virtual bool DeletePerformanceProfile(const std::string& name) = 0;
    virtual bool ActivatePerformanceProfile(const std::string& name) = 0;
    virtual bool DeactivatePerformanceProfile(const std::string& name) = 0;
    virtual std::vector<std::string> GetAvailableProfiles() const = 0;
    virtual PerformanceProfile* GetProfile(const std::string& name) = 0;
    
    // ========================================================================
    // Real-time Optimization
    // ========================================================================
    
    virtual bool EnableRealTimeOptimization(bool enable) = 0;
    virtual bool IsRealTimeOptimizationEnabled() const = 0;
    virtual bool ProcessRealTimeOptimization() = 0;
    virtual bool SetOptimizationInterval(int milliseconds) = 0;
    
    // ========================================================================
    // Monitoring and Statistics
    // ========================================================================
    
    virtual const PerformanceMetrics& GetCurrentMetrics() const = 0;
    virtual bool UpdatePerformanceMetrics() = 0;
    virtual std::vector<OptimizationResult> GetOptimizationHistory() const = 0;
    virtual double GetOverallImprovement() const = 0;
    virtual std::string GetOptimizationStatus() const = 0;
    
    // ========================================================================
    // Configuration and Settings
    // ========================================================================
    
    virtual bool SetTargetPerformance(double target) = 0;
    virtual double GetTargetPerformance() const = 0;
    virtual bool SetOptimizationAggressiveness(int level) = 0;
    virtual int GetOptimizationAggressiveness() const = 0;
    virtual bool EnableAutoOptimization(bool enable) = 0;
    virtual bool IsAutoOptimizationEnabled() const = 0;
};

// ============================================================================
// Performance Optimizer Implementation
// ============================================================================

class PerformanceOptimizer : public IPerformanceOptimizer {
private:
    // Core Components
    std::unique_ptr<GPUAccelerator> m_gpuAccelerator;
    std::unique_ptr<MemoryOptimizer> m_memoryOptimizer;
    std::unique_ptr<ParallelProcessor> m_parallelProcessor;
    std::unique_ptr<AIPerformanceAnalyzer> m_aiAnalyzer;
    std::unique_ptr<OptimizationEngine> m_optimizationEngine;
    
    // Performance Management
    std::unordered_map<std::string, std::unique_ptr<PerformanceProfile>> m_profiles;
    std::vector<OptimizationTask> m_availableTasks;
    std::vector<OptimizationResult> m_optimizationHistory;
    
    // Configuration
    double m_targetPerformance;
    int m_optimizationAggressiveness;
    bool m_autoOptimizationEnabled;
    bool m_realTimeOptimizationEnabled;
    bool m_aiOptimizationEnabled;
    bool m_isInitialized;
    
    // Current State
    PerformanceMetrics m_currentMetrics;
    std::string m_activeProfile;
    std::atomic<bool> m_optimizationInProgress;
    std::chrono::steady_clock::time_point m_lastOptimization;
    
    // Optimization Settings
    int m_optimizationInterval;
    double m_overallImprovement;
    std::chrono::steady_clock::time_point m_lastMetricsUpdate;
    
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    // ========================================================================
    // IPerformanceOptimizer Implementation
    // ========================================================================
    
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_isInitialized; }
    
    PerformanceAnalysis AnalyzePerformance(const PerformanceMetrics& metrics) override;
    std::vector<OptimizationTask> GenerateOptimizationTasks(const PerformanceAnalysis& analysis) override;
    bool ExecuteOptimizationTask(const std::string& taskName) override;
    bool ExecuteAllOptimizationTasks() override;
    
    bool ApplyAIOptimizations(const std::vector<double>& aiRecommendations) override;
    std::vector<double> GetAIOptimizationRecommendations() override;
    bool IsAIOptimizationEnabled() const override { return m_aiOptimizationEnabled; }
    bool EnableAIOptimization(bool enable) override;
    
    bool EnableGPUAcceleration(bool enable) override;
    bool IsGPUAccelerationEnabled() const override;
    bool OptimizeGPUUsage() override;
    double GetGPUOptimizationImprovement() const override;
    
    bool OptimizeMemoryUsage() override;
    bool IsMemoryOptimizationEnabled() const override;
    double GetMemoryOptimizationImprovement() const override;
    bool SetMemoryOptimizationLevel(int level) override;
    
    bool EnableParallelProcessing(bool enable) override;
    bool IsParallelProcessingEnabled() const override;
    bool OptimizeParallelExecution() override;
    double GetParallelProcessingImprovement() const override;
    
    bool CreatePerformanceProfile(const std::string& name, const std::string& description) override;
    bool DeletePerformanceProfile(const std::string& name) override;
    bool ActivatePerformanceProfile(const std::string& name) override;
    bool DeactivatePerformanceProfile(const std::string& name) override;
    std::vector<std::string> GetAvailableProfiles() const override;
    PerformanceProfile* GetProfile(const std::string& name) override;
    
    bool EnableRealTimeOptimization(bool enable) override;
    bool IsRealTimeOptimizationEnabled() const override { return m_realTimeOptimizationEnabled; }
    bool ProcessRealTimeOptimization() override;
    bool SetOptimizationInterval(int milliseconds) override;
    
    const PerformanceMetrics& GetCurrentMetrics() const override { return m_currentMetrics; }
    bool UpdatePerformanceMetrics() override;
    std::vector<OptimizationResult> GetOptimizationHistory() const override { return m_optimizationHistory; }
    double GetOverallImprovement() const override { return m_overallImprovement; }
    std::string GetOptimizationStatus() const override;
    
    bool SetTargetPerformance(double target) override;
    double GetTargetPerformance() const override { return m_targetPerformance; }
    bool SetOptimizationAggressiveness(int level) override;
    int GetOptimizationAggressiveness() const override { return m_optimizationAggressiveness; }
    bool EnableAutoOptimization(bool enable) override;
    bool IsAutoOptimizationEnabled() const override { return m_autoOptimizationEnabled; }
    
private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================
    
    bool InitializeGPUAccelerator();
    bool InitializeMemoryOptimizer();
    bool InitializeParallelProcessor();
    bool InitializeAIAnalyzer();
    bool InitializeOptimizationEngine();
    
    bool ValidateOptimizationTask(const OptimizationTask& task);
    bool ExecuteOptimizationTaskInternal(const OptimizationTask& task);
    bool RollbackOptimizationTask(const OptimizationTask& task);
    
    PerformanceAnalysis PerformDeepPerformanceAnalysis(const PerformanceMetrics& metrics);
    std::vector<OptimizationTask> GenerateAIOptimizationTasks(const PerformanceAnalysis& analysis);
    bool ValidateOptimizationResult(const OptimizationResult& result);
    
    bool ApplyPerformanceProfile(const std::string& profileName);
    bool RemovePerformanceProfile(const std::string& profileName);
    void UpdateProfileStatistics(const std::string& profileName);
    
    bool ProcessGPUOptimization();
    bool ProcessMemoryOptimization();
    bool ProcessParallelProcessingOptimization();
    bool ProcessAIOptimization();
    
    void UpdateOptimizationStatistics();
    void CalculateOverallImprovement();
    void LogOptimizationOperation(const std::string& operation, const std::string& details);
    void LogOptimizationResult(const OptimizationResult& result);
    
    bool IsOptimizationSafe(const OptimizationTask& task);
    bool CheckOptimizationCompatibility(const OptimizationTask& task);
    void HandleOptimizationError(const OptimizationTask& task, const std::string& error);
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<IPerformanceOptimizer> CreatePerformanceOptimizer();
std::unique_ptr<IPerformanceOptimizer> CreatePerformanceOptimizerWithConfig(const std::string& configPath);

} // namespace UEVRLCX
