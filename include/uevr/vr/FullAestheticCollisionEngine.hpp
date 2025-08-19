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

#include "uevr/vr/SimpleTypes.hpp"
#include <memory>
#include <vector>
#include <string>
#include <cstdint>

namespace uevr::vr {

// Forward declarations
class FullPhysicsIntegration;

// Basic types
using ObjectID = uint64_t;
using CollisionID = uint64_t;

// Basic math types (replacing GLM temporarily)
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Quat {
    float x, y, z, w;
    Quat() : x(0), y(0), z(0), w(1) {}
    Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

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

enum class HapticType {
    NONE = 0,
    LIGHT,
    MEDIUM,
    HEAVY,
    CUSTOM
};

enum class HandType {
    NONE = 0,
    LEFT,
    RIGHT,
    BOTH
};

// Structs
struct CollisionResult {
    ObjectID colliding_object;
    CollisionType collision_type;
    Vec3 contact_point;
    Vec3 contact_normal;
    float penetration_depth;
    bool is_valid;
};

struct PhysicsObject {
    ObjectID id;
    PhysicsType type;
    Vec3 position;
    Quat rotation;
    Vec3 scale;
    Vec3 velocity;
    Vec3 angular_velocity;
    float mass;
    float friction;
    float restitution;
};

struct HapticFeedback {
    HandType hand;
    HapticType type;
    float intensity;
    float duration;
    Vec3 position;
    bool is_active;
};

struct VisualFeedback {
    ObjectID object_id;
    Vec3 highlight_color;
    float intensity;
    bool is_active;
};

// Main collision engine class
class FullAestheticCollisionEngine {
public:
    FullAestheticCollisionEngine();
    virtual ~FullAestheticCollisionEngine();

    // Initialization and shutdown
    bool initialize();
    void shutdown();

    // Core collision detection
    virtual std::vector<CollisionResult> detectCollisions(const Vec3& position, float radius);
    virtual bool checkCollision(ObjectID obj1, ObjectID obj2);
    virtual CollisionResult getCollisionResult(ObjectID obj1, ObjectID obj2);

    // Object management
    virtual ObjectID addObject(const PhysicsObject& obj);
    virtual void removeObject(ObjectID id);
    virtual void updateObject(ObjectID id, const PhysicsObject& obj);
    virtual PhysicsObject getObject(ObjectID id) const;

    // Physics integration
    virtual void setPhysicsIntegration(FullPhysicsIntegration* physics);
    virtual void updatePhysicsSimulation(float delta_time);

    // Haptic and visual feedback
    virtual void addHapticFeedback(const HapticFeedback& feedback);
    virtual void addVisualFeedback(const VisualFeedback& feedback);
    virtual void clearHapticFeedback(HandType hand);
    virtual void clearVisualFeedback(ObjectID id);

    // Game-specific interactions
    virtual bool handleDoorInteraction(ObjectID door_id, HandType hand);
    virtual bool handleWeaponInteraction(ObjectID weapon_id, HandType hand);
    virtual bool handleVehicleInteraction(ObjectID vehicle_id, HandType hand);
    virtual bool handleNPCInteraction(ObjectID npc_id, HandType hand);
    virtual bool handleEnvironmentInteraction(ObjectID env_id, HandType hand);
    virtual bool handleInventoryInteraction(ObjectID item_id, HandType hand);

    // Configuration
    virtual void setCollisionLayers(uint32_t layers);
    virtual void setCollisionMask(uint32_t mask);
    virtual void setMaxCollisions(size_t max);
    virtual void setCollisionTolerance(float tolerance);

    // Performance and debugging
    virtual size_t getActiveCollisionCount() const;
    virtual float getLastUpdateTime() const;
    virtual void resetPerformanceMetrics();

protected:
    // Internal data structures
    struct ObjectData {
        PhysicsObject physics_data;
        PhysicsType physics_type;
        uint32_t collision_layers;
        uint32_t collision_mask;
        bool is_active;
        bool is_static;
    };

    struct CollisionPair {
        ObjectID obj1;
        ObjectID obj2;
        CollisionResult result;
        bool is_active;
    };

    // Member variables
    std::unordered_map<ObjectID, ObjectData> m_objects;
    std::vector<CollisionPair> m_active_collisions;
    std::vector<HapticFeedback> m_haptic_queue;
    std::vector<VisualFeedback> m_visual_queue;
    
    FullPhysicsIntegration* m_physics_integration;
    uint32_t m_collision_layers;
    uint32_t m_collision_mask;
    size_t m_max_collisions;
    float m_collision_tolerance;
    
    ObjectID m_next_object_id;
    CollisionID m_next_collision_id;
    
    // Performance metrics
    float m_last_update_time;
    size_t m_collision_checks_per_frame;
    float m_average_collision_time;
};

} // namespace uevr::vr