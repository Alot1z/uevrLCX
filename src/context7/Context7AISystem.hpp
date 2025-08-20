#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <future>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace UEVR {
namespace Context7 {

// Forward declarations
class NeuralNetworkEngine;
class MachineLearningProcessor;
class NaturalLanguageProcessor;
class ComputerVisionEngine;
class PerformancePredictor;
class DeepWikiAI;
class RealTimeLearningAI;

// AI system state
enum class AISystemState {
    Uninitialized,
    Initializing,
    Ready,
    Processing,
    Training,
    Error,
    Shutdown
};

// AI model types
enum class AIModelType {
    NeuralNetwork,
    ConvolutionalNN,
    RecurrentNN,
    Transformer,
    Custom
};

// AI model
struct AIModel {
    std::string model_id;
    AIModelType type;
    std::string version;
    std::string file_path;
    bool is_loaded;
    bool is_training;
    float accuracy;
    float confidence;
    std::chrono::steady_clock::time_point last_updated;
    std::map<std::string, float> performance_metrics;
};

// AI configuration
struct AIConfiguration {
    // Neural network settings
    uint32_t max_neural_layers;
    uint32_t max_neurons_per_layer;
    float default_learning_rate;
    float default_dropout_rate;
    
    // Processing settings
    uint32_t max_threads;
    uint32_t max_batch_size;
    bool enable_gpu_acceleration;
    bool enable_parallel_processing;
    
    // Memory settings
    size_t max_memory_usage_mb;
    bool enable_memory_mapping;
    bool enable_compression_caching;
    
    // DeepWiki settings
    std::string deepwiki_api_endpoint;
    std::string deepwiki_api_key;
    uint32_t max_concurrent_requests;
    float request_throttle_rate;
    
    // Learning settings
    bool enable_online_learning;
    bool enable_feedback_learning;
    float learning_rate;
    uint32_t min_training_samples;
};

// Game features for AI analysis
struct GameFeatures {
    // Engine properties
    float engine_type;              // Normalized engine type
    float engine_version;           // Engine version number
    float graphics_api;             // Graphics API type
    
    // Game properties
    float game_genre;               // Game genre classification
    float release_year;             // Release year (normalized)
    float complexity_score;         // Game complexity score
    
    // Technical properties
    float polygon_count;            // Estimated polygon count
    float texture_resolution;       // Texture resolution
    float shader_complexity;        // Shader complexity score
    
    // VR-specific properties
    float camera_type;              // Camera system type
    float ui_complexity;            // UI complexity score
    float input_system;             // Input system type
    
    // Performance metrics
    float base_fps;                 // Base frame rate
    float memory_usage;             // Memory usage
    float cpu_usage;                // CPU usage
    float gpu_usage;                // GPU usage
};

// Game predictions from AI
struct GamePredictions {
    // Overall scores
    float vr_compatibility_score;   // VR compatibility (0.0-1.0)
    float performance_score;        // Expected performance (0.0-1.0)
    float comfort_score;            // VR comfort level (0.0-1.0)
    float optimization_potential;   // Optimization potential (0.0-1.0)
    
    // Detailed predictions
    std::vector<float> feature_importance; // Feature importance weights
    std::vector<std::string> recommendations; // AI-generated recommendations
    float confidence_level;         // Prediction confidence (0.0-1.0)
    
    // Performance predictions
    float predicted_fps;            // Predicted frame rate
    float predicted_latency;        // Predicted latency
    std::vector<float> fps_timeline; // FPS timeline prediction
    
    // VR-specific predictions
    float motion_sickness_risk;     // Motion sickness risk
    float room_scale_compatibility; // Room scale compatibility
    float seated_play_compatibility; // Seated play compatibility
};

// VR analysis result
struct VRAnalysis {
    std::string game_name;
    std::string engine_type;
    GamePredictions predictions;
    std::vector<std::string> vr_settings;
    std::vector<std::string> optimizations;
    std::vector<std::string> warnings;
    std::vector<std::string> workarounds;
    float analysis_confidence;
    std::chrono::steady_clock::time_point analysis_timestamp;
};

// User profile for AI learning
struct UserProfile {
    std::string user_id;
    std::string hardware_profile;
    std::vector<std::string> preferred_settings;
    std::map<std::string, float> game_ratings;
    std::vector<std::string> comfort_preferences;
    float vr_experience_level;
    std::chrono::steady_clock::time_point profile_created;
    std::chrono::steady_clock::time_point last_updated;
};

// AI task definition
enum class AITaskType {
    GameAnalysis,
    PerformancePrediction,
    VROptimization,
    UserRecommendation,
    ModelTraining,
    DataProcessing
};

enum class AITaskPriority {
    Low,
    Normal,
    High,
    Critical
};

struct AITask {
    std::string task_id;
    AITaskType type;
    AITaskPriority priority;
    std::vector<uint8_t> input_data;
    std::function<void(const AITask&)> processor;
    bool is_completed;
    std::chrono::steady_clock::time_point created_at;
    std::chrono::steady_clock::time_point completed_at;
    std::string error_message;
};

// Task status
struct TaskStatus {
    std::string task_id;
    bool is_running;
    bool is_completed;
    bool has_error;
    float progress;                 // 0.0 to 1.0
    std::string status_message;
    std::chrono::steady_clock::time_point last_updated;
};

// Main Context7 AI System class
class Context7AISystem {
public:
    Context7AISystem();
    ~Context7AISystem();
    
    // Core initialization and management
    bool initialize(const AIConfiguration& config);
    void shutdown();
    bool isInitialized() const;
    AISystemState getState() const;
    
    // AI analysis capabilities
    bool analyzeGameCompatibility(const std::string& game_name);
    bool predictPerformanceMetrics(const GameProfile& profile);
    bool optimizeVRSettings(const VRAnalysis& analysis);
    bool generateIntelligentRecommendations(const UserProfile& user);
    
    // Task management
    std::string submitTask(const AITask& task);
    bool cancelTask(const std::string& task_id);
    TaskStatus getTaskStatus(const std::string& task_id);
    bool waitForTaskCompletion(const std::string& task_id, uint32_t timeout_ms);
    
    // Model management
    bool loadModel(const std::string& model_id);
    bool unloadModel(const std::string& model_id);
    std::vector<AIModel> getLoadedModels() const;
    AIModel getModelInfo(const std::string& model_id) const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    std::map<std::string, float> getPerformanceMetrics() const;
    
    // DeepWiki integration
    bool fetchDeepWikiData(const std::string& game_name);
    bool updateFromDeepWiki(const std::string& game_name);
    std::vector<std::string> getDeepWikiSuggestions(const std::string& query);
    
    // Real-time learning
    bool processUserFeedback(const std::string& game_name, const UserFeedback& feedback);
    bool updateAIModels(const std::vector<UserFeedback>& feedback);
    
    // Configuration
    void updateConfiguration(const AIConfiguration& config);
    AIConfiguration getConfiguration() const;
    
private:
    // Core AI components
    std::unique_ptr<NeuralNetworkEngine> m_neural_engine;
    std::unique_ptr<MachineLearningProcessor> m_ml_processor;
    std::unique_ptr<NaturalLanguageProcessor> m_nlp_engine;
    std::unique_ptr<ComputerVisionEngine> m_cv_engine;
    std::unique_ptr<PerformancePredictor> m_perf_predictor;
    
    // DeepWiki and learning components
    std::unique_ptr<DeepWikiAI> m_deepwiki_ai;
    std::unique_ptr<RealTimeLearningAI> m_learning_ai;
    
    // AI system state
    AISystemState m_ai_state;
    AIConfiguration m_config;
    std::vector<AIModel> m_loaded_models;
    std::string m_active_model;
    
    // Task management
    std::map<std::string, AITask> m_tasks;
    std::map<std::string, TaskStatus> m_task_status;
    std::queue<AITask> m_task_queue;
    std::vector<std::thread> m_worker_threads;
    
    // Threading and synchronization
    std::mutex m_system_mutex;
    std::mutex m_task_mutex;
    std::condition_variable m_task_cv;
    std::atomic<bool> m_shutdown_requested;
    
    // Helper methods
    bool initializeComponents();
    void shutdownComponents();
    void workerThreadFunction();
    bool processTask(const AITask& task);
    void updateTaskStatus(const std::string& task_id, const TaskStatus& status);
    bool validateConfiguration(const AIConfiguration& config);
    
    // Performance optimization
    void enableParallelProcessing();
    void enableMemoryMapping();
    void enableCompressionCaching();
    void enableAIAcceleration();
    
    // Model validation
    bool validateModel(const AIModel& model);
    bool checkModelCompatibility(const AIModel& model);
    
    // Error handling
    void logError(const std::string& error_message);
    void logWarning(const std::string& warning_message);
    void logInfo(const std::string& info_message);
    
    // Performance metrics
    struct PerformanceMetrics {
        float processing_speed;
        float response_time;
        float throughput;
        float efficiency;
        float cpu_usage;
        float gpu_usage;
        float memory_usage;
        uint32_t active_tasks;
        uint32_t completed_tasks;
        uint32_t failed_tasks;
    };
    
    PerformanceMetrics m_performance_metrics;
    bool m_performance_monitoring_enabled;
    
    void updatePerformanceMetrics();
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
};

} // namespace Context7
} // namespace UEVR
