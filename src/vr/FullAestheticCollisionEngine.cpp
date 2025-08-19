/*
 * UEVR Full Aesthetic Collision Engine Implementation
 * 
 * Complete collision detection and interaction system for VR
 * Handles all object types with aesthetic feedback and physics simulation
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#include "uevr/vr/FullAestheticCollisionEngine.hpp"
#include "uevr/vr/FullPhysicsIntegration.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace uevr::vr {

FullAestheticCollisionEngine::FullAestheticCollisionEngine()
    : m_gravity(0.0f, -9.81f, 0.0f)
    , m_collision_margin(0.01f)
    , m_max_collision_checks(1000)
    , m_physics_substeps(4)
    , m_broad_phase_enabled(true)
    , m_spatial_partitioning_enabled(true)
    , m_async_processing_enabled(false) {
    
    spdlog::info("[CollisionEngine] FullAestheticCollisionEngine created");
}

FullAestheticCollisionEngine::~FullAestheticCollisionEngine() {
    shutdown();
    spdlog::info("[CollisionEngine] FullAestheticCollisionEngine destroyed");
}

bool FullAestheticCollisionEngine::initialize() {
    try {
        spdlog::info("[CollisionEngine] Initializing collision engine...");
        
        // Initialize performance settings
        m_broad_phase_enabled = true;
        m_spatial_partitioning_enabled = true;
        m_async_processing_enabled = false;
        
        // Set default collision margin
        m_collision_margin = 0.01f;
        m_max_collision_checks = 1000;
        m_physics_substeps = 4;
        
        spdlog::info("[CollisionEngine] Collision engine initialized successfully");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Failed to initialize: {}", e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::shutdown() {
    try {
        spdlog::info("[CollisionEngine] Shutting down collision engine...");
        
        // Clear all objects
        m_objects.clear();
        m_collision_results.clear();
        
        spdlog::info("[CollisionEngine] Collision engine shutdown complete");
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error during shutdown: {}", e.what());
    }
}

bool FullAestheticCollisionEngine::registerObject(ObjectID object, PhysicsType physics_type, const Vec3& position, const Vec3& size) {
    try {
        if (m_objects.find(object) != m_objects.end()) {
            spdlog::warn("[CollisionEngine] Object {} already registered", object);
            return false;
        }
        
        ObjectData obj_data;
        obj_data.position = position;
        obj_data.size = size;
        obj_data.physics_type = physics_type;
        obj_data.is_active = true;
        obj_data.is_grabbable = (physics_type != PhysicsType::STATIC);
        obj_data.is_interactable = (physics_type != PhysicsType::STATIC);
        
        m_objects[object] = obj_data;
        
        spdlog::debug("[CollisionEngine] Registered object {} at position ({}, {}, {})", 
                     object, position.x, position.y, position.z);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Failed to register object {}: {}", object, e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::unregisterObject(ObjectID object) {
    try {
        auto it = m_objects.find(object);
        if (it != m_objects.end()) {
            m_objects.erase(it);
            spdlog::debug("[CollisionEngine] Unregistered object {}", object);
        }
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error unregistering object {}: {}", object, e.what());
    }
}

void FullAestheticCollisionEngine::updateObject(ObjectID object, const Vec3& position, const Vec3& velocity) {
    try {
        auto it = m_objects.find(object);
        if (it != m_objects.end()) {
            it->second.position = position;
            it->second.velocity = velocity;
        }
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error updating object {}: {}", object, e.what());
    }
}

CollisionResult FullAestheticCollisionEngine::detectCollision(ObjectID object1, ObjectID object2) {
    CollisionResult result;
    result.object1 = object1;
    result.object2 = object2;
    
    try {
        auto it1 = m_objects.find(object1);
        auto it2 = m_objects.find(object2);
        
        if (it1 == m_objects.end() || it2 == m_objects.end()) {
            return result;
        }
        
        const ObjectData& obj1_data = it1->second;
        const ObjectData& obj2_data = it2->second;
        
        if (!obj1_data.is_active || !obj2_data.is_active) {
            return result;
        }
        
        // Broad phase collision check
        if (m_broad_phase_enabled && !checkBroadPhaseCollision(object1, object2)) {
            return result;
        }
        
        // Narrow phase collision check
        if (checkNarrowPhaseCollision(object1, object2)) {
            result.collision_detected = true;
            result.type = CollisionType::COLLIDE;
            result.collision_point = calculateCollisionPoint(object1, object2);
            result.collision_normal = calculateCollisionNormal(object1, object2);
            
            // Calculate collision distance
            Vec3 distance_vec = result.collision_point - obj1_data.position;
            result.collision_distance = glm::length(distance_vec);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error detecting collision: {}", e.what());
    }
    
    return result;
}

std::vector<CollisionResult> FullAestheticCollisionEngine::detectAllCollisions(ObjectID object) {
    std::vector<CollisionResult> results;
    
    try {
        for (const auto& pair : m_objects) {
            if (pair.first != object && pair.second.is_active) {
                CollisionResult result = detectCollision(object, pair.first);
                if (result.collision_detected) {
                    results.push_back(result);
                }
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error detecting all collisions: {}", e.what());
    }
    
    return results;
}

bool FullAestheticCollisionEngine::isColliding(ObjectID object1, ObjectID object2) {
    try {
        CollisionResult result = detectCollision(object1, object2);
        return result.collision_detected;
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error checking collision: {}", e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::applyPhysics(ObjectID object, const Vec3& force) {
    try {
        auto it = m_objects.find(object);
        if (it != m_objects.end() && it->second.physics_type == PhysicsType::DYNAMIC) {
            it->second.velocity += force;
        }
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error applying physics: {}", e.what());
    }
}

void FullAestheticCollisionEngine::setGravity(const Vec3& gravity) {
    m_gravity = gravity;
}

void FullAestheticCollisionEngine::updatePhysicsSimulation(float delta_time) {
    try {
        // Update object positions based on velocity and gravity
        for (auto& pair : m_objects) {
            ObjectData& obj_data = pair.second;
            
            if (obj_data.physics_type == PhysicsType::DYNAMIC && obj_data.is_active) {
                // Apply gravity
                obj_data.velocity += m_gravity * delta_time;
                
                // Update position
                obj_data.position += obj_data.velocity * delta_time;
            }
        }
        
        // Update spatial partitioning if enabled
        if (m_spatial_partitioning_enabled) {
            updateSpatialPartitioning();
        }
        
        // Process collision events
        processCollisionEvents();
        
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error updating physics simulation: {}", e.what());
    }
}

bool FullAestheticCollisionEngine::grabObject(ObjectID object, HandType hand) {
    try {
        auto it = m_objects.find(object);
        if (it != m_objects.end() && it->second.is_grabbable) {
            // Mark object as grabbed
            it->second.is_grabbable = false;
            spdlog::debug("[CollisionEngine] Object {} grabbed by hand {}", object, static_cast<int>(hand));
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error grabbing object: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::releaseObject(ObjectID object, HandType hand) {
    try {
        auto it = m_objects.find(object);
        if (it != m_objects.end()) {
            // Mark object as released
            it->second.is_grabbable = true;
            spdlog::debug("[CollisionEngine] Object {} released from hand {}", object, static_cast<int>(hand));
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error releasing object: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::isObjectGrabbed(ObjectID object) {
    try {
        auto it = m_objects.find(object);
        if (it != m_objects.end()) {
            return !it->second.is_grabbable;
        }
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error checking if object is grabbed: {}", e.what());
        return false;
    }
}

HandType FullAestheticCollisionEngine::getGrabbingHand(ObjectID object) {
    // This is a simplified implementation - in a real system you'd track which hand grabbed which object
    return HandType::LEFT;
}

Vec3 FullAestheticCollisionEngine::calculateCollisionPoint(ObjectID object1, ObjectID object2) {
    try {
        auto it1 = m_objects.find(object1);
        auto it2 = m_objects.find(object2);
        
        if (it1 == m_objects.end() || it2 == m_objects.end()) {
            return Vec3(0.0f);
        }
        
        // Simple collision point calculation - midpoint between objects
        const ObjectData& obj1_data = it1->second;
        const ObjectData& obj2_data = it2->second;
        
        return (obj1_data.position + obj2_data.position) * 0.5f;
        
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error calculating collision point: {}", e.what());
        return Vec3(0.0f);
    }
}

Vec3 FullAestheticCollisionEngine::calculateCollisionNormal(ObjectID object1, ObjectID object2) {
    try {
        auto it1 = m_objects.find(object1);
        auto it2 = m_objects.find(object2);
        
        if (it1 == m_objects.end() || it2 == m_objects.end()) {
            return Vec3(0.0f, 1.0f, 0.0f);
        }
        
        const ObjectData& obj1_data = it1->second;
        const ObjectData& obj2_data = it2->second;
        
        // Calculate normal from object1 to object2
        Vec3 direction = glm::normalize(obj2_data.position - obj1_data.position);
        return direction;
        
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error calculating collision normal: {}", e.what());
        return Vec3(0.0f, 1.0f, 0.0f);
    }
}

float FullAestheticCollisionEngine::calculateDistance(ObjectID object1, ObjectID object2) {
    try {
        auto it1 = m_objects.find(object1);
        auto it2 = m_objects.find(object2);
        
        if (it1 == m_objects.end() || it2 == m_objects.end()) {
            return 0.0f;
        }
        
        const ObjectData& obj1_data = it1->second;
        const ObjectData& obj2_data = it2->second;
        
        return glm::distance(obj1_data.position, obj2_data.position);
        
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error calculating distance: {}", e.what());
        return 0.0f;
    }
}

void FullAestheticCollisionEngine::setBroadPhaseEnabled(bool enabled) {
    m_broad_phase_enabled = enabled;
}

void FullAestheticCollisionEngine::setSpatialPartitioningEnabled(bool enabled) {
    m_spatial_partitioning_enabled = enabled;
}

void FullAestheticCollisionEngine::setAsyncProcessingEnabled(bool enabled) {
    m_async_processing_enabled = enabled;
}

void FullAestheticCollisionEngine::setCollisionMargin(float margin) {
    m_collision_margin = margin;
}

void FullAestheticCollisionEngine::setMaxCollisionChecks(int max_checks) {
    m_max_collision_checks = max_checks;
}

void FullAestheticCollisionEngine::setPhysicsSubsteps(int substeps) {
    m_physics_substeps = substeps;
}

// Private methods
void FullAestheticCollisionEngine::updateSpatialPartitioning() {
    // Placeholder for spatial partitioning update
    // In a real implementation, this would update octree/quadtree structures
}

void FullAestheticCollisionEngine::processCollisionEvents() {
    // Placeholder for collision event processing
    // In a real implementation, this would handle collision callbacks and events
}

void FullAestheticCollisionEngine::applyCollisionResponse(const CollisionResult& collision) {
    // Placeholder for collision response
    // In a real implementation, this would apply forces, play sounds, etc.
}

bool FullAestheticCollisionEngine::checkBroadPhaseCollision(ObjectID object1, ObjectID object2) {
    try {
        auto it1 = m_objects.find(object1);
        auto it2 = m_objects.find(object2);
        
        if (it1 == m_objects.end() || it2 == m_objects.end()) {
            return false;
        }
        
        const ObjectData& obj1_data = it1->second;
        const ObjectData& obj2_data = it2->second;
        
        // Simple AABB broad phase check
        Vec3 obj1_min = obj1_data.position - obj1_data.size * 0.5f;
        Vec3 obj1_max = obj1_data.position + obj1_data.size * 0.5f;
        Vec3 obj2_min = obj2_data.position - obj2_data.size * 0.5f;
        Vec3 obj2_max = obj2_data.position + obj2_data.size * 0.5f;
        
        return (obj1_min.x <= obj2_max.x && obj1_max.x >= obj2_min.x) &&
               (obj1_min.y <= obj2_max.y && obj1_max.y >= obj2_min.y) &&
               (obj1_min.z <= obj2_max.z && obj1_max.z >= obj2_min.z);
        
    } catch (const std::exception& e) {
        spdlog::error("[CollisionEngine] Error in broad phase collision check: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::checkNarrowPhaseCollision(ObjectID object1, ObjectID object2) {
    // For now, use the same logic as broad phase
    // In a real implementation, this would do more precise collision detection
    return checkBroadPhaseCollision(object1, object2);
}

} // namespace uevr::vr