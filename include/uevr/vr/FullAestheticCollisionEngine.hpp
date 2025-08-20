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

enum class HapticType {
    LIGHT = 0,
    MEDIUM,
    HEAVY,
    CUSTOM
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

// Collision pair structure for tracking active collisions
struct CollisionPair {
    ObjectID obj1;
    ObjectID obj2;
    CollisionResult result;
    bool is_active;
    
    CollisionPair() : obj1(0), obj2(0), is_active(false) {}
};

// Haptic feedback structure
struct HapticFeedback {
    HandType hand;
    HapticType type;
    float intensity;
    float duration;
    Vec3 position;
    bool is_active;
    
    HapticFeedback() : hand(HandType::LEFT), type(HapticType::LIGHT), 
                      intensity(0.5f), duration(0.1f), position(0.0f), is_active(false) {}
};

// Visual feedback structure
struct VisualFeedback {
    ObjectID object_id;
    Vec3 highlight_color;
    float intensity;
    bool is_active;
    
    VisualFeedback() : object_id(0), highlight_color(1.0f, 1.0f, 1.0f), 
                      intensity(1.0f), is_active(false) {}
};

// Physics object structure for collision engine
struct PhysicsObject {
    ObjectID id;
    Vec3 position;
    Vec3 velocity;
    Vec3 size;
    Quat rotation;
    PhysicsType type;
    
    PhysicsObject() : id(0), position(0.0f), velocity(0.0f), size(1.0f), 
                      rotation(1.0f, 0.0f, 0.0f, 0.0f), type(PhysicsType::STATIC) {}
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
    
    // Alternative object management methods
    ObjectID addObject(const PhysicsObject& obj);
    void removeObject(ObjectID id);
    void updateObject(ObjectID id, const PhysicsObject& obj);
    PhysicsObject getObject(ObjectID id) const;
    
    // Collision detection
    CollisionResult detectCollision(ObjectID object1, ObjectID object2);
    std::vector<CollisionResult> detectAllCollisions(ObjectID object);
    bool isColliding(ObjectID object1, ObjectID object2);
    
    // Advanced collision detection
    std::vector<CollisionResult> detectCollisions(const Vec3& position, float radius);
    bool checkCollision(ObjectID obj1, ObjectID obj2);
    CollisionResult getCollisionResult(ObjectID obj1, ObjectID obj2);
    
    // Physics and interaction
    void applyPhysics(ObjectID object, const Vec3& force);
    void setGravity(const Vec3& gravity);
    void updatePhysicsSimulation(float delta_time);
    
    // Object manipulation
    bool grabObject(ObjectID object, HandType hand);
    bool releaseObject(ObjectID object, HandType hand);
    bool isObjectGrabbed(ObjectID object);
    HandType getGrabbingHand(ObjectID object);
    
    // Utility functions
    Vec3 calculateCollisionPoint(ObjectID object1, ObjectID object2);
    Vec3 calculateCollisionNormal(ObjectID object1, ObjectID object2);
    float calculateDistance(ObjectID object1, ObjectID object2);
    
    // Performance and debugging
    size_t getActiveCollisionCount() const;
    float getLastUpdateTime() const;
    void resetPerformanceMetrics();
    
    // Configuration
    void setCollisionLayers(uint32_t layers);
    void setCollisionMask(uint32_t mask);
    void setMaxCollisions(size_t max);
    void setCollisionTolerance(float tolerance);
    
    // Physics integration
    void setPhysicsIntegration(FullPhysicsIntegration* physics);
    void updatePhysicsSimulation(float delta_time);
    
    // Haptic and visual feedback
    void addHapticFeedback(const HapticFeedback& feedback);
    void addVisualFeedback(const VisualFeedback& feedback);
    void clearHapticFeedback(HandType hand);
    void clearVisualFeedback(ObjectID id);
    
    // Game-specific interactions
    bool handleDoorInteraction(ObjectID door_id, HandType hand);
    bool handleWeaponInteraction(ObjectID weapon_id, HandType hand);
    bool handleVehicleInteraction(ObjectID vehicle_id, HandType hand);
    bool handleNPCInteraction(ObjectID npc_id, HandType hand);
    bool handleEnvironmentInteraction(ObjectID env_id, HandType hand);
    bool handleInventoryInteraction(ObjectID item_id, HandType hand);
    
    // Performance and debugging

private:
    // Internal data structures
    std::unordered_map<ObjectID, ObjectData> m_objects;
    std::vector<CollisionPair> m_active_collisions;
    std::vector<HapticFeedback> m_haptic_queue;
    std::vector<VisualFeedback> m_visual_queue;
    
    // Physics integration
    FullPhysicsIntegration* m_physics_integration;
    
    // Physics settings
    Vec3 m_gravity;
    
    // Collision detection settings
    uint32_t m_collision_layers;
    uint32_t m_collision_mask;
    size_t m_max_collisions;
    float m_collision_tolerance;
    
    // Object management
    ObjectID m_next_object_id;
    CollisionID m_next_collision_id;
    
    // Performance tracking
    float m_last_update_time;
    size_t m_collision_checks_per_frame;
    float m_average_collision_time;
    
    // Internal methods
    bool initializeCollisionDetection();
    bool initializePhysicsSimulation();
    bool initializeHapticSystem();
    bool initializeVisualFeedback();
    void updateCollisionDetection(float delta_time);
    void updateHapticFeedback(float delta_time);
    void updateVisualFeedback(float delta_time);
    bool checkCollision(ObjectID obj1, ObjectID obj2);
    CollisionResult getCollisionResult(ObjectID obj1, ObjectID obj2);
};

} // namespace uevr::vr