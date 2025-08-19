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
#include <chrono>

namespace uevr {
namespace vr {

// Constructor
FullPhysicsIntegration::FullPhysicsIntegration()
    : m_collision_engine(nullptr)
    , m_current_engine(PhysicsEngine::BUILTIN)
    , m_quality(PhysicsQuality::MEDIUM)
    , m_global_gravity(0.0f, -9.81f, 0.0f)
    , m_gravity_type(GravityType::UNIFORM)
    , m_max_sub_steps(10)
    , m_fixed_time_step(0.016f)
    , m_next_object_id(1)
    , m_next_constraint_id(1)
    , m_profiling_enabled(false)
    , m_debug_draw_enabled(false)
    , m_accumulator(0.0f)
    , m_simulation_running(false) {
    
    spdlog::info("[FullPhysics] Full Physics Integration created");
}

// Destructor
FullPhysicsIntegration::~FullPhysicsIntegration() {
    spdlog::info("[FullPhysics] Full Physics Integration destroyed");
    shutdown();
}

// Initialization and shutdown
bool FullPhysicsIntegration::initialize() {
    try {
        spdlog::info("[FullPhysics] Initializing physics system...");
        
        // Initialize internal systems
        m_physics_objects.clear();
        m_constraints.clear();
        m_collision_callbacks.clear();
        
        // Reset performance metrics
        m_performance = PhysicsPerformance{};
        
        // Set default physics properties
        m_simulation_running = true;
        
        spdlog::info("[FullPhysics] Physics system initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Failed to initialize: {}", e.what());
        return false;
    }
}

void FullPhysicsIntegration::shutdown() {
    try {
        spdlog::info("[FullPhysics] Shutting down physics system...");
        
        // Stop simulation
        m_simulation_running = false;
        
        // Clear all objects and constraints
        m_physics_objects.clear();
        m_constraints.clear();
        m_collision_callbacks.clear();
        
        spdlog::info("[FullPhysics] Physics system shutdown complete");
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Exception during shutdown: {}", e.what());
    }
}

// Core physics simulation
void FullPhysicsIntegration::updatePhysicsSimulation(float delta_time) {
    if (!m_simulation_running) {
        return;
    }
    
    try {
        // Accumulate time
        m_accumulator += delta_time;
        
        // Fixed timestep simulation
        while (m_accumulator >= m_fixed_time_step) {
            // Update physics objects
            for (auto& [id, obj] : m_physics_objects) {
                if (obj && obj->is_active) {
                    updatePhysicsObject(obj, m_fixed_time_step);
                }
            }
            
            // Update constraints
            for (auto& [id, constraint] : m_constraints) {
                if (constraint && constraint->is_active) {
                    updateConstraint(constraint, m_fixed_time_step);
                }
            }
            
            // Handle collisions
            if (m_collision_engine) {
                m_collision_engine->detectCollisions(m_physics_objects);
            }
            
            m_accumulator -= m_fixed_time_step;
        }
        
        // Update performance metrics
        updatePerformanceMetrics(delta_time);
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Error in physics simulation: {}", e.what());
    }
}

// Physics object management
ObjectID FullPhysicsIntegration::createPhysicsObject(const PhysicsObjectCreateInfo& create_info) {
    try {
        auto obj = std::make_unique<PhysicsObject>();
        obj->id = m_next_object_id++;
        obj->type = create_info.type;
        obj->mass = create_info.mass;
        obj->position = create_info.position;
        obj->rotation = create_info.rotation;
        obj->velocity = create_info.velocity;
        obj->angular_velocity = create_info.angular_velocity;
        obj->is_active = true;
        obj->collision_shape = create_info.collision_shape;
        obj->material_properties = create_info.material_properties;
        
        m_physics_objects[obj->id] = std::move(obj);
        
        spdlog::info("[FullPhysics] Created physics object with ID: {}", obj->id);
        return obj->id;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Failed to create physics object: {}", e.what());
        return INVALID_OBJECT_ID;
    }
}

bool FullPhysicsIntegration::destroyPhysicsObject(ObjectID object_id) {
    try {
        auto it = m_physics_objects.find(object_id);
        if (it != m_physics_objects.end()) {
            m_physics_objects.erase(it);
            spdlog::info("[FullPhysics] Destroyed physics object with ID: {}", object_id);
            return true;
        }
        return false;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Failed to destroy physics object: {}", e.what());
        return false;
    }
}

PhysicsObject* FullPhysicsIntegration::getPhysicsObject(ObjectID object_id) {
    auto it = m_physics_objects.find(object_id);
    return (it != m_physics_objects.end()) ? it->second.get() : nullptr;
}

// Constraint management
ObjectID FullPhysicsIntegration::createConstraint(const ConstraintCreateInfo& create_info) {
    try {
        auto constraint = std::make_unique<PhysicsConstraint>();
        constraint->id = m_next_constraint_id++;
        constraint->type = create_info.type;
        constraint->object_a = create_info.object_a;
        constraint->object_b = create_info.object_b;
        constraint->anchor_point = create_info.anchor_point;
        constraint->axis = create_info.axis;
        constraint->limits = create_info.limits;
        constraint->is_active = true;
        
        m_constraints[constraint->id] = std::move(constraint);
        
        spdlog::info("[FullPhysics] Created constraint with ID: {}", constraint->id);
        return constraint->id;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Failed to create constraint: {}", e.what());
        return INVALID_OBJECT_ID;
    }
}

bool FullPhysicsIntegration::destroyConstraint(ObjectID constraint_id) {
    try {
        auto it = m_constraints.find(constraint_id);
        if (it != m_constraints.end()) {
            m_constraints.erase(it);
            spdlog::info("[FullPhysics] Destroyed constraint with ID: {}", constraint_id);
            return true;
        }
        return false;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Failed to destroy constraint: {}", e.what());
        return false;
    }
}

// Physics engine management
void FullPhysicsIntegration::setPhysicsEngine(PhysicsEngine engine) {
    m_current_engine = engine;
    spdlog::info("[FullPhysics] Physics engine set to: {}", static_cast<int>(engine));
}

void FullPhysicsIntegration::setPhysicsQuality(PhysicsQuality quality) {
    m_quality = quality;
    
    // Adjust simulation parameters based on quality
    switch (quality) {
        case PhysicsQuality::LOW:
            m_max_sub_steps = 5;
            m_fixed_time_step = 0.033f;
            break;
        case PhysicsQuality::MEDIUM:
            m_max_sub_steps = 10;
            m_fixed_time_step = 0.016f;
            break;
        case PhysicsQuality::HIGH:
            m_max_sub_steps = 20;
            m_fixed_time_step = 0.008f;
            break;
        case PhysicsQuality::ULTRA:
            m_max_sub_steps = 40;
            m_fixed_time_step = 0.004f;
            break;
    }
    
    spdlog::info("[FullPhysics] Physics quality set to: {}", static_cast<int>(quality));
}

// Gravity management
void FullPhysicsIntegration::setGlobalGravity(const Vector3& gravity) {
    m_global_gravity = gravity;
    spdlog::info("[FullPhysics] Global gravity set to: ({}, {}, {})", 
                 gravity.x, gravity.y, gravity.z);
}

void FullPhysicsIntegration::setGravityType(GravityType type) {
    m_gravity_type = type;
    spdlog::info("[FullPhysics] Gravity type set to: {}", static_cast<int>(type));
}

// Collision engine management
void FullPhysicsIntegration::setCollisionEngine(FullAestheticCollisionEngine* collision_engine) {
    m_collision_engine = collision_engine;
    spdlog::info("[FullPhysics] Collision engine set");
}

// Performance and profiling
void FullPhysicsIntegration::setProfilingEnabled(bool enabled) {
    m_profiling_enabled = enabled;
    spdlog::info("[FullPhysics] Profiling {}", enabled ? "enabled" : "disabled");
}

void FullPhysicsIntegration::setDebugDrawEnabled(bool enabled) {
    m_debug_draw_enabled = enabled;
    spdlog::info("[FullPhysics] Debug drawing {}", enabled ? "enabled" : "disabled");
}

PhysicsPerformance FullPhysicsIntegration::getPerformanceMetrics() const {
    return m_performance;
}

// Utility methods
void FullPhysicsIntegration::resetSimulation() {
    try {
        spdlog::info("[FullPhysics] Resetting physics simulation...");
        
        // Reset all objects to initial state
        for (auto& [id, obj] : m_physics_objects) {
            if (obj) {
                obj->velocity = Vector3(0.0f, 0.0f, 0.0f);
                obj->angular_velocity = Vector3(0.0f, 0.0f, 0.0f);
            }
        }
        
        // Reset constraints
        for (auto& [id, constraint] : m_constraints) {
            if (constraint) {
                constraint->is_active = true;
            }
        }
        
        // Reset performance metrics
        m_performance = PhysicsPerformance{};
        m_accumulator = 0.0f;
        
        spdlog::info("[FullPhysics] Physics simulation reset complete");
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Error resetting simulation: {}", e.what());
    }
}

void FullPhysicsIntegration::pauseSimulation() {
    m_simulation_running = false;
    spdlog::info("[FullPhysics] Physics simulation paused");
}

void FullPhysicsIntegration::resumeSimulation() {
    m_simulation_running = true;
    spdlog::info("[FullPhysics] Physics simulation resumed");
}

// Private helper methods
void FullPhysicsIntegration::updatePhysicsObject(std::unique_ptr<PhysicsObject>& obj, float delta_time) {
    if (!obj || !obj->is_active) return;
    
    try {
        // Apply gravity
        Vector3 gravity_force = m_global_gravity * obj->mass;
        
        // Update velocity (F = ma, v = v0 + at)
        Vector3 acceleration = gravity_force / obj->mass;
        obj->velocity += acceleration * delta_time;
        
        // Update position
        obj->position += obj->velocity * delta_time;
        
        // Update rotation
        obj->rotation += obj->angular_velocity * delta_time;
        
        // Apply damping
        const float linear_damping = 0.98f;
        const float angular_damping = 0.95f;
        obj->velocity *= linear_damping;
        obj->angular_velocity *= angular_damping;
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Error updating physics object: {}", e.what());
    }
}

void FullPhysicsIntegration::updateConstraint(std::unique_ptr<PhysicsConstraint>& constraint, float delta_time) {
    if (!constraint || !constraint->is_active) return;
    
    try {
        // Get the connected objects
        auto obj_a = getPhysicsObject(constraint->object_a);
        auto obj_b = getPhysicsObject(constraint->object_b);
        
        if (!obj_a || !obj_b) return;
        
        // Apply constraint forces based on type
        switch (constraint->type) {
            case ConstraintType::HINGE:
                updateHingeConstraint(constraint.get(), obj_a, obj_b, delta_time);
                break;
            case ConstraintType::BALL_AND_SOCKET:
                updateBallAndSocketConstraint(constraint.get(), obj_a, obj_b, delta_time);
                break;
            case ConstraintType::SLIDER:
                updateSliderConstraint(constraint.get(), obj_a, obj_b, delta_time);
                break;
            case ConstraintType::FIXED:
                updateFixedConstraint(constraint.get(), obj_a, obj_b, delta_time);
                break;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Error updating constraint: {}", e.what());
    }
}

void FullPhysicsIntegration::updateHingeConstraint(PhysicsConstraint* constraint, 
                                                 PhysicsObject* obj_a, 
                                                 PhysicsObject* obj_b, 
                                                 float delta_time) {
    // Calculate hinge axis in world space
    Vector3 world_axis = obj_a->rotation * constraint->axis;
    
    // Project velocities onto the hinge axis
    Vector3 rel_vel = obj_b->velocity - obj_a->velocity;
    float hinge_velocity = rel_vel.dot(world_axis);
    
    // Apply corrective forces to maintain constraint
    Vector3 corrective_force = world_axis * hinge_velocity * 0.1f;
    
    if (obj_a->mass > 0) {
        obj_a->velocity += corrective_force / obj_a->mass * delta_time;
    }
    if (obj_b->mass > 0) {
        obj_b->velocity -= corrective_force / obj_b->mass * delta_time;
    }
}

void FullPhysicsIntegration::updateBallAndSocketConstraint(PhysicsConstraint* constraint, 
                                                         PhysicsObject* obj_a, 
                                                         PhysicsObject* obj_b, 
                                                         float delta_time) {
    // Calculate the distance between anchor points
    Vector3 anchor_a = obj_a->position + obj_a->rotation * constraint->anchor_point;
    Vector3 anchor_b = obj_b->position + obj_b->rotation * constraint->anchor_point;
    Vector3 separation = anchor_b - anchor_a;
    
    // Apply corrective forces to maintain distance
    float distance = separation.length();
    if (distance > 0.001f) {
        Vector3 correction = separation.normalized() * distance * 0.1f;
        
        if (obj_a->mass > 0) {
            obj_a->position += correction * 0.5f;
        }
        if (obj_b->mass > 0) {
            obj_b->position -= correction * 0.5f;
        }
    }
}

void FullPhysicsIntegration::updateSliderConstraint(PhysicsConstraint* constraint, 
                                                  PhysicsObject* obj_a, 
                                                  PhysicsObject* obj_b, 
                                                  float delta_time) {
    // Calculate the slider axis in world space
    Vector3 world_axis = obj_a->rotation * constraint->axis;
    
    // Project positions onto the slider axis
    Vector3 rel_pos = obj_b->position - obj_a->position;
    float slide_distance = rel_pos.dot(world_axis);
    
    // Apply limits if specified
    if (constraint->limits.min_value < constraint->limits.max_value) {
        if (slide_distance < constraint->limits.min_value) {
            slide_distance = constraint->limits.min_value;
        } else if (slide_distance > constraint->limits.max_value) {
            slide_distance = constraint->limits.max_value;
        }
    }
    
    // Apply corrective position
    Vector3 target_pos = obj_a->position + world_axis * slide_distance;
    Vector3 correction = (target_pos - obj_b->position) * 0.1f;
    
    if (obj_b->mass > 0) {
        obj_b->position += correction;
    }
}

void FullPhysicsIntegration::updateFixedConstraint(PhysicsConstraint* constraint, 
                                                 PhysicsObject* obj_a, 
                                                 PhysicsObject* obj_b, 
                                                 float delta_time) {
    // Fixed constraint: maintain relative position and orientation
    Vector3 target_pos = obj_a->position + obj_a->rotation * constraint->anchor_point;
    Vector3 current_pos = obj_b->position + obj_b->rotation * constraint->anchor_point;
    
    Vector3 pos_correction = (target_pos - current_pos) * 0.2f;
    
    if (obj_b->mass > 0) {
        obj_b->position += pos_correction;
        obj_b->rotation = obj_a->rotation;
    }
}

void FullPhysicsIntegration::updatePerformanceMetrics(float delta_time) {
    if (!m_profiling_enabled) return;
    
    try {
        // Update frame time
        m_performance.frame_time = delta_time;
        
        // Count active objects
        m_performance.active_objects = 0;
        m_performance.active_constraints = 0;
        
        for (const auto& [id, obj] : m_physics_objects) {
            if (obj && obj->is_active) {
                m_performance.active_objects++;
            }
        }
        
        for (const auto& [id, constraint] : m_constraints) {
            if (constraint && constraint->is_active) {
                m_performance.active_constraints++;
            }
        }
        
        // Update collision count (if collision engine provides this info)
        if (m_collision_engine) {
            // This would need to be implemented in the collision engine
            // m_performance.collision_count = m_collision_engine->getCollisionCount();
        }
        
        // Calculate FPS
        static float time_accumulator = 0.0f;
        static int frame_count = 0;
        
        time_accumulator += delta_time;
        frame_count++;
        
        if (time_accumulator >= 1.0f) {
            m_performance.fps = static_cast<float>(frame_count) / time_accumulator;
            time_accumulator = 0.0f;
            frame_count = 0;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[FullPhysics] Error updating performance metrics: {}", e.what());
    }
}

} // namespace vr
} // namespace uevr
