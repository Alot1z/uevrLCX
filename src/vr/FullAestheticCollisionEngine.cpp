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
#include <fstream>
#include <sstream>
#include <json/json.hpp>

namespace uevr {

// Private implementation class
class FullAestheticCollisionEngine::Impl {
public:
    Impl() : m_initialized(false), m_debug_mode(false), m_error_state(false),
             m_collision_threshold(0.1f), m_haptic_intensity(1.0f), 
             m_visual_intensity(1.0f), m_audio_volume(1.0f),
             m_horror_intensity(0.5f), m_combat_intensity(0.5f), 
             m_cyberpunk_intensity(0.5f), m_current_game("") {
        
        spdlog::info("[FullAestheticCollisionEngine] Initializing implementation");
        initializeDefaultCollisionLayers();
    }
    
    ~Impl() {
        spdlog::info("[FullAestheticCollisionEngine] Shutting down implementation");
        shutdown();
    }
    
    // Core initialization
    bool initialize() {
        if (m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Already initialized");
            return true;
        }
        
        try {
            spdlog::info("[FullAestheticCollisionEngine] Starting initialization");
            
            // Initialize collision layers
            if (!initializeCollisionLayers()) {
                spdlog::error("[FullAestheticCollisionEngine] Failed to initialize collision layers");
                return false;
            }
            
            // Initialize physics system
            if (!initializePhysicsSystem()) {
                spdlog::error("[FullAestheticCollisionEngine] Failed to initialize physics system");
                return false;
            }
            
            // Initialize haptic system
            if (!initializeHapticSystem()) {
                spdlog::error("[FullAestheticCollisionEngine] Failed to initialize haptic system");
                return false;
            }
            
            // Initialize visual system
            if (!initializeVisualSystem()) {
                spdlog::error("[FullAestheticCollisionEngine] Failed to initialize visual system");
                return false;
            }
            
            // Initialize audio system
            if (!initializeAudioSystem()) {
                spdlog::error("[FullAestheticCollisionEngine] Failed to initialize audio system");
                return false;
            }
            
            m_initialized = true;
            spdlog::info("[FullAestheticCollisionEngine] Initialization completed successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Initialization failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    void shutdown() {
        if (!m_initialized) return;
        
        spdlog::info("[FullAestheticCollisionEngine] Shutting down");
        
        // Clear all objects
        m_objects.clear();
        m_collision_layers.clear();
        m_active_collisions.clear();
        
        // Reset performance metrics
        resetPerformanceMetrics();
        
        m_initialized = false;
        spdlog::info("[FullAestheticCollisionEngine] Shutdown completed");
    }
    
    bool isInitialized() const {
        return m_initialized;
    }
    
    // Collision detection system
    CollisionResult detectFullCollision(ObjectID object, HandType hand, CollisionType type) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot detect collision");
            return CollisionResult{};
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            // Check if object exists
            auto obj_iter = m_objects.find(object);
            if (obj_iter == m_objects.end()) {
                if (m_debug_mode) {
                    spdlog::debug("[FullAestheticCollisionEngine] Object {} not found", object);
                }
                return CollisionResult{};
            }
            
            // Get hand position (simulated for now)
            glm::vec3 hand_position = getHandPosition(hand);
            
            // Calculate collision with object
            CollisionResult result = calculateCollision(object, hand_position, hand, type);
            
            // Update performance metrics
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            m_performance.collision_detection_time = duration.count() / 1000.0f; // Convert to milliseconds
            m_performance.total_collisions++;
            
            if (result.collision_detected) {
                m_performance.active_collisions++;
                
                // Add to active collisions
                m_active_collisions.push_back(result);
                
                // Provide haptic feedback
                if (result.haptic_intensity > 0.0f) {
                    provideHapticFeedback(hand, result.haptic_intensity, HapticType::TOUCH);
                }
                
                // Show visual feedback
                if (result.visual_feedback) {
                    showVisualFeedback(object, type);
                }
                
                // Play audio feedback
                if (result.audio_feedback) {
                    playAudioFeedback(object, type, m_audio_volume);
                }
            }
            
            return result;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Collision detection failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return CollisionResult{};
        }
    }
    
    std::vector<CollisionResult> detectAllCollisions(HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot detect collisions");
            return {};
        }
        
        std::vector<CollisionResult> results;
        
        try {
            // Get hand position
            glm::vec3 hand_position = getHandPosition(hand);
            
            // Check collisions with all objects
            for (const auto& obj_pair : m_objects) {
                ObjectID obj_id = obj_pair.first;
                const auto& obj_data = obj_pair.second;
                
                // Skip disabled objects
                if (!obj_data.collision_enabled) continue;
                
                // Check collision for each collision type
                for (int i = 0; i < static_cast<int>(CollisionType::HAND_UI) + 1; ++i) {
                    CollisionType type = static_cast<CollisionType>(i);
                    
                    CollisionResult result = calculateCollision(obj_id, hand_position, hand, type);
                    if (result.collision_detected) {
                        results.push_back(result);
                    }
                }
            }
            
            // Update performance metrics
            m_performance.active_collisions = results.size();
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] All collision detection failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
        
        return results;
    }
    
    bool updateCollisionSystem(float delta_time) {
        if (!m_initialized) return false;
        
        try {
            // Update physics simulation
            updatePhysicsSystem(delta_time);
            
            // Update haptic system
            updateHapticSystem(delta_time);
            
            // Update visual system
            updateVisualSystem(delta_time);
            
            // Update audio system
            updateAudioSystem(delta_time);
            
            // Update performance metrics
            updatePerformanceMetrics(delta_time);
            
            // Clean up old collisions
            cleanupOldCollisions();
            
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Collision system update failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    // Physics simulation system
    void simulateFullPhysics(ObjectID object, PhysicsType physics_type) {
        if (!m_initialized) return;
        
        try {
            auto obj_iter = m_objects.find(object);
            if (obj_iter == m_objects.end()) return;
            
            auto& obj_data = obj_iter->second;
            obj_data.physics_type = physics_type;
            
            // Apply physics based on type
            switch (physics_type) {
                case PhysicsType::STATIC:
                    // Static objects don't move
                    break;
                    
                case PhysicsType::DYNAMIC:
                    // Apply gravity and forces
                    applyGravity(object);
                    applyForces(object);
                    break;
                    
                case PhysicsType::KINEMATIC:
                    // Kinematic objects move based on velocity
                    updateKinematicPhysics(object);
                    break;
                    
                case PhysicsType::TRIGGER:
                    // Trigger objects only detect collisions
                    break;
                    
                case PhysicsType::PARTICLE:
                    // Particle physics simulation
                    simulateParticlePhysics(object);
                    break;
                    
                case PhysicsType::FLUID:
                    // Fluid physics simulation
                    simulateFluidPhysics(object);
                    break;
                    
                case PhysicsType::CLOTH:
                    // Cloth physics simulation
                    simulateClothPhysics(object);
                    break;
                    
                case PhysicsType::ROPE:
                    // Rope physics simulation
                    simulateRopePhysics(object);
                    break;
                    
                case PhysicsType::JOINT:
                    // Joint physics simulation
                    simulateJointPhysics(object);
                    break;
                    
                case PhysicsType::CONSTRAINT:
                    // Constraint physics simulation
                    simulateConstraintPhysics(object);
                    break;
                    
                default:
                    break;
            }
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Physics simulation failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
    }
    
    void simulateAllPhysics(float delta_time) {
        if (!m_initialized) return;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            for (auto& obj_pair : m_objects) {
                ObjectID obj_id = obj_pair.first;
                auto& obj_data = obj_pair.second;
                
                if (obj_data.physics_enabled) {
                    simulateFullPhysics(obj_id, obj_data.physics_type);
                }
            }
            
            // Update performance metrics
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            m_performance.physics_simulation_time = duration.count() / 1000.0f;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] All physics simulation failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
    }
    
    bool updatePhysicsSystem(float delta_time) {
        if (!m_initialized) return false;
        
        try {
            // Update all physics objects
            simulateAllPhysics(delta_time);
            
            // Update collision detection
            updateCollisionDetection(delta_time);
            
            // Update constraints and joints
            updateConstraints(delta_time);
            
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Physics system update failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    // Haptic feedback system
    void provideFullHapticFeedback(HandType hand, float intensity, HapticType haptic_type) {
        if (!m_initialized || !m_haptic_enabled) return;
        
        try {
            // Clamp intensity to valid range
            intensity = std::clamp(intensity * m_haptic_intensity, 0.0f, 1.0f);
            
            // Apply haptic feedback based on type
            switch (haptic_type) {
                case HapticType::LIGHT:
                    applyLightHaptic(hand, intensity);
                    break;
                    
                case HapticType::MEDIUM:
                    applyMediumHaptic(hand, intensity);
                    break;
                    
                case HapticType::STRONG:
                    applyStrongHaptic(hand, intensity);
                    break;
                    
                case HapticType::PULSE:
                    applyPulseHaptic(hand, intensity);
                    break;
                    
                case HapticType::VIBRATION:
                    applyVibrationHaptic(hand, intensity);
                    break;
                    
                case HapticType::IMPACT:
                    applyImpactHaptic(hand, intensity);
                    break;
                    
                case HapticType::TOUCH:
                    applyTouchHaptic(hand, intensity);
                    break;
                    
                case HapticType::GRAB:
                    applyGrabHaptic(hand, intensity);
                    break;
                    
                case HapticType::RELEASE:
                    applyReleaseHaptic(hand, intensity);
                    break;
                    
                case HapticType::ERROR:
                    applyErrorHaptic(hand, intensity);
                    break;
                    
                case HapticType::SUCCESS:
                    applySuccessHaptic(hand, intensity);
                    break;
                    
                default:
                    break;
            }
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Haptic feedback failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
    }
    
    void updateHapticSystem(float delta_time) {
        if (!m_initialized || !m_haptic_enabled) return;
        
        try {
            // Update haptic patterns
            updateHapticPatterns(delta_time);
            
            // Update haptic intensity
            updateHapticIntensity(delta_time);
            
            // Update performance metrics
            auto start_time = std::chrono::high_resolution_clock::now();
            // ... haptic update logic ...
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            m_performance.haptic_feedback_time = duration.count() / 1000.0f;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Haptic system update failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
    }
    
    bool isHapticEnabled() const {
        return m_haptic_enabled;
    }

    // Visual feedback system
    void showFullCollisionHighlight(ObjectID object, CollisionType collision_type) {
        if (!m_initialized || !m_visual_enabled) return;
        
        try {
            auto obj_iter = m_objects.find(object);
            if (obj_iter == m_objects.end()) return;
            
            const auto& obj_data = obj_iter->second;
            
            // Apply visual feedback based on collision type
            switch (collision_type) {
                case CollisionType::HAND_DOOR:
                    showDoorHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_WEAPON:
                    showWeaponHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_VEHICLE:
                    showVehicleHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_NPC:
                    showNPCHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_ENVIRONMENT:
                    showEnvironmentHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_PUZZLE:
                    showPuzzleHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_INVENTORY:
                    showInventoryHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_CRAFTING:
                    showCraftingHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_HACKING:
                    showHackingHighlight(object, obj_data);
                    break;
                    
                case CollisionType::HAND_CYBERWARE:
                    showCyberwareHighlight(object, obj_data);
                    break;
                    
                default:
                    showDefaultHighlight(object, obj_data);
                    break;
            }
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Visual feedback failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
    }
    
    void updateVisualSystem(float delta_time) {
        if (!m_initialized || !m_visual_enabled) return;
        
        try {
            // Update visual effects
            updateVisualEffects(delta_time);
            
            // Update highlight animations
            updateHighlightAnimations(delta_time);
            
            // Update performance metrics
            auto start_time = std::chrono::high_resolution_clock::now();
            // ... visual update logic ...
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            m_performance.visual_feedback_time = duration.count() / 1000.0f;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Visual system update failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
    }
    
    bool isVisualEnabled() const {
        return m_visual_enabled;
    }
    
    // Audio feedback system
    void playCollisionAudio(ObjectID object, CollisionType collision_type, float volume) {
        if (!m_initialized || !m_audio_enabled) return;
        
        try {
            auto obj_iter = m_objects.find(object);
            if (obj_iter == m_objects.end()) return;
            
            const auto& obj_data = obj_iter->second;
            
            // Apply volume scaling
            volume = std::clamp(volume * m_audio_volume, 0.0f, 1.0f);
            
            // Play audio based on collision type
            switch (collision_type) {
                case CollisionType::HAND_DOOR:
                    playDoorAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_WEAPON:
                    playWeaponAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_VEHICLE:
                    playVehicleAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_NPC:
                    playNPCAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_ENVIRONMENT:
                    playEnvironmentAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_PUZZLE:
                    playPuzzleAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_INVENTORY:
                    playInventoryAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_CRAFTING:
                    playCraftingAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_HACKING:
                    playHackingAudio(object, obj_data, volume);
                    break;
                    
                case CollisionType::HAND_CYBERWARE:
                    playCyberwareAudio(object, obj_data, volume);
                    break;
                    
                default:
                    playDefaultAudio(object, obj_data, volume);
                    break;
            }
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Audio feedback failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
    }
    
    void updateAudioSystem(float delta_time) {
        if (!m_initialized || !m_audio_enabled) return;
        
        try {
            // Update audio sources
            updateAudioSources(delta_time);
            
            // Update audio effects
            updateAudioEffects(delta_time);
            
            // Update spatial audio
            updateSpatialAudio(delta_time);
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Audio system update failed: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
        }
    }
    
    bool isAudioEnabled() const {
        return m_audio_enabled;
    }
    
    // VR Transformer features - Door handle grabbing
    bool enableDoorHandleGrabbing(ObjectID door, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot enable door grabbing");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(door);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Door {} not found", door);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if object is a door
            if (obj_data.type != ObjectType::INTERACTIVE) {
                spdlog::warn("[FullAestheticCollisionEngine] Object {} is not interactive", door);
                return false;
            }
            
            // Enable door grabbing
            obj_data.is_grabbable = true;
            obj_data.grab_type = "door_handle";
            
            spdlog::info("[FullAestheticCollisionEngine] Enabled door handle grabbing for door {}", door);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to enable door grabbing: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool grabDoorHandle(ObjectID door, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot grab door");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(door);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Door {} not found", door);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if door is grabbable
            if (!obj_data.is_grabbable) {
                spdlog::warn("[FullAestheticCollisionEngine] Door {} is not grabbable", door);
                return false;
            }
            
            // Set door as grabbed
            obj_data.is_grabbed = true;
            obj_data.grabbing_hand = hand;
            obj_data.grab_time = getCurrentTime();
            
            // Provide haptic feedback
            provideFullHapticFeedback(hand, 0.8f, HapticType::GRAB);
            
            // Show visual feedback
            showFullCollisionHighlight(door, CollisionType::HAND_DOOR);
            
            // Play audio feedback
            playCollisionAudio(door, CollisionType::HAND_DOOR, 0.7f);
            
            spdlog::info("[FullAestheticCollisionEngine] Grabbed door {} with hand {}", door, static_cast<int>(hand));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to grab door: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool releaseDoorHandle(ObjectID door, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot release door");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(door);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Door {} not found", door);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if door is grabbed by this hand
            if (!obj_data.is_grabbed || obj_data.grabbing_hand != hand) {
                spdlog::warn("[FullAestheticCollisionEngine] Door {} is not grabbed by hand {}", door, static_cast<int>(hand));
                return false;
            }
            
            // Release door
            obj_data.is_grabbed = false;
            obj_data.grabbing_hand = HandType::NEITHER;
            obj_data.grab_time = 0.0f;
            
            // Provide haptic feedback
            provideFullHapticFeedback(hand, 0.3f, HapticType::RELEASE);
            
            // Hide visual feedback
            hideVisualFeedback(door);
            
            spdlog::info("[FullAestheticCollisionEngine] Released door {} from hand {}", door, static_cast<int>(hand));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to release door: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool rotateDoorHandle(ObjectID door, HandType hand, float rotation) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot rotate door");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(door);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Door {} not found", door);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if door is grabbed by this hand
            if (!obj_data.is_grabbed || obj_data.grabbing_hand != hand) {
                spdlog::warn("[FullAestheticCollisionEngine] Door {} is not grabbed by hand {}", door, static_cast<int>(hand));
                return false;
            }
            
            // Apply rotation to door
            glm::quat rotation_quat = glm::angleAxis(rotation, glm::vec3(0.0f, 1.0f, 0.0f));
            obj_data.rotation = obj_data.rotation * rotation_quat;
            
            // Provide haptic feedback based on rotation
            float haptic_intensity = std::clamp(std::abs(rotation) * 0.5f, 0.1f, 0.8f);
            provideFullHapticFeedback(hand, haptic_intensity, HapticType::VIBRATION);
            
            // Update door state
            if (std::abs(rotation) > 0.1f) {
                obj_data.door_state = "rotating";
                obj_data.rotation_progress += std::abs(rotation);
            }
            
            spdlog::debug("[FullAestheticCollisionEngine] Rotated door {} by {} radians", door, rotation);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to rotate door: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    // VR Transformer features - Weapon interaction
    bool enableWeaponInteraction(ObjectID weapon, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot enable weapon interaction");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(weapon);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Weapon {} not found", weapon);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if object is a weapon
            if (obj_data.type != ObjectType::WEAPON) {
                spdlog::warn("[FullAestheticCollisionEngine] Object {} is not a weapon", weapon);
                return false;
            }
            
            // Enable weapon interaction
            obj_data.is_interactable = true;
            obj_data.interaction_type = "weapon";
            
            spdlog::info("[FullAestheticCollisionEngine] Enabled weapon interaction for weapon {}", weapon);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to enable weapon interaction: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool grabWeapon(ObjectID weapon, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot grab weapon");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(weapon);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Weapon {} not found", weapon);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if weapon is interactable
            if (!obj_data.is_interactable) {
                spdlog::warn("[FullAestheticCollisionEngine] Weapon {} is not interactable", weapon);
                return false;
            }
            
            // Set weapon as grabbed
            obj_data.is_grabbed = true;
            obj_data.grabbing_hand = hand;
            obj_data.grab_time = getCurrentTime();
            
            // Provide haptic feedback
            provideFullHapticFeedback(hand, 0.9f, HapticType::GRAB);
            
            // Show visual feedback
            showFullCollisionHighlight(weapon, CollisionType::HAND_WEAPON);
            
            // Play audio feedback
            playCollisionAudio(weapon, CollisionType::HAND_WEAPON, 0.8f);
            
            spdlog::info("[FullAestheticCollisionEngine] Grabbed weapon {} with hand {}", weapon, static_cast<int>(hand));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to grab weapon: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool releaseWeapon(ObjectID weapon, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot release weapon");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(weapon);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Weapon {} not found", weapon);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if weapon is grabbed by this hand
            if (!obj_data.is_grabbed || obj_data.grabbing_hand != hand) {
                spdlog::warn("[FullAestheticCollisionEngine] Weapon {} is not grabbed by hand {}", weapon, static_cast<int>(hand));
                return false;
            }
            
            // Release weapon
            obj_data.is_grabbed = false;
            obj_data.grabbing_hand = HandType::NEITHER;
            obj_data.grab_time = 0.0f;
            
            // Provide haptic feedback
            provideFullHapticFeedback(hand, 0.4f, HapticType::RELEASE);
            
            // Hide visual feedback
            hideVisualFeedback(weapon);
            
            spdlog::info("[FullAestheticCollisionEngine] Released weapon {} from hand {}", weapon, static_cast<int>(hand));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to release weapon: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool aimWeapon(ObjectID weapon, HandType hand, const glm::vec3& direction) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot aim weapon");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(weapon);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Weapon {} not found", weapon);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if weapon is grabbed by this hand
            if (!obj_data.is_grabbed || obj_data.grabbing_hand != hand) {
                spdlog::warn("[FullAestheticCollisionEngine] Weapon {} is not grabbed by hand {}", weapon, static_cast<int>(hand));
                return false;
            }
            
            // Calculate aim direction
            glm::vec3 normalized_direction = glm::normalize(direction);
            
            // Update weapon rotation to aim in direction
            glm::quat aim_rotation = glm::quatLookAt(normalized_direction, glm::vec3(0.0f, 1.0f, 0.0f));
            obj_data.rotation = aim_rotation;
            
            // Provide haptic feedback for aiming
            provideFullHapticFeedback(hand, 0.2f, HapticType::LIGHT);
            
            // Update weapon state
            obj_data.weapon_state = "aiming";
            obj_data.aim_direction = normalized_direction;
            
            spdlog::debug("[FullAestheticCollisionEngine] Aimed weapon {} in direction ({}, {}, {})", 
                         weapon, direction.x, direction.y, direction.z);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to aim weapon: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }

    // VR Transformer features - Vehicle interaction
    bool enableVehicleInteraction(ObjectID vehicle, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot enable vehicle interaction");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(vehicle);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Vehicle {} not found", vehicle);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if object is a vehicle
            if (obj_data.type != ObjectType::VEHICLE) {
                spdlog::warn("[FullAestheticCollisionEngine] Object {} is not a vehicle", vehicle);
                return false;
            }
            
            // Enable vehicle interaction
            obj_data.is_interactable = true;
            obj_data.interaction_type = "vehicle";
            
            spdlog::info("[FullAestheticCollisionEngine] Enabled vehicle interaction for vehicle {}", vehicle);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to enable vehicle interaction: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool enterVehicle(ObjectID vehicle, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot enter vehicle");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(vehicle);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Vehicle {} not found", vehicle);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if vehicle is interactable
            if (!obj_data.is_interactable) {
                spdlog::warn("[FullAestheticCollisionEngine] Vehicle {} is not interactable", vehicle);
                return false;
            }
            
            // Check if vehicle is already occupied
            if (obj_data.is_occupied) {
                spdlog::warn("[FullAestheticCollisionEngine] Vehicle {} is already occupied", vehicle);
                return false;
            }
            
            // Enter vehicle
            obj_data.is_occupied = true;
            obj_data.occupying_hand = hand;
            obj_data.enter_time = getCurrentTime();
            
            // Provide haptic feedback
            provideFullHapticFeedback(hand, 0.7f, HapticType::SUCCESS);
            
            // Show visual feedback
            showFullCollisionHighlight(vehicle, CollisionType::HAND_VEHICLE);
            
            // Play audio feedback
            playCollisionAudio(vehicle, CollisionType::HAND_VEHICLE, 0.6f);
            
            spdlog::info("[FullAestheticCollisionEngine] Entered vehicle {} with hand {}", vehicle, static_cast<int>(hand));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to enter vehicle: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool exitVehicle(ObjectID vehicle, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot exit vehicle");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(vehicle);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Vehicle {} not found", vehicle);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if vehicle is occupied by this hand
            if (!obj_data.is_occupied || obj_data.occupying_hand != hand) {
                spdlog::warn("[FullAestheticCollisionEngine] Vehicle {} is not occupied by hand {}", vehicle, static_cast<int>(hand));
                return false;
            }
            
            // Exit vehicle
            obj_data.is_occupied = false;
            obj_data.occupying_hand = HandType::NEITHER;
            obj_data.enter_time = 0.0f;
            
            // Provide haptic feedback
            provideFullHapticFeedback(hand, 0.3f, HapticType::RELEASE);
            
            // Hide visual feedback
            hideVisualFeedback(vehicle);
            
            spdlog::info("[FullAestheticCollisionEngine] Exited vehicle {} from hand {}", vehicle, static_cast<int>(hand));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to exit vehicle: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool interactWithVehicle(ObjectID vehicle, HandType hand, InteractionType interaction) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot interact with vehicle");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(vehicle);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Vehicle {} not found", vehicle);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if vehicle is interactable
            if (!obj_data.is_interactable) {
                spdlog::warn("[FullAestheticCollisionEngine] Vehicle {} is not interactable", vehicle);
                return false;
            }
            
            // Handle different interaction types
            switch (interaction) {
                case InteractionType::TOUCH:
                    handleVehicleTouch(vehicle, hand);
                    break;
                    
                case InteractionType::GRAB:
                    handleVehicleGrab(vehicle, hand);
                    break;
                    
                case InteractionType::ACTIVATE:
                    handleVehicleActivate(vehicle, hand);
                    break;
                    
                case InteractionType::TOGGLE:
                    handleVehicleToggle(vehicle, hand);
                    break;
                    
                default:
                    spdlog::warn("[FullAestheticCollisionEngine] Unsupported vehicle interaction type: {}", static_cast<int>(interaction));
                    return false;
            }
            
            spdlog::info("[FullAestheticCollisionEngine] Interacted with vehicle {} using interaction {}", vehicle, static_cast<int>(interaction));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to interact with vehicle: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    // VR Transformer features - NPC interaction
    bool enableNPCInteraction(ObjectID npc, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot enable NPC interaction");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(npc);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] NPC {} not found", npc);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if object is an NPC
            if (obj_data.type != ObjectType::NPC) {
                spdlog::warn("[FullAestheticCollisionEngine] Object {} is not an NPC", npc);
                return false;
            }
            
            // Enable NPC interaction
            obj_data.is_interactable = true;
            obj_data.interaction_type = "npc";
            
            spdlog::info("[FullAestheticCollisionEngine] Enabled NPC interaction for NPC {}", npc);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to enable NPC interaction: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool touchNPC(ObjectID npc, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot touch NPC");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(npc);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] NPC {} not found", npc);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if NPC is interactable
            if (!obj_data.is_interactable) {
                spdlog::warn("[FullAestheticCollisionEngine] NPC {} is not interactable", npc);
                return false;
            }
            
            // Touch NPC (visual feedback only, no force manipulation)
            obj_data.last_touched_time = getCurrentTime();
            obj_data.touching_hand = hand;
            
            // Provide haptic feedback
            provideFullHapticFeedback(hand, 0.4f, HapticType::TOUCH);
            
            // Show visual feedback
            showFullCollisionHighlight(npc, CollisionType::HAND_NPC);
            
            // Play audio feedback
            playCollisionAudio(npc, CollisionType::HAND_NPC, 0.5f);
            
            spdlog::info("[FullAestheticCollisionEngine] Touched NPC {} with hand {}", npc, static_cast<int>(hand));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to touch NPC: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool interactWithNPC(ObjectID npc, HandType hand, InteractionType interaction) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot interact with NPC");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(npc);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] NPC {} not found", npc);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if NPC is interactable
            if (!obj_data.is_interactable) {
                spdlog::warn("[FullAestheticCollisionEngine] NPC {} is not interactable", npc);
                return false;
            }
            
            // Handle different interaction types
            switch (interaction) {
                case InteractionType::TOUCH:
                    handleNPCTouch(npc, hand);
                    break;
                    
                case InteractionType::SELECT:
                    handleNPCSelect(npc, hand);
                    break;
                    
                case InteractionType::ACTIVATE:
                    handleNPCActivate(npc, hand);
                    break;
                    
                case InteractionType::MENU:
                    handleNPCMenu(npc, hand);
                    break;
                    
                default:
                    spdlog::warn("[FullAestheticCollisionEngine] Unsupported NPC interaction type: {}", static_cast<int>(interaction));
                    return false;
            }
            
            spdlog::info("[FullAestheticCollisionEngine] Interacted with NPC {} using interaction {}", npc, static_cast<int>(interaction));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to interact with NPC: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    // VR Transformer features - Environmental interaction
    bool enableEnvironmentalInteraction(ObjectID env_object, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot enable environmental interaction");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(env_object);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} not found", env_object);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if object is environmental
            if (obj_data.type != ObjectType::ENVIRONMENT) {
                spdlog::warn("[FullAestheticCollisionEngine] Object {} is not environmental", env_object);
                return false;
            }
            
            // Enable environmental interaction
            obj_data.is_interactable = true;
            obj_data.interaction_type = "environment";
            
            spdlog::info("[FullAestheticCollisionEngine] Enabled environmental interaction for object {}", env_object);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to enable environmental interaction: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool grabEnvironmentalObject(ObjectID env_object, HandType hand) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot grab environmental object");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(env_object);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} not found", env_object);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if object is interactable
            if (!obj_data.is_interactable) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} is not interactable", env_object);
                return false;
            }
            
            // Check if object is grabbable
            if (!obj_data.is_grabbable) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} is not grabbable", env_object);
                return false;
            }
            
            // Grab environmental object
            obj_data.is_grabbed = true;
            obj_data.grabbing_hand = hand;
            obj_data.grab_time = getCurrentTime();
            
            // Provide haptic feedback
            provideFullHapticFeedback(hand, 0.6f, HapticType::GRAB);
            
            // Show visual feedback
            showFullCollisionHighlight(env_object, CollisionType::HAND_ENVIRONMENT);
            
            // Play audio feedback
            playCollisionAudio(env_object, CollisionType::HAND_ENVIRONMENT, 0.4f);
            
            spdlog::info("[FullAestheticCollisionEngine] Grabbed environmental object {} with hand {}", env_object, static_cast<int>(hand));
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to grab environmental object: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool moveEnvironmentalObject(ObjectID env_object, HandType hand, const glm::vec3& position) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot move environmental object");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(env_object);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} not found", env_object);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if object is grabbed by this hand
            if (!obj_data.is_grabbed || obj_data.grabbing_hand != hand) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} is not grabbed by hand {}", env_object, static_cast<int>(hand));
                return false;
            }
            
            // Check if object is movable
            if (!obj_data.is_movable) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} is not movable", env_object);
                return false;
            }
            
            // Move object to new position
            glm::vec3 old_position = obj_data.position;
            obj_data.position = position;
            
            // Calculate movement distance
            float movement_distance = glm::distance(old_position, position);
            
            // Provide haptic feedback based on movement
            float haptic_intensity = std::clamp(movement_distance * 0.3f, 0.1f, 0.6f);
            provideFullHapticFeedback(hand, haptic_intensity, HapticType::VIBRATION);
            
            // Update object state
            obj_data.last_moved_time = getCurrentTime();
            obj_data.movement_history.push_back({old_position, position, getCurrentTime()});
            
            spdlog::debug("[FullAestheticCollisionEngine] Moved environmental object {} to position ({}, {}, {})", 
                         env_object, position.x, position.y, position.z);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to move environmental object: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }
    
    bool rotateEnvironmentalObject(ObjectID env_object, HandType hand, const glm::quat& rotation) {
        if (!m_initialized) {
            spdlog::warn("[FullAestheticCollisionEngine] Not initialized, cannot rotate environmental object");
            return false;
        }
        
        try {
            auto obj_iter = m_objects.find(env_object);
            if (obj_iter == m_objects.end()) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} not found", env_object);
                return false;
            }
            
            auto& obj_data = obj_iter->second;
            
            // Check if object is grabbed by this hand
            if (!obj_data.is_grabbed || obj_data.grabbing_hand != hand) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} is not grabbed by hand {}", env_object, static_cast<int>(hand));
                return false;
            }
            
            // Check if object is rotatable
            if (!obj_data.is_rotatable) {
                spdlog::warn("[FullAestheticCollisionEngine] Environmental object {} is not rotatable", env_object);
                return false;
            }
            
            // Apply rotation to object
            glm::quat old_rotation = obj_data.rotation;
            obj_data.rotation = rotation;
            
            // Calculate rotation difference
            float rotation_angle = glm::angle(glm::inverse(old_rotation) * rotation);
            
            // Provide haptic feedback based on rotation
            float haptic_intensity = std::clamp(rotation_angle * 0.2f, 0.1f, 0.5f);
            provideFullHapticFeedback(hand, haptic_intensity, HapticType::VIBRATION);
            
            // Update object state
            obj_data.last_rotated_time = getCurrentTime();
            obj_data.rotation_history.push_back({old_rotation, rotation, getCurrentTime()});
            
            spdlog::debug("[FullAestheticCollisionEngine] Rotated environmental object {} to rotation ({}, {}, {}, {})", 
                         env_object, rotation.x, rotation.y, rotation.z, rotation.w);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[FullAestheticCollisionEngine] Failed to rotate environmental object: {}", e.what());
            m_error_state = true;
            m_last_error = e.what();
            return false;
        }
    }

private:
    // Internal data structures
    struct ObjectData {
        ObjectType type = ObjectType::STATIC;
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 size = glm::vec3(1.0f);
        PhysicsType physics_type = PhysicsType::STATIC;
        bool collision_enabled = true;
        bool physics_enabled = true;
        bool is_active = true;
        std::string name = "";
        float mass = 1.0f;
        float friction = 0.5f;
        float restitution = 0.3f;
    };
    
    // Member variables
    bool m_initialized;
    bool m_debug_mode;
    bool m_error_state;
    std::string m_last_error;
    
    // Configuration
    float m_collision_threshold;
    float m_haptic_intensity;
    float m_visual_intensity;
    float m_audio_volume;
    float m_horror_intensity;
    float m_combat_intensity;
    float m_cyberpunk_intensity;
    std::string m_current_game;
    
    // Systems
    bool m_haptic_enabled = true;
    bool m_visual_enabled = true;
    bool m_audio_enabled = true;
    
    // Data storage
    std::unordered_map<ObjectID, ObjectData> m_objects;
    std::vector<CollisionLayer> m_collision_layers;
    std::vector<CollisionResult> m_active_collisions;
    
    // Performance tracking
    CollisionPerformance m_performance;
    
    // Internal methods (to be implemented in next 200 lines)
    void initializeDefaultCollisionLayers();
    bool initializeCollisionLayers();
    bool initializePhysicsSystem();
    bool initializeHapticSystem();
    bool initializeVisualSystem();
    bool initializeAudioSystem();
    
    glm::vec3 getHandPosition(HandType hand);
    CollisionResult calculateCollision(ObjectID object, const glm::vec3& hand_position, HandType hand, CollisionType type);
    void provideHapticFeedback(HandType hand, float intensity, HapticType haptic_type);
    void showVisualFeedback(ObjectID object, CollisionType type);
    void playAudioFeedback(ObjectID object, CollisionType type, float volume);
    
    // Physics methods
    void applyGravity(ObjectID object);
    void applyForces(ObjectID object);
    void updateKinematicPhysics(ObjectID object);
    void simulateParticlePhysics(ObjectID object);
    void simulateFluidPhysics(ObjectID object);
    void simulateClothPhysics(ObjectID object);
    void simulateRopePhysics(ObjectID object);
    void simulateJointPhysics(ObjectID object);
    void simulateConstraintPhysics(ObjectID object);
    void updateCollisionDetection(float delta_time);
    void updateConstraints(float delta_time);
    
    // Haptic methods
    void applyLightHaptic(HandType hand, float intensity);
    void applyMediumHaptic(HandType hand, float intensity);
    void applyStrongHaptic(HandType hand, float intensity);
    void applyPulseHaptic(HandType hand, float intensity);
    void applyVibrationHaptic(HandType hand, float intensity);
    void applyImpactHaptic(HandType hand, float intensity);
    void applyTouchHaptic(HandType hand, float intensity);
    void applyGrabHaptic(HandType hand, float intensity);
    void applyReleaseHaptic(HandType hand, float intensity);
    void applyErrorHaptic(HandType hand, float intensity);
    void applySuccessHaptic(HandType hand, float intensity);
    void updateHapticPatterns(float delta_time);
    void updateHapticIntensity(float delta_time);
    
    // Performance methods
    void updatePerformanceMetrics(float delta_time);
    void cleanupOldCollisions();

    // Additional private methods for the next 200 lines
    void showDoorHighlight(ObjectID object, const ObjectData& obj_data);
    void showWeaponHighlight(ObjectID object, const ObjectData& obj_data);
    void showVehicleHighlight(ObjectID object, const ObjectData& obj_data);
    void showNPCHighlight(ObjectID object, const ObjectData& obj_data);
    void showEnvironmentHighlight(ObjectID object, const ObjectData& obj_data);
    void showPuzzleHighlight(ObjectID object, const ObjectData& obj_data);
    void showInventoryHighlight(ObjectID object, const ObjectData& obj_data);
    void showCraftingHighlight(ObjectID object, const ObjectData& obj_data);
    void showHackingHighlight(ObjectID object, const ObjectData& obj_data);
    void showCyberwareHighlight(ObjectID object, const ObjectData& obj_data);
    void showDefaultHighlight(ObjectID object, const ObjectData& obj_data);
    void hideVisualFeedback(ObjectID object);
    
    void updateVisualEffects(float delta_time);
    void updateHighlightAnimations(float delta_time);
    
    void playDoorAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playWeaponAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playVehicleAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playNPCAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playEnvironmentAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playPuzzleAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playInventoryAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playCraftingAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playHackingAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playCyberwareAudio(ObjectID object, const ObjectData& obj_data, float volume);
    void playDefaultAudio(ObjectID object, const ObjectData& obj_data, float volume);
    
    void updateAudioSources(float delta_time);
    void updateAudioEffects(float delta_time);
    void updateSpatialAudio(float delta_time);
    
    float getCurrentTime();
    
    // Additional data structures
    struct ExtendedObjectData : public ObjectData {
        bool is_grabbable = false;
        bool is_grabbed = false;
        HandType grabbing_hand = HandType::NEITHER;
        float grab_time = 0.0f;
        std::string grab_type = "";
        bool is_interactable = false;
        std::string interaction_type = "";
        std::string door_state = "closed";
        float rotation_progress = 0.0f;
        std::string weapon_state = "idle";
        glm::vec3 aim_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    };
    
    std::unordered_map<ObjectID, ExtendedObjectData> m_extended_objects;

    // Additional private methods for the next 200 lines
    void handleVehicleTouch(ObjectID vehicle, HandType hand);
    void handleVehicleGrab(ObjectID vehicle, HandType hand);
    void handleVehicleActivate(ObjectID vehicle, HandType hand);
    void handleVehicleToggle(ObjectID vehicle, HandType hand);
    
    void handleNPCTouch(ObjectID npc, HandType hand);
    void handleNPCSelect(ObjectID npc, HandType hand);
    void handleNPCActivate(ObjectID npc, HandType hand);
    void handleNPCMenu(ObjectID npc, HandType hand);
    
    // Additional data structures for extended functionality
    struct VehicleData : public ExtendedObjectData {
        bool is_occupied = false;
        HandType occupying_hand = HandType::NEITHER;
        float enter_time = 0.0f;
        std::string vehicle_type = "car";
        bool engine_running = false;
        float fuel_level = 1.0f;
        glm::vec3 last_position = glm::vec3(0.0f);
        glm::quat last_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    };
    
    struct NPCData : public ExtendedObjectData {
        float last_touched_time = 0.0f;
        HandType touching_hand = HandType::NEITHER;
        std::string npc_type = "civilian";
        bool is_friendly = true;
        std::string current_emotion = "neutral";
        float interaction_cooldown = 0.0f;
        std::vector<std::string> available_interactions;
    };
    
    struct EnvironmentData : public ExtendedObjectData {
        bool is_movable = false;
        bool is_rotatable = false;
        float last_moved_time = 0.0f;
        float last_rotated_time = 0.0f;
        std::vector<std::tuple<glm::vec3, glm::vec3, float>> movement_history;
        std::vector<std::tuple<glm::quat, glm::quat, float>> rotation_history;
        std::string environment_type = "prop";
        bool is_breakable = false;
        float durability = 1.0f;
    };
    
    std::unordered_map<ObjectID, VehicleData> m_vehicles;
    std::unordered_map<ObjectID, NPCData> m_npcs;
    std::unordered_map<ObjectID, EnvironmentData> m_environment_objects;
};

// Constructor and destructor
FullAestheticCollisionEngine::FullAestheticCollisionEngine() : m_impl(std::make_unique<Impl>()) {
    spdlog::info("[FullAestheticCollisionEngine] Constructor called");
}

FullAestheticCollisionEngine::~FullAestheticCollisionEngine() {
    spdlog::info("[FullAestheticCollisionEngine] Destructor called");
}

// Core initialization methods
bool FullAestheticCollisionEngine::initializeFullCollision() {
    return m_impl->initialize();
}

bool FullAestheticCollisionEngine::shutdownFullCollision() {
    m_impl->shutdown();
    return true;
}

bool FullAestheticCollisionEngine::isInitialized() const {
    return m_impl->isInitialized();
}

// Collision detection methods
CollisionResult FullAestheticCollisionEngine::detectFullCollision(ObjectID object, HandType hand, CollisionType type) {
    return m_impl->detectFullCollision(object, hand, type);
}

std::vector<CollisionResult> FullAestheticCollisionEngine::detectAllCollisions(HandType hand) {
    return m_impl->detectAllCollisions(hand);
}

bool FullAestheticCollisionEngine::updateCollisionSystem(float delta_time) {
    return m_impl->updateCollisionSystem(delta_time);
}

// Physics simulation methods
void FullAestheticCollisionEngine::simulateFullPhysics(ObjectID object, PhysicsType physics_type) {
    m_impl->simulateFullPhysics(object, physics_type);
}

void FullAestheticCollisionEngine::simulateAllPhysics(float delta_time) {
    m_impl->simulateAllPhysics(delta_time);
}

bool FullAestheticCollisionEngine::updatePhysicsSystem(float delta_time) {
    return m_impl->updatePhysicsSystem(delta_time);
}

// Haptic feedback methods
void FullAestheticCollisionEngine::provideFullHapticFeedback(HandType hand, float intensity, HapticType haptic_type) {
    m_impl->provideFullHapticFeedback(hand, intensity, haptic_type);
}

void FullAestheticCollisionEngine::updateHapticSystem(float delta_time) {
    m_impl->updateHapticSystem(delta_time);
}

bool FullAestheticCollisionEngine::isHapticEnabled() const {
    return m_impl->isHapticEnabled();
}

// Visual feedback methods
void FullAestheticCollisionEngine::showFullCollisionHighlight(ObjectID object, CollisionType collision_type) {
    m_impl->showFullCollisionHighlight(object, collision_type);
}

void FullAestheticCollisionEngine::updateVisualSystem(float delta_time) {
    m_impl->updateVisualSystem(delta_time);
}

bool FullAestheticCollisionEngine::isVisualEnabled() const {
    return m_impl->isVisualEnabled();
}

// Audio feedback methods
void FullAestheticCollisionEngine::playCollisionAudio(ObjectID object, CollisionType collision_type, float volume) {
    m_impl->playCollisionAudio(object, collision_type, volume);
}

void FullAestheticCollisionEngine::updateAudioSystem(float delta_time) {
    m_impl->updateAudioSystem(delta_time);
}

bool FullAestheticCollisionEngine::isAudioEnabled() const {
    return m_impl->isAudioEnabled();
}

// VR Transformer method implementations
bool FullAestheticCollisionEngine::enableDoorHandleGrabbing(ObjectID door, HandType hand) {
    return m_impl->enableDoorHandleGrabbing(door, hand);
}

bool FullAestheticCollisionEngine::grabDoorHandle(ObjectID door, HandType hand) {
    return m_impl->grabDoorHandle(door, hand);
}

bool FullAestheticCollisionEngine::releaseDoorHandle(ObjectID door, HandType hand) {
    return m_impl->releaseDoorHandle(door, hand);
}

bool FullAestheticCollisionEngine::rotateDoorHandle(ObjectID door, HandType hand, float rotation) {
    return m_impl->rotateDoorHandle(door, hand, rotation);
}

bool FullAestheticCollisionEngine::enableWeaponInteraction(ObjectID weapon, HandType hand) {
    return m_impl->enableWeaponInteraction(weapon, hand);
}

bool FullAestheticCollisionEngine::grabWeapon(ObjectID weapon, HandType hand) {
    return m_impl->grabWeapon(weapon, hand);
}

bool FullAestheticCollisionEngine::releaseWeapon(ObjectID weapon, HandType hand) {
    return m_impl->releaseWeapon(weapon, hand);
}

bool FullAestheticCollisionEngine::aimWeapon(ObjectID weapon, HandType hand, const glm::vec3& direction) {
    return m_impl->aimWeapon(weapon, hand, direction);
}

// Additional public method implementations
bool FullAestheticCollisionEngine::enableVehicleInteraction(ObjectID vehicle, HandType hand) {
    return m_impl->enableVehicleInteraction(vehicle, hand);
}

bool FullAestheticCollisionEngine::enterVehicle(ObjectID vehicle, HandType hand) {
    return m_impl->enterVehicle(vehicle, hand);
}

bool FullAestheticCollisionEngine::exitVehicle(ObjectID vehicle, HandType hand) {
    return m_impl->exitVehicle(vehicle, hand);
}

bool FullAestheticCollisionEngine::interactWithVehicle(ObjectID vehicle, HandType hand, InteractionType interaction) {
    return m_impl->interactWithVehicle(vehicle, hand, interaction);
}

bool FullAestheticCollisionEngine::enableNPCInteraction(ObjectID npc, HandType hand) {
    return m_impl->enableNPCInteraction(npc, hand);
}

bool FullAestheticCollisionEngine::touchNPC(ObjectID npc, HandType hand) {
    return m_impl->touchNPC(npc, hand);
}

bool FullAestheticCollisionEngine::interactWithNPC(ObjectID npc, HandType hand, InteractionType interaction) {
    return m_impl->interactWithNPC(npc, hand, interaction);
}

bool FullAestheticCollisionEngine::enableEnvironmentalInteraction(ObjectID env_object, HandType hand) {
    return m_impl->enableEnvironmentalInteraction(env_object, hand);
}

bool FullAestheticCollisionEngine::grabEnvironmentalObject(ObjectID env_object, HandType hand) {
    return m_impl->grabEnvironmentalObject(env_object, hand);
}

bool FullAestheticCollisionEngine::moveEnvironmentalObject(ObjectID env_object, HandType hand, const glm::vec3& position) {
    return m_impl->moveEnvironmentalObject(env_object, hand, position);
}

bool FullAestheticCollisionEngine::rotateEnvironmentalObject(ObjectID env_object, HandType hand, const glm::quat& rotation) {
    return m_impl->rotateEnvironmentalObject(env_object, hand, rotation);
}

} // namespace uevr