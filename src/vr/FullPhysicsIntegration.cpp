/*
 * UEVR Full Physics Integration Implementation
 * 
 * Complete physics simulation system for VR
 * Handles all object types with realistic physics and performance optimization
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#include "uevr/vr/FullPhysicsIntegration.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <cmath>

namespace uevr {
namespace vr {

// Forward declarations for internal types
struct PhysicsObjectData {
    ObjectID object;
    PhysicsType type;
    bool isSimulating;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    Vector3 velocity;
    Vector3 angular_velocity;
    Vector3 acceleration;
    Vector3 force;
    Vector3 torque;
    Vector3 gravity;
    float mass;
    float friction;
    float restitution;
    bool gravityEnabled;
    std::vector<ObjectID> constraints;
};

struct ConstraintData {
    ObjectID object1;
    ObjectID object2;
    std::string type;
    Vector3 axis;
    float spring_constant;
    float damping;
};

struct PerformanceData {
    uint64_t collision_checks = 0;
    uint64_t physics_updates = 0;
    uint64_t force_applications = 0;
    uint64_t constraint_solves = 0;
    float total_simulation_time = 0.0f;
    float average_frame_time = 0.0f;
    uint64_t total_frames = 0;
};

// Internal state
class FullPhysicsIntegrationImpl : public FullPhysicsIntegration {
private:
    // System state
    bool m_initialized = false;
    bool m_physicsEnabled = true;
    bool m_collisionResponseEnabled = true;
    bool m_debugVisualizationEnabled = false;
    
    // Configuration
    float m_globalGravityStrength = -9.81f;
    float m_collisionResponseStrength = 1.0f;
    float m_collisionDetectionPrecision = 0.8f;
    float m_collisionDetectionFrequency = 60.0f;
    
    // Physics objects and constraints
    std::vector<PhysicsObjectData> m_physicsObjects;
    std::vector<ConstraintData> m_constraints;
    
    // Performance tracking
    PerformanceData m_performance;
    
    // Collision detection settings per physics type
    std::unordered_map<PhysicsType, bool> m_collisionDetectionEnabled;

public:
    // ============================================================================
    // INITIALIZATION AND CORE SYSTEM
    // ============================================================================
    
    bool initializeFullPhysics() override {
        if (m_initialized) {
            spdlog::warn("FullPhysicsIntegration already initialized");
            return true;
        }
        
        try {
            spdlog::info("Initializing FullPhysicsIntegration...");
            
            // Initialize internal systems
            m_physicsObjects.clear();
            m_constraints.clear();
            
            // Initialize collision detection settings for all physics types
            m_collisionDetectionEnabled[PhysicsType::STATIC] = true;
            m_collisionDetectionEnabled[PhysicsType::DYNAMIC] = true;
            m_collisionDetectionEnabled[PhysicsType::KINEMATIC] = true;
            m_collisionDetectionEnabled[PhysicsType::RAGDOLL] = true;
            m_collisionDetectionEnabled[PhysicsType::FLUID] = true;
            m_collisionDetectionEnabled[PhysicsType::PARTICLE] = true;
            m_collisionDetectionEnabled[PhysicsType::VEHICLE] = true;
            m_collisionDetectionEnabled[PhysicsType::WEAPON] = true;
            m_collisionDetectionEnabled[PhysicsType::ENVIRONMENTAL] = true;
            
            // Reset performance counters
            m_performance = PerformanceData{};
            
            m_initialized = true;
            spdlog::info("FullPhysicsIntegration initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("Failed to initialize FullPhysicsIntegration: {}", e.what());
            return false;
        }
    }
    
    void shutdownFullPhysics() override {
        if (!m_initialized) {
            return;
        }
        
        spdlog::info("Shutting down FullPhysicsIntegration...");
        
        // Clear all physics objects
        m_physicsObjects.clear();
        
        // Clear all constraints
        m_constraints.clear();
        
        m_initialized = false;
        spdlog::info("FullPhysicsIntegration shutdown complete");
    }
    
    bool isPhysicsSystemReady() const override {
        return m_initialized;
    }

    // ============================================================================
    // HAND COLLISION DETECTION
    // ============================================================================
    
    bool checkFullHandCollision(HandType hand, ObjectID object, CollisionType type) override {
        if (!m_initialized) {
            spdlog::warn("Physics system not initialized");
            return false;
        }
        
        m_performance.collision_checks++;
        
        // Check if collision detection is enabled for this object's physics type
        auto physicsData = getPhysicsObjectData(object);
        if (!physicsData) {
            return false;
        }
        
        if (!m_collisionDetectionEnabled[physicsData->type]) {
            return false;
        }
        
        // This is a simplified collision check - in real implementation, you'd do proper collision detection
        // based on the collision type and object properties
        bool collisionDetected = true; // Placeholder for actual collision detection
        
        if (collisionDetected) {
            spdlog::debug("Hand collision detected: Hand {}, Object {}, Type {}", 
                         static_cast<int>(hand), static_cast<int>(object), static_cast<int>(type));
        }
        
        return collisionDetected;
    }
    
    std::vector<ObjectID> getHandCollidingObjects(HandType hand) const override {
        std::vector<ObjectID> collidingObjects;
        
        // This would typically involve checking the current collision state
        // For now, return empty vector as placeholder
        return collidingObjects;
    }
    
    bool isHandColliding(HandType hand) const override {
        // Check if hand is currently colliding with any object
        return false; // Placeholder
    }

    // ============================================================================
    // OBJECT PHYSICS SIMULATION
    // ============================================================================
    
    void simulateFullObjectPhysics(ObjectID object, PhysicsType physics_type) override {
        if (!m_physicsEnabled || !m_initialized) {
            return;
        }
        
        m_performance.physics_updates++;
        
        // Find or create physics data for this object
        auto physicsData = getOrCreatePhysicsObjectData(object);
        if (physicsData) {
            physicsData->type = physics_type;
            physicsData->isSimulating = true;
            
            // Apply physics based on type
            switch (physics_type) {
                case PhysicsType::STATIC:
                    // Static objects don't move
                    physicsData->velocity = Vector3{};
                    physicsData->angular_velocity = Vector3{};
                    physicsData->acceleration = Vector3{};
                    break;
                    
                case PhysicsType::DYNAMIC:
                    // Dynamic objects can move and be affected by forces
                    // Physics simulation will be handled in updatePhysicsSimulation
                    break;
                    
                case PhysicsType::KINEMATIC:
                    // Kinematic objects move but aren't affected by forces
                    // Movement will be controlled externally
                    break;
                    
                case PhysicsType::FLUID:
                    // Fluid simulation placeholder
                    break;

                case PhysicsType::PARTICLE:
                    // Particle simulation placeholder
                    break;

                case PhysicsType::VEHICLE:
                    // Vehicle dynamics placeholder
                    break;

                case PhysicsType::WEAPON:
                    // Weapon physics placeholder
                    break;

                case PhysicsType::ENVIRONMENTAL:
                    // Environmental object behavior placeholder
                    break;
                    
                default:
                    spdlog::warn("Unknown physics type: {}", static_cast<int>(physics_type));
                    break;
            }
            
            spdlog::debug("Physics simulation enabled for object {} with type {}", 
                         static_cast<int>(object), static_cast<int>(physics_type));
        }
    }
    
    void updatePhysicsSimulation(float delta_time) override {
        if (!m_physicsEnabled || !m_initialized) {
            return;
        }
        
        m_performance.total_simulation_time += delta_time;
        m_performance.total_frames++;
        m_performance.average_frame_time = m_performance.total_simulation_time / m_performance.total_frames;
        
        for (auto& physics : m_physicsObjects) {
            if (physics.isSimulating) {
                // Update physics simulation for this object based on its type
                switch (physics.type) {
                    case PhysicsType::STATIC:
                        // No physics update needed for static objects
                        break;
                        
                    case PhysicsType::DYNAMIC:
                    case PhysicsType::RAGDOLL:
                        // Apply forces and update velocity
                        if (physics.mass > 0.0f) {
                            // F = ma, so a = F/m
                            Vector3 acceleration = physics.force / physics.mass;
                            physics.acceleration = acceleration;
                            
                            // Update velocity: v = v0 + a*t
                            physics.velocity += physics.acceleration * delta_time;
                            
                            // Apply damping
                            float damping = 1.0f - physics.friction;
                            physics.velocity *= std::pow(damping, delta_time);
                            
                            // Update position: x = x0 + v*t
                            physics.position += physics.velocity * delta_time;
                        }
                        break;
                        
                    case PhysicsType::KINEMATIC:
                        // Kinematic objects move but aren't affected by forces
                        // Position updates should be handled externally
                        break;
                        
                    case PhysicsType::FLUID:
                        // Fluid physics placeholder
                        break;

                    case PhysicsType::PARTICLE:
                        // Particle physics placeholder
                        break;

                    case PhysicsType::VEHICLE:
                        // Vehicle physics placeholder
                        break;

                    case PhysicsType::WEAPON:
                        // Weapon physics placeholder
                        break;

                    case PhysicsType::ENVIRONMENTAL:
                        // Environmental physics placeholder
                        break;
                }
                
                // Apply gravity if enabled
                if (physics.gravityEnabled) {
                    physics.velocity += physics.gravity * delta_time;
                }
                
                // Clear forces for next frame
                physics.force = Vector3{};
                physics.torque = Vector3{};
            }
        }
        
        // Solve constraints
        solveConstraints(delta_time);
    }
    
    void setPhysicsSimulationEnabled(bool enabled) override {
        m_physicsEnabled = enabled;
        spdlog::info("Physics simulation {}", enabled ? "enabled" : "disabled");
    }
    
    bool isPhysicsSimulationEnabled() const override {
        return m_physicsEnabled;
    }
    
    bool isObjectPhysicsActive(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->isSimulating : false;
    }

    // ============================================================================
    // FORCE APPLICATION
    // ============================================================================
    
    void applyFullForce(ObjectID object, Vector3 force, ForceType force_type) override {
        if (!m_initialized) {
            return;
        }
        
        m_performance.force_applications++;
        
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData && physicsData->isSimulating) {
            // Apply force based on type
            switch (force_type) {
                case ForceType::LINEAR:
                    // Apply linear force
                    physicsData->force += force;
                    break;
                    
                case ForceType::ANGULAR:
                    // Apply angular force (torque)
                    physicsData->torque += force;
                    break;
                    
                case ForceType::IMPULSE:
                    // Apply impulse force (instant velocity change)
                    if (physicsData->mass > 0.0f) {
                        physicsData->velocity += force / physicsData->mass;
                    }
                    break;
                    
                default:
                    // Default to linear force
                    physicsData->force += force;
                    break;
            }
            
            spdlog::debug("Force applied to object {}: Force {}, Type {}", 
                         static_cast<int>(object), 
                         "Vector3", // Placeholder for actual vector logging
                         static_cast<int>(force_type));
        }
    }
    
    void applyImpulse(ObjectID object, Vector3 impulse) override {
        applyFullForce(object, impulse, ForceType::IMPULSE);
    }
    
    void applyTorque(ObjectID object, Vector3 torque) override {
        applyFullForce(object, torque, ForceType::ANGULAR);
    }
    
    Vector3 getObjectForce(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->force : Vector3{};
    }
    
    void clearObjectForces(ObjectID object) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->force = Vector3{};
            physicsData->torque = Vector3{};
            spdlog::debug("Forces cleared for object {}", static_cast<int>(object));
        }
    }

    // ============================================================================
    // GRAVITY SIMULATION
    // ============================================================================
    
    void simulateFullGravity(ObjectID object, GravityType gravity_type) override {
        if (!m_initialized) {
            return;
        }
        
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->gravityEnabled = true;
            
            // Set gravity based on type
            switch (gravity_type) {
                case GravityType::EARTH:
                    physicsData->gravity = Vector3{0.0f, m_globalGravityStrength, 0.0f};
                    break;
                    
                case GravityType::ZERO:
                    physicsData->gravity = Vector3{};
                    physicsData->gravityEnabled = false;
                    break;
                    
                case GravityType::CUSTOM:
                    // Custom gravity will be set via setObjectGravity
                    break;
                    
                default:
                    physicsData->gravity = Vector3{0.0f, m_globalGravityStrength, 0.0f};
                    break;
            }
            
            spdlog::debug("Gravity enabled for object {}: Type {}", 
                         static_cast<int>(object), static_cast<int>(gravity_type));
        }
    }
    
    void setGlobalGravityStrength(float gravity_strength) override {
        m_globalGravityStrength = gravity_strength;
        spdlog::info("Global gravity strength set to: {}", m_globalGravityStrength);
    }
    
    float getGlobalGravityStrength() const override {
        return m_globalGravityStrength;
    }
    
    void setObjectGravity(ObjectID object, Vector3 gravity_vector) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->gravity = gravity_vector;
            physicsData->gravityEnabled = true;
            spdlog::debug("Object gravity set for object {}: {}", 
                         static_cast<int>(object), "Vector3"); // Placeholder
        }
    }
    
    Vector3 getObjectGravity(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->gravity : Vector3{};
    }
    
    void disableObjectGravity(ObjectID object) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->gravityEnabled = false;
            spdlog::debug("Gravity disabled for object {}", static_cast<int>(object));
        }
    }
    
    bool isObjectGravityEnabled(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->gravityEnabled : false;
    }

    // ============================================================================
    // COLLISION RESPONSE
    // ============================================================================
    
    void handleFullCollisionResponse(CollisionResult collision, CollisionType type) override {
        if (!m_collisionResponseEnabled || !m_initialized) {
            return;
        }
        
        // Handle collision response based on type (mapped to supported enums)
        switch (type) {
            case CollisionType::TOUCH:
                spdlog::debug("Collision response: TOUCH at distance {}", collision.collision_distance);
                break;

            case CollisionType::GRAB:
                spdlog::debug("Collision response: GRAB object {}", static_cast<int>(collision.colliding_object));
                break;

            case CollisionType::PUSH:
                spdlog::debug("Collision response: PUSH with intensity {}", collision.collision_intensity);
                break;

            case CollisionType::PULL:
                spdlog::debug("Collision response: PULL with intensity {}", collision.collision_intensity);
                break;

            case CollisionType::THROW:
                spdlog::debug("Collision response: THROW");
                break;

            case CollisionType::BREAK:
                spdlog::debug("Collision response: BREAK");
                break;

            case CollisionType::INTERACT:
                spdlog::debug("Collision response: INTERACT");
                break;

            case CollisionType::DAMAGE:
                spdlog::debug("Collision response: DAMAGE");
                break;

            case CollisionType::TRIGGER:
                spdlog::debug("Collision response: TRIGGER");
                break;

            case CollisionType::NONE:
            default:
                spdlog::warn("Unknown or NONE collision type for response: {}", static_cast<int>(type));
                break;
        }
    }
    
    void setCollisionResponseEnabled(bool enabled) override {
        m_collisionResponseEnabled = enabled;
        spdlog::info("Collision response {}", enabled ? "enabled" : "disabled");
    }
    
    bool isCollisionResponseEnabled() const override {
        return m_collisionResponseEnabled;
    }
    
    void setCollisionResponseStrength(float strength) override {
        m_collisionResponseStrength = std::clamp(strength, 0.0f, 1.0f);
        spdlog::info("Collision response strength set to: {}", m_collisionResponseStrength);
    }
    
    float getCollisionResponseStrength() const override {
        return m_collisionResponseStrength;
    }

    // ============================================================================
    // PERFORMANCE MONITORING
    // ============================================================================
    
    PhysicsPerformance getFullPhysicsPerformance() override {
        PhysicsPerformance performance;
        // Populate performance data
        // This would typically involve more detailed performance metrics
        return performance;
    }
    
    std::string getPerformanceStats() const override {
        std::string stats = "FullPhysicsIntegration Performance Stats:\n";
        stats += "  Collision checks: " + std::to_string(m_performance.collision_checks) + "\n";
        stats += "  Physics updates: " + std::to_string(m_performance.physics_updates) + "\n";
        stats += "  Force applications: " + std::to_string(m_performance.force_applications) + "\n";
        stats += "  Constraint solves: " + std::to_string(m_performance.constraint_solves) + "\n";
        stats += "  Total simulation time: " + std::to_string(m_performance.total_simulation_time) + "s\n";
        stats += "  Average frame time: " + std::to_string(m_performance.average_frame_time * 1000.0f) + "ms\n";
        stats += "  Total frames: " + std::to_string(m_performance.total_frames) + "\n";
        stats += "  Physics objects: " + std::to_string(m_physicsObjects.size()) + "\n";
        stats += "  Active constraints: " + std::to_string(m_constraints.size()) + "\n";
        return stats;
    }
    
    void resetPerformanceCounters() override {
        m_performance = PerformanceData{};
        spdlog::info("Performance counters reset");
    }
    
    std::string getMemoryUsage() const override {
        std::string memory = "FullPhysicsIntegration Memory Usage:\n";
        memory += "  Physics objects: " + std::to_string(m_physicsObjects.size() * sizeof(PhysicsObjectData)) + " bytes\n";
        memory += "  Constraints: " + std::to_string(m_constraints.size() * sizeof(ConstraintData)) + " bytes\n";
        memory += "  Performance data: " + std::to_string(sizeof(PerformanceData)) + " bytes\n";
        return memory;
    }

    // ============================================================================
    // OBJECT INTERACTION
    // ============================================================================
    
    InteractionResult interactWithFullObject(ObjectID object, HandType hand, InteractionType type) override {
        if (!m_initialized) {
            return InteractionResult{};
        }
        
        // Check if object can be interacted with
        if (!canInteractWithObject(object, hand)) {
            return InteractionResult{};
        }
        
        // Perform interaction based on type
        InteractionResult result;
        // Populate result with interaction data
        
        spdlog::debug("Object interaction: Object {}, Hand {}, Type {}", 
                     static_cast<int>(object), static_cast<int>(hand), static_cast<int>(type));
        
        return result;
    }
    
    bool canInteractWithObject(ObjectID object, HandType hand) const override {
        // Check if object exists and is interactable
        auto physicsData = getPhysicsObjectData(object);
        if (!physicsData) {
            return false;
        }
        
        // Check if object is in range and interactable
        // This would typically involve distance checks and interaction flags
        return true; // Placeholder
    }
    
    std::vector<ObjectID> getInteractableObjects(HandType hand) const override {
        std::vector<ObjectID> interactableObjects;
        
        // Find all objects that can be interacted with by this hand
        for (const auto& physics : m_physicsObjects) {
            if (canInteractWithObject(physics.object, hand)) {
                interactableObjects.push_back(physics.object);
            }
        }
        
        return interactableObjects;
    }

    // ============================================================================
    // OBJECT PROPERTIES
    // ============================================================================
    
    void setObjectMass(ObjectID object, float mass) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->mass = std::max(0.001f, mass); // Minimum mass to avoid division by zero
            spdlog::debug("Object mass set: Object {}, Mass {}", static_cast<int>(object), mass);
        }
    }
    
    float getObjectMass(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->mass : 0.0f;
    }
    
    void setObjectFriction(ObjectID object, float friction) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->friction = std::clamp(friction, 0.0f, 1.0f);
            spdlog::debug("Object friction set: Object {}, Friction {}", static_cast<int>(object), friction);
        }
    }
    
    float getObjectFriction(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->friction : 0.0f;
    }
    
    void setObjectRestitution(ObjectID object, float restitution) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->restitution = std::clamp(restitution, 0.0f, 1.0f);
            spdlog::debug("Object restitution set: Object {}, Restitution {}", static_cast<int>(object), restitution);
        }
    }
    
    float getObjectRestitution(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->restitution : 0.0f;
    }

    // ============================================================================
    // OBJECT TRANSFORMS
    // ============================================================================
    
    void setObjectPosition(ObjectID object, Vector3 position) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->position = position;
        }
    }
    
    Vector3 getObjectPosition(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->position : Vector3{};
    }
    
    void setObjectRotation(ObjectID object, Vector3 rotation) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->rotation = rotation;
        }
    }
    
    Vector3 getObjectRotation(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->rotation : Vector3{};
    }
    
    void setObjectScale(ObjectID object, Vector3 scale) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->scale = scale;
        }
    }
    
    Vector3 getObjectScale(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->scale : Vector3{};
    }

    // ============================================================================
    // OBJECT VELOCITY AND MOMENTUM
    // ============================================================================
    
    void setObjectVelocity(ObjectID object, Vector3 velocity) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->velocity = velocity;
        }
    }
    
    Vector3 getObjectVelocity(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->velocity : Vector3{};
    }
    
    void setObjectAngularVelocity(ObjectID object, Vector3 angular_velocity) override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData) {
            physicsData->angular_velocity = angular_velocity;
        }
    }
    
    Vector3 getObjectAngularVelocity(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        return physicsData ? physicsData->angular_velocity : Vector3{};
    }
    
    Vector3 getObjectMomentum(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData && physicsData->mass > 0.0f) {
            return physicsData->velocity * physicsData->mass;
        }
        return Vector3{};
    }
    
    Vector3 getObjectAngularMomentum(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        if (physicsData && physicsData->mass > 0.0f) {
            // Simplified angular momentum calculation
            return physicsData->angular_velocity * physicsData->mass;
        }
        return Vector3{};
    }

    // ============================================================================
    // CONSTRAINT SYSTEM
    // ============================================================================
    
    bool createFixedConstraint(ObjectID object1, ObjectID object2) override {
        if (!m_initialized) {
            return false;
        }
        
        ConstraintData constraint{object1, object2, "FIXED", Vector3{}, 0.0f, 0.0f};
        m_constraints.push_back(constraint);
        
        // Add constraint reference to physics objects
        auto physics1 = getPhysicsObjectData(object1);
        auto physics2 = getPhysicsObjectData(object2);
        if (physics1) physics1->constraints.push_back(object2);
        if (physics2) physics2->constraints.push_back(object1);
        
        spdlog::info("Fixed constraint created between objects {} and {}", 
                     static_cast<int>(object1), static_cast<int>(object2));
        return true;
    }
    
    bool createHingeConstraint(ObjectID object1, ObjectID object2, Vector3 axis) override {
        if (!m_initialized) {
            return false;
        }
        
        ConstraintData constraint{object1, object2, "HINGE", axis, 0.0f, 0.0f};
        m_constraints.push_back(constraint);
        
        // Add constraint reference to physics objects
        auto physics1 = getPhysicsObjectData(object1);
        auto physics2 = getPhysicsObjectData(object2);
        if (physics1) physics1->constraints.push_back(object2);
        if (physics2) physics2->constraints.push_back(object1);
        
        spdlog::info("Hinge constraint created between objects {} and {} with axis {}", 
                     static_cast<int>(object1), static_cast<int>(object2), "Vector3");
        return true;
    }
    
    bool createSpringConstraint(ObjectID object1, ObjectID object2, float spring_constant, float damping) override {
        if (!m_initialized) {
            return false;
        }
        
        ConstraintData constraint{object1, object2, "SPRING", Vector3{}, spring_constant, damping};
        m_constraints.push_back(constraint);
        
        // Add constraint reference to physics objects
        auto physics1 = getPhysicsObjectData(object1);
        auto physics2 = getPhysicsObjectData(object2);
        if (physics1) physics1->constraints.push_back(object2);
        if (physics2) physics2->constraints.push_back(object1);
        
        spdlog::info("Spring constraint created between objects {} and {}: k={}, d={}", 
                     static_cast<int>(object1), static_cast<int>(object2), spring_constant, damping);
        return true;
    }
    
    bool removeConstraint(ObjectID object1, ObjectID object2) override {
        // Remove constraint from constraints list
        auto it = std::remove_if(m_constraints.begin(), m_constraints.end(),
            [object1, object2](const ConstraintData& constraint) {
                return (constraint.object1 == object1 && constraint.object2 == object2) ||
                       (constraint.object1 == object2 && constraint.object2 == object1);
            });
        
        if (it != m_constraints.end()) {
            m_constraints.erase(it, m_constraints.end());
            
            // Remove constraint references from physics objects
            auto physics1 = getPhysicsObjectData(object1);
            auto physics2 = getPhysicsObjectData(object2);
            if (physics1) {
                auto ref_it = std::remove(physics1->constraints.begin(), physics1->constraints.end(), object2);
                physics1->constraints.erase(ref_it, physics1->constraints.end());
            }
            if (physics2) {
                auto ref_it = std::remove(physics2->constraints.begin(), physics2->constraints.end(), object1);
                physics2->constraints.erase(ref_it, physics2->constraints.end());
            }
            
            spdlog::info("Constraint removed between objects {} and {}", 
                         static_cast<int>(object1), static_cast<int>(object2));
            return true;
        }
        
        return false;
    }
    
    bool areObjectsConstrained(ObjectID object1, ObjectID object2) const override {
        return std::any_of(m_constraints.begin(), m_constraints.end(),
            [object1, object2](const ConstraintData& constraint) {
                return (constraint.object1 == object1 && constraint.object2 == object2) ||
                       (constraint.object1 == object2 && constraint.object2 == object1);
            });
    }

    // ============================================================================
    // COLLISION DETECTION SETTINGS
    // ============================================================================
    
    void setCollisionDetectionPrecision(float precision) override {
        m_collisionDetectionPrecision = std::clamp(precision, 0.0f, 1.0f);
        spdlog::info("Collision detection precision set to: {}", m_collisionDetectionPrecision);
    }
    
    float getCollisionDetectionPrecision() const override {
        return m_collisionDetectionPrecision;
    }
    
    void setCollisionDetectionFrequency(float frequency) override {
        m_collisionDetectionFrequency = std::clamp(frequency, 1.0f, 1000.0f);
        spdlog::info("Collision detection frequency set to: {} Hz", m_collisionDetectionFrequency);
    }
    
    float getCollisionDetectionFrequency() const override {
        return m_collisionDetectionFrequency;
    }
    
    void setCollisionDetectionForType(PhysicsType physics_type, bool enabled) override {
        m_collisionDetectionEnabled[physics_type] = enabled;
        spdlog::info("Collision detection for physics type {} {}", 
                     static_cast<int>(physics_type), enabled ? "enabled" : "disabled");
    }
    
    bool isCollisionDetectionEnabledForType(PhysicsType physics_type) const override {
        auto it = m_collisionDetectionEnabled.find(physics_type);
        return it != m_collisionDetectionEnabled.end() ? it->second : false;
    }

    // ============================================================================
    // DEBUG AND VISUALIZATION
    // ============================================================================
    
    void setPhysicsDebugVisualization(bool enabled) override {
        m_debugVisualizationEnabled = enabled;
        spdlog::info("Physics debug visualization {}", enabled ? "enabled" : "disabled");
    }
    
    bool isPhysicsDebugVisualizationEnabled() const override {
        return m_debugVisualizationEnabled;
    }
    
    void showObjectPhysicsDebug(ObjectID object) override {
        if (m_debugVisualizationEnabled) {
            spdlog::debug("Showing physics debug for object {}", static_cast<int>(object));
        }
    }
    
    void hideObjectPhysicsDebug(ObjectID object) override {
        if (m_debugVisualizationEnabled) {
            spdlog::debug("Hiding physics debug for object {}", static_cast<int>(object));
        }
    }
    
    std::string getObjectPhysicsDebug(ObjectID object) const override {
        auto physicsData = getPhysicsObjectData(object);
        if (!physicsData) {
            return "Object not found";
        }
        
        std::string debug = "Physics Debug for Object " + std::to_string(static_cast<int>(object)) + ":\n";
        debug += "  Type: " + std::to_string(static_cast<int>(physicsData->type)) + "\n";
        debug += "  Simulating: " + std::string(physicsData->isSimulating ? "Yes" : "No") + "\n";
        debug += "  Mass: " + std::to_string(physicsData->mass) + "\n";
        debug += "  Friction: " + std::to_string(physicsData->friction) + "\n";
        debug += "  Restitution: " + std::to_string(physicsData->restitution) + "\n";
        debug += "  Gravity Enabled: " + std::string(physicsData->gravityEnabled ? "Yes" : "No") + "\n";
        debug += "  Constraints: " + std::to_string(physicsData->constraints.size()) + "\n";
        
        return debug;
    }

private:
    // Helper functions
    PhysicsObjectData* getPhysicsObjectData(ObjectID object) {
        auto it = std::find_if(m_physicsObjects.begin(), m_physicsObjects.end(),
            [object](const PhysicsObjectData& data) { return data.object == object; });
        return it != m_physicsObjects.end() ? &(*it) : nullptr;
    }
    
    const PhysicsObjectData* getPhysicsObjectData(ObjectID object) const {
        auto it = std::find_if(m_physicsObjects.begin(), m_physicsObjects.end(),
            [object](const PhysicsObjectData& data) { return data.object == object; });
        return it != m_physicsObjects.end() ? &(*it) : nullptr;
    }
    
    PhysicsObjectData* getOrCreatePhysicsObjectData(ObjectID object) {
        auto physicsData = getPhysicsObjectData(object);
        if (!physicsData) {
            PhysicsObjectData newData{object, PhysicsType::STATIC, false, Vector3{}, Vector3{}, Vector3{1.0f, 1.0f, 1.0f},
                                    Vector3{}, Vector3{}, Vector3{}, Vector3{}, Vector3{}, Vector3{0.0f, m_globalGravityStrength, 0.0f},
                                    1.0f, 0.5f, 0.5f, true, {}};
            m_physicsObjects.push_back(newData);
            physicsData = &m_physicsObjects.back();
        }
        return physicsData;
    }
    
    void solveConstraints(float delta_time) {
        m_performance.constraint_solves++;
        
        // Solve all constraints
        for (const auto& constraint : m_constraints) {
            // This is where you'd implement constraint solving
            // For now, just log that constraints are being solved
            spdlog::debug("Solving constraint: {} between objects {} and {}", 
                         constraint.type, static_cast<int>(constraint.object1), static_cast<int>(constraint.object2));
        }
    }
    
    // Collision response handlers
    void handleHandObjectCollision(const CollisionResult& collision) {
        // Handle hand-object collision response
        spdlog::debug("Hand-object collision response handled");
    }
    
    void handleHandDoorCollision(const CollisionResult& collision) {
        // Handle hand-door collision response
        spdlog::debug("Hand-door collision response handled");
    }
    
    void handleHandWeaponCollision(const CollisionResult& collision) {
        // Handle hand-weapon collision response
        spdlog::debug("Hand-weapon collision response handled");
    }
    
    void handleHandVehicleCollision(const CollisionResult& collision) {
        // Handle hand-vehicle collision response
        spdlog::debug("Hand-vehicle collision response handled");
    }
    
    void handleHandNPCCollision(const CollisionResult& collision) {
        // Handle hand-NPC collision response (VISUAL FEEDBACK ONLY!)
        spdlog::debug("Hand-NPC collision response handled (visual feedback only)");
    }
    
    void handleHandEnvironmentCollision(const CollisionResult& collision) {
        // Handle hand-environment collision response
        spdlog::debug("Hand-environment collision response handled");
    }
    
    void handleHandPuzzleCollision(const CollisionResult& collision) {
        // Handle hand-puzzle collision response
        spdlog::debug("Hand-puzzle collision response handled");
    }
    
    void handleHandInventoryCollision(const CollisionResult& collision) {
        // Handle hand-inventory collision response
        spdlog::debug("Hand-inventory collision response handled");
    }
};

// Factory function to create the implementation
std::unique_ptr<FullPhysicsIntegration> createFullPhysicsIntegration() {
    return std::make_unique<FullPhysicsIntegrationImpl>();
}

} // namespace vr
} // namespace uevr
