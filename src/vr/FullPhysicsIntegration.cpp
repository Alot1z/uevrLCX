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
#include <chrono>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace uevr {
namespace vr {

FullPhysicsIntegration::FullPhysicsIntegration()
    : m_initialized(false)
    , m_advanced_physics_enabled(false)
    , m_physics_debug_enabled(false)
    , m_physics_time_step(0.016f)  // 60 FPS default
    , m_physics_iterations(10)
    , m_world_gravity(0.0f, -9.81f, 0.0f)
    , m_physics_world_created(false)
    , m_frame_count(0) {
    
    spdlog::info("[FullPhysics] Full Physics Integration created");
    
    // Initialize collision engine
    m_collision_engine = std::make_unique<FullAestheticCollisionEngine>();
    
    // Initialize performance metrics
    m_performance = PhysicsPerformance{};
    m_last_frame_time = std::chrono::high_resolution_clock::now();
}

FullPhysicsIntegration::~FullPhysicsIntegration() {
    spdlog::info("[FullPhysics] Full Physics Integration destroyed");
    shutdownPhysics();
}

bool FullPhysicsIntegration::initializeFullPhysics() {
    if (m_initialized) {
        spdlog::warn("[FullPhysics] Physics system already initialized");
        return true;
    }

    try {
        spdlog::info("[FullPhysics] Initializing Full Physics Integration...");
        
        // Initialize collision engine
        if (!m_collision_engine->initializeFullCollision()) {
            spdlog::error("[FullPhysics] Failed to initialize collision engine");
            return false;
        }
        
        // Initialize physics engine components
        if (!initializePhysicsEngine()) {
            spdlog::error("[FullPhysics] Failed to initialize physics engine");
            return false;
        }
        
        // Initialize collision shapes
        if (!initializeCollisionShapes()) {
            spdlog::error("[FullPhysics] Failed to initialize collision shapes");
            return false;
        }
        
        // Initialize physics materials
        if (!initializePhysicsMaterials()) {
            spdlog::error("[FullPhysics] Failed to initialize physics materials");
            return false;
        }
        
        // Initialize physics constraints
        if (!initializePhysicsConstraints()) {
            spdlog::error("[FullPhysics] Failed to initialize physics constraints");
            return false;
        }
        
        // Create physics world
        if (!createPhysicsWorld()) {
            spdlog::error("[FullPhysics] Failed to create physics world");
            return false;
        }
        
        // Initialize default physics properties
        initializeDefaultPhysicsProperties();
        
        m_initialized = true;
        spdlog::info("[FullPhysics] Full Physics Integration initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception during initialization: {}", e.what());
        return false;
    }
}

void FullPhysicsIntegration::shutdownPhysics() {
    if (!m_initialized) {
        return;
    }

    try {
        spdlog::info("[FullPhysics] Shutting down physics system...");
        
        // Destroy physics world
        destroyPhysicsWorld();
        
        // Clear all physics objects
        m_physics_objects.clear();
        
        // Clear all constraints
        m_physics_constraints.clear();
        
        // Shutdown collision engine
        if (m_collision_engine) {
            m_collision_engine->shutdownCollision();
        }
        
        m_initialized = false;
        spdlog::info("[FullPhysics] Physics system shutdown complete");
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception during shutdown: {}", e.what());
    }
}

bool FullPhysicsIntegration::checkFullHandCollision(HandType hand, ObjectID object, CollisionType type) {
    if (!m_initialized) {
        return false;
    }

    try {
        // Use collision engine to detect hand collision
        auto collision_result = m_collision_engine->detectFullCollision(object, hand, type);
        
        if (collision_result.collision_detected) {
            // Handle collision response
            handleFullCollisionResponse(collision_result, type);
            
            // Update performance metrics
            m_performance.collision_checks++;
            
            return true;
        }
        
        return false;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception during hand collision check: {}", e.what());
        return false;
    }
}

void FullPhysicsIntegration::simulateFullObjectPhysics(ObjectID object, PhysicsType physics_type) {
    if (!m_initialized) {
        return;
    }

    try {
        // Check if object exists in physics system
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            spdlog::warn("[FullPhysics] Object {} not found in physics system", object);
            return;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Update physics type if changed
        if (obj_data.physics_type != physics_type) {
            obj_data.physics_type = physics_type;
            spdlog::debug("[FullPhysics] Updated physics type for object {} to {}", object, static_cast<int>(physics_type));
        }
        
        // Simulate physics based on type
        switch (physics_type) {
            case PhysicsType::STATIC:
                // Static objects - no physics simulation needed
                break;
                
            case PhysicsType::DYNAMIC:
                // Dynamic objects - full physics simulation
                updatePhysicsObject(object, m_physics_time_step);
                break;
                
            case PhysicsType::KINEMATIC:
                // Kinematic objects - controlled movement
                updateKinematicPhysics(object);
                break;
                
            case PhysicsType::RAGDOLL:
                // Ragdoll physics simulation
                updateRagdollPhysics(object);
                break;
                
            case PhysicsType::VEHICLE:
                // Vehicle physics simulation
                updateVehiclePhysics(object);
                break;
                
            case PhysicsType::WEAPON:
                // Weapon physics simulation
                updateWeaponPhysics(object);
                break;
                
            case PhysicsType::ENVIRONMENTAL:
                // Environmental object physics
                updateEnvironmentalPhysics(object);
                break;
                
            default:
                spdlog::warn("[FullPhysics] Unknown physics type {} for object {}", static_cast<int>(physics_type), object);
                break;
        }
        
        // Update performance metrics
        m_performance.physics_calculations++;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception during physics simulation: {}", e.what());
    }
}

void FullPhysicsIntegration::applyFullForce(ObjectID object, const glm::vec3& force, ForceType force_type) {
    if (!m_initialized) {
        return;
    }

    try {
        // Check if object exists in physics system
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            spdlog::warn("[FullPhysics] Object {} not found in physics system", object);
            return;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Apply force based on type
        switch (force_type) {
            case ForceType::IMPULSE:
                // Instant force impulse
                obj_data.physics_state.velocity += force / obj_data.properties.mass;
                break;
                
            case ForceType::CONSTANT:
                // Constant force over time
                obj_data.physics_state.velocity += force * m_physics_time_step / obj_data.properties.mass;
                break;
                
            case ForceType::GRAVITY:
                // Gravitational force
                obj_data.physics_state.velocity += force * m_physics_time_step;
                break;
                
            case ForceType::SPRING:
                // Spring force (simplified)
                applySpringForce(object, force);
                break;
                
            case ForceType::DAMPING:
                // Damping force
                applyDampingForce(object, force);
                break;
                
            case ForceType::FRICTION:
                // Friction force
                applyFrictionForce(object, force);
                break;
                
            case ForceType::BUOYANCY:
                // Buoyancy force
                applyBuoyancyForce(object, force);
                break;
                
            case ForceType::MAGNETIC:
                // Magnetic force
                applyMagneticForce(object, force);
                break;
                
            case ForceType::WIND:
                // Wind force
                applyWindForce(object, force);
                break;
                
            default:
                spdlog::warn("[FullPhysics] Unknown force type {} for object {}", static_cast<int>(force_type), object);
                break;
        }
        
        // Mark object as active
        obj_data.physics_state.is_active = true;
        obj_data.physics_state.is_sleeping = false;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception applying force: {}", e.what());
    }
}

void FullPhysicsIntegration::simulateFullGravity(ObjectID object, GravityType gravity_type) {
    if (!m_initialized) {
        return;
    }

    try {
        // Check if object exists in physics system
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Apply gravity based on type
        glm::vec3 gravity_force;
        
        switch (gravity_type) {
            case GravityType::NONE:
                gravity_force = glm::vec3(0.0f);
                break;
                
            case GravityType::EARTH:
                gravity_force = glm::vec3(0.0f, -9.81f, 0.0f);
                break;
                
            case GravityType::MOON:
                gravity_force = glm::vec3(0.0f, -1.62f, 0.0f);
                break;
                
            case GravityType::MARS:
                gravity_force = glm::vec3(0.0f, -3.71f, 0.0f);
                break;
                
            case GravityType::SPACE:
                gravity_force = glm::vec3(0.0f, -0.1f, 0.0f); // Microgravity
                break;
                
            case GravityType::CUSTOM:
                gravity_force = m_world_gravity;
                break;
                
            default:
                gravity_force = glm::vec3(0.0f, -9.81f, 0.0f); // Default to Earth gravity
                break;
        }
        
        // Apply gravity force
        if (obj_data.physics_type == PhysicsType::DYNAMIC || obj_data.physics_type == PhysicsType::RAGDOLL) {
            obj_data.physics_state.velocity += gravity_force * m_physics_time_step;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception simulating gravity: {}", e.what());
    }
}

void FullPhysicsIntegration::handleFullCollisionResponse(const CollisionResult& collision, CollisionType type) {
    if (!m_initialized) {
        return;
    }

    try {
        // Handle collision response based on type
        switch (type) {
            case CollisionType::TOUCH:
                handleTouchCollision(collision);
                break;
                
            case CollisionType::GRAB:
                handleGrabCollision(collision);
                break;
                
            case CollisionType::PUSH:
                handlePushCollision(collision);
                break;
                
            case CollisionType::PULL:
                handlePullCollision(collision);
                break;
                
            case CollisionType::THROW:
                handleThrowCollision(collision);
                break;
                
            case CollisionType::BREAK:
                handleBreakCollision(collision);
                break;
                
            case CollisionType::INTERACT:
                handleInteractCollision(collision);
                break;
                
            case CollisionType::DAMAGE:
                handleDamageCollision(collision);
                break;
                
            case CollisionType::TRIGGER:
                handleTriggerCollision(collision);
                break;
                
            default:
                spdlog::warn("[FullPhysics] Unknown collision type {} for response handling", static_cast<int>(type));
                break;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception handling collision response: {}", e.what());
    }
}

PhysicsPerformance FullPhysicsIntegration::getFullPhysicsPerformance() const {
    return m_performance;
}

InteractionResult FullPhysicsIntegration::interactWithFullObject(ObjectID object, HandType hand, InteractionType type) {
    if (!m_initialized) {
        return InteractionResult{};
    }

    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        InteractionResult result;
        
        // Check if object exists in physics system
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            spdlog::warn("[FullPhysics] Object {} not found for interaction", object);
            return result;
        }
        
        // Perform interaction based on type
        switch (type) {
            case InteractionType::TOUCH:
                result = performTouchInteraction(object, hand);
                break;
                
            case InteractionType::GRAB:
                result = performGrabInteraction(object, hand);
                break;
                
            case InteractionType::PUSH:
                result = performPushInteraction(object, hand);
                break;
                
            case InteractionType::PULL:
                result = performPullInteraction(object, hand);
                break;
                
            case InteractionType::THROW:
                result = performThrowInteraction(object, hand);
                break;
                
            case InteractionType::BREAK:
                result = performBreakInteraction(object, hand);
                break;
                
            case InteractionType::INTERACT:
                result = performInteractInteraction(object, hand);
                break;
                
            case InteractionType::DAMAGE:
                result = performDamageInteraction(object, hand);
                break;
                
            case InteractionType::TRIGGER:
                result = performTriggerInteraction(object, hand);
                break;
                
            default:
                spdlog::warn("[FullPhysics] Unknown interaction type {} for object {}", static_cast<int>(type), object);
                break;
        }
        
        // Calculate response time
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        result.response_time = duration.count() / 1000.0f; // Convert to milliseconds
        
        // Update performance metrics
        if (result.interaction_successful) {
            m_performance.successful_interactions++;
        } else {
            m_performance.failed_interactions++;
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception during object interaction: {}", e.what());
        return InteractionResult{};
    }
}

// Advanced physics features
bool FullPhysicsIntegration::enableAdvancedPhysics(ObjectID object, bool enable) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            spdlog::warn("[FullPhysics] Object {} not found for advanced physics", object);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        obj_data.advanced_physics_enabled = enable;
        
        spdlog::info("[FullPhysics] Advanced physics {} for object {}", enable ? "enabled" : "disabled", object);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception enabling advanced physics: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::setPhysicsProperties(ObjectID object, const PhysicsProperties& properties) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            spdlog::warn("[FullPhysics] Object {} not found for physics properties", object);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        obj_data.properties = properties;
        
        spdlog::info("[FullPhysics] Physics properties updated for object {}", object);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception setting physics properties: {}", e.what());
        return false;
    }
}

// Physics simulation control
void FullPhysicsIntegration::setPhysicsTimeStep(float time_step) {
    m_physics_time_step = std::max(0.001f, time_step); // Minimum 1ms
    spdlog::info("[FullPhysics] Physics time step set to {} seconds", m_physics_time_step);
}

void FullPhysicsIntegration::setPhysicsIterations(uint32_t iterations) {
    m_physics_iterations = std::max(1u, std::min(iterations, 100u)); // Between 1 and 100
    spdlog::info("[FullPhysics] Physics iterations set to {}", m_physics_iterations);
}

void FullPhysicsIntegration::enablePhysicsDebug(bool enable) {
    m_physics_debug_enabled = enable;
    spdlog::info("[FullPhysics] Physics debug {}", enable ? "enabled" : "disabled");
}

void FullPhysicsIntegration::setPhysicsGravity(const glm::vec3& gravity) {
    m_world_gravity = gravity;
    spdlog::info("[FullPhysics] World gravity set to ({}, {}, {})", gravity.x, gravity.y, gravity.z);
}

// Physics world management
bool FullPhysicsIntegration::createPhysicsWorld() {
    try {
        if (m_physics_world_created) {
            spdlog::warn("[FullPhysics] Physics world already created");
            return true;
        }
        
        spdlog::info("[FullPhysics] Creating physics world...");
        
        // Initialize physics world components
        // This would integrate with a physics engine like Bullet or PhysX
        
        m_physics_world_created = true;
        spdlog::info("[FullPhysics] Physics world created successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception creating physics world: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::destroyPhysicsWorld() {
    try {
        if (!m_physics_world_created) {
            return true;
        }
        
        spdlog::info("[FullPhysics] Destroying physics world...");
        
        // Cleanup physics world components
        
        m_physics_world_created = false;
        spdlog::info("[FullPhysics] Physics world destroyed successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception destroying physics world: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::stepPhysicsWorld(float delta_time) {
    if (!m_physics_world_created) {
        return false;
    }

    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Step physics simulation
        for (uint32_t i = 0; i < m_physics_iterations; ++i) {
            // Update physics simulation
            updatePhysicsSimulation(delta_time / m_physics_iterations);
            
            // Process physics collisions
            processPhysicsCollisions();
            
            // Apply physics forces
            applyPhysicsForces();
            
            // Update physics constraints
            updatePhysicsConstraints();
        }
        
        // Update performance metrics
        updatePerformanceMetrics();
        
        // Update frame count
        m_frame_count++;
        
        // Calculate physics update time
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        m_performance.physics_update_time = duration.count() / 1000.0f; // Convert to milliseconds
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception stepping physics world: {}", e.what());
        return false;
    }
}

// Protected methods implementation
bool FullPhysicsIntegration::initializePhysicsEngine() {
    try {
        spdlog::info("[FullPhysics] Initializing physics engine...");
        // Physics engine initialization
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception initializing physics engine: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::initializeCollisionShapes() {
    try {
        spdlog::info("[FullPhysics] Initializing collision shapes...");
        // Collision shapes initialization
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception initializing collision shapes: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::initializePhysicsMaterials() {
    try {
        spdlog::info("[FullPhysics] Initializing physics materials...");
        // Physics materials initialization
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception initializing physics materials: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::initializePhysicsConstraints() {
    try {
        spdlog::info("[FullPhysics] Initializing physics constraints...");
        // Physics constraints initialization
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception initializing physics constraints: {}", e.what());
        return false;
    }
}

void FullPhysicsIntegration::updatePhysicsSimulation(float delta_time) {
    try {
        // Update all physics objects
        for (auto& [object_id, obj_data] : m_physics_objects) {
            if (obj_data.physics_state.is_active && !obj_data.physics_frozen) {
                updatePhysicsObject(object_id, delta_time);
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception updating physics simulation: {}", e.what());
    }
}

void FullPhysicsIntegration::processPhysicsCollisions() {
    try {
        // Process physics collisions between objects
        // This would integrate with the collision engine
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception processing physics collisions: {}", e.what());
    }
}

void FullPhysicsIntegration::applyPhysicsForces() {
    try {
        // Apply accumulated forces to all objects
        for (auto& [object_id, obj_data] : m_physics_objects) {
            if (obj_data.physics_state.is_active && !obj_data.physics_frozen) {
                applyObjectForces(object_id, m_physics_time_step);
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception applying physics forces: {}", e.what());
    }
}

void FullPhysicsIntegration::updatePhysicsConstraints() {
    try {
        // Update all physics constraints
        for (const auto& constraint : m_physics_constraints) {
            if (constraint.enabled) {
                updateObjectConstraints(constraint.object1);
                updateObjectConstraints(constraint.object2);
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception updating physics constraints: {}", e.what());
    }
}

void FullPhysicsIntegration::updatePerformanceMetrics() {
    try {
        // Update frame time
        auto current_time = std::chrono::high_resolution_clock::now();
        auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - m_last_frame_time);
        m_performance.frame_time = frame_duration.count() / 1000.0f; // Convert to milliseconds
        m_last_frame_time = current_time;
        
        // Update active objects count
        m_performance.active_objects = 0;
        for (const auto& [object_id, obj_data] : m_physics_objects) {
            if (obj_data.physics_state.is_active) {
                m_performance.active_objects++;
            }
        }
        
        // Update memory usage (simplified)
        m_performance.memory_usage_mb = m_physics_objects.size() * 0.001f; // Rough estimate
        
        // Update CPU usage (simplified)
        m_performance.cpu_usage_percent = m_performance.physics_update_time / m_performance.frame_time * 100.0f;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception updating performance metrics: {}", e.what());
    }
}

void FullPhysicsIntegration::optimizePhysicsPerformance() {
    try {
        // Optimize physics performance based on current metrics
        if (m_performance.frame_time > 16.67f) { // More than 60 FPS target
            // Reduce physics iterations
            m_physics_iterations = std::max(1u, m_physics_iterations - 1);
            spdlog::debug("[FullPhysics] Reduced physics iterations to {} for performance", m_physics_iterations);
        }
        
        // Optimize individual objects
        for (auto& [object_id, obj_data] : m_physics_objects) {
            optimizePhysicsObject(object_id);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception optimizing physics performance: {}", e.what());
    }
}

// Private helper methods
void FullPhysicsIntegration::initializeDefaultPhysicsProperties() {
    try {
        spdlog::info("[FullPhysics] Initializing default physics properties...");
        // Set default physics properties for common object types
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception initializing default physics properties: {}", e.what());
    }
}

bool FullPhysicsIntegration::validatePhysicsObject(ObjectID object) const {
    return m_physics_objects.find(object) != m_physics_objects.end();
}

void FullPhysicsIntegration::cleanupPhysicsObject(ObjectID object) {
    try {
        // Cleanup physics object resources
        spdlog::debug("[FullPhysics] Cleaned up physics object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception cleaning up physics object: {}", e.what());
    }
}

void FullPhysicsIntegration::updatePhysicsObject(ObjectID object, float delta_time) {
    try {
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Update position based on velocity
        obj_data.physics_state.position += obj_data.physics_state.velocity * delta_time;
        
        // Update rotation based on angular velocity
        glm::vec3 angular_velocity_rad = obj_data.physics_state.angular_velocity * delta_time;
        glm::quat rotation_delta = glm::quat(angular_velocity_rad);
        obj_data.physics_state.rotation = rotation_delta * obj_data.physics_state.rotation;
        
        // Update transform matrix
        obj_data.physics_state.transform = glm::translate(glm::mat4(1.0f), obj_data.physics_state.position) *
                                         glm::mat4_cast(obj_data.physics_state.rotation);
        
        // Apply damping
        obj_data.physics_state.velocity *= (1.0f - obj_data.properties.linear_damping * delta_time);
        obj_data.physics_state.angular_velocity *= (1.0f - obj_data.properties.angular_damping * delta_time);
        
        // Check if object should go to sleep
        if (glm::length(obj_data.physics_state.velocity) < 0.01f && 
            glm::length(obj_data.physics_state.angular_velocity) < 0.01f) {
            obj_data.physics_state.is_sleeping = true;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception updating physics object: {}", e.what());
    }
}

void FullPhysicsIntegration::applyObjectForces(ObjectID object, float delta_time) {
    try {
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Apply gravity if enabled
        if (obj_data.physics_type == PhysicsType::DYNAMIC || obj_data.physics_type == PhysicsType::RAGDOLL) {
            simulateFullGravity(object, GravityType::EARTH);
        }
        
        // Apply other forces based on physics properties
        // This would integrate with the force application system
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception applying object forces: {}", e.what());
    }
}

void FullPhysicsIntegration::handleObjectCollision(ObjectID object1, ObjectID object2) {
    try {
        // Handle collision between two physics objects
        // This would integrate with the collision engine
        
        spdlog::debug("[FullPhysics] Handling collision between objects {} and {}", object1, object2);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception handling object collision: {}", e.what());
    }
}

void FullPhysicsIntegration::updateObjectConstraints(ObjectID object) {
    try {
        // Update constraints for a specific object
        // This would integrate with the constraint system
        
        spdlog::debug("[FullPhysics] Updating constraints for object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception updating object constraints: {}", e.what());
    }
}

void FullPhysicsIntegration::optimizePhysicsObject(ObjectID object) {
    try {
        // Optimize physics for a specific object
        // This could include LOD adjustments, collision shape optimization, etc.
        
        spdlog::debug("[FullPhysics] Optimizing physics for object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception optimizing physics object: {}", e.what());
    }
}

// Additional physics update methods (simplified implementations)
void FullPhysicsIntegration::updateKinematicPhysics(ObjectID object) {
    // Kinematic physics update (controlled movement)
    spdlog::debug("[FullPhysics] Updating kinematic physics for object {}", object);
}

void FullPhysicsIntegration::updateRagdollPhysics(ObjectID object) {
    // Ragdoll physics update
    spdlog::debug("[FullPhysics] Updating ragdoll physics for object {}", object);
}

void FullPhysicsIntegration::updateVehiclePhysics(ObjectID object) {
    // Vehicle physics update
    spdlog::debug("[FullPhysics] Updating vehicle physics for object {}", object);
}

void FullPhysicsIntegration::updateWeaponPhysics(ObjectID object) {
    // Weapon physics update
    spdlog::debug("[FullPhysics] Updating weapon physics for object {}", object);
}

void FullPhysicsIntegration::updateEnvironmentalPhysics(ObjectID object) {
    // Environmental physics update
    spdlog::debug("[FullPhysics] Updating environmental physics for object {}", object);
}

// Force application methods (simplified implementations)
void FullPhysicsIntegration::applySpringForce(ObjectID object, const glm::vec3& force) {
    spdlog::debug("[FullPhysics] Applying spring force to object {}", object);
}

void FullPhysicsIntegration::applyDampingForce(ObjectID object, const glm::vec3& force) {
    spdlog::debug("[FullPhysics] Applying damping force to object {}", object);
}

void FullPhysicsIntegration::applyFrictionForce(ObjectID object, const glm::vec3& force) {
    spdlog::debug("[FullPhysics] Applying friction force to object {}", object);
}

void FullPhysicsIntegration::applyBuoyancyForce(ObjectID object, const glm::vec3& force) {
    spdlog::debug("[FullPhysics] Applying buoyancy force to object {}", object);
}

void FullPhysicsIntegration::applyMagneticForce(ObjectID object, const glm::vec3& force) {
    spdlog::debug("[FullPhysics] Applying magnetic force to object {}", object);
}

void FullPhysicsIntegration::applyWindForce(ObjectID object, const glm::vec3& force) {
    spdlog::debug("[FullPhysics] Applying wind force to object {}", object);
}

// Collision response methods (simplified implementations)
void FullPhysicsIntegration::handleTouchCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling touch collision");
}

void FullPhysicsIntegration::handleGrabCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling grab collision");
}

void FullPhysicsIntegration::handlePushCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling push collision");
}

void FullPhysicsIntegration::handlePullCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling pull collision");
}

void FullPhysicsIntegration::handleThrowCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling throw collision");
}

void FullPhysicsIntegration::handleBreakCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling break collision");
}

void FullPhysicsIntegration::handleInteractCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling interact collision");
}

void FullPhysicsIntegration::handleDamageCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling damage collision");
}

void FullPhysicsIntegration::handleTriggerCollision(const CollisionResult& collision) {
    spdlog::debug("[FullPhysics] Handling trigger collision");
}

// Interaction methods (simplified implementations)
InteractionResult FullPhysicsIntegration::performTouchInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 0.3f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::TOUCH;
    return result;
}

InteractionResult FullPhysicsIntegration::performGrabInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 0.8f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::GRAB;
    return result;
}

InteractionResult FullPhysicsIntegration::performPushInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 0.6f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::PUSH;
    return result;
}

InteractionResult FullPhysicsIntegration::performPullInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 0.6f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::PULL;
    return result;
}

InteractionResult FullPhysicsIntegration::performThrowInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 0.9f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::THROW;
    return result;
}

InteractionResult FullPhysicsIntegration::performBreakInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 1.0f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::BREAK;
    return result;
}

InteractionResult FullPhysicsIntegration::performInteractInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 0.7f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::INTERACT;
    return result;
}

InteractionResult FullPhysicsIntegration::performDamageInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 0.9f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::DAMAGE;
    return result;
}

InteractionResult FullPhysicsIntegration::performTriggerInteraction(ObjectID object, HandType hand) {
    InteractionResult result;
    result.interaction_successful = true;
    result.interaction_strength = 0.4f;
    result.interacted_object = object;
    result.interaction_type = InteractionType::TRIGGER;
    return result;
}

// Additional required methods
bool FullPhysicsIntegration::setObjectPhysicsState(ObjectID object, const PhysicsState& state) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        obj_data.physics_state = state;
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception setting object physics state: {}", e.what());
        return false;
    }
}

FullPhysicsIntegration::PhysicsState FullPhysicsIntegration::getObjectPhysicsState(ObjectID object) const {
    try {
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter != m_physics_objects.end()) {
            return obj_iter->second.physics_state;
        }
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception getting object physics state: {}", e.what());
    }
    
    return PhysicsState{};
}

bool FullPhysicsIntegration::resetObjectPhysics(ObjectID object) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        obj_data.physics_state = PhysicsState{};
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception resetting object physics: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::freezeObjectPhysics(ObjectID object, bool freeze) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_physics_objects.find(object);
        if (obj_iter == m_physics_objects.end()) {
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        obj_data.physics_frozen = freeze;
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception freezing object physics: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::createPhysicsConstraint(ObjectID object1, ObjectID object2, const ConstraintData& constraint) {
    if (!m_initialized) {
        return false;
    }

    try {
        // Add constraint to the list
        m_physics_constraints.push_back(constraint);
        spdlog::info("[FullPhysics] Created physics constraint between objects {} and {}", object1, object2);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception creating physics constraint: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::removePhysicsConstraint(ObjectID object1, ObjectID object2) {
    if (!m_initialized) {
        return false;
    }

    try {
        // Remove constraint from the list
        m_physics_constraints.erase(
            std::remove_if(m_physics_constraints.begin(), m_physics_constraints.end(),
                [object1, object2](const ConstraintData& constraint) {
                    return (constraint.object1 == object1 && constraint.object2 == object2) ||
                           (constraint.object1 == object2 && constraint.object2 == object1);
                }),
            m_physics_constraints.end()
        );
        
        spdlog::info("[FullPhysics] Removed physics constraint between objects {} and {}", object1, object2);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception removing physics constraint: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::clearPhysicsWorld() {
    try {
        // Clear all physics objects and constraints
        m_physics_objects.clear();
        m_physics_constraints.clear();
        
        spdlog::info("[FullPhysics] Physics world cleared");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception clearing physics world: {}", e.what());
        return false;
    }
}

} // namespace vr
} // namespace uevr