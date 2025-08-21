#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <spdlog/spdlog.h>

// Physics Core Integration
namespace uevrLCX {
namespace Physics {

// Core Physics Configuration
struct PhysicsConfig {
    bool enableBulletPhysics = true;
    bool enablePhysX = true;
    bool enableHavok = true;
    bool enableRealCollision = true;
    bool enableHapticFeedback = true;
    bool enableForceFeedback = true;
    
    // Collision settings
    bool enableCollisionDetection = true;
    bool enableCollisionResponse = true;
    bool enableSpatialPartitioning = true;
    bool enableBroadPhase = true;
    bool enableNarrowPhase = true;
    
    // Physics simulation settings
    bool enableRealTimeSimulation = true;
    bool enableMultiThreading = true;
    bool enableGPUAcceleration = true;
    bool enableContinuousCollision = true;
    bool enablePenetrationResolution = true;
    
    // Advanced features
    bool enableNeuralPhysics = true;
    bool enableAIPhysicsOptimization = true;
    bool enablePhysicsPrediction = true;
    bool enableAdaptivePhysics = true;
    
    // Community integration
    bool enableRedditPhysics = true;
    bool enableCommunityPhysics = true;
    bool enableResearchPhysics = true;
};

// Physics Metrics
struct PhysicsMetrics {
    int physicsObjects = 0;
    int collisionEvents = 0;
    int physicsSteps = 0;
    float simulationTime = 0.0f;
    float collisionAccuracy = 0.0f;
    float physicsPerformance = 0.0f;
    int redditPhysicsApplied = 0;
    int communityPhysicsUsed = 0;
    int researchPhysicsApplied = 0;
    
    // Performance metrics
    float cpuUtilization = 0.0f;
    float gpuUtilization = 0.0f;
    float memoryUsage = 0.0f;
    int activeCollisions = 0;
    float physicsLatency = 0.0f;
};

// Physics Object Data Structure
struct PhysicsObject {
    enum class Type {
        Box,
        Sphere,
        Capsule,
        Cylinder,
        Mesh,
        Terrain,
        Character,
        Vehicle,
        Particle,
        Fluid
    };
    
    std::string id;
    Type type;
    float mass;
    float friction;
    float restitution;
    bool isStatic;
    bool isKinematic;
    bool isTrigger;
    std::vector<float> transform;
    std::vector<float> velocity;
    std::vector<float> angularVelocity;
    std::vector<float> forces;
    std::vector<float> torques;
    
    PhysicsObject() : type(Type::Box), mass(1.0f), friction(0.5f), restitution(0.5f), 
                      isStatic(false), isKinematic(false), isTrigger(false) {}
};

// Collision Event Data Structure
struct CollisionEvent {
    std::string objectA;
    std::string objectB;
    std::vector<float> contactPoint;
    std::vector<float> contactNormal;
    float penetrationDepth;
    float relativeVelocity;
    float impactForce;
    float timestamp;
    bool isTrigger;
    
    CollisionEvent() : penetrationDepth(0.0f), relativeVelocity(0.0f), impactForce(0.0f), 
                       timestamp(0.0f), isTrigger(false) {}
};

// Game Profile for Physics
struct PhysicsGameProfile {
    std::string gameName;
    std::string engineType;
    std::string physicsAPI;
    std::vector<std::string> physicsObjects;
    std::vector<std::string> redditPhysics;
    std::vector<std::string> communityPhysics;
    std::vector<std::string> researchPhysics;
    float physicsQuality;
    bool isVerified;
    std::string lastTested;
};

// Physics Event System
struct PhysicsEvent {
    enum class Type {
        ObjectCreated,
        ObjectDestroyed,
        CollisionDetected,
        CollisionResolved,
        PhysicsStep,
        ErrorOccurred,
        PerformanceWarning,
        RedditPhysicsApplied,
        CommunityPhysicsUsed,
        ResearchPhysicsApplied
    };
    
    Type type;
    std::string message;
    std::string gameName;
    std::string objectId;
    float timestamp;
    std::unordered_map<std::string, std::string> data;
};

// Core Physics Interface
class IPhysicsCore {
public:
    virtual ~IPhysicsCore() = default;
    
    virtual bool Initialize(const PhysicsConfig& config) = 0;
    virtual bool CreatePhysicsWorld(const std::string& gameName) = 0;
    virtual bool AddPhysicsObject(const std::string& gameName, const PhysicsObject& object) = 0;
    virtual bool RemovePhysicsObject(const std::string& gameName, const std::string& objectId) = 0;
    virtual bool UpdatePhysicsObject(const std::string& gameName, const std::string& objectId, const PhysicsObject& object) = 0;
    virtual bool StepPhysics(const std::string& gameName, float deltaTime) = 0;
    virtual bool ApplyRedditPhysics(const std::string& gameName, const std::string& physicsId) = 0;
    virtual bool ApplyCommunityPhysics(const std::string& gameName, const std::string& physicsId) = 0;
    virtual bool ApplyResearchPhysics(const std::string& gameName, const std::string& physicsId) = 0;
    virtual PhysicsMetrics GetMetrics() const = 0;
    virtual std::vector<PhysicsGameProfile> GetGameProfiles() const = 0;
    virtual std::vector<PhysicsObject> GetPhysicsObjects(const std::string& gameName) const = 0;
    virtual std::vector<CollisionEvent> GetCollisionEvents(const std::string& gameName) const = 0;
    virtual void Shutdown() = 0;
    
    // Event system
    using EventCallback = std::function<void(const PhysicsEvent&)>;
    virtual void RegisterEventCallback(EventCallback callback) = 0;
    virtual void UnregisterEventCallback() = 0;
    
    // Collision callback
    using CollisionCallback = std::function<void(const CollisionEvent&)>;
    virtual void RegisterCollisionCallback(CollisionCallback callback) = 0;
    virtual void UnregisterCollisionCallback() = 0;
};

// Bullet Physics Interface
class IBulletPhysics {
public:
    virtual ~IBulletPhysics() = default;
    
    virtual bool Initialize(const PhysicsConfig& config) = 0;
    virtual bool CreateBulletWorld(const std::string& gameName) = 0;
    virtual bool AddBulletObject(const std::string& gameName, const PhysicsObject& object) = 0;
    virtual bool RemoveBulletObject(const std::string& gameName, const std::string& objectId) = 0;
    virtual bool StepBulletPhysics(const std::string& gameName, float deltaTime) = 0;
    virtual bool DetectBulletCollisions(const std::string& gameName) = 0;
    virtual bool ResolveBulletCollisions(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// PhysX Physics Interface
class IPhysXPhysics {
public:
    virtual ~IPhysXPhysics() = default;
    
    virtual bool Initialize(const PhysicsConfig& config) = 0;
    virtual bool CreatePhysXScene(const std::string& gameName) = 0;
    virtual bool AddPhysXObject(const std::string& gameName, const PhysicsObject& object) = 0;
    virtual bool RemovePhysXObject(const std::string& gameName, const std::string& objectId) = 0;
    virtual bool StepPhysXPhysics(const std::string& gameName, float deltaTime) = 0;
    virtual bool DetectPhysXCollisions(const std::string& gameName) = 0;
    virtual bool ResolvePhysXCollisions(const std::string& gameName) = 0;
    virtual void Shutdown() = 0;
};

// Collision Detector Interface
class ICollisionDetector {
public:
    virtual ~ICollisionDetector() = default;
    
    virtual bool Initialize(const PhysicsConfig& config) = 0;
    virtual bool SetupBroadPhase(const std::string& gameName) = 0;
    virtual bool SetupNarrowPhase(const std::string& gameName) = 0;
    virtual bool DetectCollisions(const std::string& gameName) = 0;
    virtual bool ResolveCollisions(const std::string& gameName) = 0;
    virtual bool HandleTriggers(const std::string& gameName) = 0;
    virtual std::vector<CollisionEvent> GetCollisionEvents(const std::string& gameName) const = 0;
    virtual void Shutdown() = 0;
};

// Physics Optimizer Interface
class IPhysicsOptimizer {
public:
    virtual ~IPhysicsOptimizer() = default;
    
    virtual bool Initialize(const PhysicsConfig& config) = 0;
    virtual bool OptimizePhysics(const std::string& gameName) = 0;
    virtual bool OptimizeCollisions(const std::string& gameName) = 0;
    virtual bool OptimizePerformance(const std::string& gameName) = 0;
    virtual bool ApplyRedditOptimizations(const std::string& gameName) = 0;
    virtual bool ApplyCommunityOptimizations(const std::string& gameName) = 0;
    virtual bool ApplyResearchOptimizations(const std::string& gameName) = 0;
    virtual PhysicsMetrics GetOptimizationMetrics() const = 0;
    virtual void Shutdown() = 0;
};

// Main Physics Integration Manager
class PhysicsIntegrationManager {
private:
    std::unique_ptr<IPhysicsCore> m_core;
    std::unique_ptr<IBulletPhysics> m_bullet;
    std::unique_ptr<IPhysXPhysics> m_physx;
    std::unique_ptr<ICollisionDetector> m_collision;
    std::unique_ptr<IPhysicsOptimizer> m_optimizer;
    
    PhysicsConfig m_config;
    PhysicsMetrics m_metrics;
    std::vector<PhysicsGameProfile> m_gameProfiles;
    std::vector<PhysicsEvent> m_events;
    std::vector<CollisionEvent> m_collisions;
    
    bool m_initialized;
    bool m_running;

public:
    PhysicsIntegrationManager();
    ~PhysicsIntegrationManager();
    
    bool Initialize(const PhysicsConfig& config);
    bool StartPhysics();
    bool StopPhysics();
    bool CreatePhysicsWorld(const std::string& gameName);
    bool AddPhysicsObject(const std::string& gameName, const PhysicsObject& object);
    bool StepPhysics(const std::string& gameName, float deltaTime);
    bool ApplyAllPhysics(const std::string& gameName);
    bool ApplyAllOptimizations(const std::string& gameName);
    
    PhysicsMetrics GetMetrics() const;
    std::vector<PhysicsGameProfile> GetGameProfiles() const;
    std::vector<PhysicsEvent> GetEvents() const;
    std::vector<CollisionEvent> GetCollisions(const std::string& gameName) const;
    
    void Shutdown();
    
private:
    void OnPhysicsEvent(const PhysicsEvent& event);
    void OnCollisionEvent(const CollisionEvent& event);
    void UpdateMetrics();
    void LogPhysicsStatus();
};

// Factory functions
std::unique_ptr<IPhysicsCore> CreatePhysicsCore();
std::unique_ptr<IBulletPhysics> CreateBulletPhysics();
std::unique_ptr<IPhysXPhysics> CreatePhysXPhysics();
std::unique_ptr<ICollisionDetector> CreateCollisionDetector();
std::unique_ptr<IPhysicsOptimizer> CreatePhysicsOptimizer();

} // namespace Physics
} // namespace uevrLCX
