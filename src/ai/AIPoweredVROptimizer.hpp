#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <future>

// AI/ML includes
#include <torch/torch.h>
#include <opencv2/opencv.hpp>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>

// VR-specific includes
#include "../vr/VRSystem.hpp"
#include "../vr/OpenXRIntegration.hpp"
#include "../vr/AdvancedPhysicsIntegration.hpp"

namespace uevr {

enum class AIOptimizationType {
    GAME_ANALYSIS,           // Analyze game structure and patterns
    PERFORMANCE_OPTIMIZATION, // Optimize VR performance automatically
    COLLISION_PREDICTION,    // Predict collisions using AI
    RENDERING_ENHANCEMENT,   // Enhance rendering quality with AI
    INPUT_PREDICTION,        // Predict user input for reduced latency
    ADAPTIVE_QUALITY,        // Dynamically adjust quality based on performance
    NEURAL_RENDERING,        // AI-powered rendering techniques
    BEHAVIOR_LEARNING        // Learn from user behavior patterns
};

enum class AIModelType {
    CONVOLUTIONAL_NN,        // For image analysis and rendering
    RECURRENT_NN,           // For sequence prediction and behavior
    TRANSFORMER,            // For complex pattern recognition
    REINFORCEMENT_LEARNING, // For adaptive optimization
    HYBRID_MODEL            // Combination of multiple approaches
};

struct AIGameAnalysis {
    std::string gameName;
    std::string engineType;
    std::string rendererType;
    
    // Performance characteristics
    float averageFrameRate;
    float frameTimeVariance;
    float gpuUtilization;
    float cpuUtilization;
    float memoryUsage;
    
    // VR-specific analysis
    bool hasDepthBuffer;
    bool supportsStereoRendering;
    bool hasMotionControllers;
    bool supportsRoomScale;
    
    // Game mechanics analysis
    std::vector<std::string> detectedMechanics;
    std::vector<std::string> inputMethods;
    std::vector<std::string> renderingTechniques;
    
    // Optimization opportunities
    std::vector<std::string> suggestedOptimizations;
    float optimizationPotential;
    int estimatedVRScore;
    
    // AI confidence scores
    float analysisConfidence;
    float predictionAccuracy;
    float optimizationEffectiveness;
};

struct AIPerformanceProfile {
    std::string profileName;
    std::string gameSignature;
    
    // Performance targets
    float targetFrameRate;
    float targetLatency;
    float targetResolution;
    
    // Optimization settings
    struct OptimizationSettings {
        bool enableFoveatedRendering;
        bool enableAsyncTimewarp;
        bool enableMultiResolutionShading;
        bool enableDynamicQualityScaling;
        float qualityScale;
        int renderScale;
        bool enableMotionPrediction;
        bool enableCollisionPrediction;
    } settings;
    
    // AI model parameters
    std::vector<float> modelWeights;
    std::vector<float> learningRates;
    float confidenceThreshold;
    
    // Performance metrics
    float currentFrameRate;
    float currentLatency;
    float currentVRScore;
    float optimizationEffectiveness;
    
    // Learning data
    std::vector<float> historicalPerformance;
    std::vector<float> optimizationHistory;
    int trainingIterations;
    float modelAccuracy;
};

struct AICollisionPrediction {
    std::string objectId;
    glm::vec3 predictedPosition;
    glm::vec3 predictedVelocity;
    glm::quat predictedRotation;
    float predictionTime;
    float confidence;
    
    // Collision predictions
    std::vector<std::string> predictedCollisions;
    std::vector<float> collisionProbabilities;
    std::vector<glm::vec3> collisionPoints;
    std::vector<glm::vec3> collisionNormals;
    
    // Trajectory analysis
    std::vector<glm::vec3> predictedTrajectory;
    std::vector<float> trajectoryConfidence;
    bool isTrajectoryStable;
    float trajectoryUncertainty;
};

struct AIRenderingEnhancement {
    std::string enhancementType;
    float qualityImprovement;
    float performanceCost;
    
    // Neural rendering parameters
    struct NeuralRenderingParams {
        bool enableSuperResolution;
        bool enableDenoising;
        bool enableUpscaling;
        bool enableFrameInterpolation;
        int upscaleFactor;
        float denoisingStrength;
        float interpolationWeight;
    } neuralParams;
    
    // Enhancement results
    float visualQualityScore;
    float performanceImpact;
    float userSatisfaction;
    bool isRecommended;
};

struct AIBehaviorLearning {
    std::string userId;
    std::string sessionId;
    
    // User behavior patterns
    struct BehaviorPattern {
        std::string patternType;
        float frequency;
        float consistency;
        std::vector<float> patternData;
        float confidence;
    };
    
    std::vector<BehaviorPattern> patterns;
    
    // Input prediction
    struct InputPrediction {
        std::string inputType;
        glm::vec3 predictedInput;
        float predictionTime;
        float confidence;
        bool wasCorrect;
    };
    
    std::vector<InputPrediction> inputPredictions;
    
    // Learning metrics
    float predictionAccuracy;
    float learningRate;
    int totalSessions;
    float averageSessionTime;
};

class AIPoweredVROptimizer {
public:
    AIPoweredVROptimizer();
    ~AIPoweredVROptimizer();
    
    // Initialization and configuration
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }
    
    // AI model management
    bool loadAIModel(const std::string& modelPath, AIModelType modelType);
    bool saveAIModel(const std::string& modelPath);
    bool trainModel(const std::vector<AIGameAnalysis>& trainingData);
    float getModelAccuracy() const;
    
    // Game analysis
    AIGameAnalysis analyzeGame(const std::string& gamePath, const std::string& gameName);
    bool updateGameAnalysis(const std::string& gameName, const AIGameAnalysis& analysis);
    std::vector<AIGameAnalysis> getAllGameAnalyses() const;
    
    // Performance optimization
    AIPerformanceProfile createPerformanceProfile(const std::string& gameName);
    bool optimizePerformance(const std::string& gameName, AIPerformanceProfile& profile);
    bool applyOptimizationProfile(const AIPerformanceProfile& profile);
    std::vector<AIPerformanceProfile> getOptimizationProfiles() const;
    
    // Collision prediction
    AICollisionPrediction predictCollision(const std::string& objectId, float predictionTime);
    std::vector<AICollisionPrediction> predictAllCollisions(float predictionTime);
    bool updateCollisionPrediction(const AICollisionPrediction& prediction);
    float getCollisionPredictionAccuracy() const;
    
    // Rendering enhancement
    AIRenderingEnhancement enhanceRendering(const std::string& enhancementType);
    bool applyRenderingEnhancement(const AIRenderingEnhancement& enhancement);
    std::vector<AIRenderingEnhancement> getAvailableEnhancements() const;
    float getRenderingQualityScore() const;
    
    // Behavior learning
    AIBehaviorLearning learnUserBehavior(const std::string& userId);
    bool updateBehaviorLearning(const AIBehaviorLearning& learning);
    std::vector<AIBehaviorLearning> getAllBehaviorLearning() const;
    float getBehaviorPredictionAccuracy() const;
    
    // Real-time optimization
    void startRealTimeOptimization();
    void stopRealTimeOptimization();
    bool isOptimizing() const { return m_optimizing.load(); }
    
    void updateOptimization();
    void processGameFrame(const cv::Mat& frame);
    void processUserInput(const std::string& inputType, const glm::vec3& input);
    
    // AI-powered features
    bool enableNeuralRendering(bool enable);
    bool enableCollisionPrediction(bool enable);
    bool enableInputPrediction(bool enable);
    bool enableAdaptiveQuality(bool enable);
    
    // Performance monitoring
    struct AIMetrics {
        float modelInferenceTime;
        float optimizationTime;
        float predictionAccuracy;
        float learningRate;
        float memoryUsage;
        int activeModels;
        float averageConfidence;
        float optimizationEffectiveness;
    };
    
    AIMetrics getAIMetrics() const;
    void enableMetricsCollection(bool enable);
    void setMetricsCallback(std::function<void(const AIMetrics&)> callback);
    
    // Community learning
    bool submitToCommunity(const std::string& gameName, const AIPerformanceProfile& profile);
    bool loadFromCommunity(const std::string& gameName);
    std::vector<std::string> getCommunityGames() const;
    float getCommunityConfidence(const std::string& gameName) const;
    
    // Advanced AI features
    bool enableTransferLearning(bool enable);
    bool enableFederatedLearning(bool enable);
    bool enableOnlineLearning(bool enable);
    
    // Model customization
    bool customizeModel(const std::string& gameName, const std::vector<float>& parameters);
    std::vector<float> getModelParameters(const std::string& gameName) const;
    bool saveCustomModel(const std::string& gameName, const std::string& modelPath);
    
    // Debug and visualization
    void enableDebugMode(bool enable);
    void setDebugCallback(std::function<void(const std::string&, const std::vector<float>&)> callback);
    std::vector<std::string> getDebugInfo() const;

private:
    // AI model implementations
    class ConvolutionalNN;
    std::unique_ptr<ConvolutionalNN> m_convNN;
    
    class RecurrentNN;
    std::unique_ptr<RecurrentNN> m_rnn;
    
    class TransformerModel;
    std::unique_ptr<TransformerModel> m_transformer;
    
    class ReinforcementLearning;
    std::unique_ptr<ReinforcementLearning> m_rl;
    
    // Common members
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_optimizing{false};
    std::atomic<bool> m_shutdown{false};
    
    std::unordered_map<std::string, AIGameAnalysis> m_gameAnalyses;
    mutable std::mutex m_analysesMutex;
    
    std::unordered_map<std::string, AIPerformanceProfile> m_performanceProfiles;
    mutable std::mutex m_profilesMutex;
    
    std::unordered_map<std::string, AICollisionPrediction> m_collisionPredictions;
    mutable std::mutex m_collisionsMutex;
    
    std::unordered_map<std::string, AIRenderingEnhancement> m_renderingEnhancements;
    mutable std::mutex m_renderingMutex;
    
    std::unordered_map<std::string, AIBehaviorLearning> m_behaviorLearning;
    mutable std::mutex m_behaviorMutex;
    
    AIMetrics m_metrics;
    mutable std::mutex m_metricsMutex;
    bool m_metricsCollectionEnabled{false};
    std::function<void(const AIMetrics&)> m_metricsCallback;
    
    std::thread m_optimizationThread;
    std::condition_variable m_optimizationCV;
    std::mutex m_optimizationMutex;
    
    bool m_debugModeEnabled{false};
    std::function<void(const std::string&, const std::vector<float>&)> m_debugCallback;
    std::vector<std::string> m_debugInfo;
    mutable std::mutex m_debugMutex;
    
    // AI model parameters
    std::unordered_map<std::string, std::vector<float>> m_modelParameters;
    mutable std::mutex m_parametersMutex;
    
    // Community data
    std::unordered_map<std::string, AIPerformanceProfile> m_communityProfiles;
    mutable std::mutex m_communityMutex;
    
    // Feature flags
    bool m_neuralRenderingEnabled{false};
    bool m_collisionPredictionEnabled{false};
    bool m_inputPredictionEnabled{false};
    bool m_adaptiveQualityEnabled{false};
    bool m_transferLearningEnabled{false};
    bool m_federatedLearningEnabled{false};
    bool m_onlineLearningEnabled{false};
    
    // Private methods
    void optimizationLoop();
    void updateMetrics();
    void processGameAnalysis();
    void updateCollisionPredictions();
    void applyRenderingEnhancements();
    void learnUserBehavior();
    void updateCommunityData();
    bool initializeAIModels();
    void cleanupAIModels();
    void trainModels();
    void validatePredictions();
    void optimizeForVR();
    void logAIError(const std::string& error);
    void saveLearningData();
    void loadLearningData();
};

} // namespace uevr
