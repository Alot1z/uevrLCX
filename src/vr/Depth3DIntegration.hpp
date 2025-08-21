#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace uevr {

/**
 * @brief Depth3D Integration - Depth Buffer Extraction
 * 
 * Integrates BlueSkyDefender's Depth3D (https://github.com/BlueSkyDefender/Depth3D) 
 * into the uevrLCX system. Depth3D extracts depth buffer information from games
 * to enable real 3D depth perception in VR.
 * 
 * Key Features:
 * - Real depth buffer extraction (not fake depth)
 * - True 3D collision detection
 * - Depth-aware rendering
 * - Performance-optimized depth processing
 * - Multi-engine depth extraction support
 */
class Depth3DIntegration {
public:
    Depth3DIntegration();
    ~Depth3DIntegration();

    // Core lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized; }

    // Depth buffer extraction
    bool extractDepthBuffer();
    bool processDepthData();
    bool generateDepthMap();
    bool validateDepthAccuracy();
    
    // Real collision detection
    bool enableRealCollisionDetection(bool enable);
    bool setupCollisionGeometry();
    bool processCollisionData();
    bool validateCollisionAccuracy();
    
    // Depth-aware rendering
    bool enableDepthAwareRendering(bool enable);
    bool setupDepthShaders();
    bool configureDepthRendering();
    bool optimizeDepthPerformance();
    
    // Multi-engine support
    bool detectDepthFormat(const std::string& engineName);
    bool configureEngineDepthExtraction(const std::string& engineName);
    bool validateEngineCompatibility(const std::string& engineName);

    // Configuration
    struct Depth3DSettings {
        // Depth extraction settings
        bool enableDepthExtraction = true;
        bool enableRealCollision = true;
        bool enableDepthAwareRendering = true;
        int depthBufferFormat = 0; // 0 = auto-detect
        float depthScale = 1.0f;
        float depthBias = 0.0f;
        
        // Collision detection settings
        bool enableCollisionGeometry = true;
        float collisionThreshold = 0.1f;
        int collisionPrecision = 1000;
        bool enableCollisionOptimization = true;
        
        // Rendering settings
        bool enableDepthShaders = true;
        bool enableDepthOptimization = true;
        float depthQuality = 1.0f;
        int depthSamples = 16;
        
        // Performance settings
        bool enablePerformanceOptimization = true;
        int maxDepthResolution = 2048;
        bool enableAsyncProcessing = true;
        float performanceScale = 1.0f;
        
        // Advanced settings
        bool debugMode = false;
        bool enableDepthLogging = true;
        std::string logLevel = "info";
    };

    void setDepth3DSettings(const Depth3DSettings& settings);
    Depth3DSettings getDepth3DSettings() const;

    // Status and monitoring
    struct Depth3DStatus {
        bool isExtracting = false;
        bool isProcessing = false;
        bool collisionDetected = false;
        bool depthMapGenerated = false;
        float depthAccuracy = 0.0f;
        float collisionAccuracy = 0.0f;
        float processingTime = 0.0f;
        std::string lastError;
    };

    Depth3DStatus getStatus() const;
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

    // Depth data access
    struct DepthData {
        std::vector<float> depthBuffer;
        int width = 0;
        int height = 0;
        float minDepth = 0.0f;
        float maxDepth = 1.0f;
        std::string format;
    };

    DepthData getCurrentDepthData() const;
    bool updateDepthData();
    bool validateDepthData(const DepthData& data);

    // Collision data access
    struct CollisionData {
        std::vector<float> collisionPoints;
        std::vector<float> collisionNormals;
        std::vector<int> collisionIndices;
        int numCollisions = 0;
        float collisionVolume = 0.0f;
    };

    CollisionData getCurrentCollisionData() const;
    bool updateCollisionData();
    bool validateCollisionData(const CollisionData& data);

private:
    // Depth3D framework integration
    struct Depth3DFramework;
    std::unique_ptr<Depth3DFramework> m_depth3DFramework;
    
    // Settings and state
    Depth3DSettings m_settings;
    Depth3DStatus m_status;
    bool m_initialized = false;
    
    // Current data
    DepthData m_currentDepthData;
    CollisionData m_currentCollisionData;
    
    // Error tracking
    std::vector<std::string> m_errors;
    
    // Internal methods
    bool loadDepth3DLibrary();
    bool initializeDepth3DFramework();
    bool setupDepth3DCallbacks();
    void updateStatus();
    void logError(const std::string& error);
    bool validateDepthFormat(int format);
    bool optimizeDepthProcessing();
    bool validateCollisionGeometry();
};

} // namespace uevr
