/*
 * UEVR Full Aesthetic Collision Engine
 * 
 * Complete collision detection and interaction system for VR
 * Handles all object types with aesthetic feedback and physics simulation
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace uevr::vr {

// Forward declarations
class FullPhysicsIntegration;

// Basic types
using ObjectID = uint64_t;
using CollisionID = uint64_t;

// Use GLM types directly
using Vec3 = glm::vec3;
using Quat = glm::quat;
using Mat4 = glm::mat4;

// Enums
enum class CollisionType {
    NONE = 0,
    TOUCH,
    GRAB,
    PUSH,
    PULL,
    COLLIDE
};

enum class PhysicsType {
    STATIC = 0,
    DYNAMIC,
    KINEMATIC,
    TRIGGER
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
    Vec3 collision_point;
    Vec3 collision_normal;
    float collision_distance;
    ObjectID object1;
    ObjectID object2;
    HandType hand;
    
    CollisionResult() : collision_detected(false), type(CollisionType::NONE), 
                       collision_point(0.0f), collision_normal(0.0f, 1.0f, 0.0f),
                       collision_distance(0.0f), object1(0), object2(0), hand(HandType::LEFT) {}
};

// Object data structure
struct ObjectData {
    Vec3 position;
    Vec3 velocity;
    Vec3 size;
    Quat rotation;
    PhysicsType physics_type;
    bool is_active;
    bool is_grabbable;
    bool is_interactable;
    
    ObjectData() : position(0.0f), velocity(0.0f), size(1.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f),
                   physics_type(PhysicsType::STATIC), is_active(true), is_grabbable(false), is_interactable(false) {}
};

// Main collision engine class
class FullAestheticCollisionEngine {
public:
    FullAestheticCollisionEngine();
    virtual ~FullAestheticCollisionEngine();
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Object management
    bool registerObject(ObjectID object, PhysicsType physics_type, const Vec3& position, const Vec3& size);
    void unregisterObject(ObjectID object);
    void updateObject(ObjectID object, const Vec3& position, const Vec3& velocity);
    
    // Collision detection
    CollisionResult detectCollision(ObjectID object1, ObjectID object2);
    std::vector<CollisionResult> detectAllCollisions(ObjectID object);
    bool isColliding(ObjectID object1, ObjectID object2);
    
    // Physics integration
    void applyPhysics(ObjectID object, const Vec3& force);
    void setGravity(const Vec3& gravity);
    void updatePhysicsSimulation(float delta_time);
    
    // VR-specific features
    bool grabObject(ObjectID object, HandType hand);
    bool releaseObject(ObjectID object, HandType hand);
    bool isObjectGrabbed(ObjectID object);
    HandType getGrabbingHand(ObjectID object);
    
    // Utility functions
    Vec3 calculateCollisionPoint(ObjectID object1, ObjectID object2);
    Vec3 calculateCollisionNormal(ObjectID object1, ObjectID object2);
    float calculateDistance(ObjectID object1, ObjectID object2);
    
    // Performance optimization
    void setBroadPhaseEnabled(bool enabled);
    void setSpatialPartitioningEnabled(bool enabled);
    void setAsyncProcessingEnabled(bool enabled);
    
    // Configuration
    void setCollisionMargin(float margin);
    void setMaxCollisionChecks(int max_checks);
    void setPhysicsSubsteps(int substeps);

private:
    // Internal data structures
    std::unordered_map<ObjectID, ObjectData> m_objects;
    std::vector<CollisionResult> m_collision_results;
    Vec3 m_gravity;
    float m_collision_margin;
    int m_max_collision_checks;
    int m_physics_substeps;
    
    // Performance settings
    bool m_broad_phase_enabled;
    bool m_spatial_partitioning_enabled;
    bool m_async_processing_enabled;
    
    // Internal methods
    void updateSpatialPartitioning();
    void processCollisionEvents();
    void applyCollisionResponse(const CollisionResult& collision);
    bool checkBroadPhaseCollision(ObjectID object1, ObjectID object2);
    bool checkNarrowPhaseCollision(ObjectID object1, ObjectID object2);
};

} // namespace uevr::vr