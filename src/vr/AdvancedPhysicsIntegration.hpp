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

// Physics engine includes
#include <btBulletDynamicsCommon.h>
#include <PxPhysicsAPI.h>

// VR-specific includes
#include "VRSystem.hpp"
#include "OpenXRIntegration.hpp"

namespace uevr {

enum class PhysicsEngineType {
    BULLET_PHYSICS,
    PHYSX,
    HYBRID,  // Use both engines for different aspects
    AUTO     // Automatically select best engine
};

enum class CollisionDetectionType {
    DISCRETE,      // Standard collision detection
    CONTINUOUS,    // Continuous collision detection for fast objects
    ADAPTIVE,      // Automatically switch based on object velocity
    VR_OPTIMIZED   // VR-specific collision detection
};

struct VRPhysicsSettings {
    PhysicsEngineType engineType = PhysicsEngineType::AUTO;
    CollisionDetectionType collisionType = CollisionDetectionType::VR_OPTIMIZED;
    
    // Performance settings
    float physicsTimeStep = 1.0f / 90.0f;  // 90Hz for VR
    int maxSubSteps = 10;
    bool enableMultithreading = true;
    int numThreads = 4;
    
    // VR-specific settings
    bool enableHandCollision = true;
    bool enableHeadCollision = true;
    bool enableBodyCollision = true;
    float collisionMargin = 0.01f;  // 1cm margin for VR comfort
    
    // Advanced settings
    bool enableSoftBodyPhysics = false;
    bool enableFluidPhysics = false;
    bool enableClothPhysics = false;
    bool enableParticleSystems = true;
    
    // Optimization settings
    bool enableSpatialHashing = true;
    bool enableBroadphaseOptimization = true;
    bool enableNarrowphaseOptimization = true;
    float broadphaseMargin = 0.1f;
};

struct VRPhysicsObject {
    std::string id;
    std::string name;
    bool isStatic;
    bool isKinematic;
    bool isTrigger;
    
    // Transform
    glm::mat4 transform;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    
    // Physics properties
    float mass;
    float friction;
    float restitution;
    glm::vec3 linearVelocity;
    glm::vec3 angularVelocity;
    
    // Collision shape
    enum class ShapeType {
        BOX,
        SPHERE,
        CAPSULE,
        CYLINDER,
        MESH,
        CONVEX_HULL,
        COMPOUND
    } shapeType;
    
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    
    // VR-specific properties
    bool isVRController;
    bool isVRHeadset;
    bool isVRBody;
    int controllerIndex = -1;
    
    // Callbacks
    std::function<void(const VRPhysicsObject&, const VRPhysicsObject&)> onCollisionEnter;
    std::function<void(const VRPhysicsObject&, const VRPhysicsObject&)> onCollisionStay;
    std::function<void(const VRPhysicsObject&, const VRPhysicsObject&)> onCollisionExit;
    std::function<void(const VRPhysicsObject&, const VRPhysicsObject&)> onTriggerEnter;
    std::function<void(const VRPhysicsObject&, const VRPhysicsObject&)> onTriggerExit;
};

struct VRPhysicsContact {
    VRPhysicsObject* objectA;
    VRPhysicsObject* objectB;
    glm::vec3 contactPoint;
    glm::vec3 contactNormal;
    float penetrationDepth;
    float impulse;
    bool isTrigger;
};

struct VRPhysicsMetrics {
    float simulationTime;
    float collisionDetectionTime;
    int activeObjects;
    int totalContacts;
    int broadphasePairs;
    int narrowphaseTests;
    float memoryUsage;
    int physicsThreads;
    float averageFrameTime;
    float peakFrameTime;
};

class AdvancedPhysicsIntegration {
public:
    AdvancedPhysicsIntegration();
    ~AdvancedPhysicsIntegration();
    
    // Initialization and configuration
    bool initialize(const VRPhysicsSettings& settings = VRPhysicsSettings{});
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }
    
    void setPhysicsSettings(const VRPhysicsSettings& settings);
    VRPhysicsSettings getPhysicsSettings() const;
    
    // Physics engine management
    bool switchPhysicsEngine(PhysicsEngineType engineType);
    PhysicsEngineType getCurrentPhysicsEngine() const;
    std::vector<PhysicsEngineType> getSupportedEngines() const;
    
    // Object management
    std::string createPhysicsObject(const VRPhysicsObject& object);
    bool updatePhysicsObject(const std::string& objectId, const VRPhysicsObject& object);
    bool removePhysicsObject(const std::string& objectId);
    VRPhysicsObject* getPhysicsObject(const std::string& objectId);
    std::vector<VRPhysicsObject*> getAllPhysicsObjects();
    
    // Physics simulation
    void startSimulation();
    void stopSimulation();
    void pauseSimulation();
    void resumeSimulation();
    bool isSimulating() const { return m_simulating.load(); }
    
    void stepSimulation(float deltaTime);
    void updatePhysics();
    
    // VR-specific physics
    bool setupVRPhysics(const VRSystem* vrSystem);
    void updateVRControllers(const std::vector<glm::mat4>& controllerTransforms);
    void updateVRHeadset(const glm::mat4& headsetTransform);
    void updateVRBody(const glm::mat4& bodyTransform);
    
    // Collision detection
    std::vector<VRPhysicsContact> getCollisions(const std::string& objectId);
    std::vector<VRPhysicsContact> getAllCollisions();
    bool checkCollision(const std::string& objectA, const std::string& objectB);
    float getDistance(const std::string& objectA, const std::string& objectB);
    
    // Advanced physics features
    bool enableSoftBodyPhysics(bool enable);
    bool enableFluidPhysics(bool enable);
    bool enableClothPhysics(bool enable);
    bool enableParticleSystems(bool enable);
    
    // Physics constraints
    std::string createConstraint(const std::string& objectA, const std::string& objectB, 
                                const glm::vec3& pivotA, const glm::vec3& pivotB);
    bool removeConstraint(const std::string& constraintId);
    
    // Force and impulse application
    void applyForce(const std::string& objectId, const glm::vec3& force, const glm::vec3& point = glm::vec3(0));
    void applyImpulse(const std::string& objectId, const glm::vec3& impulse, const glm::vec3& point = glm::vec3(0));
    void applyTorque(const std::string& objectId, const glm::vec3& torque);
    void applyAngularImpulse(const std::string& objectId, const glm::vec3& angularImpulse);
    
    // Physics queries
    std::vector<VRPhysicsObject*> raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance);
    std::vector<VRPhysicsObject*> sphereCast(const glm::vec3& origin, float radius, const glm::vec3& direction, float maxDistance);
    std::vector<VRPhysicsObject*> overlapSphere(const glm::vec3& center, float radius);
    std::vector<VRPhysicsObject*> overlapBox(const glm::vec3& center, const glm::vec3& halfExtents, const glm::quat& rotation);
    
    // Performance monitoring
    VRPhysicsMetrics getPhysicsMetrics() const;
    void enablePerformanceMonitoring(bool enable);
    void setPerformanceCallback(std::function<void(const VRPhysicsMetrics&)> callback);
    
    // Debug and visualization
    void enableDebugVisualization(bool enable);
    void setDebugDrawCallback(std::function<void(const std::vector<glm::vec3>&, const std::vector<uint32_t>&)> callback);
    std::vector<glm::vec3> getDebugVertices() const;
    std::vector<uint32_t> getDebugIndices() const;
    
    // Physics material management
    struct PhysicsMaterial {
        std::string name;
        float friction;
        float restitution;
        float density;
        bool isTrigger;
    };
    
    std::string createPhysicsMaterial(const PhysicsMaterial& material);
    bool applyPhysicsMaterial(const std::string& objectId, const std::string& materialId);
    
    // Advanced collision detection
    bool enableContinuousCollisionDetection(const std::string& objectId, bool enable);
    bool setCollisionFilter(const std::string& objectId, uint32_t collisionGroup, uint32_t collisionMask);
    
    // Physics optimization
    void optimizePhysicsWorld();
    void setBroadphaseAlgorithm(const std::string& algorithm);
    void setSolverIterations(int iterations);
    void setSolverMode(int mode);

private:
    // Bullet Physics implementation
    class BulletPhysicsImpl;
    std::unique_ptr<BulletPhysicsImpl> m_bulletImpl;
    
    // PhysX implementation
    class PhysXImpl;
    std::unique_ptr<PhysXImpl> m_physXImpl;
    
    // Hybrid implementation
    class HybridPhysicsImpl;
    std::unique_ptr<HybridPhysicsImpl> m_hybridImpl;
    
    // Common members
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_simulating{false};
    std::atomic<bool> m_paused{false};
    std::atomic<bool> m_shutdown{false};
    
    VRPhysicsSettings m_settings;
    mutable std::mutex m_settingsMutex;
    
    PhysicsEngineType m_currentEngine;
    std::unordered_map<std::string, std::unique_ptr<VRPhysicsObject>> m_physicsObjects;
    mutable std::mutex m_objectsMutex;
    
    std::vector<VRPhysicsContact> m_contacts;
    mutable std::mutex m_contactsMutex;
    
    VRPhysicsMetrics m_metrics;
    mutable std::mutex m_metricsMutex;
    bool m_performanceMonitoringEnabled{false};
    std::function<void(const VRPhysicsMetrics&)> m_performanceCallback;
    
    std::thread m_physicsThread;
    std::condition_variable m_physicsCV;
    std::mutex m_physicsMutex;
    
    bool m_debugVisualizationEnabled{false};
    std::function<void(const std::vector<glm::vec3>&, const std::vector<uint32_t>&)> m_debugDrawCallback;
    std::vector<glm::vec3> m_debugVertices;
    std::vector<uint32_t> m_debugIndices;
    mutable std::mutex m_debugMutex;
    
    std::unordered_map<std::string, PhysicsMaterial> m_materials;
    mutable std::mutex m_materialsMutex;
    
    // VR-specific members
    const VRSystem* m_vrSystem{nullptr};
    std::vector<glm::mat4> m_controllerTransforms;
    glm::mat4 m_headsetTransform{1.0f};
    glm::mat4 m_bodyTransform{1.0f};
    mutable std::mutex m_vrMutex;
    
    // Private methods
    void physicsLoop();
    void updateMetrics();
    void processCollisions();
    void updateVRPhysics();
    void optimizeForVR();
    bool initializePhysicsEngine(PhysicsEngineType engineType);
    void cleanupPhysicsEngine();
    void syncPhysicsObjects();
    void handleCollisionCallbacks(const VRPhysicsContact& contact);
    void updateDebugVisualization();
    void logPhysicsError(const std::string& error);
};

} // namespace uevr
