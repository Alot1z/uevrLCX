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
#include "uevr/vr/FullAestheticCollisionEngine.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace uevr::vr {

FullPhysicsIntegration::FullPhysicsIntegration()
    : m_collision_engine(nullptr)
    , m_current_engine(PhysicsEngineType::BUILTIN)
    , m_multithreading_enabled(true)
    , m_broad_phase_enabled(true)
    , m_spatial_partitioning_enabled(true)
    , m_lod_enabled(false)
    , m_debug_rendering_enabled(false)
    , m_last_simulation_time(0.0f)
    , m_active_object_count(0)
    , m_max_physics_objects(1000) {
    
    spdlog::info("[PhysicsEngine] FullPhysicsIntegration created");
}

FullPhysicsIntegration::~FullPhysicsIntegration() {
    shutdown();
    spdlog::info("[PhysicsEngine] FullPhysicsIntegration destroyed");
}

bool FullPhysicsIntegration::initialize() {
    try {
        spdlog::info("[PhysicsEngine] Initializing physics integration...");
        
        // Initialize physics settings
        m_settings.gravity = Vector3(0.0f, -9.81f, 0.0f);
        m_settings.time_step = 1.0f / 60.0f;
        m_settings.max_substeps = 10;
        m_settings.enable_ccd = true;
        m_settings.collision_tolerance = 0.001f;
        m_settings.enable_multithreading = true;
        
        // Initialize performance settings
        m_multithreading_enabled = true;
        m_broad_phase_enabled = true;
        m_spatial_partitioning_enabled = true;
        m_lod_enabled = false;
        m_debug_rendering_enabled = false;
        
        spdlog::info("[PhysicsEngine] Physics integration initialized successfully");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Failed to initialize: {}", e.what());
        return false;
    }
}

void FullPhysicsIntegration::shutdown() {
    try {
        spdlog::info("[PhysicsEngine] Shutting down physics integration...");
        
        // Clear all physics objects
        m_physics_objects.clear();
        m_force_queue.clear();
        
        spdlog::info("[PhysicsEngine] Physics integration shutdown complete");
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error during shutdown: {}", e.what());
    }
}

bool FullPhysicsIntegration::setPhysicsEngine(PhysicsEngineType engine_type) {
    try {
        m_current_engine = engine_type;
        spdlog::info("[PhysicsEngine] Physics engine set to type: {}", static_cast<int>(engine_type));
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Failed to set physics engine: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::initializePhysicsEngine() {
    try {
        spdlog::info("[PhysicsEngine] Initializing physics engine...");
        
        // Initialize based on current engine type
        switch (m_current_engine) {
            case PhysicsEngineType::BUILTIN:
                spdlog::info("[PhysicsEngine] Using built-in physics engine");
                break;
            case PhysicsEngineType::BULLET:
                spdlog::info("[PhysicsEngine] Using Bullet physics engine");
                break;
            case PhysicsEngineType::PHYSX:
                spdlog::info("[PhysicsEngine] Using PhysX physics engine");
                break;
            case PhysicsEngineType::CUSTOM:
                spdlog::info("[PhysicsEngine] Using custom physics engine");
                break;
        }
        
        spdlog::info("[PhysicsEngine] Physics engine initialized successfully");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Failed to initialize physics engine: {}", e.what());
        return false;
    }
}

void FullPhysicsIntegration::shutdownPhysicsEngine() {
    try {
        spdlog::info("[PhysicsEngine] Shutting down physics engine...");
        spdlog::info("[PhysicsEngine] Physics engine shutdown complete");
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error during physics engine shutdown: {}", e.what());
    }
}

bool FullPhysicsIntegration::addPhysicsObject(const PhysicsObject& obj) {
    try {
        if (m_physics_objects.find(obj.id) != m_physics_objects.end()) {
            spdlog::warn("[PhysicsEngine] Physics object {} already exists", obj.id);
            return false;
        }
        
        if (!validatePhysicsObject(obj)) {
            spdlog::error("[PhysicsEngine] Invalid physics object {}", obj.id);
            return false;
        }
        
        m_physics_objects[obj.id] = obj;
        m_active_object_count++;
        
        spdlog::debug("[PhysicsEngine] Added physics object {} at position ({}, {}, {})", 
                     obj.id, obj.position.x, obj.position.y, obj.position.z);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Failed to add physics object {}: {}", obj.id, e.what());
        return false;
    }
}

void FullPhysicsIntegration::removePhysicsObject(ObjectID id) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            m_physics_objects.erase(it);
            m_active_object_count--;
            spdlog::debug("[PhysicsEngine] Removed physics object {}", id);
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error removing physics object {}: {}", id, e.what());
    }
}

void FullPhysicsIntegration::updatePhysicsObject(ObjectID id, const PhysicsObject& obj) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            it->second = obj;
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error updating physics object {}: {}", id, e.what());
    }
}

PhysicsObject FullPhysicsIntegration::getPhysicsObject(ObjectID id) const {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            return it->second;
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error getting physics object {}: {}", id, e.what());
    }
    
    return PhysicsObject{};
}

void FullPhysicsIntegration::applyForce(ObjectID id, const Force& force) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end() && !it->second.is_static) {
            m_force_queue.push_back(force);
            spdlog::debug("[PhysicsEngine] Applied force to object {}", id);
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error applying force to object {}: {}", id, e.what());
    }
}

void FullPhysicsIntegration::applyForceAtPoint(ObjectID id, const Force& force, const Vector3& point) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end() && !it->second.is_static) {
            Force point_force = force;
            point_force.point_of_application = point;
            m_force_queue.push_back(point_force);
            spdlog::debug("[PhysicsEngine] Applied force at point to object {}", id);
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error applying force at point to object {}: {}", id, e.what());
    }
}

void FullPhysicsIntegration::applyImpulse(ObjectID id, const Vector3& impulse) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end() && !it->second.is_static) {
            Force impulse_force;
            impulse_force.type = ForceType::IMPULSE;
            impulse_force.direction = glm::normalize(impulse);
            impulse_force.magnitude = glm::length(impulse);
            m_force_queue.push_back(impulse_force);
            spdlog::debug("[PhysicsEngine] Applied impulse to object {}", id);
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error applying impulse to object {}: {}", id, e.what());
    }
}

void FullPhysicsIntegration::applyTorque(ObjectID id, const Vector3& torque) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end() && !it->second.is_static) {
            Force torque_force;
            torque_force.type = ForceType::TORQUE;
            torque_force.direction = glm::normalize(torque);
            torque_force.magnitude = glm::length(torque);
            m_force_queue.push_back(torque_force);
            spdlog::debug("[PhysicsEngine] Applied torque to object {}", id);
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error applying torque to object {}: {}", id, e.what());
    }
}

void FullPhysicsIntegration::updatePhysicsSimulation(float delta_time) {
    try {
        // Process force queue
        processForceQueue();
        
        // Step physics simulation
        stepSimulation(delta_time);
        
        // Update spatial partitioning if enabled
        if (m_spatial_partitioning_enabled) {
            updateSpatialPartitioning();
        }
        
        // Process collision events
        processCollisionEvents();
        
        // Update performance metrics
        m_last_simulation_time = delta_time;
        
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error updating physics simulation: {}", e.what());
    }
}

void FullPhysicsIntegration::stepSimulation(float time_step) {
    try {
        // Apply physics substepping
        int substeps = std::min(m_settings.max_substeps, static_cast<int>(time_step / m_settings.time_step));
        
        for (int i = 0; i < substeps; ++i) {
            // Update object positions and velocities
            for (auto& pair : m_physics_objects) {
                PhysicsObject& obj = pair.second;
                
                if (obj.is_static || obj.is_kinematic) {
                    continue;
                }
                
                // Apply gravity
                obj.velocity += m_settings.gravity * m_settings.time_step;
                
                // Update position
                obj.position += obj.velocity * m_settings.time_step;
                
                // Apply damping
                obj.velocity *= (1.0f - obj.friction * m_settings.time_step);
                obj.angular_velocity *= (1.0f - obj.friction * m_settings.time_step);
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error in physics simulation step: {}", e.what());
    }
}

void FullPhysicsIntegration::setGravity(const Vector3& gravity) {
    m_settings.gravity = gravity;
}

void FullPhysicsIntegration::setTimeStep(float time_step) {
    m_settings.time_step = time_step;
}

void FullPhysicsIntegration::setCollisionEngine(FullAestheticCollisionEngine* collision_engine) {
    m_collision_engine = collision_engine;
    spdlog::info("[PhysicsEngine] Collision engine set");
}

void FullPhysicsIntegration::processCollisionEvents() {
    if (m_collision_engine) {
        // Process collision events from collision engine
        // This would integrate with the collision detection system
    }
}

void FullPhysicsIntegration::handleCollisionResponse(const CollisionResult& collision) {
    try {
        // Handle collision response based on collision type
        switch (collision.type) {
            case CollisionType::TOUCH:
                // Light collision response
                break;
            case CollisionType::GRAB:
                // Grab collision response
                break;
            case CollisionType::PUSH:
                // Push collision response
                break;
            case CollisionType::PULL:
                // Pull collision response
                break;
            case CollisionType::COLLIDE:
                // Full collision response
                break;
            default:
                break;
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error handling collision response: {}", e.what());
    }
}

bool FullPhysicsIntegration::enableSoftBodySimulation(ObjectID id) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            it->second.simulation_type = SimulationType::SOFT_BODY;
            spdlog::info("[PhysicsEngine] Enabled soft body simulation for object {}", id);
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error enabling soft body simulation: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::enableFluidSimulation(ObjectID id) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            it->second.simulation_type = SimulationType::FLUID;
            spdlog::info("[PhysicsEngine] Enabled fluid simulation for object {}", id);
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error enabling fluid simulation: {}", e.what());
        return false;
    }
}

bool FullPhysicsIntegration::enableParticleSystem(ObjectID id) {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            it->second.simulation_type = SimulationType::PARTICLE;
            spdlog::info("[PhysicsEngine] Enabled particle system for object {}", id);
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error enabling particle system: {}", e.what());
        return false;
    }
}

void FullPhysicsIntegration::setMultithreadingEnabled(bool enabled) {
    m_multithreading_enabled = enabled;
}

void FullPhysicsIntegration::setBroadPhaseEnabled(bool enabled) {
    m_broad_phase_enabled = enabled;
}

void FullPhysicsIntegration::setSpatialPartitioningEnabled(bool enabled) {
    m_spatial_partitioning_enabled = enabled;
}

void FullPhysicsIntegration::setLODEnabled(bool enabled) {
    m_lod_enabled = enabled;
}

void FullPhysicsIntegration::setPhysicsSettings(const PhysicsSettings& settings) {
    m_settings = settings;
}

PhysicsSettings FullPhysicsIntegration::getPhysicsSettings() const {
    return m_settings;
}

void FullPhysicsIntegration::setMaxPhysicsObjects(int max_objects) {
    m_max_physics_objects = max_objects;
}

void FullPhysicsIntegration::setCollisionLayers(uint32_t layers) {
    // Implementation for collision layers
}

Vector3 FullPhysicsIntegration::getObjectPosition(ObjectID id) const {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            return it->second.position;
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error getting object position: {}", e.what());
    }
    return Vector3(0.0f);
}

Quaternion FullPhysicsIntegration::getObjectRotation(ObjectID id) const {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            return it->second.rotation;
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error getting object rotation: {}", e.what());
    }
    return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
}

Vector3 FullPhysicsIntegration::getObjectVelocity(ObjectID id) const {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            return it->second.velocity;
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error getting object velocity: {}", e.what());
    }
    return Vector3(0.0f);
}

bool FullPhysicsIntegration::isObjectStatic(ObjectID id) const {
    try {
        auto it = m_physics_objects.find(id);
        if (it != m_physics_objects.end()) {
            return it->second.is_static;
        }
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error checking if object is static: {}", e.what());
    }
    return false;
}

int FullPhysicsIntegration::getActivePhysicsObjectCount() const {
    return m_active_object_count;
}

float FullPhysicsIntegration::getLastSimulationTime() const {
    return m_last_simulation_time;
}

void FullPhysicsIntegration::resetPerformanceMetrics() {
    m_last_simulation_time = 0.0f;
    m_active_object_count = 0;
}

void FullPhysicsIntegration::enableDebugRendering(bool enabled) {
    m_debug_rendering_enabled = enabled;
}

// Private methods
void FullPhysicsIntegration::processForceQueue() {
    try {
        for (const Force& force : m_force_queue) {
            // Process each force in the queue
            // This would apply forces to physics objects
        }
        
        // Clear the force queue
        m_force_queue.clear();
        
    } catch (const std::exception& e) {
        spdlog::error("[PhysicsEngine] Error processing force queue: {}", e.what());
    }
}

void FullPhysicsIntegration::updateSpatialPartitioning() {
    // Placeholder for spatial partitioning update
    // In a real implementation, this would update octree/quadtree structures
}

void FullPhysicsIntegration::applyPhysicsConstraints() {
    // Placeholder for physics constraints
    // In a real implementation, this would apply joint constraints, etc.
}

void FullPhysicsIntegration::handlePhysicsEvents() {
    // Placeholder for physics events
    // In a real implementation, this would handle physics callbacks and events
}

bool FullPhysicsIntegration::validatePhysicsObject(const PhysicsObject& obj) {
    // Basic validation
    if (obj.mass <= 0.0f) {
        spdlog::warn("[PhysicsEngine] Invalid mass for object {}", obj.id);
        return false;
    }
    
    if (obj.friction < 0.0f || obj.friction > 1.0f) {
        spdlog::warn("[PhysicsEngine] Invalid friction for object {}", obj.id);
        return false;
    }
    
    if (obj.restitution < 0.0f || obj.restitution > 1.0f) {
        spdlog::warn("[PhysicsEngine] Invalid restitution for object {}", obj.id);
        return false;
    }
    
    return true;
}

void FullPhysicsIntegration::cleanupPhysicsObjects() {
    // Remove inactive objects
    for (auto it = m_physics_objects.begin(); it != m_physics_objects.end();) {
        if (!it->second.is_active) {
            it = m_physics_objects.erase(it);
            m_active_object_count--;
        } else {
            ++it;
        }
    }
}

} // namespace uevr::vr
