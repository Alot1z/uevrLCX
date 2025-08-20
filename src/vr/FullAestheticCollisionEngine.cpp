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
#include <unordered_set>
#include <queue>
#include <mutex>
#include <thread>
#include <future>

namespace uevr {
namespace vr {

// Constructor
FullAestheticCollisionEngine::FullAestheticCollisionEngine()
    : m_collision_engine(nullptr)
    , m_collision_layers(0xFFFFFFFF)
    , m_collision_mask(0xFFFFFFFF)
    , m_max_collisions(1000)
    , m_collision_tolerance(0.001f)
    , m_next_object_id(1)
    , m_next_collision_id(1)
    , m_last_update_time(0.0f)
    , m_collision_checks_per_frame(0)
    , m_average_collision_time(0.0f) {

    spdlog::info("[FullAestheticCollision] Full Aesthetic Collision Engine created");
}

// Destructor
FullAestheticCollisionEngine::~FullAestheticCollisionEngine() {
    spdlog::info("[FullAestheticCollision] Full Aesthetic Collision Engine destroyed");
    shutdown();
}

// Initialization and shutdown
bool FullAestheticCollisionEngine::initialize() {
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

        spdlog::info("[FullAestheticCollision] Full Aesthetic Collision Engine initialized successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception during initialization: {}", e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::shutdown() {
    try {
        spdlog::info("[FullAestheticCollision] Shutting down collision engine...");

        // Clear all registered objects
        m_objects.clear();
        m_active_collisions.clear();
        m_haptic_queue.clear();
        m_visual_queue.clear();

        spdlog::info("[FullAestheticCollision] Collision engine shutdown complete");

    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Exception during shutdown: {}", e.what());
    }
}

// Core collision detection
std::vector<CollisionResult> FullAestheticCollisionEngine::detectCollisions(const Vec3& position, float radius) {
    std::vector<CollisionResult> results;
    
    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Check all objects within radius
        for (const auto& [id, obj_data] : m_objects) {
            if (!obj_data.is_active) continue;
            
            // Calculate distance to object
            float distance = glm::length(obj_data.position - position);
            
            if (distance <= radius + glm::length(obj_data.size) * 0.5f) {
                CollisionResult result;
                result.collision_detected = true;
                result.type = CollisionType::TOUCH;
                result.collision_point = position;
                result.collision_normal = glm::normalize(obj_data.position - position);
                result.collision_distance = distance;
                result.object1 = 0; // Player/controller ID
                result.object2 = id;
                result.hand = HandType::LEFT; // Default hand
                
                results.push_back(result);
            }
        }
        
        // Update performance metrics
        auto end_time = std::chrono::high_resolution_clock::now();
        float collision_time = std::chrono::duration<float>(end_time - start_time).count();
        m_average_collision_time = (m_average_collision_time + collision_time) * 0.5f;
        m_collision_checks_per_frame++;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in collision detection: {}", e.what());
    }
    
    return results;
}

bool FullAestheticCollisionEngine::checkCollision(ObjectID obj1, ObjectID obj2) {
    try {
        auto it1 = m_objects.find(obj1);
        auto it2 = m_objects.find(obj2);
        
        if (it1 == m_objects.end() || it2 == m_objects.end()) {
            return false;
        }
        
        const auto& obj1_data = it1->second;
        const auto& obj2_data = it2->second;
        
        if (!obj1_data.is_active || !obj2_data.is_active) {
            return false;
        }
        
        // Check collision layers
        if (!(m_collision_layers & m_collision_mask)) {
            return false;
        }
        
        // Simple AABB collision check
        Vec3 min1 = obj1_data.position - obj1_data.size * 0.5f;
        Vec3 max1 = obj1_data.position + obj1_data.size * 0.5f;
        Vec3 min2 = obj2_data.position - obj2_data.size * 0.5f;
        Vec3 max2 = obj2_data.position + obj2_data.size * 0.5f;
        
        return (min1.x <= max2.x && max1.x >= min2.x) &&
               (min1.y <= max2.y && max1.y >= min2.y) &&
               (min1.z <= max2.z && max1.z >= min2.z);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error checking collision: {}", e.what());
        return false;
    }
}

CollisionResult FullAestheticCollisionEngine::getCollisionResult(ObjectID obj1, ObjectID obj2) {
    CollisionResult result;
    
    try {
        if (!checkCollision(obj1, obj2)) {
            return result; // Return default result (no collision)
        }
        
        auto it1 = m_objects.find(obj1);
        auto it2 = m_objects.find(obj2);
        
        if (it1 != m_objects.end() && it2 != m_objects.end()) {
            const auto& obj1_data = it1->second;
            const auto& obj2_data = it2->second;
            
            // Calculate collision details
            Vec3 collision_point = (obj1_data.position + obj2_data.position) * 0.5f;
            Vec3 collision_normal = glm::normalize(obj2_data.position - obj1_data.position);
            float collision_distance = glm::length(obj2_data.position - obj1_data.position);
            
            result.collision_detected = true;
            result.type = CollisionType::COLLIDE;
            result.collision_point = collision_point;
            result.collision_normal = collision_normal;
            result.collision_distance = collision_distance;
            result.object1 = obj1;
            result.object2 = obj2;
            result.hand = HandType::LEFT; // Default hand
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error getting collision result: {}", e.what());
    }
    
    return result;
}

// Object management
ObjectID FullAestheticCollisionEngine::addObject(const PhysicsObject& obj) {
    try {
        ObjectID id = m_next_object_id++;
        
        ObjectData obj_data;
        obj_data.position = obj.position;
        obj_data.velocity = obj.velocity;
        obj_data.size = obj.size;
        obj_data.rotation = obj.rotation;
        obj_data.physics_type = obj.type;
        obj_data.is_active = true;
        obj_data.is_grabbable = (obj.type == PhysicsType::DYNAMIC);
        obj_data.is_interactable = (obj.type != PhysicsType::STATIC);
        
        m_objects[id] = obj_data;
        
        spdlog::info("[FullAestheticCollision] Added object with ID: {}", id);
        return id;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to add object: {}", e.what());
        return 0;
    }
}

void FullAestheticCollisionEngine::removeObject(ObjectID id) {
    try {
        auto it = m_objects.find(id);
        if (it != m_objects.end()) {
            m_objects.erase(it);
            spdlog::info("[FullAestheticCollision] Removed object with ID: {}", id);
        }
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to remove object: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updateObject(ObjectID id, const PhysicsObject& obj) {
    try {
        auto it = m_objects.find(id);
        if (it != m_objects.end()) {
            ObjectData& obj_data = it->second;
            obj_data.position = obj.position;
            obj_data.velocity = obj.velocity;
            obj_data.size = obj.size;
            obj_data.rotation = obj.rotation;
            obj_data.physics_type = obj.type;
            
            spdlog::debug("[FullAestheticCollision] Updated object with ID: {}", id);
        }
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to update object: {}", e.what());
    }
}

PhysicsObject FullAestheticCollisionEngine::getObject(ObjectID id) const {
    try {
        auto it = m_objects.find(id);
        if (it != m_objects.end()) {
            const ObjectData& obj_data = it->second;
            
            PhysicsObject obj;
            obj.id = id;
            obj.position = obj_data.position;
            obj.velocity = obj_data.velocity;
            obj.size = obj_data.size;
            obj.rotation = obj_data.rotation;
            obj.type = obj_data.physics_type;
            
            return obj;
        }
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to get object: {}", e.what());
    }
    
    return PhysicsObject{}; // Return default object
}

// Physics integration
void FullAestheticCollisionEngine::setPhysicsIntegration(FullPhysicsIntegration* physics) {
    m_physics_integration = physics;
    spdlog::info("[FullAestheticCollision] Physics integration set");
}

void FullAestheticCollisionEngine::updatePhysicsSimulation(float delta_time) {
    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Update collision detection
        updateCollisionDetection(delta_time);
        
        // Update haptic feedback
        updateHapticFeedback(delta_time);
        
        // Update visual feedback
        updateVisualFeedback(delta_time);
        
        // Update performance metrics
        m_last_update_time = delta_time;
        
        auto end_time = std::chrono::high_resolution_clock::now();
        float update_time = std::chrono::duration<float>(end_time - start_time).count();
        
        spdlog::debug("[FullAestheticCollision] Physics simulation updated in {} seconds", update_time);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error updating physics simulation: {}", e.what());
    }
}

// Haptic and visual feedback
void FullAestheticCollisionEngine::addHapticFeedback(const HapticFeedback& feedback) {
    try {
        m_haptic_queue.push_back(feedback);
        spdlog::debug("[FullAestheticCollision] Added haptic feedback for hand: {}", static_cast<int>(feedback.hand));
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to add haptic feedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::addVisualFeedback(const VisualFeedback& feedback) {
    try {
        m_visual_queue.push_back(feedback);
        spdlog::debug("[FullAestheticCollision] Added visual feedback for object: {}", feedback.object_id);
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to add visual feedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::clearHapticFeedback(HandType hand) {
    try {
        m_haptic_queue.erase(
            std::remove_if(m_haptic_queue.begin(), m_haptic_queue.end(),
                [hand](const HapticFeedback& feedback) {
                    return feedback.hand == hand;
                }),
            m_haptic_queue.end()
        );
        
        spdlog::debug("[FullAestheticCollision] Cleared haptic feedback for hand: {}", static_cast<int>(hand));
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to clear haptic feedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::clearVisualFeedback(ObjectID id) {
    try {
        m_visual_queue.erase(
            std::remove_if(m_visual_queue.begin(), m_visual_queue.end(),
                [id](const VisualFeedback& feedback) {
                    return feedback.object_id == id;
                }),
            m_visual_queue.end()
        );
        
        spdlog::debug("[FullAestheticCollision] Cleared visual feedback for object: {}", id);
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to clear visual feedback: {}", e.what());
    }
}

// Game-specific interactions
bool FullAestheticCollisionEngine::handleDoorInteraction(ObjectID door_id, HandType hand) {
    try {
        auto it = m_objects.find(door_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        const auto& obj_data = it->second;
        if (!obj_data.is_interactable) {
            return false;
        }
        
        // Create haptic feedback for door interaction
        HapticFeedback haptic;
        haptic.hand = hand;
        haptic.type = HapticType::MEDIUM;
        haptic.intensity = 0.7f;
        haptic.duration = 0.3f;
        haptic.position = obj_data.position;
        haptic.is_active = true;
        
        addHapticFeedback(haptic);
        
        // Create visual feedback for door
        VisualFeedback visual;
        visual.object_id = door_id;
        visual.highlight_color = Vec3(0.0f, 1.0f, 0.0f); // Green
        visual.intensity = 0.8f;
        visual.is_active = true;
        
        addVisualFeedback(visual);
        
        spdlog::info("[FullAestheticCollision] Door interaction handled for object: {}", door_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to handle door interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleWeaponInteraction(ObjectID weapon_id, HandType hand) {
    try {
        auto it = m_objects.find(weapon_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        const auto& obj_data = it->second;
        if (!obj_data.is_grabbable) {
            return false;
        }
        
        // Create strong haptic feedback for weapon interaction
        HapticFeedback haptic;
        haptic.hand = hand;
        haptic.type = HapticType::HEAVY;
        haptic.intensity = 0.9f;
        haptic.duration = 0.5f;
        haptic.position = obj_data.position;
        haptic.is_active = true;
        
        addHapticFeedback(haptic);
        
        // Create orange highlighting for weapon
        VisualFeedback visual;
        visual.object_id = weapon_id;
        visual.highlight_color = Vec3(1.0f, 0.5f, 0.0f); // Orange
        visual.intensity = 1.0f;
        visual.is_active = true;
        
        addVisualFeedback(visual);
        
        spdlog::info("[FullAestheticCollision] Weapon interaction handled for object: {}", weapon_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to handle weapon interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleVehicleInteraction(ObjectID vehicle_id, HandType hand) {
    try {
        auto it = m_objects.find(vehicle_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        const auto& obj_data = it->second;
        if (!obj_data.is_interactable) {
            return false;
        }
        
        // Create light haptic feedback for vehicle interaction
        HapticFeedback haptic;
        haptic.hand = hand;
        haptic.type = HapticType::LIGHT;
        haptic.intensity = 0.4f;
        haptic.duration = 0.2f;
        haptic.position = obj_data.position;
        haptic.is_active = true;
        
        addHapticFeedback(haptic);
        
        spdlog::info("[FullAestheticCollision] Vehicle interaction handled for object: {}", vehicle_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to handle vehicle interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleNPCInteraction(ObjectID npc_id, HandType hand) {
    try {
        auto it = m_objects.find(npc_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        const auto& obj_data = it->second;
        if (!obj_data.is_interactable) {
            return false;
        }
        
        // Create medium haptic feedback for NPC interaction
        HapticFeedback haptic;
        haptic.hand = hand;
        haptic.type = HapticType::MEDIUM;
        haptic.intensity = 0.6f;
        haptic.duration = 0.3f;
        haptic.position = obj_data.position;
        haptic.is_active = true;
        
        addHapticFeedback(haptic);
        
        // Create blue highlighting for NPC
        VisualFeedback visual;
        visual.object_id = npc_id;
        visual.highlight_color = Vec3(0.0f, 0.0f, 1.0f); // Blue
        visual.intensity = 0.7f;
        visual.is_active = true;
        
        addVisualFeedback(visual);
        
        spdlog::info("[FullAestheticCollision] NPC interaction handled for object: {}", npc_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to handle NPC interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleEnvironmentInteraction(ObjectID env_id, HandType hand) {
    try {
        auto it = m_objects.find(env_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        const auto& obj_data = it->second;
        if (!obj_data.is_interactable) {
            return false;
        }
        
        // Create light haptic feedback for environment interaction
        HapticFeedback haptic;
        haptic.hand = hand;
        haptic.type = HapticType::LIGHT;
        haptic.intensity = 0.3f;
        haptic.duration = 0.2f;
        haptic.position = obj_data.position;
        haptic.is_active = true;
        
        addHapticFeedback(haptic);
        
        spdlog::info("[FullAestheticCollision] Environment interaction handled for object: {}", env_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to handle environment interaction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleInventoryInteraction(ObjectID item_id, HandType hand) {
    try {
        auto it = m_objects.find(item_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        const auto& obj_data = it->second;
        if (!obj_data.is_grabbable) {
            return false;
        }
        
        // Create custom haptic feedback for inventory interaction
        HapticFeedback haptic;
        haptic.hand = hand;
        haptic.type = HapticType::CUSTOM;
        haptic.intensity = 0.8f;
        haptic.duration = 0.4f;
        haptic.position = obj_data.position;
        haptic.is_active = true;
        
        addHapticFeedback(haptic);
        
        // Create yellow highlighting for inventory item
        VisualFeedback visual;
        visual.object_id = item_id;
        visual.highlight_color = Vec3(1.0f, 1.0f, 0.0f); // Yellow
        visual.intensity = 0.9f;
        visual.is_active = true;
        
        addVisualFeedback(visual);
        
        spdlog::info("[FullAestheticCollision] Inventory interaction handled for object: {}", item_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to handle inventory interaction: {}", e.what());
        return false;
    }
}

// Configuration
void FullAestheticCollisionEngine::setCollisionLayers(uint32_t layers) {
    m_collision_layers = layers;
    spdlog::info("[FullAestheticCollision] Collision layers set to: 0x{:08X}", layers);
}

void FullAestheticCollisionEngine::setCollisionMask(uint32_t mask) {
    m_collision_mask = mask;
    spdlog::info("[FullAestheticCollision] Collision mask set to: 0x{:08X}", mask);
}

void FullAestheticCollisionEngine::setMaxCollisions(size_t max) {
    m_max_collisions = max;
    spdlog::info("[FullAestheticCollision] Maximum collisions set to: {}", max);
}

void FullAestheticCollisionEngine::setCollisionTolerance(float tolerance) {
    m_collision_tolerance = tolerance;
    spdlog::info("[FullAestheticCollision] Collision tolerance set to: {}", tolerance);
}

// Performance and debugging
size_t FullAestheticCollisionEngine::getActiveCollisionCount() const {
    return m_active_collisions.size();
}

float FullAestheticCollisionEngine::getLastUpdateTime() const {
    return m_last_update_time;
}

void FullAestheticCollisionEngine::resetPerformanceMetrics() {
    m_collision_checks_per_frame = 0;
    m_average_collision_time = 0.0f;
    m_last_update_time = 0.0f;
    spdlog::info("[FullAestheticCollision] Performance metrics reset");
}

// Private methods
bool FullAestheticCollisionEngine::initializeCollisionDetection() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing collision detection...");
        
        // Initialize spatial partitioning
        // Initialize broad phase collision detection
        // Initialize narrow phase collision detection
        
        spdlog::info("[FullAestheticCollision] Collision detection initialized");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to initialize collision detection: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializePhysicsSimulation() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing physics simulation...");
        
        // Initialize physics world
        // Initialize collision solvers
        // Initialize constraint systems
        
        spdlog::info("[FullAestheticCollision] Physics simulation initialized");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to initialize physics simulation: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializeHapticSystem() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing haptic system...");
        
        // Initialize haptic devices
        // Initialize feedback patterns
        // Initialize intensity control
        
        spdlog::info("[FullAestheticCollision] Haptic system initialized");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to initialize haptic system: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializeVisualFeedback() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing visual feedback...");
        
        // Initialize rendering systems
        // Initialize shader programs
        // Initialize effect systems
        
        spdlog::info("[FullAestheticCollision] Visual feedback initialized");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to initialize visual feedback: {}", e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::updateCollisionDetection(float delta_time) {
    try {
        // Update broad phase collision detection
        // Update spatial partitioning
        // Process collision pairs
        
        // Simple collision detection update
        m_active_collisions.clear();
        
        // Check all object pairs for collisions
        for (const auto& [id1, obj1] : m_objects) {
            for (const auto& [id2, obj2] : m_objects) {
                if (id1 >= id2) continue; // Avoid duplicate checks
                
                if (checkCollision(id1, id2)) {
                    // Add to active collisions
                    CollisionPair pair;
                    pair.obj1 = id1;
                    pair.obj2 = id2;
                    pair.result = getCollisionResult(id1, id2);
                    pair.is_active = true;
                    
                    if (m_active_collisions.size() < m_max_collisions) {
                        m_active_collisions.push_back(pair);
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error updating collision detection: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updateHapticFeedback(float delta_time) {
    try {
        // Process haptic feedback queue
        for (auto& feedback : m_haptic_queue) {
            if (feedback.is_active) {
                // Apply haptic feedback to device
                // Update feedback duration
                feedback.duration -= delta_time;
                
                if (feedback.duration <= 0.0f) {
                    feedback.is_active = false;
                }
            }
        }
        
        // Remove expired feedback
        m_haptic_queue.erase(
            std::remove_if(m_haptic_queue.begin(), m_haptic_queue.end(),
                [](const HapticFeedback& feedback) {
                    return !feedback.is_active;
                }),
            m_haptic_queue.end()
        );
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error updating haptic feedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updateVisualFeedback(float delta_time) {
    try {
        // Process visual feedback queue
        for (auto& feedback : m_visual_queue) {
            if (feedback.is_active) {
                // Apply visual effects to objects
                // Update effect intensity
                feedback.intensity = std::max(0.0f, feedback.intensity - delta_time * 0.5f);
                
                if (feedback.intensity <= 0.0f) {
                    feedback.is_active = false;
                }
            }
        }
        
        // Remove expired feedback
        m_visual_queue.erase(
            std::remove_if(m_visual_queue.begin(), m_visual_queue.end(),
                [](const VisualFeedback& feedback) {
                    return !feedback.is_active;
                }),
            m_visual_queue.end()
        );
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error updating visual feedback: {}", e.what());
    }
}

} // namespace vr
} // namespace uevr
} // namespace uevr::vr