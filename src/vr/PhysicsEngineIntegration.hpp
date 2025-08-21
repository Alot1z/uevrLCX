#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace uevr {

/**
 * @brief Physics Engine Integration - Real Collision Detection
 * 
 * Integrates Bullet Physics (https://github.com/bulletphysics/bullet3) and 
 * PhysX (https://github.com/NVIDIA-Omniverse/PhysX) into the uevrLCX system
 * to provide real collision detection and physics simulation in VR.
 * 
 * Key Features:
 * - Real collision detection (not fake depth)
 * - Physics simulation integration
 * - Multi-physics engine support
 * - VR-optimized physics
 * - Performance-optimized collision detection
 */
class PhysicsEngineIntegration {
public:
    PhysicsEngineIntegration();
    ~PhysicsEngineIntegration();

    // Core lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized; }

    // Physics engine selection
    enum class PhysicsEngine {
        BULLET,
        PHYSX,
        AUTO
    };

    bool selectPhysicsEngine(PhysicsEngine engine);
    PhysicsEngine getCurrentEngine() const;
    std::vector<PhysicsEngine> getAvailableEngines() const;
    
    // Collision detection
    bool enableCollisionDetection(bool enable);
    bool setupCollisionWorld();
    bool processCollisionData();
    bool validateCollisionAccuracy();
    
    // Physics simulation
    bool enablePhysicsSimulation(bool enable);
    bool setupPhysicsWorld();
    bool simulatePhysics(float deltaTime);
    bool validatePhysicsAccuracy();
    
    // VR-specific physics
    bool enableVRPhysics(bool enable);
    bool setupVRPhysicsWorld();
    bool configureVRPhysics();
    bool optimizeVRPhysics();
    
    // Multi-engine support
    bool detectGamePhysics(const std::string& gamePath);
    bool integrateGamePhysics(const std::string& gamePath);
    bool validateGamePhysicsCompatibility(const std::string& gamePath);

    // Configuration
    struct PhysicsSettings {
        // Engine settings
        PhysicsEngine selectedEngine = PhysicsEngine::AUTO;
        bool enableCollisionDetection = true;
        bool enablePhysicsSimulation = true;
        bool enableVRPhysics = true;
        
        // Collision settings
        float collisionThreshold = 0.01f;
        int collisionPrecision = 1000;
        bool enableCollisionOptimization = true;
        bool enableBroadPhaseOptimization = true;
        
        // Physics settings
        float gravity = -9.81f;
        float timeStep = 1.0f / 60.0f;
        int maxSubSteps = 10;
        bool enableContinuousCollision = true;
        
        // VR-specific settings
        bool enableVRHandPhysics = true;
        bool enableVRHeadPhysics = true;
        bool enableVRBodyPhysics = true;
        float vrPhysicsScale = 1.0f;
        
        // Performance settings
        bool enablePerformanceOptimization = true;
        int maxPhysicsObjects = 10000;
        bool enableAsyncPhysics = true;
        float performanceScale = 1.0f;
        
        // Advanced settings
        bool debugMode = false;
        bool enablePhysicsLogging = true;
        std::string logLevel = "info";
    };

    void setPhysicsSettings(const PhysicsSettings& settings);
    PhysicsSettings getPhysicsSettings() const;

    // Status and monitoring
    struct PhysicsStatus {
        bool isInitialized = false;
        bool collisionDetectionActive = false;
        bool physicsSimulationActive = false;
        bool vrPhysicsActive = false;
        int activeCollisions = 0;
        int activePhysicsObjects = 0;
        float simulationTime = 0.0f;
        std::string lastError;
    };

    PhysicsStatus getStatus() const;
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

    // Collision data access
    struct CollisionData {
        std::vector<float> collisionPoints;
        std::vector<float> collisionNormals;
        std::vector<float> collisionForces;
        std::vector<int> collisionIndices;
        int numCollisions = 0;
        float totalCollisionForce = 0.0f;
    };

    CollisionData getCurrentCollisionData() const;
    bool updateCollisionData();
    bool validateCollisionData(const CollisionData& data);

    // Physics object management
    struct PhysicsObject {
        std::string id;
        std::string type; // "rigid", "soft", "kinematic"
        std::vector<float> position;
        std::vector<float> rotation;
        std::vector<float> velocity;
        std::vector<float> angularVelocity;
        std::vector<float> mass;
        std::vector<float> inertia;
        bool isActive = true;
    };

    std::vector<PhysicsObject> getPhysicsObjects() const;
    bool addPhysicsObject(const PhysicsObject& object);
    bool removePhysicsObject(const std::string& objectId);
    bool updatePhysicsObject(const PhysicsObject& object);

    // Performance monitoring
    struct PhysicsPerformance {
        float collisionDetectionTime = 0.0f;
        float physicsSimulationTime = 0.0f;
        float vrPhysicsTime = 0.0f;
        int collisionChecks = 0;
        int physicsSteps = 0;
        float memoryUsage = 0.0f;
    };

    PhysicsPerformance getPerformance() const;
    float getTotalPhysicsTime() const;

private:
    // Physics engine implementations
    struct BulletPhysicsEngine;
    struct PhysXEngine;
    
    std::unique_ptr<BulletPhysicsEngine> m_bulletEngine;
    std::unique_ptr<PhysXEngine> m_physXEngine;
    
    // Settings and state
    PhysicsSettings m_settings;
    PhysicsStatus m_status;
    bool m_initialized = false;
    
    // Current data
    CollisionData m_currentCollisionData;
    std::vector<PhysicsObject> m_physicsObjects;
    PhysicsPerformance m_performance;
    
    // Error tracking
    std::vector<std::string> m_errors;
    
    // Internal methods
    bool initializeBulletPhysics();
    bool initializePhysX();
    bool setupPhysicsCallbacks();
    void updateStatus();
    void logError(const std::string& error);
    bool validatePhysicsEngine(PhysicsEngine engine);
    bool optimizePhysicsPerformance();
    bool validatePhysicsObjects();
    void updatePhysicsPerformance();
};

} // namespace uevr
