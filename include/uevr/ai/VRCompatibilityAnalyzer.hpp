#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <chrono>

// Forward declarations from other modules
namespace uevr {
    namespace ai {
        class AIAnalysisEngine;
    }
}

namespace uevr {
namespace ai {

// Forward declarations
struct GameProfile;
struct VRCompatibilityScore;
struct OptimizationHint;

/**
 * @brief Neural network-based VR compatibility analysis engine
 * 
 * This class uses machine learning models to analyze games and predict
 * VR compatibility, performance characteristics, and optimization strategies.
 */
class VRCompatibilityAnalyzer {
public:
    /**
     * @brief VR compatibility factors analyzed by AI
     */
    enum class CompatibilityFactor {
        RENDERING_PIPELINE,     // Graphics API and rendering techniques
        CAMERA_SYSTEM,          // Camera control and perspective
        INPUT_METHOD,           // Input handling and control schemes
        UI_SYSTEM,              // User interface design and interaction
        PERFORMANCE_PROFILE,    // Performance characteristics and bottlenecks
        MOTION_HANDLING,        // Animation and physics systems
        AUDIO_SYSTEM,           // 3D audio and spatial sound
        PHYSICS_ENGINE,         // Physics simulation compatibility
        SHADER_COMPLEXITY,     // Shader and visual effects complexity
        MEMORY_USAGE           // Memory allocation patterns
    };

    /**
     * @brief AI model types used for analysis
     */
    enum class ModelType {
        COMPATIBILITY_CLASSIFIER,   // Binary/multi-class VR compatibility
        PERFORMANCE_PREDICTOR,      // Performance estimation model
        OPTIMIZATION_RECOMMENDER,   // Optimization strategy generator
        COMFORT_ANALYZER,          // Motion sickness and comfort assessment
        ADAPTATION_PLANNER         // VR adaptation strategy planner
    };

    /**
     * @brief Analysis confidence levels
     */
    enum class ConfidenceLevel {
        LOW = 0,        // < 60% confidence
        MEDIUM = 1,     // 60-80% confidence
        HIGH = 2,       // 80-95% confidence
        VERY_HIGH = 3   // > 95% confidence
    };

    /**
     * @brief Detailed analysis result for a game
     */
    struct AnalysisResult {
        VRCompatibilityScore overall_score;
        std::map<CompatibilityFactor, float> factor_scores;
        std::vector<OptimizationHint> recommendations;
        ConfidenceLevel confidence;
        std::chrono::milliseconds analysis_time;
        std::string model_version;
        std::map<std::string, float> engine_specific_metrics;
    };

    /**
     * @brief Training data sample for model improvement
     */
    struct TrainingSample {
        std::string game_id;
        std::string engine_type;
        std::vector<float> features;
        VRCompatibilityScore ground_truth;
        std::map<std::string, float> performance_metrics;
        std::vector<std::string> user_feedback;
    };

public:
    VRCompatibilityAnalyzer();
    ~VRCompatibilityAnalyzer();

    // Disable copy constructor and assignment
    VRCompatibilityAnalyzer(const VRCompatibilityAnalyzer&) = delete;
    VRCompatibilityAnalyzer& operator=(const VRCompatibilityAnalyzer&) = delete;

    /**
     * @brief Initialize the AI analysis engine
     * @param model_path Path to pre-trained model files
     * @param config_path Path to model configuration
     * @return true if initialization successful
     */
    bool initialize(const std::string& model_path, const std::string& config_path);

    /**
     * @brief Shutdown and cleanup the analysis engine
     */
    void shutdown();

    /**
     * @brief Check if the analyzer is ready for analysis
     * @return true if models are loaded and ready
     */
    bool isReady() const;

    /**
     * @brief Analyze a game for VR compatibility
     * @param game_profile Game profile with extracted features
     * @return Detailed analysis result
     */
    AnalysisResult analyzeCompatibility(const GameProfile& game_profile);

    /**
     * @brief Quick compatibility check (faster, less detailed)
     * @param game_id Game identifier
     * @param engine_type Engine type string
     * @return Basic compatibility score
     */
    VRCompatibilityScore quickAnalysis(const std::string& game_id, const std::string& engine_type);

    /**
     * @brief Generate optimization recommendations
     * @param game_profile Game profile to analyze
     * @param target_performance Target performance metrics
     * @return Vector of optimization hints
     */
    std::vector<OptimizationHint> generateOptimizations(
        const GameProfile& game_profile,
        const std::map<std::string, float>& target_performance
    );

    /**
     * @brief Predict VR performance characteristics
     * @param game_profile Game profile to analyze
     * @return Predicted performance metrics
     */
    std::map<std::string, float> predictPerformance(const GameProfile& game_profile);

    /**
     * @brief Analyze comfort and motion sickness factors
     * @param game_profile Game profile to analyze
     * @return Comfort analysis with recommendations
     */
    std::map<std::string, float> analyzeComfort(const GameProfile& game_profile);

    /**
     * @brief Update models with new training data
     * @param samples Training data samples
     * @return true if update successful
     */
    bool updateModels(const std::vector<TrainingSample>& samples);

    /**
     * @brief Get model performance metrics
     * @param model_type Type of model to query
     * @return Performance metrics map
     */
    std::map<std::string, float> getModelMetrics(ModelType model_type) const;

    /**
     * @brief Export analysis results for external processing
     * @param result Analysis result to export
     * @param format Export format ("json", "csv", "xml")
     * @return Exported data as string
     */
    std::string exportAnalysis(const AnalysisResult& result, const std::string& format) const;

    /**
     * @brief Register custom analysis callback
     * @param factor Compatibility factor to analyze
     * @param callback Custom analysis function
     */
    void registerCustomAnalyzer(
        CompatibilityFactor factor,
        std::function<float(const GameProfile&)> callback
    );

    /**
     * @brief Set analysis configuration options
     * @param options Configuration map
     */
    void setConfiguration(const std::map<std::string, std::string>& options);

    /**
     * @brief Get current configuration
     * @return Configuration map
     */
    std::map<std::string, std::string> getConfiguration() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    // Internal analysis methods
    std::vector<float> extractFeatures(const GameProfile& game_profile);
    float analyzeRenderingPipeline(const GameProfile& game_profile);
    float analyzeCameraSystem(const GameProfile& game_profile);
    float analyzeInputMethod(const GameProfile& game_profile);
    float analyzeUISystem(const GameProfile& game_profile);
    float analyzePerformanceProfile(const GameProfile& game_profile);
    float analyzeMotionHandling(const GameProfile& game_profile);
    float analyzeAudioSystem(const GameProfile& game_profile);
    float analyzePhysicsEngine(const GameProfile& game_profile);
    float analyzeShaderComplexity(const GameProfile& game_profile);
    float analyzeMemoryUsage(const GameProfile& game_profile);

    // AI engine integration
    std::vector<float> runInference(const std::string& model_id, const uevr::ai::AIAnalysisEngine::FeatureTensor& input);
    uevr::ai::AIAnalysisEngine::FeatureTensor extractGameFeatures(const GameProfile& game_profile);

    // Model management
    bool loadModel(ModelType type, const std::string& path);
    void unloadModels();
    bool validateModel(ModelType type) const;

    // Utility methods
    ConfidenceLevel calculateConfidence(const std::vector<float>& model_outputs) const;
    std::string generateRecommendationText(const OptimizationHint& hint) const;
    void logAnalysisResult(const AnalysisResult& result) const;
};

} // namespace ai
} // namespace uevr