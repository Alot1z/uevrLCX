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

// Neural rendering includes
#include <torch/torch.h>
#include <opencv2/opencv.hpp>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>

// Graphics includes
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// VR-specific includes
#include "../vr/VRSystem.hpp"
#include "../vr/OpenXRIntegration.hpp"
#include "../ai/AIPoweredVROptimizer.hpp"

namespace uevr {

enum class NeuralRenderingType {
    SUPER_RESOLUTION,      // AI-powered upscaling
    DENOISING,            // AI-powered noise reduction
    FRAME_INTERPOLATION,  // AI-powered frame generation
    DEPTH_ENHANCEMENT,    // AI-powered depth improvement
    TEXTURE_SYNTHESIS,    // AI-powered texture generation
    LIGHTING_ENHANCEMENT, // AI-powered lighting improvement
    MOTION_VECTOR_PREDICTION, // AI-powered motion prediction
    STEREO_ENHANCEMENT    // AI-powered stereo improvement
};

enum class NeuralModelType {
    ESRGAN,              // Enhanced Super-Resolution GAN
    REAL_ESRGAN,         // Real-ESRGAN for realistic upscaling
    NAFNET,              // Non-Local Attention Network for denoising
    RIFE,                // Real-time Intermediate Flow Estimation
    DPT,                 // Dense Prediction Transformer
    STYLEGAN,            // Style-based GAN for texture synthesis
    NEURAL_RADIOSITY,    // Neural radiosity for lighting
    TRANSFORMER_VR       // Transformer for VR-specific enhancement
};

struct NeuralRenderingSettings {
    NeuralRenderingType renderingType = NeuralRenderingType::SUPER_RESOLUTION;
    NeuralModelType modelType = NeuralModelType::REAL_ESRGAN;
    
    // Quality settings
    int targetResolution = 2448;  // Target resolution for upscaling
    float qualityLevel = 0.8f;    // Quality vs performance trade-off
    bool enableRealTime = true;   // Real-time processing
    bool enableBatchProcessing = false; // Batch processing for better quality
    
    // Performance settings
    int maxBatchSize = 4;
    float maxProcessingTime = 16.67f; // 60 FPS target
    bool enableGPUAcceleration = true;
    bool enableTensorRT = false;  // NVIDIA TensorRT optimization
    
    // VR-specific settings
    bool enableStereoConsistency = true;
    bool enableMotionCompensation = true;
    bool enableDepthAwareProcessing = true;
    float stereoConsistencyWeight = 0.5f;
    
    // Advanced settings
    bool enableAdaptiveQuality = true;
    bool enableDynamicResolution = true;
    bool enableQualityPrediction = true;
    float adaptiveThreshold = 0.1f;
};

struct NeuralRenderingInput {
    std::string frameId;
    cv::Mat leftEye;
    cv::Mat rightEye;
    cv::Mat depthMap;
    cv::Mat motionVectors;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    float timestamp;
    
    // Additional context
    std::vector<glm::vec3> cameraPositions;
    std::vector<glm::quat> cameraRotations;
    std::vector<float> frameTimes;
    std::vector<cv::Mat> previousFrames;
};

struct NeuralRenderingOutput {
    std::string frameId;
    cv::Mat enhancedLeftEye;
    cv::Mat enhancedRightEye;
    cv::Mat enhancedDepthMap;
    cv::Mat confidenceMap;
    float processingTime;
    float qualityScore;
    float confidence;
    
    // Performance metrics
    float gpuUtilization;
    float memoryUsage;
    float throughput;
    int framesProcessed;
};

struct NeuralRenderingModel {
    std::string modelName;
    NeuralModelType modelType;
    std::string modelPath;
    std::vector<int> inputShape;
    std::vector<int> outputShape;
    float modelSize;
    float inferenceTime;
    float accuracy;
    
    // Model parameters
    std::vector<float> weights;
    std::vector<float> biases;
    std::unordered_map<std::string, std::vector<float>> parameters;
    
    // Performance characteristics
    bool isLoaded;
    bool isOptimized;
    float memoryUsage;
    float computeComplexity;
};

struct NeuralRenderingMetrics {
    float averageProcessingTime;
    float averageQualityScore;
    float averageConfidence;
    float throughput;
    float gpuUtilization;
    float memoryUsage;
    int totalFramesProcessed;
    int successfulFrames;
    float successRate;
    
    // Quality metrics
    float psnr;  // Peak Signal-to-Noise Ratio
    float ssim;  // Structural Similarity Index
    float lpips; // Learned Perceptual Image Patch Similarity
    float fid;   // Fréchet Inception Distance
    
    // Performance metrics
    float latency;
    float frameRate;
    float efficiency;
    float optimizationLevel;
};

class NeuralRenderingSystem {
public:
    NeuralRenderingSystem();
    ~NeuralRenderingSystem();
    
    // Initialization and configuration
    bool initialize(const NeuralRenderingSettings& settings = NeuralRenderingSettings{});
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }
    
    void setRenderingSettings(const NeuralRenderingSettings& settings);
    NeuralRenderingSettings getRenderingSettings() const;
    
    // Model management
    bool loadNeuralModel(const std::string& modelPath, NeuralModelType modelType);
    bool unloadNeuralModel(const std::string& modelName);
    bool optimizeModel(const std::string& modelName);
    std::vector<NeuralRenderingModel> getLoadedModels() const;
    
    // Neural rendering operations
    NeuralRenderingOutput processFrame(const NeuralRenderingInput& input);
    std::vector<NeuralRenderingOutput> processBatch(const std::vector<NeuralRenderingInput>& inputs);
    
    // Specific enhancement functions
    cv::Mat enhanceSuperResolution(const cv::Mat& input, float scaleFactor = 2.0f);
    cv::Mat enhanceDenoising(const cv::Mat& input, float noiseLevel = 0.1f);
    cv::Mat enhanceFrameInterpolation(const cv::Mat& frame1, const cv::Mat& frame2, float interpolationFactor = 0.5f);
    cv::Mat enhanceDepthMap(const cv::Mat& depthMap, const cv::Mat& colorImage);
    cv::Mat enhanceTexture(const cv::Mat& texture, const cv::Mat& normalMap);
    cv::Mat enhanceLighting(const cv::Mat& image, const cv::Mat& depthMap, const cv::Mat& normalMap);
    
    // VR-specific enhancements
    std::pair<cv::Mat, cv::Mat> enhanceStereoPair(const cv::Mat& leftEye, const cv::Mat& rightEye);
    cv::Mat enhanceMotionVectors(const cv::Mat& motionVectors, const cv::Mat& depthMap);
    cv::Mat enhanceStereoConsistency(const cv::Mat& leftEye, const cv::Mat& rightEye, const cv::Mat& disparity);
    
    // Real-time processing
    void startRealTimeProcessing();
    void stopRealTimeProcessing();
    bool isProcessing() const { return m_processing.load(); }
    
    void updateProcessing();
    void processRealTimeFrame(const NeuralRenderingInput& input);
    NeuralRenderingOutput getLatestOutput() const;
    
    // Performance optimization
    bool enableGPUAcceleration(bool enable);
    bool enableTensorRTOptimization(bool enable);
    bool enableAdaptiveProcessing(bool enable);
    void setPerformanceTarget(float targetFrameRate, float targetLatency);
    
    // Quality control
    float evaluateQuality(const cv::Mat& original, const cv::Mat& enhanced);
    float evaluateConfidence(const NeuralRenderingOutput& output);
    bool validateOutput(const NeuralRenderingOutput& output);
    
    // Performance monitoring
    NeuralRenderingMetrics getMetrics() const;
    void enableMetricsCollection(bool enable);
    void setMetricsCallback(std::function<void(const NeuralRenderingMetrics&)> callback);
    
    // Advanced features
    bool enableMultiModelProcessing(bool enable);
    bool enableQualityPrediction(bool enable);
    bool enableAdaptiveModelSelection(bool enable);
    
    // Model training and fine-tuning
    bool trainModel(const std::vector<cv::Mat>& trainingData, const std::vector<cv::Mat>& groundTruth);
    bool fineTuneModel(const std::string& modelName, const std::vector<cv::Mat>& fineTuneData);
    float evaluateModelAccuracy(const std::string& modelName, const std::vector<cv::Mat>& testData);
    
    // Debug and visualization
    void enableDebugMode(bool enable);
    void setDebugCallback(std::function<void(const std::string&, const cv::Mat&)> callback);
    cv::Mat getDebugVisualization() const;
    
    // Memory management
    void clearCache();
    void optimizeMemory();
    float getMemoryUsage() const;
    
    // Export and import
    bool exportModel(const std::string& modelName, const std::string& exportPath);
    bool importModel(const std::string& importPath, const std::string& modelName);

private:
    // Neural model implementations
    class SuperResolutionModel;
    std::unique_ptr<SuperResolutionModel> m_superResolution;
    
    class DenoisingModel;
    std::unique_ptr<DenoisingModel> m_denoising;
    
    class FrameInterpolationModel;
    std::unique_ptr<FrameInterpolationModel> m_frameInterpolation;
    
    class DepthEnhancementModel;
    std::unique_ptr<DepthEnhancementModel> m_depthEnhancement;
    
    class TextureSynthesisModel;
    std::unique_ptr<TextureSynthesisModel> m_textureSynthesis;
    
    class LightingEnhancementModel;
    std::unique_ptr<LightingEnhancementModel> m_lightingEnhancement;
    
    class StereoEnhancementModel;
    std::unique_ptr<StereoEnhancementModel> m_stereoEnhancement;
    
    // Common members
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_processing{false};
    std::atomic<bool> m_shutdown{false};
    
    NeuralRenderingSettings m_settings;
    mutable std::mutex m_settingsMutex;
    
    std::unordered_map<std::string, NeuralRenderingModel> m_models;
    mutable std::mutex m_modelsMutex;
    
    std::vector<NeuralRenderingInput> m_inputQueue;
    std::vector<NeuralRenderingOutput> m_outputQueue;
    mutable std::mutex m_queueMutex;
    
    NeuralRenderingMetrics m_metrics;
    mutable std::mutex m_metricsMutex;
    bool m_metricsCollectionEnabled{false};
    std::function<void(const NeuralRenderingMetrics&)> m_metricsCallback;
    
    std::thread m_processingThread;
    std::condition_variable m_processingCV;
    std::mutex m_processingMutex;
    
    bool m_debugModeEnabled{false};
    std::function<void(const std::string&, const cv::Mat&)> m_debugCallback;
    cv::Mat m_debugVisualization;
    mutable std::mutex m_debugMutex;
    
    // Performance optimization
    bool m_gpuAccelerationEnabled{true};
    bool m_tensorRTOptimizationEnabled{false};
    bool m_adaptiveProcessingEnabled{true};
    float m_targetFrameRate{90.0f};
    float m_targetLatency{11.0f};
    
    // Advanced features
    bool m_multiModelProcessingEnabled{false};
    bool m_qualityPredictionEnabled{true};
    bool m_adaptiveModelSelectionEnabled{true};
    
    // Memory management
    std::vector<cv::Mat> m_frameCache;
    std::vector<cv::Mat> m_modelCache;
    size_t m_maxCacheSize{1000};
    mutable std::mutex m_cacheMutex;
    
    // Private methods
    void processingLoop();
    void updateMetrics();
    void processInputQueue();
    void updateOutputQueue();
    bool initializeNeuralModels();
    void cleanupNeuralModels();
    void optimizeModels();
    void validateModels();
    void updateModelSelection();
    void predictQuality();
    void adaptProcessing();
    void logNeuralError(const std::string& error);
    void saveModelState();
    void loadModelState();
    void clearModelCache();
    void optimizeMemoryUsage();
};

} // namespace uevr
