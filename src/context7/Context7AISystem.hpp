#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <thread>
#include <future>

// Neural network and AI libraries
#include <torch/torch.h>
#include <opencv2/opencv.hpp>
#include <tensorflow/cc/client/client_session.h>
#include <tensorflow/cc/ops/standard_ops.h>

namespace UEVR {
namespace Context7 {

// Forward declarations
class NeuralNetworkEngine;
class DeepWikiAI;
class Context7Database;
class GameAnalyzer;
class PerformanceOptimizer;
class AITrainingManager;

// AI model types
enum class AIModelType {
    CONVOLUTIONAL_NEURAL_NETWORK,    // CNN for image processing
    RECURRENT_NEURAL_NETWORK,        // RNN for sequence processing
    TRANSFORMER,                     // Transformer for attention mechanisms
    GENERATIVE_ADVERSARIAL_NETWORK,  // GAN for content generation
    REINFORCEMENT_LEARNING,          // RL for decision making
    TRANSFER_LEARNING,               // Transfer learning models
    ENSEMBLE_MODEL,                  // Ensemble of multiple models
    CUSTOM_MODEL                     // Custom architecture
};

// AI task types
enum class AITaskType {
    IMAGE_CLASSIFICATION,            // Image classification
    OBJECT_DETECTION,                // Object detection
    SEMANTIC_SEGMENTATION,           // Semantic segmentation
    INSTANCE_SEGMENTATION,           // Instance segmentation
    OPTICAL_CHARACTER_RECOGNITION,   // OCR
    NATURAL_LANGUAGE_PROCESSING,     // NLP
    SPEECH_RECOGNITION,              // Speech recognition
    GAME_ANALYSIS,                   // Game state analysis
    PERFORMANCE_OPTIMIZATION,        // Performance optimization
    USER_BEHAVIOR_PREDICTION,        // User behavior prediction
    CONTENT_GENERATION,              // Content generation
    ANOMALY_DETECTION                // Anomaly detection
};

// AI model configuration
struct AIModelConfig {
    AIModelType model_type;
    AITaskType task_type;
    std::string model_name;
    std::string model_version;
    std::string model_path;
    std::vector<uint32_t> input_dimensions;
    std::vector<uint32_t> output_dimensions;
    uint32_t batch_size;
    float learning_rate;
    uint32_t max_epochs;
    bool enable_gpu_acceleration;
    bool enable_quantization;
    bool enable_pruning;
    std::map<std::string, std::string> hyperparameters;
};

// AI training configuration
struct AITrainingConfig {
    std::string training_data_path;
    std::string validation_data_path;
    std::string test_data_path;
    uint32_t training_epochs;
    uint32_t validation_interval;
    uint32_t checkpoint_interval;
    float early_stopping_patience;
    bool enable_data_augmentation;
    bool enable_transfer_learning;
    std::string pretrained_model_path;
    std::map<std::string, float> loss_weights;
    std::vector<std::string> metrics;
};

// AI inference configuration
struct AIInferenceConfig {
    bool enable_batch_processing;
    bool enable_streaming;
    bool enable_caching;
    uint32_t max_batch_size;
    float confidence_threshold;
    bool enable_post_processing;
    std::vector<std::string> post_processing_steps;
    bool enable_uncertainty_estimation;
    uint32_t max_inference_time_ms;
};

// AI performance metrics
struct AIModelMetrics {
    float accuracy;
    float precision;
    float recall;
    float f1_score;
    float mean_average_precision;
    float intersection_over_union;
    float dice_coefficient;
    float hausdorff_distance;
    float inference_time_ms;
    float training_time_hours;
    uint64_t total_samples;
    uint64_t correct_predictions;
    std::map<std::string, float> custom_metrics;
};

// AI training progress
struct AITrainingProgress {
    uint32_t current_epoch;
    uint32_t total_epochs;
    float current_loss;
    float current_accuracy;
    float validation_loss;
    float validation_accuracy;
    std::vector<float> training_loss_history;
    std::vector<float> training_accuracy_history;
    std::vector<float> validation_loss_history;
    std::vector<float> validation_accuracy_history;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point last_update;
    bool is_completed;
    bool is_cancelled;
};

// AI model information
struct AIModelInfo {
    std::string model_id;
    std::string model_name;
    std::string model_version;
    AIModelType model_type;
    AITaskType task_type;
    std::string description;
    std::string author;
    std::string license;
    std::chrono::steady_clock::time_point creation_date;
    std::chrono::steady_clock::time_point last_modified;
    uint64_t model_size_bytes;
    uint64_t parameter_count;
    AIModelMetrics metrics;
    bool is_trained;
    bool is_deployed;
    std::vector<std::string> dependencies;
    std::vector<std::string> tags;
};

// AI training job
struct AITrainingJob {
    std::string job_id;
    std::string model_name;
    AIModelConfig model_config;
    AITrainingConfig training_config;
    AITrainingProgress progress;
    std::string status;
    std::string error_message;
    std::chrono::steady_clock::time_point created_time;
    std::chrono::steady_clock::time_point started_time;
    std::chrono::steady_clock::time_point completed_time;
    uint32_t priority;
    bool is_background;
};

// AI inference request
struct AIInferenceRequest {
    std::string request_id;
    std::string model_name;
    std::vector<torch::Tensor> input_data;
    AIInferenceConfig config;
    std::chrono::steady_clock::time_point request_time;
    uint32_t priority;
    bool is_urgent;
    std::function<void(const std::vector<torch::Tensor>&)> callback;
};

// AI inference result
struct AIInferenceResult {
    std::string request_id;
    std::vector<torch::Tensor> output_data;
    float confidence;
    float inference_time_ms;
    std::map<std::string, float> additional_metrics;
    std::string error_message;
    bool is_success;
    std::chrono::steady_clock::time_point completion_time;
};

// AI system configuration
struct AISystemConfig {
    // Model management
    std::string models_directory;
    std::string cache_directory;
    std::string logs_directory;
    uint32_t max_models_in_memory;
    bool enable_model_compression;
    bool enable_model_quantization;
    
    // Training configuration
    uint32_t max_training_jobs;
    uint32_t max_concurrent_training;
    bool enable_distributed_training;
    std::string training_backend;
    
    // Inference configuration
    uint32_t max_inference_requests;
    uint32_t max_batch_size;
    bool enable_gpu_acceleration;
    bool enable_cpu_optimization;
    
    // Performance configuration
    bool enable_performance_monitoring;
    bool enable_auto_optimization;
    float target_inference_time_ms;
    uint32_t max_memory_usage_mb;
    
    // Logging and debugging
    std::string log_level;
    bool enable_debug_mode;
    bool enable_profiling;
    bool enable_tensorboard;
};

// AI system state
enum class AISystemState {
    UNINITIALIZED,
    INITIALIZING,
    INITIALIZED,
    RUNNING,
    TRAINING,
    INFERRING,
    OPTIMIZING,
    SHUTTING_DOWN,
    ERROR
};

// AI event types
enum class AIEventType {
    ModelLoaded,
    ModelUnloaded,
    TrainingStarted,
    TrainingCompleted,
    TrainingFailed,
    InferenceStarted,
    InferenceCompleted,
    InferenceFailed,
    ModelOptimized,
    PerformanceAlert,
    ErrorOccurred
};

// AI event data
struct AIEvent {
    AIEventType type;
    std::string source;
    std::string message;
    std::map<std::string, std::string> data;
    std::chrono::steady_clock::time_point timestamp;
    int severity_level;
};

// AI event listener
using AIEventListener = std::function<void(const AIEvent&)>;

// Main AI system class
class Context7AISystem {
public:
    // Constructor and destructor
    Context7AISystem();
    ~Context7AISystem();
    
    // Lifecycle management
    bool initialize(const AISystemConfig& config = AISystemConfig{});
    void shutdown();
    bool isInitialized() const;
    bool isRunning() const;
    
    // Configuration
    void setConfiguration(const AISystemConfig& config);
    AISystemConfig getConfiguration() const;
    bool loadConfiguration(const std::string& config_file);
    bool saveConfiguration(const std::string& config_file);
    
    // Model management
    bool loadModel(const std::string& model_path);
    bool unloadModel(const std::string& model_name);
    bool reloadModel(const std::string& model_name);
    std::vector<AIModelInfo> getLoadedModels() const;
    AIModelInfo getModelInfo(const std::string& model_name) const;
    bool isModelLoaded(const std::string& model_name) const;
    
    // Model training
    std::string startTraining(const AIModelConfig& model_config, 
                             const AITrainingConfig& training_config);
    bool stopTraining(const std::string& job_id);
    bool pauseTraining(const std::string& job_id);
    bool resumeTraining(const std::string& job_id);
    std::vector<AITrainingJob> getTrainingJobs() const;
    AITrainingJob getTrainingJob(const std::string& job_id) const;
    AITrainingProgress getTrainingProgress(const std::string& job_id) const;
    
    // Model inference
    std::string submitInferenceRequest(const AIInferenceRequest& request);
    bool cancelInferenceRequest(const std::string& request_id);
    AIInferenceResult getInferenceResult(const std::string& request_id);
    std::vector<AIInferenceRequest> getPendingRequests() const;
    std::vector<AIInferenceResult> getCompletedResults() const;
    
    // Model optimization
    bool optimizeModel(const std::string& model_name);
    bool quantizeModel(const std::string& model_name, int bits = 8);
    bool pruneModel(const std::string& model_name, float sparsity = 0.5f);
    bool compressModel(const std::string& model_name);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    std::map<std::string, float> getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
    // Model evaluation
    AIModelMetrics evaluateModel(const std::string& model_name, 
                                const std::string& test_data_path);
    bool compareModels(const std::vector<std::string>& model_names);
    std::string getBestModel(const std::string& task_type) const;
    
    // AutoML and hyperparameter optimization
    bool enableAutoML(bool enable);
    bool isAutoMLEnabled() const;
    std::string optimizeHyperparameters(const std::string& model_name,
                                      const std::string& training_data_path);
    
    // Transfer learning
    bool enableTransferLearning(bool enable);
    bool isTransferLearningEnabled() const;
    std::string createTransferLearningModel(const std::string& base_model,
                                          const std::string& target_task);
    
    // Model deployment
    bool deployModel(const std::string& model_name);
    bool undeployModel(const std::string& model_name);
    bool isModelDeployed(const std::string& model_name) const;
    std::vector<std::string> getDeployedModels() const;
    
    // Event system
    void addEventListener(AIEventType event_type, AIEventListener listener);
    void removeEventListener(AIEventType event_type, AIEventListener listener);
    void dispatchEvent(const AIEvent& event);
    
    // Update and maintenance
    void update(float delta_time);
    void cleanup();
    void garbageCollect();
    
    // Utility functions
    std::string getVersion() const;
    std::string getSystemInfo() const;
    bool isAIAvailable() const;
    bool isAITraining() const;
    bool isAIInferring() const;
    
    // Debug and profiling
    void enableDebugMode(bool enable);
    void enableProfiling(bool enable);
    void setLogLevel(const std::string& level);
    
    // Error handling
    std::string getLastError() const;
    void clearErrors();
    std::vector<std::string> getErrorHistory() const;
    
    // Model registry
    bool registerModel(const std::string& model_path);
    bool unregisterModel(const std::string& model_name);
    std::vector<AIModelInfo> getRegisteredModels() const;
    
    // Batch operations
    bool loadModels(const std::vector<std::string>& model_paths);
    bool unloadModels(const std::vector<std::string>& model_names);
    bool trainModels(const std::vector<AIModelConfig>& model_configs);
    
private:
    // Internal state
    AISystemState m_state;
    AISystemConfig m_config;
    std::atomic<bool> m_initialized;
    std::atomic<bool> m_running;
    
    // Subsystems
    std::unique_ptr<NeuralNetworkEngine> m_neural_network_engine;
    std::unique_ptr<DeepWikiAI> m_deep_wiki_ai;
    std::unique_ptr<Context7Database> m_context7_database;
    std::unique_ptr<GameAnalyzer> m_game_analyzer;
    std::unique_ptr<PerformanceOptimizer> m_performance_optimizer;
    std::unique_ptr<AITrainingManager> m_training_manager;
    
    // Model management
    std::map<std::string, std::shared_ptr<torch::jit::Module>> m_loaded_models;
    std::map<std::string, AIModelInfo> m_model_info;
    std::map<std::string, AIModelConfig> m_model_configs;
    
    // Training management
    std::map<std::string, AITrainingJob> m_training_jobs;
    std::vector<std::string> m_active_training_jobs;
    std::mutex m_training_mutex;
    
    // Inference management
    std::map<std::string, AIInferenceRequest> m_pending_requests;
    std::map<std::string, AIInferenceResult> m_completed_results;
    std::vector<std::string> m_active_inference_requests;
    std::mutex m_inference_mutex;
    
    // Performance monitoring
    std::map<std::string, float> m_performance_metrics;
    std::chrono::steady_clock::time_point m_last_performance_update;
    bool m_performance_monitoring_enabled;
    
    // Event system
    std::map<AIEventType, std::vector<AIEventListener>> m_event_listeners;
    
    // Error handling
    std::string m_last_error;
    std::vector<std::string> m_error_history;
    
    // Thread safety
    mutable std::mutex m_system_mutex;
    mutable std::mutex m_models_mutex;
    mutable std::mutex m_events_mutex;
    
    // Helper methods
    bool initializeSubsystems();
    void shutdownSubsystems();
    void updateSubsystems(float delta_time);
    void validateConfiguration(const AISystemConfig& config);
    void updatePerformanceMetrics();
    void logEvent(const AIEvent& event);
    void logError(const std::string& error);
    bool checkSystemRequirements();
    void handleSystemEvents();
    
    // Model management helpers
    bool loadModelInternal(const std::string& model_path);
    bool unloadModelInternal(const std::string& model_name);
    void updateModelInfo(const std::string& model_name, const AIModelInfo& info);
    bool validateModelFile(const std::string& model_path);
    
    // Training helpers
    void processTrainingJobs();
    void updateTrainingProgress(const std::string& job_id);
    bool validateTrainingConfig(const AITrainingConfig& config);
    
    // Inference helpers
    void processInferenceRequests();
    void updateInferenceResults(const std::string& request_id);
    bool validateInferenceRequest(const AIInferenceRequest& request);
    
    // Performance helpers
    void optimizeSystemPerformance();
    void monitorResourceUsage();
    void adjustSystemParameters();
    
    // AutoML helpers
    bool runHyperparameterOptimization(const std::string& model_name);
    bool runNeuralArchitectureSearch(const std::string& task_type);
    bool runFeatureEngineering(const std::string& dataset_path);
    
    // Transfer learning helpers
    bool createTransferLearningModelInternal(const std::string& base_model,
                                           const std::string& target_task);
    bool adaptModelToTask(const std::string& model_name, const std::string& task_type);
    
    // Model deployment helpers
    bool deployModelInternal(const std::string& model_name);
    bool undeployModelInternal(const std::string& model_name);
    bool validateDeploymentRequirements(const std::string& model_name);
    
    // Utility helpers
    std::string generateUniqueID() const;
    std::string getModelFilePath(const std::string& model_name) const;
    bool createDirectoryIfNotExists(const std::string& directory_path);
    void cleanupTemporaryFiles();
    
    // Performance monitoring
    std::chrono::steady_clock::time_point m_last_frame_time;
    std::chrono::steady_clock::time_point m_last_optimization_time;
    uint64_t m_total_inference_requests;
    uint64_t m_total_training_jobs;
    float m_average_inference_time;
    float m_average_training_time;
    
    // Debug and profiling
    bool m_debug_mode_enabled;
    bool m_profiling_enabled;
    std::string m_log_level;
    std::ofstream m_profiling_log;
};

} // namespace Context7
} // namespace UEVR
