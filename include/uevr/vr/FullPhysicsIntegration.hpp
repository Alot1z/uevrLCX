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
// GLM temporarily disabled - using basic types instead
#include "FullAestheticCollisionEngine.hpp"

namespace uevr::vr {

// Forward declarations
class FullAestheticCollisionEngine;

// Enums
enum class ForceType {
    LINEAR = 0,
    ANGULAR,
    IMPULSE,
    TORQUE
};

enum class GravityType {
    NONE = 0,
    UNIFORM,
    DIRECTIONAL,
    RADIAL
};

enum class PhysicsQuality {
    LOW = 0,
    MEDIUM,
    HIGH,
    ULTRA
};

enum class PhysicsEngine {
    BUILTIN = 0,
    BULLET,
    PHYSX,
    CUSTOM
};

// Structs (using basic types instead of GLM)
struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    // Conversion operators for compatibility
    operator Vec3() const { return Vec3(x, y, z); }
    Vector3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
};

struct Quaternion {
    float x, y, z, w;
    
    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    
    // Conversion operators for compatibility
    operator Quat() const { return Quat(x, y, z, w); }
    Quaternion(const Quat& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
};

struct Transform {
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
    
    Transform() : scale(1, 1, 1) {}
};

struct PhysicsProperties {
    float mass;
    float friction;
    float restitution;
    float linear_damping;
    float angular_damping;
    bool is_static;
    bool is_kinematic;
    bool is_trigger;
};

struct PhysicsState {
    Vector3 position;
    Quaternion rotation;
    Vector3 linear_velocity;
    Vector3 angular_velocity;
    Vector3 linear_acceleration;
    Vector3 angular_acceleration;
    Vector3 force;
    Vector3 torque;
    Vector3 gravity;
};

struct PhysicsPerformance {
    float simulation_time;
    float collision_time;
    size_t active_objects;
    size_t collision_checks;
    size_t constraint_solves;
    float memory_usage;
};

struct InteractionResult {
    ObjectID object;
    HandType hand;
    bool success;
    Vector3 position;
    Vector3 normal;
    float force;
};

// Main physics integration class
class FullPhysicsIntegration {
public:
    FullPhysicsIntegration();
    virtual ~FullPhysicsIntegration();

    // Initialization and shutdown
    bool initialize();
    void shutdown();

    // Core physics simulation
    virtual void updatePhysicsSimulation(float delta_time);
    virtual void stepSimulation(float delta_time);
    virtual void resetSimulation();

    // Force application
    virtual void applyForce(ObjectID object, const Vector3& force, ForceType type);
    virtual void applyForceAtPoint(ObjectID object, const Vector3& force, const Vector3& point, ForceType type);
    virtual void applyTorque(ObjectID object, const Vector3& torque);
    virtual void applyImpulse(ObjectID object, const Vector3& impulse);
    virtual void applyAngularImpulse(ObjectID object, const Vector3& impulse);

    // Gravity
    virtual void setGravity(const Vector3& gravity);
    virtual void setGravityType(GravityType type);
    virtual Vector3 getGravity() const;
    virtual void enableGravity(ObjectID object, bool enable);

    // Object management
    virtual ObjectID createPhysicsObject(const PhysicsProperties& props, const Transform& transform);
    virtual void destroyPhysicsObject(ObjectID id);
    virtual void updateObjectTransform(ObjectID id, const Transform& transform);
    virtual Transform getObjectTransform(ObjectID id) const;
    virtual PhysicsState getObjectState(ObjectID id) const;

    // Collision handling
    virtual void setCollisionCallback(std::function<void(ObjectID, ObjectID, const Vector3&, const Vector3&)> callback);
    virtual void enableCollisionDetection(ObjectID obj1, ObjectID obj2, bool enable);
    virtual void setCollisionFilter(ObjectID object, uint32_t group, uint32_t mask);

    // Constraints and joints
    virtual void createConstraint(ObjectID obj1, ObjectID obj2, const Vector3& pivot, const Vector3& axis);
    virtual void removeConstraint(ObjectID constraint_id);
    virtual void setConstraintLimits(ObjectID constraint_id, float lower, float upper);

    // Object interaction
    virtual bool grabObject(ObjectID object, HandType hand);
    virtual bool releaseObject(ObjectID object, HandType hand);
    virtual bool throwObject(ObjectID object, const Vector3& velocity, const Vector3& angular_velocity);
    virtual bool pushObject(ObjectID object, const Vector3& force);
    virtual bool pullObject(ObjectID object, const Vector3& force);

    // Physics queries
    virtual Vector3 getObjectForce(ObjectID object) const;
    virtual Vector3 getObjectGravity(ObjectID object) const;
    virtual Vector3 getObjectPosition(ObjectID object) const;
    virtual Quaternion getObjectRotation(ObjectID object) const;
    virtual Vector3 getObjectScale(ObjectID object) const;
    virtual Vector3 getObjectVelocity(ObjectID object) const;
    virtual Vector3 getObjectAngularVelocity(ObjectID object) const;
    virtual Vector3 getObjectMomentum(ObjectID object) const;
    virtual Vector3 getObjectAngularMomentum(ObjectID object) const;

    // Physics engine management
    virtual void setPhysicsEngine(PhysicsEngine engine);
    virtual void setPhysicsQuality(PhysicsQuality quality);
    virtual void setMaxSubSteps(int max_steps);
    virtual void setFixedTimeStep(float time_step);

    // Performance and debugging
    virtual PhysicsPerformance getPerformanceMetrics() const;
    virtual void resetPerformanceMetrics();
    virtual void enableProfiling(bool enable);
    virtual void setDebugDraw(bool enable);

    // Integration with collision engine
    virtual void setCollisionEngine(FullAestheticCollisionEngine* collision_engine);
    virtual void syncWithCollisionEngine();

protected:
    // Internal data structures
    struct PhysicsObjectData {
        ObjectID id;
        PhysicsType type;
        Transform transform;
        PhysicsProperties properties;
        PhysicsState state;
        bool is_active;
        bool gravity_enabled;
    };

    struct ConstraintData {
        ObjectID id;
        ObjectID object1;
        ObjectID object2;
        Vector3 pivot;
        Vector3 axis;
        float lower_limit;
        float upper_limit;
        bool is_active;
    };

    // Member variables
    std::unordered_map<ObjectID, PhysicsObjectData> m_physics_objects;
    std::vector<ConstraintData> m_constraints;
    std::vector<std::function<void(ObjectID, ObjectID, const Vector3&, const Vector3&)>> m_collision_callbacks;
    
    FullAestheticCollisionEngine* m_collision_engine;
    PhysicsEngine m_current_engine;
    PhysicsQuality m_quality;
    
    Vector3 m_global_gravity;
    GravityType m_gravity_type;
    int m_max_sub_steps;
    float m_fixed_time_step;
    
    ObjectID m_next_object_id;
    ObjectID m_next_constraint_id;
    
    // Performance tracking
    PhysicsPerformance m_performance;
    bool m_profiling_enabled;
    bool m_debug_draw_enabled;
    
    // Simulation state
    float m_accumulator;
    bool m_simulation_running;
};

} // namespace uevr::vr
