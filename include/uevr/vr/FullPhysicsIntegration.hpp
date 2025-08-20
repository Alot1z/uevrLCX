/*
 * UEVR Full Physics Integration Header
 * 
 * Complete physics simulation system for VR
 * Handles all object types with realistic physics and performance optimization
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "FullAestheticCollisionEngine.hpp"

namespace uevr::vr {

// Forward declarations
class FullAestheticCollisionEngine;

// Basic types
using ObjectID = uint64_t;

// Forward declarations for collision types
enum class CollisionType {
    NONE = 0,
    TOUCH,
    GRAB,
    PUSH,
    PULL,
    COLLIDE
};

enum class HandType {
    LEFT = 0,
    RIGHT,
    BOTH
};

// Collision result structure
struct CollisionResult {
    bool collision_detected;
    CollisionType type;
    Vector3 collision_point;
    Vector3 collision_normal;
    float collision_distance;
    ObjectID object1;
    ObjectID object2;
    HandType hand;
    
    CollisionResult() : collision_detected(false), type(CollisionType::NONE), 
                       collision_point(0.0f), collision_normal(0.0f, 1.0f, 0.0f),
                       collision_distance(0.0f), object1(0), object2(0), hand(HandType::LEFT) {}
};

// Use GLM types directly
using Vector3 = glm::vec3;
using Quaternion = glm::quat;
using Matrix4x4 = glm::mat4;
using Transform = glm::mat4;

// Enums
enum class ForceType {
    LINEAR = 0,
    ANGULAR,
    IMPULSE,
    TORQUE
};

enum class PhysicsEngineType {
    BUILTIN = 0,
    BULLET,
    PHYSX,
    CUSTOM
};

enum class SimulationType {
    RIGID_BODY = 0,
    SOFT_BODY,
    FLUID,
    PARTICLE
};

// Physics object structure
struct PhysicsObject {
    ObjectID id;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 velocity;
    glm::vec3 angular_velocity;
    glm::vec3 size;
    float mass;
    float friction;
    float restitution;
    bool is_static;
    bool is_kinematic;
    SimulationType simulation_type;
    
    PhysicsObject() : id(0), position(0.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f),
                      velocity(0.0f), angular_velocity(0.0f), size(1.0f),
                      mass(1.0f), friction(0.5f), restitution(0.5f),
                      is_static(false), is_kinematic(false), simulation_type(SimulationType::RIGID_BODY) {}
};

// Force structure
struct Force {
    ForceType type;
    Vector3 direction;
    float magnitude;
    Vector3 point_of_application;
    float duration;
    
    Force() : type(ForceType::LINEAR), direction(0.0f, 1.0f, 0.0f), magnitude(0.0f),
              point_of_application(0.0f), duration(0.0f) {}
};

// Physics simulation settings
struct PhysicsSettings {
    Vector3 gravity;
    float time_step;
    int max_substeps;
    bool enable_ccd;
    float collision_tolerance;
    bool enable_multithreading;
    
    PhysicsSettings() : gravity(0.0f, -9.81f, 0.0f), time_step(1.0f/60.0f),
                       max_substeps(10), enable_ccd(true), collision_tolerance(0.001f),
                       enable_multithreading(true) {}
};

// Main physics integration class
class FullPhysicsIntegration {
public:
    FullPhysicsIntegration();
    virtual ~FullPhysicsIntegration();
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Physics engine management
    bool setPhysicsEngine(PhysicsEngineType engine_type);
    bool initializePhysicsEngine();
    void shutdownPhysicsEngine();
    
    // Object management
    bool addPhysicsObject(const PhysicsObject& obj);
    void removePhysicsObject(ObjectID id);
    void updatePhysicsObject(ObjectID id, const PhysicsObject& obj);
    PhysicsObject getPhysicsObject(ObjectID id) const;
    
    // Force application
    void applyForce(ObjectID id, const Force& force);
    void applyForceAtPoint(ObjectID id, const Force& force, const Vector3& point);
    void applyImpulse(ObjectID id, const Vector3& impulse);
    void applyTorque(ObjectID id, const Vector3& torque);
    
    // Physics simulation
    void updatePhysicsSimulation(float delta_time);
    void stepSimulation(float time_step);
    void setGravity(const Vector3& gravity);
    void setTimeStep(float time_step);
    
    // Collision detection integration
    void setCollisionEngine(FullAestheticCollisionEngine* collision_engine);
    void processCollisionEvents();
    void handleCollisionResponse(const CollisionResult& collision);
    
    // Advanced physics features
    bool enableSoftBodySimulation(ObjectID id);
    bool enableFluidSimulation(ObjectID id);
    bool enableParticleSystem(ObjectID id);
    
    // Performance optimization
    void setMultithreadingEnabled(bool enabled);
    void setBroadPhaseEnabled(bool enabled);
    void setSpatialPartitioningEnabled(bool enabled);
    void setLODEnabled(bool enabled);
    
    // Configuration
    void setPhysicsSettings(const PhysicsSettings& settings);
    PhysicsSettings getPhysicsSettings() const;
    void setMaxPhysicsObjects(int max_objects);
    void setCollisionLayers(uint32_t layers);
    
    // Utility functions
    Vector3 getObjectPosition(ObjectID id) const;
    Quaternion getObjectRotation(ObjectID id) const;
    Vector3 getObjectVelocity(ObjectID id) const;
    bool isObjectStatic(ObjectID id) const;
    
    // Debug and profiling
    int getActivePhysicsObjectCount() const;
    float getLastSimulationTime() const;
    void resetPerformanceMetrics();
    void enableDebugRendering(bool enabled);

private:
    // Internal data structures
    std::unordered_map<ObjectID, PhysicsObject> m_physics_objects;
    std::vector<Force> m_force_queue;
    PhysicsSettings m_settings;
    
    // Engine references
    FullAestheticCollisionEngine* m_collision_engine;
    PhysicsEngineType m_current_engine;
    
    // Performance settings
    bool m_multithreading_enabled;
    bool m_broad_phase_enabled;
    bool m_spatial_partitioning_enabled;
    bool m_lod_enabled;
    bool m_debug_rendering_enabled;
    
    // Performance metrics
    float m_last_simulation_time;
    int m_active_object_count;
    int m_max_physics_objects;
    
    // Internal methods
    void processForceQueue();
    void updateSpatialPartitioning();
    void applyPhysicsConstraints();
    void handlePhysicsEvents();
    bool validatePhysicsObject(const PhysicsObject& obj);
    void cleanupPhysicsObjects();
};

} // namespace uevr::vr
