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

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace uevr {

// Forward declarations
class VRSystem;
class PhysicsEngine;
class CollisionEngine;
class HapticSystem;

// Physics quality levels for performance optimization
enum class PhysicsQuality {
    LOW,                   // Low quality physics (60 FPS target)
    MEDIUM,                // Medium quality physics (90 FPS target)
    HIGH,                  // High quality physics (120 FPS target)
    ULTRA                  // Ultra quality physics (144 FPS target)
};

// Physics simulation types
enum class PhysicsSimType {
    REAL_TIME,             // Real-time physics simulation
    FIXED_STEP,            // Fixed time step physics
    ADAPTIVE,              // Adaptive time step physics
    HYBRID                 // Hybrid physics simulation
};

// Force types for physics application
enum class ForceType {
    IMPULSE,               // Instant force application
    CONTINUOUS,            // Continuous force application
    SPRING,                // Spring force application
    DAMPING,               // Damping force application
    TORQUE,                // Rotational force application
    GRAVITY,               // Gravity force application
    WIND,                  // Wind force application
    BUOYANCY,              // Buoyancy force application
    FRICTION,              // Friction force application
    MAGNETIC               // Magnetic force application
};

// Gravity types for different environments
enum class GravityType {
    NONE,                  // No gravity
    EARTH,                 // Earth gravity (9.81 m/s²)
    MOON,                  // Moon gravity (1.62 m/s²)
    MARS,                  // Mars gravity (3.71 m/s²)
    SPACE,                 // Microgravity
    CUSTOM,                // Custom gravity value
    VARIABLE,              // Variable gravity
    INVERSE,               // Inverse gravity
    RADIAL,                // Radial gravity
    PLANAR                 // Planar gravity
};

// Physics material properties
enum class MaterialType {
    DEFAULT,               // Default material
    METAL,                 // Metal material
    WOOD,                  // Wood material
    STONE,                 // Stone material
    PLASTIC,               // Plastic material
    RUBBER,                // Rubber material
    ICE,                   // Ice material
    WATER,                 // Water material
    AIR,                   // Air material
    VACUUM,                // Vacuum material
    CUSTOM                 // Custom material
};

// Physics constraint types
enum class ConstraintType {
    NONE,                  // No constraint
    POINT,                 // Point constraint
    HINGE,                 // Hinge constraint
    SLIDER,                // Slider constraint
    CONE,                  // Cone constraint
    PLANE,                 // Plane constraint
    FIXED,                 // Fixed constraint
    BREAKABLE,             // Breakable constraint
    SPRING,                // Spring constraint
    MOTOR                  // Motor constraint
};

// Physics collision shapes
enum class CollisionShape {
    SPHERE,                // Sphere collision shape
    BOX,                   // Box collision shape
    CYLINDER,              // Cylinder collision shape
    CAPSULE,               // Capsule collision shape
    CONE,                  // Cone collision shape
    PLANE,                 // Plane collision shape
    MESH,                  // Mesh collision shape
    HEIGHTFIELD,           // Heightfield collision shape
    COMPOUND,              // Compound collision shape
    CUSTOM                 // Custom collision shape
};

// Physics solver types
enum class SolverType {
    SEQUENTIAL,            // Sequential impulse solver
    PGS,                   // Projected Gauss-Seidel solver
    NNCG,                  // Nonlinear Conjugate Gradient solver
    DIRECT,                // Direct solver
    ITERATIVE,             // Iterative solver
    HYBRID                 // Hybrid solver
};

// Physics performance metrics
struct PhysicsPerformance {
    uint32_t active_bodies = 0;
    uint32_t total_constraints = 0;
    uint32_t collision_pairs = 0;
    float simulation_time = 0.0f;
    float collision_time = 0.0f;
    float solver_time = 0.0f;
    float broadphase_time = 0.0f;
    float narrowphase_time = 0.0f;
    uint32_t solver_iterations = 0;
    float memory_usage_mb = 0.0f;
    float cpu_usage_percent = 0.0f;
    float gpu_usage_percent = 0.0f;
    uint32_t frames_per_second = 0;
    float average_frame_time = 0.0f;
    float min_frame_time = 0.0f;
    float max_frame_time = 0.0f;
};

// Physics settings structure
struct PhysicsSettings {
    PhysicsQuality quality = PhysicsQuality::MEDIUM;
    PhysicsSimType simulation_type = PhysicsSimType::FIXED_STEP;
    float time_step = 1.0f / 60.0f;
    uint32_t max_sub_steps = 10;
    uint32_t solver_iterations = 4;
    uint32_t solver_velocity_iterations = 1;
    float gravity_magnitude = 9.81f;
    glm::vec3 gravity_direction = glm::vec3(0.0f, -1.0f, 0.0f);
    bool enable_ccd = true;
    bool enable_continuous_collision = true;
    bool enable_adaptive_force = true;
    bool enable_contact_persistence = true;
    float contact_tolerance = 0.001f;
    float penetration_tolerance = 0.001f;
    uint32_t max_contacts_per_body = 8;
    uint32_t max_manifold_points = 4;
    bool enable_debug_drawing = false;
    bool enable_profiling = true;
    bool enable_multithreading = true;
    uint32_t num_threads = 4;
    float sleep_threshold = 0.8f;
    float linear_sleep_threshold = 0.01f;
    float angular_sleep_threshold = 0.01f;
};

// Physics object data structure
struct PhysicsObject {
    uint64_t id = 0;
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 linear_velocity = glm::vec3(0.0f);
    glm::vec3 angular_velocity = glm::vec3(0.0f);
    glm::vec3 linear_acceleration = glm::vec3(0.0f);
    glm::vec3 angular_acceleration = glm::vec3(0.0f);
    glm::vec3 size = glm::vec3(1.0f);
    float mass = 1.0f;
    float inverse_mass = 1.0f;
    glm::mat3 inertia_tensor = glm::mat3(1.0f);
    glm::mat3 inverse_inertia_tensor = glm::mat3(1.0f);
    CollisionShape collision_shape = CollisionShape::BOX;
    MaterialType material = MaterialType::DEFAULT;
    bool is_static = false;
    bool is_kinematic = false;
    bool is_trigger = false;
    bool is_sleeping = false;
    bool is_active = true;
    bool collision_enabled = true;
    bool physics_enabled = true;
    float friction = 0.5f;
    float restitution = 0.3f;
    float rolling_friction = 0.0f;
    float spinning_friction = 0.0f;
    float linear_damping = 0.0f;
    float angular_damping = 0.0f;
    glm::vec3 custom_gravity = glm::vec3(0.0f);
    bool use_custom_gravity = false;
    std::string name = "";
    std::vector<uint64_t> attached_objects;
    std::vector<uint64_t> constraints;
};

// Physics force structure
struct PhysicsForce {
    uint64_t id = 0;
    uint64_t target_object = 0;
    ForceType type = ForceType::IMPULSE;
    glm::vec3 force_vector = glm::vec3(0.0f);
    glm::vec3 point_of_application = glm::vec3(0.0f);
    float magnitude = 0.0f;
    float duration = 0.0f;
    float remaining_time = 0.0f;
    bool is_active = true;
    bool is_relative = false;
    std::function<void(uint64_t, const glm::vec3&)> callback = nullptr;
};

// Physics constraint structure
struct PhysicsConstraint {
    uint64_t id = 0;
    uint64_t object_a = 0;
    uint64_t object_b = 0;
    ConstraintType type = ConstraintType::NONE;
    glm::vec3 pivot_a = glm::vec3(0.0f);
    glm::vec3 pivot_b = glm::vec3(0.0f);
    glm::vec3 axis_a = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 axis_b = glm::vec3(0.0f, 1.0f, 0.0f);
    float lower_limit = -std::numeric_limits<float>::infinity();
    float upper_limit = std::numeric_limits<float>::infinity();
    float softness = 0.0f;
    float bias_factor = 0.3f;
    float relaxation_factor = 1.0f;
    bool is_active = true;
    bool is_breakable = false;
    float breaking_threshold = std::numeric_limits<float>::infinity();
    std::function<void(uint64_t)> break_callback = nullptr;
};

// Physics collision pair structure
struct PhysicsCollisionPair {
    uint64_t id = 0;
    uint64_t object_a = 0;
    uint64_t object_b = 0;
    glm::vec3 contact_point = glm::vec3(0.0f);
    glm::vec3 contact_normal = glm::vec3(0.0f);
    float penetration_depth = 0.0f;
    float contact_distance = 0.0f;
    glm::vec3 relative_velocity = glm::vec3(0.0f);
    float friction_coefficient = 0.0f;
    float restitution_coefficient = 0.0f;
    bool is_active = true;
    bool is_persistent = false;
    uint32_t persistent_count = 0;
    float contact_time = 0.0f;
    std::vector<glm::vec3> manifold_points;
    std::vector<glm::vec3> manifold_normals;
};

// Main Full Physics Integration class
class FullPhysicsIntegration {
public:
    // Constructor and destructor
    FullPhysicsIntegration();
    virtual ~FullPhysicsIntegration();

    // Core initialization
    virtual bool initializeFullPhysics();
    virtual bool shutdownFullPhysics();
    virtual bool isInitialized() const;

    // Physics system management
    virtual bool setPhysicsQuality(PhysicsQuality quality);
    virtual bool setSimulationType(PhysicsSimType sim_type);
    virtual bool setTimeStep(float time_step);
    virtual bool setMaxSubSteps(uint32_t max_steps);
    virtual bool setSolverIterations(uint32_t iterations);
    virtual bool setGravity(float magnitude, const glm::vec3& direction);
    virtual bool setCustomGravity(uint64_t object, const glm::vec3& gravity);

    // Object management
    virtual uint64_t createPhysicsObject(const PhysicsObject& object_data);
    virtual bool destroyPhysicsObject(uint64_t object_id);
    virtual bool updatePhysicsObject(uint64_t object_id, const PhysicsObject& object_data);
    virtual PhysicsObject getPhysicsObject(uint64_t object_id) const;
    virtual std::vector<uint64_t> getAllPhysicsObjects() const;
    virtual bool setObjectActive(uint64_t object_id, bool active);
    virtual bool setObjectCollisionEnabled(uint64_t object_id, bool enabled);
    virtual bool setObjectPhysicsEnabled(uint64_t object_id, bool enabled);

    // Force application
    virtual uint64_t applyForce(uint64_t object_id, ForceType type, const glm::vec3& force, const glm::vec3& point = glm::vec3(0.0f));
    virtual uint64_t applyImpulse(uint64_t object_id, const glm::vec3& impulse, const glm::vec3& point = glm::vec3(0.0f));
    virtual uint64_t applyTorque(uint64_t object_id, const glm::vec3& torque);
    virtual uint64_t applyAngularImpulse(uint64_t object_id, const glm::vec3& angular_impulse);
    virtual bool removeForce(uint64_t force_id);
    virtual bool clearAllForces(uint64_t object_id);
    virtual std::vector<PhysicsForce> getActiveForces(uint64_t object_id) const;

    // Constraint management
    virtual uint64_t createConstraint(const PhysicsConstraint& constraint_data);
    virtual bool destroyConstraint(uint64_t constraint_id);
    virtual bool updateConstraint(uint64_t constraint_id, const PhysicsConstraint& constraint_data);
    virtual PhysicsConstraint getConstraint(uint64_t constraint_id) const;
    virtual std::vector<uint64_t> getObjectConstraints(uint64_t object_id) const;
    virtual bool setConstraintActive(uint64_t constraint_id, bool active);
    virtual bool setConstraintLimits(uint64_t constraint_id, float lower, float upper);

    // Collision detection
    virtual bool checkFullHandCollision(uint64_t hand_id, uint64_t object_id, CollisionType type);
    virtual std::vector<PhysicsCollisionPair> detectCollisions(uint64_t object_id);
    virtual std::vector<PhysicsCollisionPair> detectAllCollisions();
    virtual bool isColliding(uint64_t object_a, uint64_t object_b);
    virtual PhysicsCollisionPair getCollisionPair(uint64_t object_a, uint64_t object_b) const;

    // Physics simulation
    virtual void simulateFullObjectPhysics(uint64_t object_id, PhysicsType physics_type);
    virtual void simulateAllPhysics(float delta_time);
    virtual bool updatePhysicsSystem(float delta_time);
    virtual void stepSimulation(float delta_time);
    virtual void stepSimulation(float delta_time, uint32_t max_sub_steps);

    // Object interaction
    virtual InteractionResult interactWithFullObject(uint64_t object_id, uint64_t hand_id, InteractionType type);
    virtual bool grabObject(uint64_t object_id, uint64_t hand_id);
    virtual bool releaseObject(uint64_t object_id, uint64_t hand_id);
    virtual bool moveObject(uint64_t object_id, const glm::vec3& position);
    virtual bool rotateObject(uint64_t object_id, const glm::quat& rotation);
    virtual bool setObjectVelocity(uint64_t object_id, const glm::vec3& linear_vel, const glm::vec3& angular_vel);

    // Advanced physics features
    virtual bool enableContinuousCollisionDetection(uint64_t object_id);
    virtual bool disableContinuousCollisionDetection(uint64_t object_id);
    virtual bool setObjectMass(uint64_t object_id, float mass);
    virtual bool setObjectFriction(uint64_t object_id, float friction);
    virtual bool setObjectRestitution(uint64_t object_id, float restitution);
    virtual bool setObjectDamping(uint64_t object_id, float linear_damping, float angular_damping);
    virtual bool setObjectCollisionShape(uint64_t object_id, CollisionShape shape, const glm::vec3& size);

    // Performance monitoring
    virtual PhysicsPerformance getFullPhysicsPerformance() const;
    virtual void resetPerformanceMetrics();
    virtual bool optimizePerformance();
    virtual bool enableProfiling(bool enable);
    virtual bool enableDebugDrawing(bool enable);

    // Configuration
    virtual bool loadPhysicsConfiguration(const std::string& config_file);
    virtual bool savePhysicsConfiguration(const std::string& config_file);
    virtual PhysicsSettings getPhysicsSettings() const;
    virtual bool setPhysicsSettings(const PhysicsSettings& settings);

    // Game-specific physics features
    virtual bool enableGameSpecificPhysics(const std::string& game_name);
    virtual bool disableGameSpecificPhysics();
    virtual std::string getCurrentGame() const;

    // Horror-specific features (for RE7)
    virtual bool enableHorrorPhysics();
    virtual bool disableHorrorPhysics();
    virtual bool setHorrorPhysicsIntensity(float intensity);

    // Combat-specific features (for MHW)
    virtual bool enableCombatPhysics();
    virtual bool disableCombatPhysics();
    virtual bool setCombatPhysicsIntensity(float intensity);

    // Cyberpunk-specific features
    virtual bool enableCyberpunkPhysics();
    virtual bool disableCyberpunkPhysics();
    virtual bool setCyberpunkPhysicsIntensity(float intensity);

    // Error handling and recovery
    virtual bool handlePhysicsError(const std::string& error_message);
    virtual bool recoverFromPhysicsError();
    virtual std::string getLastPhysicsError() const;
    virtual bool isPhysicsErrorState() const;

    // Debug and development
    virtual bool enablePhysicsDebugMode();
    virtual bool disablePhysicsDebugMode();
    virtual bool isPhysicsDebugMode() const;
    virtual void logPhysicsDebugInfo(const std::string& message);
    virtual std::vector<std::string> getPhysicsDebugLog() const;

private:
    // Private implementation details
    class Impl;
    std::unique_ptr<Impl> m_impl;

    // Internal helper methods
    bool initializePhysicsEngine();
    bool initializeCollisionSystem();
    bool initializeConstraintSolver();
    bool initializeForceSystem();
    
    void updatePhysicsObjects(float delta_time);
    void updateForces(float delta_time);
    void updateConstraints(float delta_time);
    void updateCollisions(float delta_time);
    
    void applyPhysicsForces(uint64_t object_id);
    void resolveCollisions();
    void solveConstraints();
    
    void updatePerformanceMetrics(float delta_time);
    bool checkPerformanceThresholds();
    void optimizePhysicsSystem();
    
    bool handleGameSpecificPhysics(uint64_t object_id, InteractionType type);
    bool handleHorrorPhysics(uint64_t object_id, InteractionType type);
    bool handleCombatPhysics(uint64_t object_id, InteractionType type);
    bool handleCyberpunkPhysics(uint64_t object_id, InteractionType type);
};

} // namespace uevr
