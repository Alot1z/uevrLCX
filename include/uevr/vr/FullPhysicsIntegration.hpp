/*
 * UEVR Full Physics Integration
 * 
 * Complete physics simulation system for VR
 * Handles all object types with realistic physics and performance optimization
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <cstdint>

namespace uevr {
namespace vr {

// Forward declarations
class ObjectID;
class HandType;
class CollisionType;
class PhysicsType;
class Vector3;
class ForceType;
class GravityType;
class CollisionResult;
class PhysicsPerformance;
class InteractionResult;

/**
 * @brief Full Physics Integration - Universal physics system for all game engines
 * 
 * This system provides complete physics simulation, force application, and gravity
 * for all types of objects in any game engine. It respects game balance and provides
 * realistic physics without overpowering effects.
 */
class FullPhysicsIntegration {
public:
    virtual ~FullPhysicsIntegration() = default;

    // ============================================================================
    // INITIALIZATION AND CORE SYSTEM
    // ============================================================================
    
    /**
     * @brief Initialize the full physics system
     * @return true if initialization successful, false otherwise
     */
    virtual bool initializeFullPhysics() = 0;
    
    /**
     * @brief Shutdown the physics system and cleanup resources
     */
    virtual void shutdownFullPhysics() = 0;
    
    /**
     * @brief Check if the physics system is ready
     * @return true if ready, false otherwise
     */
    virtual bool isPhysicsSystemReady() const = 0;

    // ============================================================================
    // HAND COLLISION DETECTION
    // ============================================================================
    
    /**
     * @brief Check hand collision with ALL objects
     * @param hand The hand to check collision for
     * @param object The object to check collision with
     * @param type The type of collision to check
     * @return true if collision detected, false otherwise
     */
    virtual bool checkFullHandCollision(HandType hand, ObjectID object, CollisionType type) = 0;
    
    /**
     * @brief Get all objects that the hand is currently colliding with
     * @param hand The hand to get collisions for
     * @return Vector of object IDs that the hand is colliding with
     */
    virtual std::vector<ObjectID> getHandCollidingObjects(HandType hand) const = 0;
    
    /**
     * @brief Check if hand is currently colliding with any object
     * @param hand The hand to check
     * @return true if hand is colliding, false otherwise
     */
    virtual bool isHandColliding(HandType hand) const = 0;

    // ============================================================================
    // OBJECT PHYSICS SIMULATION
    // ============================================================================
    
    /**
     * @brief Simulate object physics for ALL types
     * @param object The object to simulate physics for
     * @param physics_type The type of physics to apply
     */
    virtual void simulateFullObjectPhysics(ObjectID object, PhysicsType physics_type) = 0;
    
    /**
     * @brief Update physics simulation for all objects
     * @param delta_time Time since last update in seconds
     */
    virtual void updatePhysicsSimulation(float delta_time) = 0;
    
    /**
     * @brief Set physics simulation enabled/disabled
     * @param enabled true to enable, false to disable
     */
    virtual void setPhysicsSimulationEnabled(bool enabled) = 0;
    
    /**
     * @brief Check if physics simulation is enabled
     * @return true if enabled, false otherwise
     */
    virtual bool isPhysicsSimulationEnabled() const = 0;
    
    /**
     * @brief Get physics simulation status for a specific object
     * @param object The object to check
     * @return true if physics simulation is active for this object
     */
    virtual bool isObjectPhysicsActive(ObjectID object) const = 0;

    // ============================================================================
    // FORCE APPLICATION
    // ============================================================================
    
    /**
     * @brief Apply force to ALL objects
     * @param object The object to apply force to
     * @param force The force vector to apply
     * @param force_type The type of force to apply
     */
    virtual void applyFullForce(ObjectID object, Vector3 force, ForceType force_type) = 0;
    
    /**
     * @brief Apply impulse force to an object
     * @param object The object to apply impulse to
     * @param impulse The impulse vector to apply
     */
    virtual void applyImpulse(ObjectID object, Vector3 impulse) = 0;
    
    /**
     * @brief Apply torque to an object
     * @param object The object to apply torque to
     * @param torque The torque vector to apply
     */
    virtual void applyTorque(ObjectID object, Vector3 torque) = 0;
    
    /**
     * @brief Get the current force being applied to an object
     * @param object The object to get force for
     * @return The current force vector
     */
    virtual Vector3 getObjectForce(ObjectID object) const = 0;
    
    /**
     * @brief Clear all forces applied to an object
     * @param object The object to clear forces for
     */
    virtual void clearObjectForces(ObjectID object) = 0;

    // ============================================================================
    // GRAVITY SIMULATION
    // ============================================================================
    
    /**
     * @brief Simulate gravity for ALL objects
     * @param object The object to apply gravity to
     * @param gravity_type The type of gravity to apply
     */
    virtual void simulateFullGravity(ObjectID object, GravityType gravity_type) = 0;
    
    /**
     * @brief Set global gravity strength
     * @param gravity_strength The gravity strength to set
     */
    virtual void setGlobalGravityStrength(float gravity_strength) = 0;
    
    /**
     * @brief Get global gravity strength
     * @return The current global gravity strength
     */
    virtual float getGlobalGravityStrength() const = 0;
    
    /**
     * @brief Set object-specific gravity
     * @param object The object to set gravity for
     * @param gravity_vector The gravity vector to apply
     */
    virtual void setObjectGravity(ObjectID object, Vector3 gravity_vector) = 0;
    
    /**
     * @brief Get object-specific gravity
     * @param object The object to get gravity for
     * @return The gravity vector for this object
     */
    virtual Vector3 getObjectGravity(ObjectID object) const = 0;
    
    /**
     * @brief Disable gravity for a specific object
     * @param object The object to disable gravity for
     */
    virtual void disableObjectGravity(ObjectID object) = 0;
    
    /**
     * @brief Check if gravity is enabled for an object
     * @param object The object to check
     * @return true if gravity is enabled, false otherwise
     */
    virtual bool isObjectGravityEnabled(ObjectID object) const = 0;

    // ============================================================================
    // COLLISION RESPONSE
    // ============================================================================
    
    /**
     * @brief Handle collision response for ALL types
     * @param collision The collision result to handle
     * @param type The type of collision that occurred
     */
    virtual void handleFullCollisionResponse(CollisionResult collision, CollisionType type) = 0;
    
    /**
     * @brief Set collision response enabled/disabled
     * @param enabled true to enable, false to disable
     */
    virtual void setCollisionResponseEnabled(bool enabled) = 0;
    
    /**
     * @brief Check if collision response is enabled
     * @return true if enabled, false otherwise
     */
    virtual bool isCollisionResponseEnabled() const = 0;
    
    /**
     * @brief Set collision response strength
     * @param strength The collision response strength (0.0 to 1.0)
     */
    virtual void setCollisionResponseStrength(float strength) = 0;
    
    /**
     * @brief Get collision response strength
     * @return The current collision response strength
     */
    virtual float getCollisionResponseStrength() const = 0;

    // ============================================================================
    // PERFORMANCE MONITORING
    // ============================================================================
    
    /**
     * @brief Get physics performance statistics
     * @return PhysicsPerformance object containing performance data
     */
    virtual PhysicsPerformance getFullPhysicsPerformance() = 0;
    
    /**
     * @brief Get performance statistics as string
     * @return String containing performance information
     */
    virtual std::string getPerformanceStats() const = 0;
    
    /**
     * @brief Reset performance counters
     */
    virtual void resetPerformanceCounters() = 0;
    
    /**
     * @brief Get memory usage information
     * @return String containing memory usage information
     */
    virtual std::string getMemoryUsage() const = 0;

    // ============================================================================
    // OBJECT INTERACTION
    // ============================================================================
    
    /**
     * @brief Interact with full object for ALL types
     * @param object The object to interact with
     * @param hand The hand performing the interaction
     * @param type The type of interaction to perform
     * @return InteractionResult containing interaction information
     */
    virtual InteractionResult interactWithFullObject(ObjectID object, HandType hand, InteractionType type) = 0;
    
    /**
     * @brief Check if an object can be interacted with
     * @param object The object to check
     * @param hand The hand to check
     * @return true if object can be interacted with, false otherwise
     */
    virtual bool canInteractWithObject(ObjectID object, HandType hand) const = 0;
    
    /**
     * @brief Get all objects that can be interacted with by a hand
     * @param hand The hand to get interactable objects for
     * @return Vector of object IDs that can be interacted with
     */
    virtual std::vector<ObjectID> getInteractableObjects(HandType hand) const = 0;

    // ============================================================================
    // OBJECT PROPERTIES
    // ============================================================================
    
    /**
     * @brief Set object mass
     * @param object The object to set mass for
     * @param mass The mass value to set
     */
    virtual void setObjectMass(ObjectID object, float mass) = 0;
    
    /**
     * @brief Get object mass
     * @param object The object to get mass for
     * @return The mass of the object
     */
    virtual float getObjectMass(ObjectID object) const = 0;
    
    /**
     * @brief Set object friction
     * @param object The object to set friction for
     * @param friction The friction value to set (0.0 to 1.0)
     */
    virtual void setObjectFriction(ObjectID object, float friction) = 0;
    
    /**
     * @brief Get object friction
     * @param object The object to get friction for
     * @return The friction value of the object
     */
    virtual float getObjectFriction(ObjectID object) const = 0;
    
    /**
     * @brief Set object restitution (bounciness)
     * @param object The object to set restitution for
     * @param restitution The restitution value to set (0.0 to 1.0)
     */
    virtual void setObjectRestitution(ObjectID object, float restitution) = 0;
    
    /**
     * @brief Get object restitution
     * @param object The object to get restitution for
     * @return The restitution value of the object
     */
    virtual float getObjectRestitution(ObjectID object) const = 0;

    // ============================================================================
    // OBJECT TRANSFORMS
    // ============================================================================
    
    /**
     * @brief Set object position
     * @param object The object to set position for
     * @param position The position vector to set
     */
    virtual void setObjectPosition(ObjectID object, Vector3 position) = 0;
    
    /**
     * @brief Get object position
     * @param object The object to get position for
     * @return The current position of the object
     */
    virtual Vector3 getObjectPosition(ObjectID object) const = 0;
    
    /**
     * @brief Set object rotation
     * @param object The object to set rotation for
     * @param rotation The rotation vector to set (in radians)
     */
    virtual void setObjectRotation(ObjectID object, Vector3 rotation) = 0;
    
    /**
     * @brief Get object rotation
     * @param object The object to get rotation for
     * @return The current rotation of the object (in radians)
     */
    virtual Vector3 getObjectRotation(ObjectID object) const = 0;
    
    /**
     * @brief Set object scale
     * @param object The object to set scale for
     * @param scale The scale vector to set
     */
    virtual void setObjectScale(ObjectID object, Vector3 scale) = 0;
    
    /**
     * @brief Get object scale
     * @param object The object to get scale for
     * @return The current scale of the object
     */
    virtual Vector3 getObjectScale(ObjectID object) const = 0;

    // ============================================================================
    // OBJECT VELOCITY AND MOMENTUM
    // ============================================================================
    
    /**
     * @brief Set object velocity
     * @param object The object to set velocity for
     * @param velocity The velocity vector to set
     */
    virtual void setObjectVelocity(ObjectID object, Vector3 velocity) = 0;
    
    /**
     * @brief Get object velocity
     * @param object The object to get velocity for
     * @return The current velocity of the object
     */
    virtual Vector3 getObjectVelocity(ObjectID object) const = 0;
    
    /**
     * @brief Set object angular velocity
     * @param object The object to set angular velocity for
     * @param angular_velocity The angular velocity vector to set
     */
    virtual void setObjectAngularVelocity(ObjectID object, Vector3 angular_velocity) = 0;
    
    /**
     * @brief Get object angular velocity
     * @param object The object to get angular velocity for
     * @return The current angular velocity of the object
     */
    virtual Vector3 getObjectAngularVelocity(ObjectID object) const = 0;
    
    /**
     * @brief Get object momentum
     * @param object The object to get momentum for
     * @return The current momentum of the object
     */
    virtual Vector3 getObjectMomentum(ObjectID object) const = 0;
    
    /**
     * @brief Get object angular momentum
     * @param object The object to get angular momentum for
     * @return The current angular momentum of the object
     */
    virtual Vector3 getObjectAngularMomentum(ObjectID object) const = 0;

    // ============================================================================
    // CONSTRAINT SYSTEM
    // ============================================================================
    
    /**
     * @brief Create a fixed constraint between two objects
     * @param object1 The first object
     * @param object2 The second object
     * @return true if constraint created successfully, false otherwise
     */
    virtual bool createFixedConstraint(ObjectID object1, ObjectID object2) = 0;
    
    /**
     * @brief Create a hinge constraint between two objects
     * @param object1 The first object
     * @param object2 The second object
     * @param axis The axis of rotation for the hinge
     * @return true if constraint created successfully, false otherwise
     */
    virtual bool createHingeConstraint(ObjectID object1, ObjectID object2, Vector3 axis) = 0;
    
    /**
     * @brief Create a spring constraint between two objects
     * @param object1 The first object
     * @param object2 The second object
     * @param spring_constant The spring constant
     * @param damping The damping coefficient
     * @return true if constraint created successfully, false otherwise
     */
    virtual bool createSpringConstraint(ObjectID object1, ObjectID object2, float spring_constant, float damping) = 0;
    
    /**
     * @brief Remove a constraint between two objects
     * @param object1 The first object
     * @param object2 The second object
     * @return true if constraint removed successfully, false otherwise
     */
    virtual bool removeConstraint(ObjectID object1, ObjectID object2) = 0;
    
    /**
     * @brief Check if two objects are constrained
     * @param object1 The first object
     * @param object2 The second object
     * @return true if objects are constrained, false otherwise
     */
    virtual bool areObjectsConstrained(ObjectID object1, ObjectID object2) const = 0;

    // ============================================================================
    // COLLISION DETECTION SETTINGS
    // ============================================================================
    
    /**
     * @brief Set collision detection precision
     * @param precision The precision level (0.0 to 1.0, higher = more precise)
     */
    virtual void setCollisionDetectionPrecision(float precision) = 0;
    
    /**
     * @brief Get collision detection precision
     * @return The current collision detection precision
     */
    virtual float getCollisionDetectionPrecision() const = 0;
    
    /**
     * @brief Set collision detection frequency
     * @param frequency The frequency in Hz (1.0 to 1000.0)
     */
    virtual void setCollisionDetectionFrequency(float frequency) = 0;
    
    /**
     * @brief Get collision detection frequency
     * @return The current collision detection frequency in Hz
     */
    virtual float getCollisionDetectionFrequency() const = 0;
    
    /**
     * @brief Enable/disable collision detection for specific object types
     * @param physics_type The physics type to set collision detection for
     * @param enabled true to enable, false to disable
     */
    virtual void setCollisionDetectionForType(PhysicsType physics_type, bool enabled) = 0;
    
    /**
     * @brief Check if collision detection is enabled for a specific physics type
     * @param physics_type The physics type to check
     * @return true if enabled, false otherwise
     */
    virtual bool isCollisionDetectionEnabledForType(PhysicsType physics_type) const = 0;

    // ============================================================================
    // DEBUG AND VISUALIZATION
    // ============================================================================
    
    /**
     * @brief Enable/disable physics debug visualization
     * @param enabled true to enable, false to disable
     */
    virtual void setPhysicsDebugVisualization(bool enabled) = 0;
    
    /**
     * @brief Check if physics debug visualization is enabled
     * @return true if enabled, false otherwise
     */
    virtual bool isPhysicsDebugVisualizationEnabled() const = 0;
    
    /**
     * @brief Show physics debug information for a specific object
     * @param object The object to show debug info for
     */
    virtual void showObjectPhysicsDebug(ObjectID object) = 0;
    
    /**
     * @brief Hide physics debug information for a specific object
     * @param object The object to hide debug info for
     */
    virtual void hideObjectPhysicsDebug(ObjectID object) = 0;
    
    /**
     * @brief Get debug information for a specific object
     * @param object The object to get debug info for
     * @return String containing debug information
     */
    virtual std::string getObjectPhysicsDebug(ObjectID object) const = 0;
};

} // namespace vr
} // namespace uevr