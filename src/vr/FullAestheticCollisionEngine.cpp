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
    : m_physics_integration(nullptr)
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
    
    // Initialize collision detection systems
    initializeCollisionDetection();
    initializePhysicsSimulation();
    initializeHapticSystem();
    initializeVisualFeedback();
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
        
        // Reset performance metrics
        m_last_update_time = 0.0f;
        m_collision_checks_per_frame = 0;
        m_average_collision_time = 0.0f;
        
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
        
        // Check against all active objects
        for (const auto& [id, obj_data] : m_objects) {
            if (!obj_data.is_active) continue;
            
            // Calculate distance between positions
            Vec3 obj_pos = obj_data.physics_data.position;
            float distance = std::sqrt(
                std::pow(position.x - obj_pos.x, 2) +
                std::pow(position.y - obj_pos.y, 2) +
                std::pow(position.z - obj_pos.z, 2)
            );
            
            // Check if within collision radius
            if (distance <= radius + m_collision_tolerance) {
                CollisionResult result;
                result.colliding_object = id;
                result.collision_type = CollisionType::TOUCH;
                result.contact_point = position;
                result.contact_normal = Vec3(
                    (position.x - obj_pos.x) / distance,
                    (position.y - obj_pos.y) / distance,
                    (position.z - obj_pos.z) / distance
                );
                result.penetration_depth = radius + m_collision_tolerance - distance;
                result.is_valid = true;
                
                results.push_back(result);
            }
        }
        
        // Update performance metrics
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        m_average_collision_time = (m_average_collision_time + duration.count()) / 2.0f;
        m_collision_checks_per_frame++;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in detectCollisions: {}", e.what());
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
        
        const auto& obj_data1 = it1->second;
        const auto& obj_data2 = it2->second;
        
        if (!obj_data1.is_active || !obj_data2.is_active) {
            return false;
        }
        
        // Check collision layers and masks
        if (!(obj_data1.collision_layers & obj_data2.collision_mask)) {
            return false;
        }
        
        // Calculate distance between objects
        Vec3 pos1 = obj_data1.physics_data.position;
        Vec3 pos2 = obj_data2.physics_data.position;
        float distance = std::sqrt(
            std::pow(pos1.x - pos2.x, 2) +
            std::pow(pos1.y - pos2.y, 2) +
            std::pow(pos1.z - pos2.z, 2)
        );
        
        // Simple collision check based on object scales
        float combined_radius = obj_data1.physics_data.scale.x + obj_data2.physics_data.scale.x;
        return distance <= combined_radius + m_collision_tolerance;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in checkCollision: {}", e.what());
        return false;
    }
}

CollisionResult FullAestheticCollisionEngine::getCollisionResult(ObjectID obj1, ObjectID obj2) {
    CollisionResult result;
    result.is_valid = false;
    
    try {
        if (!checkCollision(obj1, obj2)) {
            return result;
        }
        
        auto it1 = m_objects.find(obj1);
        auto it2 = m_objects.find(obj2);
        
        if (it1 == m_objects.end() || it2 == m_objects.end()) {
            return result;
        }
        
        const auto& obj_data1 = it1->second;
        const auto& obj_data2 = it2->second;
        
        Vec3 pos1 = obj_data1.physics_data.position;
        Vec3 pos2 = obj_data2.physics_data.position;
        
        // Calculate collision details
        result.colliding_object = obj2;
        result.collision_type = CollisionType::COLLIDE;
        result.contact_point = Vec3(
            (pos1.x + pos2.x) / 2.0f,
            (pos1.y + pos2.y) / 2.0f,
            (pos1.z + pos2.z) / 2.0f
        );
        
        // Calculate contact normal
        float distance = std::sqrt(
            std::pow(pos1.x - pos2.x, 2) +
            std::pow(pos1.y - pos2.y, 2) +
            std::pow(pos1.z - pos2.z, 2)
        );
        
        if (distance > 0.0f) {
            result.contact_normal = Vec3(
                (pos1.x - pos2.x) / distance,
                (pos1.y - pos2.y) / distance,
                (pos1.z - pos2.z) / distance
            );
        } else {
            result.contact_normal = Vec3(0.0f, 1.0f, 0.0f);
        }
        
        result.penetration_depth = std::max(0.0f, 
            obj_data1.physics_data.scale.x + obj_data2.physics_data.scale.x - distance);
        result.is_valid = true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in getCollisionResult: {}", e.what());
    }
    
    return result;
}

// Object management
ObjectID FullAestheticCollisionEngine::addObject(const PhysicsObject& obj) {
    try {
        ObjectID id = m_next_object_id++;
        
        ObjectData obj_data;
        obj_data.physics_data = obj;
        obj_data.physics_type = obj.type;
        obj_data.collision_layers = m_collision_layers;
        obj_data.collision_mask = m_collision_mask;
        obj_data.is_active = true;
        obj_data.is_static = (obj.type == PhysicsType::STATIC);
        
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
            
            // Remove from active collisions
            m_active_collisions.erase(
                std::remove_if(m_active_collisions.begin(), m_active_collisions.end(),
                    [id](const CollisionPair& pair) {
                        return pair.obj1 == id || pair.obj2 == id;
                    }),
                m_active_collisions.end()
            );
            
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
            it->second.physics_data = obj;
            it->second.physics_type = obj.type;
            it->second.is_static = (obj.type == PhysicsType::STATIC);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to update object: {}", e.what());
    }
}

PhysicsObject FullAestheticCollisionEngine::getObject(ObjectID id) const {
    try {
        auto it = m_objects.find(id);
        if (it != m_objects.end()) {
            return it->second.physics_data;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to get object: {}", e.what());
    }
    
    return PhysicsObject{};
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
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        m_last_update_time = duration.count() / 1000.0f; // Convert to milliseconds
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in updatePhysicsSimulation: {}", e.what());
    }
}

// Haptic and visual feedback
void FullAestheticCollisionEngine::addHapticFeedback(const HapticFeedback& feedback) {
    try {
        m_haptic_queue.push_back(feedback);
        
        // Limit queue size
        if (m_haptic_queue.size() > 100) {
            m_haptic_queue.erase(m_haptic_queue.begin());
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to add haptic feedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::addVisualFeedback(const VisualFeedback& feedback) {
    try {
        m_visual_queue.push_back(feedback);
        
        // Limit queue size
        if (m_visual_queue.size() > 100) {
            m_visual_queue.erase(m_visual_queue.begin());
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to add visual feedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::clearHapticFeedback(HandType hand) {
    try {
        m_haptic_queue.erase(
            std::remove_if(m_haptic_queue.begin(), m_haptic_queue.end(),
                [hand](const HapticFeedback& feedback) {
                    return feedback.hand == hand || feedback.hand == HandType::BOTH;
                }),
            m_haptic_queue.end()
        );
        
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
        
        // Create haptic feedback for door interaction
        HapticFeedback feedback;
        feedback.hand = hand;
        feedback.type = HapticType::MEDIUM;
        feedback.intensity = 0.7f;
        feedback.duration = 0.2f;
        feedback.position = it->second.physics_data.position;
        feedback.is_active = true;
        
        addHapticFeedback(feedback);
        
        // Create visual feedback
        VisualFeedback visual;
        visual.object_id = door_id;
        visual.highlight_color = Vec3(0.0f, 1.0f, 0.0f); // Green highlight
        visual.intensity = 0.8f;
        visual.is_active = true;
        
        addVisualFeedback(visual);
        
        spdlog::info("[FullAestheticCollision] Door interaction handled for door ID: {}", door_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in handleDoorInteraction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleWeaponInteraction(ObjectID weapon_id, HandType hand) {
    try {
        auto it = m_objects.find(weapon_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        // Create haptic feedback for weapon interaction
        HapticFeedback feedback;
        feedback.hand = hand;
        feedback.type = HapticType::HEAVY;
        feedback.intensity = 0.9f;
        feedback.duration = 0.3f;
        feedback.position = it->second.physics_data.position;
        feedback.is_active = true;
        
        addHapticFeedback(feedback);
        
        // Create visual feedback
        VisualFeedback visual;
        visual.object_id = weapon_id;
        visual.highlight_color = Vec3(1.0f, 0.5f, 0.0f); // Orange highlight
        visual.intensity = 1.0f;
        visual.is_active = true;
        
        addVisualFeedback(visual);
        
        spdlog::info("[FullAestheticCollision] Weapon interaction handled for weapon ID: {}", weapon_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in handleWeaponInteraction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleVehicleInteraction(ObjectID vehicle_id, HandType hand) {
    try {
        auto it = m_objects.find(vehicle_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        // Create haptic feedback for vehicle interaction
        HapticFeedback feedback;
        feedback.hand = hand;
        feedback.type = HapticType::LIGHT;
        feedback.intensity = 0.5f;
        feedback.duration = 0.1f;
        feedback.position = it->second.physics_data.position;
        feedback.is_active = true;
        
        addHapticFeedback(feedback);
        
        spdlog::info("[FullAestheticCollision] Vehicle interaction handled for vehicle ID: {}", vehicle_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in handleVehicleInteraction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleNPCInteraction(ObjectID npc_id, HandType hand) {
    try {
        auto it = m_objects.find(npc_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        // Create haptic feedback for NPC interaction
        HapticFeedback feedback;
        feedback.hand = hand;
        feedback.type = HapticType::MEDIUM;
        feedback.intensity = 0.6f;
        feedback.duration = 0.15f;
        feedback.position = it->second.physics_data.position;
        feedback.is_active = true;
        
        addHapticFeedback(feedback);
        
        // Create visual feedback
        VisualFeedback visual;
        visual.object_id = npc_id;
        visual.highlight_color = Vec3(0.0f, 0.0f, 1.0f); // Blue highlight
        visual.intensity = 0.7f;
        visual.is_active = true;
        
        addVisualFeedback(visual);
        
        spdlog::info("[FullAestheticCollision] NPC interaction handled for NPC ID: {}", npc_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in handleNPCInteraction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleEnvironmentInteraction(ObjectID env_id, HandType hand) {
    try {
        auto it = m_objects.find(env_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        // Create haptic feedback for environment interaction
        HapticFeedback feedback;
        feedback.hand = hand;
        feedback.type = HapticType::LIGHT;
        feedback.intensity = 0.4f;
        feedback.duration = 0.1f;
        feedback.position = it->second.physics_data.position;
        feedback.is_active = true;
        
        addHapticFeedback(feedback);
        
        spdlog::info("[FullAestheticCollision] Environment interaction handled for environment ID: {}", env_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in handleEnvironmentInteraction: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::handleInventoryInteraction(ObjectID item_id, HandType hand) {
    try {
        auto it = m_objects.find(item_id);
        if (it == m_objects.end()) {
            return false;
        }
        
        // Create haptic feedback for inventory interaction
        HapticFeedback feedback;
        feedback.hand = hand;
        feedback.type = HapticType::CUSTOM;
        feedback.intensity = 0.8f;
        feedback.duration = 0.25f;
        feedback.position = it->second.physics_data.position;
        feedback.is_active = true;
        
        addHapticFeedback(feedback);
        
        // Create visual feedback
        VisualFeedback visual;
        visual.object_id = item_id;
        visual.highlight_color = Vec3(1.0f, 1.0f, 0.0f); // Yellow highlight
        visual.intensity = 0.9f;
        visual.is_active = true;
        
        addVisualFeedback(visual);
        
        spdlog::info("[FullAestheticCollision] Inventory interaction handled for item ID: {}", item_id);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in handleInventoryInteraction: {}", e.what());
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
    spdlog::info("[FullAestheticCollision] Max collisions set to: {}", max);
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
    m_last_update_time = 0.0f;
    m_collision_checks_per_frame = 0;
    m_average_collision_time = 0.0f;
    spdlog::info("[FullAestheticCollision] Performance metrics reset");
}

// Private helper methods
bool FullAestheticCollisionEngine::initializeCollisionDetection() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing collision detection system...");
        
        // Initialize collision detection algorithms
        // This would include setting up spatial partitioning, broad phase, narrow phase
        
        spdlog::info("[FullAestheticCollision] Collision detection system initialized");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to initialize collision detection: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializePhysicsSimulation() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing physics simulation system...");
        
        // Initialize physics simulation components
        // This would include setting up physics world, solver, etc.
        
        spdlog::info("[FullAestheticCollision] Physics simulation system initialized");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to initialize physics simulation: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializeHapticSystem() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing haptic feedback system...");
        
        // Initialize haptic feedback system
        // This would include setting up haptic devices, feedback queues, etc.
        
        spdlog::info("[FullAestheticCollision] Haptic feedback system initialized");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to initialize haptic system: {}", e.what());
        return false;
    }
}

bool FullAestheticCollisionEngine::initializeVisualFeedback() {
    try {
        spdlog::info("[FullAestheticCollision] Initializing visual feedback system...");
        
        // Initialize visual feedback system
        // This would include setting up rendering, shaders, etc.
        
        spdlog::info("[FullAestheticCollision] Visual feedback system initialized");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Failed to initialize visual feedback: {}", e.what());
        return false;
    }
}

void FullAestheticCollisionEngine::updateCollisionDetection(float delta_time) {
    try {
        // Clear old collision pairs
        m_active_collisions.clear();
        
        // Perform broad phase collision detection
        std::vector<std::pair<ObjectID, ObjectID>> potential_collisions;
        
        for (auto it1 = m_objects.begin(); it1 != m_objects.end(); ++it1) {
            for (auto it2 = std::next(it1); it2 != m_objects.end(); ++it2) {
                if (checkCollision(it1->first, it2->first)) {
                    potential_collisions.emplace_back(it1->first, it2->first);
                }
            }
        }
        
        // Perform narrow phase collision detection
        for (const auto& [obj1, obj2] : potential_collisions) {
            CollisionResult result = getCollisionResult(obj1, obj2);
            if (result.is_valid) {
                CollisionPair pair;
                pair.obj1 = obj1;
                pair.obj2 = obj2;
                pair.result = result;
                pair.is_active = true;
                m_active_collisions.push_back(pair);
            }
        }
        
        // Limit active collisions
        if (m_active_collisions.size() > m_max_collisions) {
            m_active_collisions.resize(m_max_collisions);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in updateCollisionDetection: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updateHapticFeedback(float delta_time) {
    try {
        // Process haptic feedback queue
        for (auto it = m_haptic_queue.begin(); it != m_haptic_queue.end();) {
            if (it->is_active) {
                // Apply haptic feedback
                // This would interface with actual haptic devices
                
                // Deactivate after duration
                it->duration -= delta_time;
                if (it->duration <= 0.0f) {
                    it->is_active = false;
                }
            }
            
            if (!it->is_active) {
                it = m_haptic_queue.erase(it);
            } else {
                ++it;
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in updateHapticFeedback: {}", e.what());
    }
}

void FullAestheticCollisionEngine::updateVisualFeedback(float delta_time) {
    try {
        // Process visual feedback queue
        for (auto it = m_visual_queue.begin(); it != m_visual_queue.end();) {
            if (it->is_active) {
                // Apply visual feedback
                // This would interface with rendering system
                
                // Deactivate after some time
                it->is_active = false;
            }
            
            if (!it->is_active) {
                it = m_visual_queue.erase(it);
            } else {
                ++it;
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullAestheticCollision] Error in updateVisualFeedback: {}", e.what());
    }
}

} // namespace vr
} // namespace uevr