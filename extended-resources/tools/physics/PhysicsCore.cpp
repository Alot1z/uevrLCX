#include "PhysicsIntegration.hpp"
#include <spdlog/spdlog.h>
#include <btBulletDynamicsCommon.h>
#include <PxPhysicsAPI.h>
#include <Windows.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <algorithm>
#include <cmath>

namespace Physics {

class PhysicsCore : public IPhysicsCore {
private:
    PhysicsConfig m_config;
    PhysicsMetrics m_metrics;
    bool m_initialized;
    bool m_simulationActive;
    
    // Bullet Physics components
    std::unique_ptr<btDefaultCollisionConfiguration> m_bulletCollisionConfig;
    std::unique_ptr<btCollisionDispatcher> m_bulletDispatcher;
    std::unique_ptr<btBroadphaseInterface> m_bulletBroadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_bulletSolver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_bulletWorld;
    
    // PhysX components
    physx::PxFoundation* m_physxFoundation;
    physx::PxPhysics* m_physxPhysics;
    physx::PxScene* m_physxScene;
    physx::PxMaterial* m_physxDefaultMaterial;
    physx::PxCooking* m_physxCooking;
    
    // Physics objects management
    struct PhysicsObject {
        std::string id;
        PhysicsObjectType type;
        btRigidBody* bulletBody;
        physx::PxRigidActor* physxActor;
        btCollisionShape* bulletShape;
        physx::PxShape* physxShape;
        PhysicsTransform transform;
        PhysicsMaterial material;
        bool isStatic;
        bool isActive;
    };
    std::unordered_map<std::string, PhysicsObject> m_physicsObjects;
    
    // Collision detection
    struct CollisionPair {
        std::string objectA;
        std::string objectB;
        PhysicsVector3 contactPoint;
        PhysicsVector3 contactNormal;
        float penetrationDepth;
        std::chrono::steady_clock::time_point timestamp;
    };
    std::vector<CollisionPair> m_activeCollisions;
    std::vector<CollisionPair> m_collisionHistory;
    
    // Simulation thread
    std::thread m_simulationThread;
    std::mutex m_simulationMutex;
    bool m_simulationThreadRunning;
    
    // Collision callback handlers
    struct CollisionCallback {
        std::string objectId;
        std::function<void(const CollisionEvent&)> callback;
    };
    std::vector<CollisionCallback> m_collisionCallbacks;
    
    // Performance monitoring
    struct PerformanceStats {
        uint32_t activeObjects;
        uint32_t collisionChecks;
        uint32_t physicsSteps;
        float simulationTime;
        float collisionDetectionTime;
    };
    PerformanceStats m_performanceStats;

public:
    PhysicsCore() : m_initialized(false), m_simulationActive(false), 
                    m_physxFoundation(nullptr), m_physxPhysics(nullptr), 
                    m_physxScene(nullptr), m_physxDefaultMaterial(nullptr),
                    m_physxCooking(nullptr), m_simulationThreadRunning(false) {
        spdlog::info("PhysicsCore: Constructor called");
    }
    
    ~PhysicsCore() {
        Shutdown();
    }

    bool Initialize(const PhysicsConfig& config) override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        spdlog::info("PhysicsCore: Initializing with config - enableBulletPhysics: {}, enablePhysX: {}, enableRealTimeCollision: {}", 
                     config.enableBulletPhysics, config.enablePhysX, config.enableRealTimeCollision);
        
        m_config = config;
        m_metrics = PhysicsMetrics{};
        
        // Initialize Bullet Physics if enabled
        if (config.enableBulletPhysics && !InitializeBulletPhysics()) {
            spdlog::error("PhysicsCore: Failed to initialize Bullet Physics");
            return false;
        }
        
        // Initialize PhysX if enabled
        if (config.enablePhysX && !InitializePhysX()) {
            spdlog::error("PhysicsCore: Failed to initialize PhysX");
            return false;
        }
        
        // Initialize performance monitoring
        m_performanceStats = PerformanceStats{};
        
        m_initialized = true;
        spdlog::info("PhysicsCore: Initialization completed successfully");
        return true;
    }

    bool CreatePhysicsObject(const PhysicsObject& object) override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        if (!m_initialized) {
            spdlog::error("PhysicsCore: Cannot create physics object - not initialized");
            return false;
        }
        
        spdlog::info("PhysicsCore: Creating physics object: {} (Type: {})", object.id, static_cast<int>(object.type));
        
        // Create Bullet Physics object if enabled
        if (m_config.enableBulletPhysics) {
            if (!CreateBulletPhysicsObject(object)) {
                spdlog::error("PhysicsCore: Failed to create Bullet Physics object: {}", object.id);
                return false;
            }
        }
        
        // Create PhysX object if enabled
        if (m_config.enablePhysX) {
            if (!CreatePhysXObject(object)) {
                spdlog::error("PhysicsCore: Failed to create PhysX object: {}", object.id);
                return false;
            }
        }
        
        // Store object reference
        m_physicsObjects[object.id] = object;
        m_performanceStats.activeObjects = m_physicsObjects.size();
        
        spdlog::info("PhysicsCore: Successfully created physics object: {}", object.id);
        return true;
    }

    bool UpdatePhysicsObject(const std::string& objectId, const PhysicsTransform& transform) override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        auto it = m_physicsObjects.find(objectId);
        if (it == m_physicsObjects.end()) {
            spdlog::error("PhysicsCore: Physics object not found: {}", objectId);
            return false;
        }
        
        PhysicsObject& obj = it->second;
        obj.transform = transform;
        
        // Update Bullet Physics object
        if (m_config.enableBulletPhysics && obj.bulletBody) {
            btTransform btTransform;
            btTransform.setIdentity();
            btTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
            btTransform.setRotation(btQuaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w));
            obj.bulletBody->setWorldTransform(btTransform);
        }
        
        // Update PhysX object
        if (m_config.enablePhysX && obj.physxActor) {
            physx::PxTransform pxTransform;
            pxTransform.p = physx::PxVec3(transform.position.x, transform.position.y, transform.position.z);
            pxTransform.q = physx::PxQuat(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
            obj.physxActor->setGlobalPose(pxTransform);
        }
        
        spdlog::debug("PhysicsCore: Updated physics object: {}", objectId);
        return true;
    }

    bool RemovePhysicsObject(const std::string& objectId) override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        auto it = m_physicsObjects.find(objectId);
        if (it == m_physicsObjects.end()) {
            spdlog::error("PhysicsCore: Physics object not found: {}", objectId);
            return false;
        }
        
        PhysicsObject& obj = it->second;
        
        // Remove from Bullet Physics world
        if (m_config.enableBulletPhysics && obj.bulletBody) {
            m_bulletWorld->removeRigidBody(obj.bulletBody);
            delete obj.bulletBody;
            delete obj.bulletShape;
        }
        
        // Remove from PhysX scene
        if (m_config.enablePhysX && obj.physxActor) {
            m_physxScene->removeActor(*obj.physxActor);
            obj.physxActor->release();
        }
        
        // Remove from object collection
        m_physicsObjects.erase(it);
        m_performanceStats.activeObjects = m_physicsObjects.size();
        
        spdlog::info("PhysicsCore: Removed physics object: {}", objectId);
        return true;
    }

    bool StartSimulation() override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        if (!m_initialized) {
            spdlog::error("PhysicsCore: Cannot start simulation - not initialized");
            return false;
        }
        
        if (m_simulationActive) {
            spdlog::warn("PhysicsCore: Simulation already active");
            return true;
        }
        
        spdlog::info("PhysicsCore: Starting physics simulation");
        
        // Start simulation thread
        if (m_config.enableRealTimeCollision) {
            StartSimulationThread();
        }
        
        m_simulationActive = true;
        spdlog::info("PhysicsCore: Physics simulation started");
        return true;
    }

    bool StopSimulation() override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        if (!m_simulationActive) {
            spdlog::warn("PhysicsCore: Simulation not active");
            return true;
        }
        
        spdlog::info("PhysicsCore: Stopping physics simulation");
        
        // Stop simulation thread
        if (m_simulationThreadRunning) {
            m_simulationThreadRunning = false;
            if (m_simulationThread.joinable()) {
                m_simulationThread.join();
            }
        }
        
        m_simulationActive = false;
        spdlog::info("PhysicsCore: Physics simulation stopped");
        return true;
    }

    std::vector<CollisionEvent> GetActiveCollisions() const override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        std::vector<CollisionEvent> events;
        for (const auto& collision : m_activeCollisions) {
            CollisionEvent event;
            event.objectA = collision.objectA;
            event.objectB = collision.objectB;
            event.contactPoint = collision.contactPoint;
            event.contactNormal = collision.contactNormal;
            event.penetrationDepth = collision.penetrationDepth;
            event.timestamp = collision.timestamp;
            events.push_back(event);
        }
        
        return events;
    }

    bool RegisterCollisionCallback(const std::string& objectId, std::function<void(const CollisionEvent&)> callback) override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        CollisionCallback collisionCallback;
        collisionCallback.objectId = objectId;
        collisionCallback.callback = callback;
        m_collisionCallbacks.push_back(collisionCallback);
        
        spdlog::info("PhysicsCore: Registered collision callback for object: {}", objectId);
        return true;
    }

    PhysicsMetrics GetMetrics() const override {
        return m_metrics;
    }

    void Shutdown() override {
        std::lock_guard<std::mutex> lock(m_simulationMutex);
        
        spdlog::info("PhysicsCore: Shutting down");
        
        // Stop simulation
        if (m_simulationActive) {
            StopSimulation();
        }
        
        // Clean up physics objects
        for (auto& [id, obj] : m_physicsObjects) {
            if (m_config.enableBulletPhysics && obj.bulletBody) {
                m_bulletWorld->removeRigidBody(obj.bulletBody);
                delete obj.bulletBody;
                delete obj.bulletShape;
            }
            
            if (m_config.enablePhysX && obj.physxActor) {
                m_physxScene->removeActor(*obj.physxActor);
                obj.physxActor->release();
            }
        }
        m_physicsObjects.clear();
        
        // Clean up Bullet Physics
        if (m_config.enableBulletPhysics) {
            CleanupBulletPhysics();
        }
        
        // Clean up PhysX
        if (m_config.enablePhysX) {
            CleanupPhysX();
        }
        
        // Clear collision data
        m_activeCollisions.clear();
        m_collisionHistory.clear();
        m_collisionCallbacks.clear();
        
        m_initialized = false;
        m_simulationActive = false;
        
        spdlog::info("PhysicsCore: Shutdown completed");
    }

private:
    bool InitializeBulletPhysics() {
        spdlog::info("PhysicsCore: Initializing Bullet Physics");
        
        // Create collision configuration
        m_bulletCollisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
        
        // Create collision dispatcher
        m_bulletDispatcher = std::make_unique<btCollisionDispatcher>(m_bulletCollisionConfig.get());
        
        // Create broadphase
        m_bulletBroadphase = std::make_unique<btDbvtBroadphase>();
        
        // Create constraint solver
        m_bulletSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
        
        // Create dynamics world
        m_bulletWorld = std::make_unique<btDiscreteDynamicsWorld>(
            m_bulletDispatcher.get(),
            m_bulletBroadphase.get(),
            m_bulletSolver.get(),
            m_bulletCollisionConfig.get()
        );
        
        // Set gravity
        m_bulletWorld->setGravity(btVector3(0, -9.81f, 0));
        
        spdlog::info("PhysicsCore: Bullet Physics initialized successfully");
        return true;
    }
    
    bool InitializePhysX() {
        spdlog::info("PhysicsCore: Initializing PhysX");
        
        // Create foundation
        m_physxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, 
                                              physx::PxDefaultAllocator(), 
                                              physx::PxDefaultErrorCallback());
        if (!m_physxFoundation) {
            spdlog::error("PhysicsCore: Failed to create PhysX foundation");
            return false;
        }
        
        // Create physics
        m_physxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_physxFoundation, 
                                        physx::PxTolerancesScale());
        if (!m_physxPhysics) {
            spdlog::error("PhysicsCore: Failed to create PhysX physics");
            return false;
        }
        
        // Create cooking
        m_physxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_physxFoundation, 
                                        physx::PxCookingParams(physx::PxTolerancesScale()));
        if (!m_physxCooking) {
            spdlog::error("PhysicsCore: Failed to create PhysX cooking");
            return false;
        }
        
        // Create default material
        m_physxDefaultMaterial = m_physxPhysics->createMaterial(0.5f, 0.5f, 0.1f);
        if (!m_physxDefaultMaterial) {
            spdlog::error("PhysicsCore: Failed to create PhysX default material");
            return false;
        }
        
        // Create scene
        physx::PxSceneDesc sceneDesc(m_physxPhysics->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
        sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(4);
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;
        
        m_physxScene = m_physxPhysics->createScene(sceneDesc);
        if (!m_physxScene) {
            spdlog::error("PhysicsCore: Failed to create PhysX scene");
            return false;
        }
        
        spdlog::info("PhysicsCore: PhysX initialized successfully");
        return true;
    }
    
    bool CreateBulletPhysicsObject(const PhysicsObject& object) {
        // Create collision shape based on object type
        btCollisionShape* shape = nullptr;
        
        switch (object.type) {
            case PhysicsObjectType::Box:
                shape = new btBoxShape(btVector3(object.material.dimensions.x * 0.5f,
                                                object.material.dimensions.y * 0.5f,
                                                object.material.dimensions.z * 0.5f));
                break;
                
            case PhysicsObjectType::Sphere:
                shape = new btSphereShape(object.material.dimensions.x * 0.5f);
                break;
                
            case PhysicsObjectType::Capsule:
                shape = new btCapsuleShape(object.material.dimensions.x * 0.5f,
                                          object.material.dimensions.y);
                break;
                
            case PhysicsObjectType::Cylinder:
                shape = new btCylinderShape(btVector3(object.material.dimensions.x * 0.5f,
                                                     object.material.dimensions.y * 0.5f,
                                                     object.material.dimensions.z * 0.5f));
                break;
                
            default:
                spdlog::error("PhysicsCore: Unsupported Bullet Physics object type: {}", static_cast<int>(object.type));
                return false;
        }
        
        // Create motion state
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(object.transform.position.x,
                                     object.transform.position.y,
                                     object.transform.position.z));
        transform.setRotation(btQuaternion(object.transform.rotation.x,
                                          object.transform.rotation.y,
                                          object.transform.rotation.z,
                                          object.transform.rotation.w));
        
        btDefaultMotionState* motionState = new btDefaultMotionState(transform);
        
        // Calculate mass and inertia
        btScalar mass = object.isStatic ? 0.0f : object.material.mass;
        btVector3 inertia(0, 0, 0);
        if (mass > 0.0f) {
            shape->calculateLocalInertia(mass, inertia);
        }
        
        // Create rigid body
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
        rbInfo.m_friction = object.material.friction;
        rbInfo.m_restitution = object.material.restitution;
        
        btRigidBody* body = new btRigidBody(rbInfo);
        
        // Add to world
        m_bulletWorld->addRigidBody(body);
        
        // Store references
        PhysicsObject& obj = const_cast<PhysicsObject&>(object);
        obj.bulletBody = body;
        obj.bulletShape = shape;
        
        return true;
    }
    
    bool CreatePhysXObject(const PhysicsObject& object) {
        // Create shape based on object type
        physx::PxShape* shape = nullptr;
        
        switch (object.type) {
            case PhysicsObjectType::Box:
                shape = m_physxPhysics->createShape(physx::PxBoxGeometry(
                    object.material.dimensions.x * 0.5f,
                    object.material.dimensions.y * 0.5f,
                    object.material.dimensions.z * 0.5f), *m_physxDefaultMaterial);
                break;
                
            case PhysicsObjectType::Sphere:
                shape = m_physxPhysics->createShape(physx::PxSphereGeometry(
                    object.material.dimensions.x * 0.5f), *m_physxDefaultMaterial);
                break;
                
            case PhysicsObjectType::Capsule:
                shape = m_physxPhysics->createShape(physx::PxCapsuleGeometry(
                    object.material.dimensions.x * 0.5f,
                    object.material.dimensions.y * 0.5f), *m_physxDefaultMaterial);
                break;
                
            case PhysicsObjectType::Cylinder:
                shape = m_physxPhysics->createShape(physx::PxCapsuleGeometry(
                    object.material.dimensions.x * 0.5f,
                    object.material.dimensions.y * 0.5f), *m_physxDefaultMaterial);
                break;
                
            default:
                spdlog::error("PhysicsCore: Unsupported PhysX object type: {}", static_cast<int>(object.type));
                return false;
        }
        
        if (!shape) {
            spdlog::error("PhysicsCore: Failed to create PhysX shape");
            return false;
        }
        
        // Create transform
        physx::PxTransform transform;
        transform.p = physx::PxVec3(object.transform.position.x,
                                   object.transform.position.y,
                                   object.transform.position.z);
        transform.q = physx::PxQuat(object.transform.rotation.x,
                                   object.transform.rotation.y,
                                   object.transform.rotation.z,
                                   object.transform.rotation.w);
        
        // Create actor
        physx::PxRigidActor* actor = nullptr;
        if (object.isStatic) {
            actor = m_physxPhysics->createRigidStatic(transform);
        } else {
            physx::PxRigidDynamic* dynamicActor = m_physxPhysics->createRigidDynamic(transform);
            if (dynamicActor) {
                dynamicActor->setMass(object.material.mass);
                dynamicActor->setLinearDamping(0.1f);
                dynamicActor->setAngularDamping(0.1f);
            }
            actor = dynamicActor;
        }
        
        if (!actor) {
            spdlog::error("PhysicsCore: Failed to create PhysX actor");
            shape->release();
            return false;
        }
        
        // Attach shape to actor
        actor->attachShape(*shape);
        
        // Add to scene
        m_physxScene->addActor(*actor);
        
        // Store references
        PhysicsObject& obj = const_cast<PhysicsObject&>(object);
        obj.physxActor = actor;
        obj.physxShape = shape;
        
        return true;
    }
    
    void StartSimulationThread() {
        if (m_simulationThreadRunning) {
            return;
        }
        
        m_simulationThreadRunning = true;
        m_simulationThread = std::thread([this]() {
            spdlog::info("PhysicsCore: Simulation thread started");
            
            while (m_simulationThreadRunning) {
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Step Bullet Physics simulation
                if (m_config.enableBulletPhysics && m_bulletWorld) {
                    m_bulletWorld->stepSimulation(1.0f / 60.0f, 10);
                    DetectBulletCollisions();
                }
                
                // Step PhysX simulation
                if (m_config.enablePhysX && m_physxScene) {
                    m_physxScene->simulate(1.0f / 60.0f);
                    m_physxScene->fetchResults(true);
                    DetectPhysXCollisions();
                }
                
                // Update performance stats
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                m_performanceStats.simulationTime = duration.count() / 1000.0f; // Convert to milliseconds
                m_performanceStats.physicsSteps++;
                
                // Sleep to maintain 60 FPS
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
            
            spdlog::info("PhysicsCore: Simulation thread stopped");
        });
    }
    
    void DetectBulletCollisions() {
        // Get collision pairs from Bullet Physics
        int numManifolds = m_bulletDispatcher->getNumManifolds();
        
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold* contactManifold = m_bulletDispatcher->getManifoldByIndexInternal(i);
            const btRigidBody* obA = static_cast<const btRigidBody*>(contactManifold->getBody0());
            const btRigidBody* obB = static_cast<const btRigidBody*>(contactManifold->getBody1());
            
            // Find corresponding objects
            std::string objectA, objectB;
            for (const auto& [id, obj] : m_physicsObjects) {
                if (obj.bulletBody == obA) objectA = id;
                if (obj.bulletBody == obB) objectB = id;
            }
            
            if (!objectA.empty() && !objectB.empty()) {
                int numContacts = contactManifold->getNumContacts();
                
                for (int j = 0; j < numContacts; j++) {
                    btManifoldPoint& pt = contactManifold->getContactPoint(j);
                    
                    if (pt.getDistance() < 0.0f) { // Collision detected
                        CollisionPair collision;
                        collision.objectA = objectA;
                        collision.objectB = objectB;
                        collision.contactPoint = PhysicsVector3{
                            pt.getPositionWorldOnA().x(),
                            pt.getPositionWorldOnA().y(),
                            pt.getPositionWorldOnA().z()
                        };
                        collision.contactNormal = PhysicsVector3{
                            pt.m_normalWorldOnB.x(),
                            pt.m_normalWorldOnB.y(),
                            pt.m_normalWorldOnB.z()
                        };
                        collision.penetrationDepth = -pt.getDistance();
                        collision.timestamp = std::chrono::steady_clock::now();
                        
                        ProcessCollision(collision);
                    }
                }
            }
        }
        
        m_performanceStats.collisionChecks++;
    }
    
    void DetectPhysXCollisions() {
        // Get collision pairs from PhysX
        physx::PxU32 nbPairs = 0;
        physx::PxContactPair* pairs = nullptr;
        
        // This would require implementing a custom collision callback
        // For now, we'll use a simplified approach
        
        m_performanceStats.collisionChecks++;
    }
    
    void ProcessCollision(const CollisionPair& collision) {
        // Check if collision is new
        bool isNewCollision = true;
        for (const auto& existing : m_activeCollisions) {
            if ((existing.objectA == collision.objectA && existing.objectB == collision.objectB) ||
                (existing.objectA == collision.objectB && existing.objectB == collision.objectA)) {
                isNewCollision = false;
                break;
            }
        }
        
        if (isNewCollision) {
            m_activeCollisions.push_back(collision);
            m_collisionHistory.push_back(collision);
            
            // Keep collision history manageable
            if (m_collisionHistory.size() > 1000) {
                m_collisionHistory.erase(m_collisionHistory.begin());
            }
            
            // Trigger collision callbacks
            CollisionEvent event;
            event.objectA = collision.objectA;
            event.objectB = collision.objectB;
            event.contactPoint = collision.contactPoint;
            event.contactNormal = collision.contactNormal;
            event.penetrationDepth = collision.penetrationDepth;
            event.timestamp = collision.timestamp;
            
            for (const auto& callback : m_collisionCallbacks) {
                if (callback.objectId == collision.objectA || callback.objectId == collision.objectB) {
                    callback.callback(event);
                }
            }
            
            // Update metrics
            m_metrics.collisionCount++;
            m_metrics.lastCollisionTime = collision.timestamp;
        }
    }
    
    void CleanupBulletPhysics() {
        if (m_bulletWorld) {
            // Remove all objects from world
            for (int i = m_bulletWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
                btCollisionObject* obj = m_bulletWorld->getCollisionObjectArray()[i];
                btRigidBody* body = btRigidBody::upcast(obj);
                if (body && body->getMotionState()) {
                    delete body->getMotionState();
                }
                m_bulletWorld->removeCollisionObject(obj);
                delete obj;
            }
        }
        
        // Clean up components
        m_bulletWorld.reset();
        m_bulletSolver.reset();
        m_bulletBroadphase.reset();
        m_bulletDispatcher.reset();
        m_bulletCollisionConfig.reset();
    }
    
    void CleanupPhysX() {
        if (m_physxScene) {
            m_physxScene->release();
            m_physxScene = nullptr;
        }
        
        if (m_physxDefaultMaterial) {
            m_physxDefaultMaterial->release();
            m_physxDefaultMaterial = nullptr;
        }
        
        if (m_physxCooking) {
            m_physxCooking->release();
            m_physxCooking = nullptr;
        }
        
        if (m_physxPhysics) {
            m_physxPhysics->release();
            m_physxPhysics = nullptr;
        }
        
        if (m_physxFoundation) {
            m_physxFoundation->release();
            m_physxFoundation = nullptr;
        }
    }
};

// Factory function
std::unique_ptr<IPhysicsCore> CreatePhysicsCore() {
    return std::make_unique<PhysicsCore>();
}

} // namespace Physics
