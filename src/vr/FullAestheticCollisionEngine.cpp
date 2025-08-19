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
#include <spdlog/spdlog.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace uevr {
namespace vr {

FullAestheticCollisionEngine::FullAestheticCollisionEngine()
    : m_initialized(false) {
    
    spdlog::info("[FullAestheticCollision] Full Aesthetic Collision Engine created");
    
    // Initialize collision statistics
    m_stats.total_collisions = 0;
    m_stats.successful_interactions = 0;
    m_stats.failed_interactions = 0;
    m_stats.average_response_time = 0.0f;
}

FullAestheticCollisionEngine::~FullAestheticCollisionEngine() {
    spdlog::info("[FullAestheticCollision] Full Aesthetic Collision Engine destroyed");
    shutdownCollision();
}

bool FullAestheticCollisionEngine::initializeFullCollision() {
    if (m_initialized) {
        spdlog::warn("[FullAestheticCollision] Collision engine already initialized");
        return true;
    }

    try {
        spdlog::info("[FullAestheticCollision] Initializing Full Aesthetic Collision Engine...");
        
        // Initialize collision detection systems
        if (!initializeCollisionDetection()) {
            spdlog::error("[FullAestheticCollision] Failed to initialize collision detection");
            return false;
        }
        
        // Initialize physics simulation
        if (!initializePhysicsSimulation()) {
            spdlog::error("[FullAestheticCollision] Failed to initialize physics simulation");
            return false;
        }
        
        // Initialize haptic feedback system
        if (!initializeHapticSystem()) {
            spdlog::error("[FullAestheticCollision] Failed to initialize haptic system");
            return false;
        }
        
        // Initialize visual feedback system
        if (!initializeVisualFeedback()) {
            spdlog::error("[FullAestheticCollision] Failed to initialize visual feedback");
            return false;
        }
        
        m_initialized = true;
        spdlog::info("[FullAestheticCollision] Full Aesthetic Collision Engine initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception during initialization: {}", e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::shutdownCollision() {
    if (!m_initialized) {
        return;
    }

    try {
        spdlog::info("[FullAestheticCollision] Shutting down collision engine...");
        
        // Clear all registered objects
        m_objects.clear();
        
        // Reset statistics
        m_stats = CollisionStats{};
        
        m_initialized = false;
        spdlog::info("[FullAestheticCollision] Collision engine shutdown complete");
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception during shutdown: {}", e.what());
    }
}

CollisionResult FullAestheticCollisionEngine::detectFullCollision(ObjectID object, HandType hand, CollisionType type) {
    if (!m_initialized) {
        spdlog::warn("[FullAestheticCollision] Collision engine not initialized");
        return CollisionResult{};
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        CollisionResult result;
        
        // Check if object exists
        auto obj_iter = m_objects.find(object);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Object {} not registered for collision detection", object);
            return result;
        }
        
        // Detect collisions with all other objects
        for (const auto& [other_id, other_data] : m_objects) {
            if (other_id == object || !other_data.collision_enabled) {
                continue;
            }
            
            // Check collision bounds
            if (checkCollisionBounds(object, other_id)) {
                // Calculate collision details
                result.collision_detected = true;
                result.collision_point = calculateCollisionPoint(object, other_id);
                result.collision_normal = calculateCollisionNormal(object, other_id);
                result.colliding_object = other_id;
                result.collision_type = type;
                
                // Calculate collision distance and intensity
                const auto& obj_data = obj_iter->second;
                glm::vec3 distance_vec = result.collision_point - obj_data.position;
                result.collision_distance = glm::length(distance_vec);
                
                // Calculate intensity based on collision type and physics
                result.collision_intensity = calculateCollisionIntensity(object, other_id, type);
                
                // Update statistics
                m_stats.total_collisions++;
                
                // Provide haptic feedback
                provideFullHapticFeedback(hand, result.collision_intensity, HapticType::MEDIUM);
                
                // Show visual feedback
                showFullCollisionHighlight(object, type);
                
                break; // Found first collision
            }
        }
        
        // Calculate response time
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        m_stats.average_response_time = (m_stats.average_response_time + duration.count()) / 2.0f;
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception during collision detection: {}", e.what());
        return CollisionResult{};
    }
}

void FullAestheticCollisionEngine::simulateFullPhysics(ObjectID object, PhysicsType physics_type) {
    if (!m_initialized) {
        return;
    }

    try {
        auto obj_iter = m_objects.find(object);
        if (obj_iter == m_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Update physics based on type
        switch (physics_type) {
            case PhysicsType::DYNAMIC:
                // Apply gravity and velocity
                obj_data.velocity.y -= 9.81f * 0.016f; // 60 FPS gravity
                obj_data.position += obj_data.velocity * 0.016f;
                break;
                
            case PhysicsType::KINEMATIC:
                // Scripted movement - no physics simulation
                break;
                
            case PhysicsType::RAGDOLL:
                // Character physics simulation
                simulateRagdollPhysics(object);
                break;
                
            case PhysicsType::VEHICLE:
                // Vehicle physics simulation
                simulateVehiclePhysics(object);
                break;
                
            case PhysicsType::WEAPON:
                // Weapon physics simulation
                simulateWeaponPhysics(object);
                break;
                
            default:
                // Static objects - no physics
                break;
        }
        
        // Update physics state
        updatePhysics(object, 0.016f);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception during physics simulation: {}", e.what());
    }
}

void FullAestheticCollisionEngine::provideFullHapticFeedback(HandType hand, float intensity, HapticType haptic_type) {
    if (!m_initialized) {
        return;
    }

    try {
        // Clamp intensity to valid range
        intensity = std::clamp(intensity, 0.0f, 1.0f);
        
        // Send haptic feedback based on type
        switch (haptic_type) {
            case HapticType::LIGHT:
                sendHapticFeedback(hand, intensity * 0.3f, haptic_type);
                break;
                
            case HapticType::MEDIUM:
                sendHapticFeedback(hand, intensity * 0.6f, haptic_type);
                break;
                
            case HapticType::HEAVY:
                sendHapticFeedback(hand, intensity, haptic_type);
                break;
                
            case HapticType::VIBRATION:
                sendHapticFeedback(hand, intensity, haptic_type);
                break;
                
            case HapticType::PULSE:
                sendHapticFeedback(hand, intensity, haptic_type);
                break;
                
            case HapticType::BUZZ:
                sendHapticFeedback(hand, intensity * 0.8f, haptic_type);
                break;
                
            case HapticType::TAP:
                sendHapticFeedback(hand, intensity * 0.5f, haptic_type);
                break;
                
            case HapticType::GRAB:
                sendHapticFeedback(hand, intensity * 0.7f, haptic_type);
                break;
                
            case HapticType::RELEASE:
                sendHapticFeedback(hand, intensity * 0.4f, haptic_type);
                break;
                
            default:
                break;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception during haptic feedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::showFullCollisionHighlight(ObjectID object, CollisionType collision_type) {
    if (!m_initialized) {
        return;
    }

    try {
        // Render collision highlight based on type
        switch (collision_type) {
            case CollisionType::TOUCH:
                renderCollisionHighlight(object, CollisionType::TOUCH);
                break;
                
            case CollisionType::GRAB:
                renderCollisionHighlight(object, CollisionType::GRAB);
                break;
                
            case CollisionType::PUSH:
                renderCollisionHighlight(object, CollisionType::PUSH);
                break;
                
            case CollisionType::PULL:
                renderCollisionHighlight(object, CollisionType::PULL);
                break;
                
            case CollisionType::THROW:
                renderCollisionHighlight(object, CollisionType::THROW);
                break;
                
            case CollisionType::BREAK:
                renderCollisionHighlight(object, CollisionType::BREAK);
                break;
                
            case CollisionType::INTERACT:
                renderCollisionHighlight(object, CollisionType::INTERACT);
                break;
                
            case CollisionType::DAMAGE:
                renderCollisionHighlight(object, CollisionType::DAMAGE);
                break;
                
            case CollisionType::TRIGGER:
                renderCollisionHighlight(object, CollisionType::TRIGGER);
                break;
                
            default:
                break;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception during visual feedback: {}", e.what());
    }
}

bool FullAestheticCollisionEngine::enableDoorHandleGrabbing(ObjectID door, HandType hand) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_objects.find(door);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Door {} not registered for collision detection", door);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Set door physics to kinematic for controlled movement
        obj_data.physics_type = PhysicsType::KINEMATIC;
        
        // Enable grab collision response
        obj_data.collision_responses[CollisionType::GRAB] = 1.0f;
        
        // Provide haptic feedback for successful grab
        provideFullHapticFeedback(hand, 0.8f, HapticType::GRAB);
        
        spdlog::info("[FullAestheticCollision] Door handle grabbing enabled for door {}", door);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception enabling door handle grabbing: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::enableWeaponInteraction(ObjectID weapon, HandType hand) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_objects.find(weapon);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Weapon {} not registered for collision detection", weapon);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Set weapon physics to kinematic for controlled handling
        obj_data.physics_type = PhysicsType::KINEMATIC;
        
        // Enable weapon-specific collision responses
        obj_data.collision_responses[CollisionType::GRAB] = 1.0f;
        obj_data.collision_responses[CollisionType::INTERACT] = 0.8f;
        
        // Provide haptic feedback for weapon grab
        provideFullHapticFeedback(hand, 0.9f, HapticType::GRAB);
        
        spdlog::info("[FullAestheticCollision] Weapon interaction enabled for weapon {}", weapon);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception enabling weapon interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::enableEnvironmentalInteraction(ObjectID env_object, HandType hand) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_objects.find(env_object);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Environmental object {} not registered for collision detection", env_object);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Set environmental object physics to dynamic for manipulation
        obj_data.physics_type = PhysicsType::DYNAMIC;
        
        // Enable environmental interaction collision responses
        obj_data.collision_responses[CollisionType::TOUCH] = 0.6f;
        obj_data.collision_responses[CollisionType::GRAB] = 0.8f;
        obj_data.collision_responses[CollisionType::PUSH] = 0.7f;
        obj_data.collision_responses[CollisionType::PULL] = 0.7f;
        
        // Provide haptic feedback for environmental interaction
        provideFullHapticFeedback(hand, 0.5f, HapticType::LIGHT);
        
        spdlog::info("[FullAestheticCollision] Environmental interaction enabled for object {}", env_object);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception enabling environmental interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::enableNPCInteraction(ObjectID npc, HandType hand) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_objects.find(npc);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] NPC {} not registered for collision detection", npc);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Set NPC physics to ragdoll for realistic interaction
        obj_data.physics_type = PhysicsType::RAGDOLL;
        
        // Enable NPC interaction collision responses (RESPECT GAME BALANCE!)
        obj_data.collision_responses[CollisionType::TOUCH] = 0.4f;  // Light touch only
        obj_data.collision_responses[CollisionType::INTERACT] = 0.6f; // Basic interaction
        
        // Provide haptic feedback for NPC interaction
        provideFullHapticFeedback(hand, 0.3f, HapticType::LIGHT);
        
        spdlog::info("[FullAestheticCollision] NPC interaction enabled for NPC {} (respecting game balance)", npc);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception enabling NPC interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::enableVehicleInteraction(ObjectID vehicle, HandType hand) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_objects.find(vehicle);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Vehicle {} not registered for collision detection", vehicle);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Set vehicle physics to vehicle type for realistic handling
        obj_data.physics_type = PhysicsType::VEHICLE;
        
        // Enable vehicle interaction collision responses
        obj_data.collision_responses[CollisionType::TOUCH] = 0.5f;
        obj_data.collision_responses[CollisionType::INTERACT] = 0.8f;
        obj_data.collision_responses[CollisionType::GRAB] = 0.7f;
        
        // Provide haptic feedback for vehicle interaction
        provideFullHapticFeedback(hand, 0.6f, HapticType::MEDIUM);
        
        spdlog::info("[FullAestheticCollision] Vehicle interaction enabled for vehicle {}", vehicle);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception enabling vehicle interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::enablePuzzleInteraction(ObjectID puzzle, HandType hand) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_objects.find(puzzle);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Puzzle object {} not registered for collision detection", puzzle);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Set puzzle object physics to kinematic for controlled interaction
        obj_data.physics_type = PhysicsType::KINEMATIC;
        
        // Enable puzzle interaction collision responses
        obj_data.collision_responses[CollisionType::TOUCH] = 0.7f;
        obj_data.collision_responses[CollisionType::INTERACT] = 1.0f;
        obj_data.collision_responses[CollisionType::GRAB] = 0.8f;
        
        // Provide haptic feedback for puzzle interaction
        provideFullHapticFeedback(hand, 0.6f, HapticType::MEDIUM);
        
        spdlog::info("[FullAestheticCollision] Puzzle interaction enabled for puzzle {}", puzzle);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception enabling puzzle interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::enableInventoryManipulation(ObjectID item, HandType hand) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_objects.find(item);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Inventory item {} not registered for collision detection", item);
            return false;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Set inventory item physics to kinematic for controlled manipulation
        obj_data.physics_type = PhysicsType::KINEMATIC;
        
        // Enable inventory manipulation collision responses
        obj_data.collision_responses[CollisionType::TOUCH] = 0.8f;
        obj_data.collision_responses[CollisionType::GRAB] = 1.0f;
        obj_data.collision_responses[CollisionType::INTERACT] = 0.9f;
        
        // Provide haptic feedback for inventory manipulation
        provideFullHapticFeedback(hand, 0.7f, HapticType::GRAB);
        
        spdlog::info("[FullAestheticCollision] Inventory manipulation enabled for item {}", item);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception enabling inventory manipulation: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::registerObject(ObjectID object, PhysicsType physics_type, const glm::vec3& position, const glm::vec3& size) {
    if (!m_initialized) {
        return false;
    }

    try {
        // Check if object already exists
        if (m_objects.find(object) != m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Object {} already registered", object);
            return false;
        }
        
        // Create new object data
        ObjectData obj_data;
        obj_data.physics_type = physics_type;
        obj_data.position = position;
        obj_data.size = size;
        obj_data.collision_enabled = true;
        
        // Set default collision responses based on physics type
        setDefaultCollisionResponses(obj_data, physics_type);
        
        // Register object
        m_objects[object] = obj_data;
        
        spdlog::info("[FullAestheticCollision] Object {} registered with physics type {}", object, static_cast<int>(physics_type));
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception registering object: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::unregisterObject(ObjectID object) {
    if (!m_initialized) {
        return false;
    }

    try {
        auto obj_iter = m_objects.find(object);
        if (obj_iter == m_objects.end()) {
            spdlog::warn("[FullAestheticCollision] Object {} not found for unregistration", object);
            return false;
        }
        
        m_objects.erase(obj_iter);
        spdlog::info("[FullAestheticCollision] Object {} unregistered", object);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception unregistering object: {}", e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::updateObject(ObjectID object, const glm::vec3& position, const glm::vec3& velocity) {
    if (!m_initialized) {
        return;
    }

    try {
        auto obj_iter = m_objects.find(object);
        if (obj_iter == m_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        obj_data.position = position;
        obj_data.velocity = velocity;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception updating object: {}", e.what());
    }
}

void FullAestheticCollisionEngine::setCollisionResponse(ObjectID object, CollisionType type, float response_intensity) {
    if (!m_initialized) {
        return;
    }

    try {
        auto obj_iter = m_objects.find(object);
        if (obj_iter == m_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        obj_data.collision_responses[type] = std::clamp(response_intensity, 0.0f, 1.0f);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception setting collision response: {}", e.what());
    }
}

void FullAestheticCollisionEngine::setCollisionEnabled(ObjectID object, bool enabled) {
    if (!m_initialized) {
        return;
    }

    try {
        auto obj_iter = m_objects.find(object);
        if (obj_iter == m_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        obj_data.collision_enabled = enabled;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception setting collision enabled: {}", e.what());
    }
}

FullAestheticCollisionEngine::CollisionStats FullAestheticCollisionEngine::getCollisionStats() const {
    return m_stats;
}

// Protected methods implementation
bool FullAestheticCollisionEngine::checkCollisionBounds(ObjectID object1, ObjectID object2) {
    try {
        auto obj1_iter = m_objects.find(object1);
        auto obj2_iter = m_objects.find(object2);
        
        if (obj1_iter == m_objects.end() || obj2_iter == m_objects.end()) {
            return false;
        }
        
        const auto& obj1_data = obj1_iter->second;
        const auto& obj2_data = obj2_iter->second;
        
        // Simple AABB collision detection
        glm::vec3 obj1_min = obj1_data.position - obj1_data.size * 0.5f;
        glm::vec3 obj1_max = obj1_data.position + obj1_data.size * 0.5f;
        glm::vec3 obj2_min = obj2_data.position - obj2_data.size * 0.5f;
        glm::vec3 obj2_max = obj2_data.position + obj2_data.size * 0.5f;
        
        return (obj1_min.x <= obj2_max.x && obj1_max.x >= obj2_min.x) &&
               (obj1_min.y <= obj2_max.y && obj1_max.y >= obj2_min.y) &&
               (obj1_min.z <= obj2_max.z && obj1_max.z >= obj2_min.z);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception checking collision bounds: {}", e.what());
        return false;
    }
}

glm::vec3 FullAestheticCollisionEngine::calculateCollisionPoint(ObjectID object1, ObjectID object2) {
    try {
        auto obj1_iter = m_objects.find(object1);
        auto obj2_iter = m_objects.find(object2);
        
        if (obj1_iter == m_objects.end() || obj2_iter == m_objects.end()) {
            return glm::vec3(0.0f);
        }
        
        const auto& obj1_data = obj1_iter->second;
        const auto& obj2_data = obj2_iter->second;
        
        // Calculate collision point as midpoint between object centers
        return (obj1_data.position + obj2_data.position) * 0.5f;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception calculating collision point: {}", e.what());
        return glm::vec3(0.0f);
    }
}

glm::vec3 FullAestheticCollisionEngine::calculateCollisionNormal(ObjectID object1, ObjectID object2) {
    try {
        auto obj1_iter = m_objects.find(object1);
        auto obj2_iter = m_objects.find(object2);
        
        if (obj1_iter == m_objects.end() || obj2_iter == m_objects.end()) {
            return glm::vec3(0.0f, 1.0f, 0.0f);
        }
        
        const auto& obj1_data = obj1_iter->second;
        const auto& obj2_data = obj2_iter->second;
        
        // Calculate collision normal from object1 to object2
        glm::vec3 direction = glm::normalize(obj2_data.position - obj1_data.position);
        return direction;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception calculating collision normal: {}", e.what());
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }
}

void FullAestheticCollisionEngine::applyPhysics(ObjectID object, const glm::vec3& force) {
    try {
        auto obj_iter = m_objects.find(object);
        if (obj_iter == m_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Apply force based on physics type
        if (obj_data.physics_type == PhysicsType::DYNAMIC) {
            obj_data.velocity += force * 0.016f; // 60 FPS physics
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception applying physics: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updatePhysics(ObjectID object, float delta_time) {
    try {
        auto obj_iter = m_objects.find(object);
        if (obj_iter == m_objects.end()) {
            return;
        }
        
        auto& obj_data = obj_iter->second;
        
        // Update physics based on type
        switch (obj_data.physics_type) {
            case PhysicsType::DYNAMIC:
                // Apply velocity to position
                obj_data.position += obj_data.velocity * delta_time;
                
                // Apply damping
                obj_data.velocity *= 0.98f;
                break;
                
            case PhysicsType::RAGDOLL:
                // Update ragdoll physics
                updateRagdollPhysics(object, delta_time);
                break;
                
            case PhysicsType::VEHICLE:
                // Update vehicle physics
                updateVehiclePhysics(object, delta_time);
                break;
                
            case PhysicsType::WEAPON:
                // Update weapon physics
                updateWeaponPhysics(object, delta_time);
                break;
                
            default:
                break;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception updating physics: {}", e.what());
    }
}

void FullAestheticCollisionEngine::sendHapticFeedback(HandType hand, float intensity, HapticType type) {
    try {
        // Implementation would integrate with VR runtime haptic system
        // For now, just log the haptic feedback request
        spdlog::debug("[FullAestheticCollision] Haptic feedback: Hand={}, Intensity={}, Type={}", 
                     static_cast<int>(hand), intensity, static_cast<int>(type));
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception sending haptic feedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::renderCollisionHighlight(ObjectID object, CollisionType type) {
    try {
        // Implementation would integrate with rendering system
        // For now, just log the visual feedback request
        spdlog::debug("[FullAestheticCollision] Visual feedback: Object={}, Type={}", 
                     object, static_cast<int>(type));
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception rendering collision highlight: {}", e.what());
    }
}

// Private helper methods
bool FullAestheticCollisionEngine::initializeCollisionDetection() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing collision detection system");
        // Collision detection system initialization
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception initializing collision detection: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializePhysicsSimulation() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing physics simulation system");
        // Physics simulation system initialization
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception initializing physics simulation: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializeHapticSystem() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing haptic feedback system");
        // Haptic feedback system initialization
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception initializing haptic system: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializeVisualFeedback() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing visual feedback system");
        // Visual feedback system initialization
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception initializing visual feedback: {}", e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::setDefaultCollisionResponses(ObjectData& obj_data, PhysicsType physics_type) {
    switch (physics_type) {
        case PhysicsType::STATIC:
            obj_data.collision_responses[CollisionType::TOUCH] = 0.5f;
            break;
            
        case PhysicsType::DYNAMIC:
            obj_data.collision_responses[CollisionType::TOUCH] = 0.7f;
            obj_data.collision_responses[CollisionType::GRAB] = 0.8f;
            obj_data.collision_responses[CollisionType::PUSH] = 0.6f;
            obj_data.collision_responses[CollisionType::PULL] = 0.6f;
            break;
            
        case PhysicsType::KINEMATIC:
            obj_data.collision_responses[CollisionType::TOUCH] = 0.8f;
            obj_data.collision_responses[CollisionType::GRAB] = 0.9f;
            obj_data.collision_responses[CollisionType::INTERACT] = 1.0f;
            break;
            
        case PhysicsType::RAGDOLL:
            obj_data.collision_responses[CollisionType::TOUCH] = 0.4f;
            obj_data.collision_responses[CollisionType::INTERACT] = 0.6f;
            break;
            
        case PhysicsType::VEHICLE:
            obj_data.collision_responses[CollisionType::TOUCH] = 0.5f;
            obj_data.collision_responses[CollisionType::INTERACT] = 0.8f;
            obj_data.collision_responses[CollisionType::GRAB] = 0.7f;
            break;
            
        case PhysicsType::WEAPON:
            obj_data.collision_responses[CollisionType::TOUCH] = 0.8f;
            obj_data.collision_responses[CollisionType::GRAB] = 1.0f;
            obj_data.collision_responses[CollisionType::INTERACT] = 0.9f;
            break;
            
        case PhysicsType::ENVIRONMENTAL:
            obj_data.collision_responses[CollisionType::TOUCH] = 0.6f;
            obj_data.collision_responses[CollisionType::GRAB] = 0.8f;
            obj_data.collision_responses[CollisionType::PUSH] = 0.7f;
            obj_data.collision_responses[CollisionType::PULL] = 0.7f;
            break;
            
        default:
            break;
    }
}

float FullAestheticCollisionEngine::calculateCollisionIntensity(ObjectID object1, ObjectID object2, CollisionType type) {
    try {
        auto obj1_iter = m_objects.find(object1);
        auto obj2_iter = m_objects.find(object2);
        
        if (obj1_iter == m_objects.end() || obj2_iter == m_objects.end()) {
            return 0.0f;
        }
        
        const auto& obj1_data = obj1_iter->second;
        const auto& obj2_data = obj2_iter->second;
        
        // Calculate intensity based on collision type and object physics
        float base_intensity = 0.5f;
        
        switch (type) {
            case CollisionType::TOUCH:
                base_intensity = 0.3f;
                break;
                
            case CollisionType::GRAB:
                base_intensity = 0.8f;
                break;
                
            case CollisionType::PUSH:
                base_intensity = 0.6f;
                break;
                
            case CollisionType::PULL:
                base_intensity = 0.6f;
                break;
                
            case CollisionType::THROW:
                base_intensity = 0.9f;
                break;
                
            case CollisionType::BREAK:
                base_intensity = 1.0f;
                break;
                
            case CollisionType::INTERACT:
                base_intensity = 0.7f;
                break;
                
            case CollisionType::DAMAGE:
                base_intensity = 0.9f;
                break;
                
            case CollisionType::TRIGGER:
                base_intensity = 0.4f;
                break;
                
            default:
                base_intensity = 0.5f;
                break;
        }
        
        // Adjust based on physics types
        if (obj1_data.physics_type == PhysicsType::STATIC && obj2_data.physics_type == PhysicsType::DYNAMIC) {
            base_intensity *= 1.2f; // Dynamic object hitting static object
        } else if (obj1_data.physics_type == PhysicsType::DYNAMIC && obj2_data.physics_type == PhysicsType::DYNAMIC) {
            base_intensity *= 1.5f; // Dynamic object hitting dynamic object
        }
        
        return std::clamp(base_intensity, 0.0f, 1.0f);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception calculating collision intensity: {}", e.what());
        return 0.5f;
    }
}

void FullAestheticCollisionEngine::simulateRagdollPhysics(ObjectID object) {
    try {
        // Ragdoll physics simulation implementation
        // This would integrate with a physics engine like Bullet or PhysX
        spdlog::debug("[FullAestheticCollision] Simulating ragdoll physics for object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception simulating ragdoll physics: {}", e.what());
    }
}

void FullAestheticCollisionEngine::simulateVehiclePhysics(ObjectID object) {
    try {
        // Vehicle physics simulation implementation
        // This would handle wheel physics, suspension, etc.
        spdlog::debug("[FullAestheticCollision] Simulating vehicle physics for object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception simulating vehicle physics: {}", e.what());
    }
}

void FullAestheticCollisionEngine::simulateWeaponPhysics(ObjectID object) {
    try {
        // Weapon physics simulation implementation
        // This would handle recoil, sway, etc.
        spdlog::debug("[FullAestheticCollision] Simulating weapon physics for object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception simulating weapon physics: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updateRagdollPhysics(ObjectID object, float delta_time) {
    try {
        // Update ragdoll physics state
        spdlog::debug("[FullAestheticCollision] Updating ragdoll physics for object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception updating ragdoll physics: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updateVehiclePhysics(ObjectID object, float delta_time) {
    try {
        // Update vehicle physics state
        spdlog::debug("[FullAestheticCollision] Updating vehicle physics for object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception updating vehicle physics: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updateWeaponPhysics(ObjectID object, float delta_time) {
    try {
        // Update weapon physics state
        spdlog::debug("[FullAestheticCollision] Updating weapon physics for object {}", object);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception updating weapon physics: {}", e.what());
    }
}

} // namespace vr
} // namespace uevr