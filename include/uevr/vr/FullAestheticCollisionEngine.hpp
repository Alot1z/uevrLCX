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

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace uevr {
namespace vr {

// Forward declarations
class CollisionResult;
class PhysicsType;
class HapticType;
class CollisionType;
class HandType;
class ObjectID;

// Collision result structure
struct CollisionResult {
    bool collision_detected;
    float collision_distance;
    glm::vec3 collision_point;
    glm::vec3 collision_normal;
    ObjectID colliding_object;
    CollisionType collision_type;
    float collision_intensity;
    
    CollisionResult() : collision_detected(false), collision_distance(0.0f), 
                       collision_intensity(0.0f) {}
};

// Physics types for different objects
enum class PhysicsType {
    STATIC,         // Immovable objects (walls, floors)
    DYNAMIC,        // Movable objects (boxes, items)
    KINEMATIC,      // Scripted movement (doors, elevators)
    RAGDOLL,        // Character physics (NPCs, monsters)
    FLUID,          // Liquid simulation
    PARTICLE,       // Particle systems
    VEHICLE,        // Vehicle physics
    WEAPON,         // Weapon handling
    ENVIRONMENTAL   // Environmental objects
};

// Haptic feedback types
enum class HapticType {
    NONE,           // No haptic feedback
    LIGHT,          // Light touch
    MEDIUM,         // Medium impact
    HEAVY,          // Heavy impact
    VIBRATION,      // Continuous vibration
    PULSE,          // Pulsing feedback
    BUZZ,           // Buzzing sensation
    TAP,            // Tap feedback
    GRAB,           // Grab feedback
    RELEASE         // Release feedback
};

// Collision types
enum class CollisionType {
    NONE,           // No collision
    TOUCH,          // Light touch
    GRAB,           // Object grabbing
    PUSH,           // Pushing objects
    PULL,           // Pulling objects
    THROW,          // Throwing objects
    BREAK,          // Breaking objects
    INTERACT,       // Interaction (buttons, switches)
    DAMAGE,         // Damage collision
    TRIGGER         // Trigger zones
};

// Hand types for VR controllers
enum class HandType {
    LEFT,           // Left hand
    RIGHT,          // Right hand
    BOTH,           // Both hands
    NONE            // No hand
};

// Object ID type
using ObjectID = uint64_t;

// Main collision engine class
class FullAestheticCollisionEngine {
public:
    FullAestheticCollisionEngine();
    virtual ~FullAestheticCollisionEngine();
    
    // Initialize full collision system
    virtual bool initializeFullCollision();
    
    // Shutdown collision system
    virtual void shutdownCollision();
    
    // Detect ALL types of collisions
    virtual CollisionResult detectFullCollision(ObjectID object, HandType hand, CollisionType type);
    
    // Physics simulation for ALL objects
    virtual void simulateFullPhysics(ObjectID object, PhysicsType physics_type);
    
    // Haptic feedback for ALL interactions
    virtual void provideFullHapticFeedback(HandType hand, float intensity, HapticType haptic_type);
    
    // Visual feedback for ALL collisions
    virtual void showFullCollisionHighlight(ObjectID object, CollisionType collision_type);
    
    // Door handle grabbing system
    virtual bool enableDoorHandleGrabbing(ObjectID door, HandType hand);
    
    // Weapon interaction system
    virtual bool enableWeaponInteraction(ObjectID weapon, HandType hand);
    
    // Environmental object interaction
    virtual bool enableEnvironmentalInteraction(ObjectID env_object, HandType hand);
    
    // NPC interaction system
    virtual bool enableNPCInteraction(ObjectID npc, HandType hand);
    
    // Vehicle interaction system
    virtual bool enableVehicleInteraction(ObjectID vehicle, HandType hand);
    
    // Puzzle object interaction
    virtual bool enablePuzzleInteraction(ObjectID puzzle, HandType hand);
    
    // Inventory manipulation
    virtual bool enableInventoryManipulation(ObjectID item, HandType hand);
    
    // Register object for collision detection
    virtual bool registerObject(ObjectID object, PhysicsType physics_type, const glm::vec3& position, const glm::vec3& size);
    
    // Unregister object from collision detection
    virtual bool unregisterObject(ObjectID object);
    
    // Update object position and state
    virtual void updateObject(ObjectID object, const glm::vec3& position, const glm::vec3& velocity);
    
    // Set collision response for object type
    virtual void setCollisionResponse(ObjectID object, CollisionType type, float response_intensity);
    
    // Enable/disable collision for specific object
    virtual void setCollisionEnabled(ObjectID object, bool enabled);
    
    // Get collision statistics
    virtual struct CollisionStats {
        uint64_t total_collisions;
        uint64_t successful_interactions;
        uint64_t failed_interactions;
        float average_response_time;
    } getCollisionStats() const;

protected:
    // Internal collision detection methods
    virtual bool checkCollisionBounds(ObjectID object1, ObjectID object2);
    virtual glm::vec3 calculateCollisionPoint(ObjectID object1, ObjectID object2);
    virtual glm::vec3 calculateCollisionNormal(ObjectID object1, ObjectID object2);
    
    // Physics simulation helpers
    virtual void applyPhysics(ObjectID object, const glm::vec3& force);
    virtual void updatePhysics(ObjectID object, float delta_time);
    
    // Haptic feedback implementation
    virtual void sendHapticFeedback(HandType hand, float intensity, HapticType type);
    
    // Visual feedback implementation
    virtual void renderCollisionHighlight(ObjectID object, CollisionType type);

private:
    // Internal state
    bool m_initialized;
    std::unordered_map<ObjectID, struct ObjectData> m_objects;
    CollisionStats m_stats;
    
    // Object data structure
    struct ObjectData {
        PhysicsType physics_type;
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 size;
        bool collision_enabled;
        std::unordered_map<CollisionType, float> collision_responses;
        
        ObjectData() : physics_type(PhysicsType::STATIC), collision_enabled(true) {}
    };
};

} // namespace vr
} // namespace uevr