#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace UEVR {
namespace Context7 {

// Forward declarations
class NeuralLayer;
class NeuralWeight;
class NeuralBias;
class ActivationFunction;
class LossFunction;
class Optimizer;

// Neural network architecture configuration
struct NetworkArchitecture {
    uint32_t input_layer_size;              // Number of input neurons
    std::vector<uint32_t> hidden_layers;    // Hidden layer sizes
    uint32_t output_layer_size;             // Number of output neurons
    std::string activation_function;        // Activation function type
    std::string loss_function;              // Loss function type
    std::string optimizer_type;             // Optimizer type
    float learning_rate;                    // Learning rate for training
    float dropout_rate;                     // Dropout rate for regularization
    float momentum;                         // Momentum for optimization
    float weight_decay;                     // Weight decay for regularization
    bool enable_batch_normalization;        // Enable batch normalization
    bool enable_early_stopping;             // Enable early stopping
    uint32_t patience_epochs;               // Patience for early stopping
    float validation_split;                 // Validation data split ratio
    uint32_t max_epochs;                    // Maximum training epochs
    uint32_t batch_size;                    // Training batch size
    bool enable_shuffle;                    // Enable data shuffling
    std::string weight_initialization;     // Weight initialization method
    float weight_scale;                     // Weight scaling factor
    bool enable_gradient_clipping;          // Enable gradient clipping
    float gradient_clip_value;              // Gradient clipping threshold
};

// Neural layer configuration
struct LayerConfig {
    uint32_t layer_id;                      // Layer identifier
    uint32_t input_size;                    // Input size for this layer
    uint32_t output_size;                   // Output size for this layer
    std::string layer_type;                 // Layer type (Dense, Conv, Pool, etc.)
    std::string activation_function;        // Activation function
    bool enable_bias;                       // Whether to use bias
    bool enable_dropout;                    // Whether to use dropout
    float dropout_rate;                     // Dropout rate for this layer
    bool enable_batch_norm;                 // Whether to use batch normalization
    std::map<std::string, float> parameters; // Layer-specific parameters
    std::string name;                       // Layer name for identification
    bool trainable;                         // Whether layer is trainable
    std::vector<uint32_t> connections;      // Connected layer IDs
    float learning_rate_multiplier;         // Learning rate multiplier
    std::string regularization;             // Regularization method
    float regularization_strength;          // Regularization strength
};

// Activation function types
enum class ActivationFunctionType {
    Linear,                 // Linear activation (no transformation)
    Sigmoid,               // Sigmoid activation (0 to 1)
    Tanh,                  // Hyperbolic tangent (-1 to 1)
    ReLU,                  // Rectified Linear Unit (max(0, x))
    LeakyReLU,             // Leaky ReLU (allows small negative values)
    ELU,                   // Exponential Linear Unit
    Softmax,               // Softmax activation (probability distribution)
    Swish,                 // Swish activation (x * sigmoid(x))
    GELU,                  // Gaussian Error Linear Unit
    Mish,                  // Mish activation (x * tanh(softplus(x)))
    Custom                  // Custom activation function
};

// Loss function types
enum class LossFunctionType {
    MeanSquaredError,       // Mean Squared Error
    MeanAbsoluteError,      // Mean Absolute Error
    BinaryCrossEntropy,     // Binary Cross Entropy
    CategoricalCrossEntropy, // Categorical Cross Entropy
    HingeLoss,              // Hinge Loss (SVM)
    HuberLoss,              // Huber Loss (robust to outliers)
    CosineSimilarity,       // Cosine Similarity Loss
    KLDivergence,           // Kullback-Leibler Divergence
    Custom                   // Custom loss function
};

// Optimizer types
enum class OptimizerType {
    SGD,                    // Stochastic Gradient Descent
    Adam,                   // Adam optimizer
    RMSprop,                // RMSprop optimizer
    Adagrad,                // Adagrad optimizer
    Adadelta,               // Adadelta optimizer
    AdamW,                  // Adam with weight decay
    Lion,                   // Lion optimizer
    Custom                   // Custom optimizer
};

// Training configuration
struct TrainingConfig {
    std::vector<std::vector<float>> training_data;      // Training input data
    std::vector<std::vector<float>> training_labels;    // Training target labels
    std::vector<std::vector<float>> validation_data;    // Validation input data
    std::vector<std::vector<float>> validation_labels;  // Validation target labels
    uint32_t epochs;                                    // Number of training epochs
    uint32_t batch_size;                                // Training batch size
    float learning_rate;                                // Learning rate
    bool enable_early_stopping;                         // Enable early stopping
    uint32_t patience_epochs;                           // Patience for early stopping
    float min_delta;                                     // Minimum improvement for early stopping
    bool enable_shuffle;                                 // Enable data shuffling
    uint32_t seed;                                       // Random seed for reproducibility
    std::string checkpoint_path;                         // Path for saving checkpoints
    uint32_t checkpoint_frequency;                       // Checkpoint frequency in epochs
    bool enable_tensorboard;                             // Enable TensorBoard logging
    std::string tensorboard_log_dir;                     // TensorBoard log directory
    bool enable_progress_bar;                            // Enable training progress bar
    std::function<void(uint32_t, float, float)> callback; // Training callback function
};

// Training state and progress
struct TrainingState {
    uint32_t current_epoch;                              // Current training epoch
    uint32_t total_epochs;                               // Total training epochs
    uint32_t current_batch;                              // Current batch in epoch
    uint32_t total_batches;                              // Total batches in epoch
    float current_loss;                                  // Current training loss
    float current_accuracy;                              // Current training accuracy
    float current_validation_loss;                       // Current validation loss
    float current_validation_accuracy;                   // Current validation accuracy
    float best_validation_loss;                          // Best validation loss so far
    float best_validation_accuracy;                      // Best validation accuracy so far
    uint32_t epochs_without_improvement;                 // Epochs without improvement
    bool early_stopping_triggered;                       // Whether early stopping was triggered
    std::chrono::steady_clock::time_point start_time;    // Training start time
    std::chrono::steady_clock::time_point last_update;   // Last update time
    std::vector<float> training_loss_history;            // Training loss history
    std::vector<float> training_accuracy_history;        // Training accuracy history
    std::vector<float> validation_loss_history;          // Validation loss history
    std::vector<float> validation_accuracy_history;      // Validation accuracy history
    std::vector<float> learning_rate_history;            // Learning rate history
    bool is_training;                                     // Whether currently training
    bool is_paused;                                       // Whether training is paused
    std::string status_message;                           // Current status message
};

// Network performance metrics
struct NetworkPerformance {
    float training_loss;                                  // Final training loss
    float training_accuracy;                              // Final training accuracy
    float validation_loss;                                // Final validation loss
    float validation_accuracy;                            // Final validation accuracy
    float test_loss;                                      // Test loss (if test data provided)
    float test_accuracy;                                  // Test accuracy (if test data provided)
    uint32_t training_epochs;                             // Actual training epochs
    std::chrono::milliseconds training_time;              // Total training time
    float average_epoch_time;                             // Average time per epoch
    float memory_usage_mb;                                // Memory usage in MB
    uint32_t total_parameters;                            // Total network parameters
    uint32_t trainable_parameters;                        // Trainable parameters
    std::map<std::string, float> additional_metrics;     // Additional performance metrics
    std::vector<float> confusion_matrix;                  // Confusion matrix (if classification)
    float f1_score;                                       // F1 score (if applicable)
    float precision;                                      // Precision (if applicable)
    float recall;                                         // Recall (if applicable)
    float auc_score;                                      // AUC score (if applicable)
    std::vector<float> roc_curve;                        // ROC curve data
    std::vector<float> precision_recall_curve;            // Precision-recall curve data
};

// Game features for neural network input
struct GameFeatures {
    // Engine properties (normalized 0.0-1.0)
    float engine_type;                                    // Engine type identifier
    float engine_version;                                 // Engine version number
    float graphics_api;                                   // Graphics API type
    float renderer_type;                                  // Renderer type
    
    // Game properties (normalized 0.0-1.0)
    float game_genre;                                     // Game genre classification
    float release_year;                                   // Release year (normalized)
    float complexity_score;                               // Game complexity score
    float art_style;                                      // Visual art style
    float gameplay_type;                                  // Gameplay type
    
    // Technical properties (normalized 0.0-1.0)
    float polygon_count;                                  // Estimated polygon count
    float texture_resolution;                             // Texture resolution
    float shader_complexity;                              // Shader complexity score
    float lighting_complexity;                            // Lighting system complexity
    float particle_system_complexity;                     // Particle system complexity
    
    // VR-specific properties (normalized 0.0-1.0)
    float camera_type;                                    // Camera system type
    float ui_complexity;                                  // UI complexity score
    float input_system;                                   // Input system type
    float audio_complexity;                               // Audio system complexity
    float physics_complexity;                             // Physics system complexity
    
    // Performance metrics (normalized 0.0-1.0)
    float base_fps;                                       // Base frame rate
    float memory_usage;                                   // Memory usage
    float cpu_usage;                                      // CPU usage
    float gpu_usage;                                      // GPU usage
    float storage_requirements;                           // Storage requirements
    float network_requirements;                           // Network requirements
    
    // User preferences (normalized 0.0-1.0)
    float user_vr_experience;                             // User VR experience level
    float user_performance_preference;                    // Performance vs quality preference
    float user_comfort_preference;                        // Comfort vs immersion preference
    float user_hardware_level;                            // User hardware capability level
};

// Game predictions from neural network
struct GamePredictions {
    // Overall scores (0.0-1.0)
    float vr_compatibility_score;                         // VR compatibility score
    float performance_score;                              // Expected performance score
    float comfort_score;                                  // VR comfort level score
    float optimization_potential;                         // Optimization potential score
    
    // Detailed predictions
    std::vector<float> feature_importance;                // Feature importance weights
    std::vector<std::string> recommendations;             // AI-generated recommendations
    float confidence_level;                               // Prediction confidence (0.0-1.0)
    
    // Performance predictions
    float predicted_fps;                                  // Predicted frame rate
    float predicted_latency;                              // Predicted latency
    std::vector<float> fps_timeline;                      // FPS timeline prediction
    float predicted_memory_usage;                         // Predicted memory usage
    float predicted_cpu_usage;                            // Predicted CPU usage
    float predicted_gpu_usage;                            // Predicted GPU usage
    
    // VR-specific predictions
    float motion_sickness_risk;                           // Motion sickness risk assessment
    float room_scale_compatibility;                       // Room scale compatibility
    float seated_play_compatibility;                      // Seated play compatibility
    float standing_play_compatibility;                    // Standing play compatibility
    float social_vr_compatibility;                        // Social VR compatibility
    
    // Optimization suggestions
    std::vector<std::string> performance_optimizations;   // Performance optimization tips
    std::vector<std::string> vr_setting_recommendations;  // VR setting recommendations
    std::vector<std::string> hardware_recommendations;    // Hardware upgrade suggestions
    std::vector<std::string> comfort_improvements;        // Comfort improvement suggestions
};

// Main Neural Network Engine class
class NeuralNetworkEngine {
public:
    NeuralNetworkEngine();
    ~NeuralNetworkEngine();
    
    // Core initialization and management
    bool initialize(const NetworkArchitecture& architecture);
    void shutdown();
    bool isInitialized() const;
    NetworkArchitecture getArchitecture() const;
    
    // Network training
    bool train(const TrainingConfig& config);
    bool trainAsync(const TrainingConfig& config);
    void pauseTraining();
    void resumeTraining();
    void stopTraining();
    TrainingState getTrainingState() const;
    
    // Prediction and inference
    GamePredictions predictGame(const GameFeatures& features);
    std::vector<float> predictRaw(const std::vector<float>& input);
    float predictSingle(const std::vector<float>& input, uint32_t output_index);
    
    // Model management
    bool saveModel(const std::string& file_path);
    bool loadModel(const std::string& file_path);
    bool exportModel(const std::string& file_path, const std::string& format);
    bool importModel(const std::string& file_path, const std::string& format);
    
    // Performance evaluation
    NetworkPerformance evaluatePerformance(const std::vector<std::vector<float>>& test_data,
                                          const std::vector<std::vector<float>>& test_labels);
    float calculateAccuracy(const std::vector<std::vector<float>>& predictions,
                           const std::vector<std::vector<float>>& labels);
    float calculateLoss(const std::vector<std::vector<float>>& predictions,
                        const std::vector<std::vector<float>>& labels);
    
    // Network analysis
    std::vector<float> getFeatureImportance(const std::vector<std::string>& feature_names);
    std::vector<std::vector<float>> getLayerActivations(const std::vector<float>& input);
    std::vector<std::vector<float>> getWeightMatrices();
    std::vector<float> getBiasVectors();
    
    // Configuration and optimization
    void setLearningRate(float learning_rate);
    void setBatchSize(uint32_t batch_size);
    void setDropoutRate(float dropout_rate);
    void setOptimizer(const std::string& optimizer_type);
    void setLossFunction(const std::string& loss_function);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    std::map<std::string, float> getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
private:
    // Network architecture and state
    NetworkArchitecture m_architecture;
    std::vector<std::unique_ptr<NeuralLayer>> m_layers;
    std::vector<std::vector<NeuralWeight>> m_weights;
    std::vector<std::vector<NeuralBias>> m_biases;
    bool m_initialized;
    
    // Training state
    TrainingState m_training_state;
    TrainingConfig m_current_training_config;
    std::unique_ptr<Optimizer> m_optimizer;
    std::unique_ptr<LossFunction> m_loss_function;
    
    // Performance monitoring
    bool m_performance_monitoring_enabled;
    std::map<std::string, float> m_performance_metrics;
    std::chrono::steady_clock::time_point m_last_metrics_update;
    
    // Helper methods
    bool initializeLayers();
    bool initializeWeights();
    bool initializeBiases();
    void forwardPass(const std::vector<float>& input, std::vector<std::vector<float>>& activations);
    void backwardPass(const std::vector<float>& input, const std::vector<float>& target,
                      std::vector<std::vector<float>>& activations,
                      std::vector<std::vector<float>>& gradients);
    void updateWeights(const std::vector<std::vector<float>>& gradients);
    float calculateLoss(const std::vector<float>& prediction, const std::vector<float>& target);
    std::vector<float> calculateGradients(const std::vector<float>& prediction,
                                         const std::vector<float>& target);
    
    // Training helpers
    void shuffleData(std::vector<std::vector<float>>& data, std::vector<std::vector<float>>& labels);
    std::vector<std::pair<std::vector<float>, std::vector<float>>> createBatches(
        const std::vector<std::vector<float>>& data, const std::vector<std::vector<float>>& labels);
    bool shouldStopEarly();
    void updateTrainingProgress(uint32_t epoch, uint32_t batch, float loss, float accuracy);
    
    // Performance optimization
    void enableGPUAcceleration();
    void enableParallelProcessing();
    void optimizeMemoryUsage();
    
    // Error handling
    void logError(const std::string& error_message);
    void logWarning(const std::string& warning_message);
    void logInfo(const std::string& info_message);
    
    // Random number generation
    std::mt19937 m_random_engine;
    std::uniform_real_distribution<float> m_weight_distribution;
    std::normal_distribution<float> m_normal_distribution;
    
    void initializeRandomEngine(uint32_t seed);
    float generateRandomWeight();
    float generateRandomBias();
};

} // namespace Context7
} // namespace UEVR
