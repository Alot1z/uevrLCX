#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "../core/PerformanceMetrics.hpp"

namespace UEVRLCX {

// ============================================================================
// Forward Declarations
// ============================================================================

class NeuralNetwork;
class AIModel;
class OptimizationEngine;
class LearningSystem;

// ============================================================================
// AI Model Structures
// ============================================================================

struct AIModel {
    std::string name;
    std::string version;
    std::string type;
    double accuracy;
    double inferenceTime;
    std::string filePath;
    bool isLoaded;
    bool isTrained;
    
    AIModel() : accuracy(0.0), inferenceTime(0.0), isLoaded(false), isTrained(false) {}
    
    AIModel(const std::string& n, const std::string& v, const std::string& t)
        : name(n), version(v), type(t), accuracy(0.0), inferenceTime(0.0),
          isLoaded(false), isTrained(false) {}
};

struct PerformanceAnalysis {
    double currentPerformance;
    double targetPerformance;
    double improvementPotential;
    std::vector<std::string> optimizationOpportunities;
    std::vector<std::string> bottlenecks;
    std::string recommendedAction;
    
    PerformanceAnalysis() : currentPerformance(0.0), targetPerformance(0.0),
                           improvementPotential(0.0) {}
};

struct OptimizationStrategy {
    std::string name;
    std::string description;
    double expectedImprovement;
    double confidence;
    std::vector<std::string> requirements;
    bool isApplicable;
    
    OptimizationStrategy() : expectedImprovement(0.0), confidence(0.0), isApplicable(false) {}
    
    OptimizationStrategy(const std::string& n, const std::string& desc, double improvement, double conf)
        : name(n), description(desc), expectedImprovement(improvement), confidence(conf),
          isApplicable(false) {}
};

// ============================================================================
// Context7 AI Database Interface
// ============================================================================

class IContext7AIDatabase {
public:
    virtual ~IContext7AIDatabase() = default;
    
    // ========================================================================
    // Initialization and Management
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ========================================================================
    // AI Model Management
    // ========================================================================
    
    virtual bool LoadModel(const std::string& modelPath) = 0;
    virtual bool UnloadModel(const std::string& modelName) = 0;
    virtual bool IsModelLoaded(const std::string& modelName) const = 0;
    virtual std::vector<std::string> GetLoadedModels() const = 0;
    virtual const AIModel* GetModel(const std::string& modelName) const = 0;
    
    // ========================================================================
    // Performance Analysis
    // ========================================================================
    
    virtual PerformanceAnalysis AnalyzePerformance(const PerformanceMetrics& metrics) = 0;
    virtual std::vector<OptimizationStrategy> GenerateOptimizationStrategies(const PerformanceAnalysis& analysis) = 0;
    virtual bool ApplyOptimization(const OptimizationStrategy& strategy) = 0;
    
    // ========================================================================
    // Learning and Training
    // ========================================================================
    
    virtual bool TrainModel(const std::string& modelName, const std::vector<double>& trainingData) = 0;
    virtual bool UpdateModel(const std::string& modelName, const std::vector<double>& newData) = 0;
    virtual double GetModelAccuracy(const std::string& modelName) const = 0;
    
    // ========================================================================
    // Real-time Processing
    // ========================================================================
    
    virtual bool ProcessRealTimeData(const std::vector<double>& data) = 0;
    virtual std::vector<double> GetRealTimePredictions() = 0;
    virtual bool IsRealTimeProcessing() const = 0;
    
    // ========================================================================
    // Configuration and Settings
    // ========================================================================
    
    virtual bool SetLearningRate(double rate) = 0;
    virtual double GetLearningRate() const = 0;
    virtual bool EnableRealTimeLearning(bool enable) = 0;
    virtual bool IsRealTimeLearningEnabled() const = 0;
    
    // ========================================================================
    // Statistics and Monitoring
    // ========================================================================
    
    virtual int GetTotalModels() const = 0;
    virtual int GetActiveModels() const = 0;
    virtual double GetOverallAccuracy() const = 0;
    virtual double GetAverageInferenceTime() const = 0;
};

// ============================================================================
// Context7 AI Database Implementation
// ============================================================================

class Context7AIDatabase : public IContext7AIDatabase {
private:
    // Core Components
    std::unique_ptr<NeuralNetwork> m_neuralNetwork;
    std::unique_ptr<OptimizationEngine> m_optimizationEngine;
    std::unique_ptr<LearningSystem> m_learningSystem;
    
    // Model Management
    std::unordered_map<std::string, std::unique_ptr<AIModel>> m_models;
    std::vector<std::string> m_loadedModels;
    
    // Configuration
    double m_learningRate;
    bool m_realTimeLearningEnabled;
    bool m_isInitialized;
    
    // Performance Tracking
    std::vector<PerformanceAnalysis> m_analysisHistory;
    std::vector<OptimizationStrategy> m_strategyHistory;
    
public:
    Context7AIDatabase();
    ~Context7AIDatabase();
    
    // ========================================================================
    // IContext7AIDatabase Implementation
    // ========================================================================
    
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_isInitialized; }
    
    bool LoadModel(const std::string& modelPath) override;
    bool UnloadModel(const std::string& modelName) override;
    bool IsModelLoaded(const std::string& modelName) const override;
    std::vector<std::string> GetLoadedModels() const override { return m_loadedModels; }
    const AIModel* GetModel(const std::string& modelName) const override;
    
    PerformanceAnalysis AnalyzePerformance(const PerformanceMetrics& metrics) override;
    std::vector<OptimizationStrategy> GenerateOptimizationStrategies(const PerformanceAnalysis& analysis) override;
    bool ApplyOptimization(const OptimizationStrategy& strategy) override;
    
    bool TrainModel(const std::string& modelName, const std::vector<double>& trainingData) override;
    bool UpdateModel(const std::string& modelName, const std::vector<double>& newData) override;
    double GetModelAccuracy(const std::string& modelName) const override;
    
    bool ProcessRealTimeData(const std::vector<double>& data) override;
    std::vector<double> GetRealTimePredictions() override;
    bool IsRealTimeProcessing() const override;
    
    bool SetLearningRate(double rate) override;
    double GetLearningRate() const override { return m_learningRate; }
    bool EnableRealTimeLearning(bool enable) override;
    bool IsRealTimeLearningEnabled() const override { return m_realTimeLearningEnabled; }
    
    int GetTotalModels() const override { return static_cast<int>(m_models.size()); }
    int GetActiveModels() const override { return static_cast<int>(m_loadedModels.size()); }
    double GetOverallAccuracy() const override;
    double GetAverageInferenceTime() const override;
    
private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================
    
    bool InitializeNeuralNetwork();
    bool InitializeOptimizationEngine();
    bool InitializeLearningSystem();
    
    bool ValidateModel(const std::string& modelPath);
    bool LoadModelData(const std::string& modelPath, AIModel& model);
    void UpdateModelStatistics(const std::string& modelName);
    
    PerformanceAnalysis PerformDeepAnalysis(const PerformanceMetrics& metrics);
    std::vector<OptimizationStrategy> GenerateAIOptimizations(const PerformanceAnalysis& analysis);
    bool ValidateOptimizationStrategy(const OptimizationStrategy& strategy);
    
    void LogAnalysis(const PerformanceAnalysis& analysis);
    void LogOptimization(const OptimizationStrategy& strategy);
    void LogModelOperation(const std::string& operation, const std::string& modelName);
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<IContext7AIDatabase> CreateContext7AIDatabase();
std::unique_ptr<IContext7AIDatabase> CreateContext7AIDatabaseWithConfig(const std::string& configPath);

} // namespace UEVRLCX
