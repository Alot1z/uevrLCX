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
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include "uevr/vr/FullAestheticCollisionEngine.hpp"

namespace uevr {
namespace vr {

// Forward declarations
class InteractionResult;
class ForceType;
class GravityType;

// Interaction result structure
struct InteractionResult {
    bool interaction_successful;
    float interaction_strength;
    glm::vec3 interaction_point;
    glm::vec3 interaction_direction;
    ObjectID interacted_object;
    CollisionType interaction_type;
    float response_time;
    
    InteractionResult() : interaction_successful(false), interaction_strength(0.0f), 
                         response_time(0.0f) {}
};

// Force types for physics simulation
enum class ForceType {
    NONE,           // No force applied
    GRAVITY,        // Gravitational force
    IMPULSE,        // Instant force impulse
    CONSTANT,       // Constant force over time
    SPRING,         // Spring force
    DAMPING,        // Damping force
    FRICTION,       // Friction force
    BUOYANCY,       // Buoyancy force
    MAGNETIC,       // Magnetic force
    WIND            // Wind force
};

// Gravity types for different environments
enum class GravityType {
    NONE,           // No gravity
    EARTH,          // Earth gravity (9.81 m/s²)
    MOON,           // Moon gravity (1.62 m/s²)
    MARS,           // Mars gravity (3.71 m/s²)
    SPACE,          // Microgravity
    CUSTOM          // Custom gravity value
};

// Physics performance metrics
struct PhysicsPerformance {
    float frame_time;
    float physics_update_time;
    uint64_t active_objects;
    uint64_t collision_checks;
    uint64_t physics_calculations;
    float memory_usage_mb;
    float cpu_usage_percent;
    
    PhysicsPerformance() : frame_time(0.0f), physics_update_time(0.0f), 
                          active_objects(0), collision_checks(0), physics_calculations(0),
                          memory_usage_mb(0.0f), cpu_usage_percent(0.0f) {}
};

// Main physics integration class
class FullPhysicsIntegration {
public:
    FullPhysicsIntegration();
    virtual ~FullPhysicsIntegration();
    
    // Initialize full physics system
    virtual bool initializeFullPhysics();
    
    // Shutdown physics system
    virtual void shutdownPhysics();
    
    // Hand collision with ALL objects
    virtual bool checkFullHandCollision(HandType hand, ObjectID object, CollisionType type);
    
    // Object physics for ALL types
    virtual void simulateFullObjectPhysics(ObjectID object, PhysicsType physics_type);
    
    // Force application to ALL objects
    virtual void applyFullForce(ObjectID object, const glm::vec3& force, ForceType force_type);
    
    // Gravity simulation for ALL objects
    virtual void simulateFullGravity(ObjectID object, GravityType gravity_type);
    
    // Collision response for ALL types
    virtual void handleFullCollisionResponse(const CollisionResult& collision, CollisionType type);
    
    // Performance monitoring for ALL physics
    virtual PhysicsPerformance getFullPhysicsPerformance() const;
    
    // Object interaction for ALL types
    virtual InteractionResult interactWithFullObject(ObjectID object, HandType hand, InteractionType type);
    
    // Advanced physics features
    virtual bool enableAdvancedPhysics(ObjectID object, bool enable);
    virtual bool setPhysicsProperties(ObjectID object, const struct PhysicsProperties& properties);
    virtual bool createPhysicsConstraint(ObjectID object1, ObjectID object2, const struct ConstraintData& constraint);
    virtual bool removePhysicsConstraint(ObjectID object1, ObjectID object2);
    
    // Physics simulation control
    virtual void setPhysicsTimeStep(float time_step);
    virtual void setPhysicsIterations(uint32_t iterations);
    virtual void enablePhysicsDebug(bool enable);
    virtual void setPhysicsGravity(const glm::vec3& gravity);
    
    // Object physics state management
    virtual bool setObjectPhysicsState(ObjectID object, const struct PhysicsState& state);
    virtual struct PhysicsState getObjectPhysicsState(ObjectID object) const;
    virtual bool resetObjectPhysics(ObjectID object);
    virtual bool freezeObjectPhysics(ObjectID object, bool freeze);
    
    // Physics world management
    virtual bool createPhysicsWorld();
    virtual bool destroyPhysicsWorld();
    virtual bool stepPhysicsWorld(float delta_time);
    virtual bool clearPhysicsWorld();

protected:
    // Internal physics methods
    virtual bool initializePhysicsEngine();
    virtual bool initializeCollisionShapes();
    virtual bool initializePhysicsMaterials();
    virtual bool initializePhysicsConstraints();
    
    // Physics simulation helpers
    virtual void updatePhysicsSimulation(float delta_time);
    virtual void processPhysicsCollisions();
    virtual void applyPhysicsForces();
    virtual void updatePhysicsConstraints();
    
    // Performance monitoring
    virtual void updatePerformanceMetrics();
    virtual void optimizePhysicsPerformance();

private:
    // Internal state
    bool m_initialized;
    bool m_advanced_physics_enabled;
    bool m_physics_debug_enabled;
    float m_physics_time_step;
    uint32_t m_physics_iterations;
    glm::vec3 m_world_gravity;
    
    // Physics engine components
    std::unique_ptr<FullAestheticCollisionEngine> m_collision_engine;
    std::unordered_map<ObjectID, struct ObjectPhysicsData> m_physics_objects;
    std::vector<struct ConstraintData> m_physics_constraints;
    
    // Performance tracking
    PhysicsPerformance m_performance;
    std::chrono::high_resolution_clock::time_point m_last_frame_time;
    
    // Physics world state
    bool m_physics_world_created;
    uint64_t m_frame_count;
    
    // Object physics data structure
    struct ObjectPhysicsData {
        PhysicsType physics_type;
        PhysicsState physics_state;
        PhysicsProperties properties;
        bool advanced_physics_enabled;
        bool physics_frozen;
        std::vector<ObjectID> constrained_objects;
        
        ObjectPhysicsData() : physics_type(PhysicsType::STATIC), advanced_physics_enabled(false), 
                             physics_frozen(false) {}
    };
    
    // Physics state structure
    struct PhysicsState {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 angular_velocity;
        glm::quat rotation;
        glm::mat4 transform;
        bool is_active;
        bool is_sleeping;
        
        PhysicsState() : position(0.0f), velocity(0.0f), angular_velocity(0.0f), 
                        rotation(1.0f, 0.0f, 0.0f, 0.0f), is_active(true), is_sleeping(false) {}
    };
    
    // Physics properties structure
    struct PhysicsProperties {
        float mass;
        float friction;
        float restitution;
        float linear_damping;
        float angular_damping;
        glm::vec3 inertia;
        bool kinematic;
        bool trigger;
        
        PhysicsProperties() : mass(1.0f), friction(0.5f), restitution(0.5f), 
                            linear_damping(0.0f), angular_damping(0.0f), 
                            inertia(1.0f), kinematic(false), trigger(false) {}
    };
    
    // Constraint data structure
    struct ConstraintData {
        ObjectID object1;
        ObjectID object2;
        glm::vec3 pivot1;
        glm::vec3 pivot2;
        glm::vec3 axis1;
        glm::vec3 axis2;
        float lower_limit;
        float upper_limit;
        bool enabled;
        
        ConstraintData() : object1(0), object2(0), lower_limit(-180.0f), upper_limit(180.0f), enabled(true) {}
    };
    
    // Private helper methods
    void initializeDefaultPhysicsProperties();
    bool validatePhysicsObject(ObjectID object) const;
    void cleanupPhysicsObject(ObjectID object);
    void updatePhysicsObject(ObjectID object, float delta_time);
    void applyObjectForces(ObjectID object, float delta_time);
    void handleObjectCollision(ObjectID object1, ObjectID object2);
    void updateObjectConstraints(ObjectID object);
    void optimizePhysicsObject(ObjectID object);
};

} // namespace vr
} // namespace uevr