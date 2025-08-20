/*
 * UEVR Full Aesthetic Collision Engine
 * 
 * Complete collision detection and interaction system for VR
 * Handles all object types with aesthetic feedback and physics simulation
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

namespace uevr {

// Forward declarations
class VRSystem;
class PhysicsEngine;
class HapticSystem;
class VisualFeedbackSystem;

// Collision types for full aesthetic collision system
enum class CollisionType {
    HAND_OBJECT,           // Hand touching any object
    HAND_DOOR,             // Hand touching door/handle
    HAND_WEAPON,           // Hand touching weapon
    HAND_VEHICLE,          // Hand touching vehicle
    HAND_NPC,              // Hand touching NPC
    HAND_ENVIRONMENT,      // Hand touching environmental object
    HAND_PUZZLE,           // Hand touching puzzle object
    HAND_INVENTORY,        // Hand touching inventory item
    HAND_CRAFTING,         // Hand touching crafting interface
    HAND_HACKING,          // Hand touching hacking interface
    HAND_CYBERWARE,        // Hand touching cyberware
    HAND_PARTICLES,        // Hand touching particle effects
    HAND_AUDIO,            // Hand touching audio sources
    HAND_LIGHTING,         // Hand touching light sources
    HAND_SHADOWS,          // Hand touching shadow objects
    HAND_REFLECTIONS,      // Hand touching reflective surfaces
    HAND_TRANSPARENCY,     // Hand touching transparent objects
    HAND_ANIMATION,        // Hand touching animated objects
    HAND_PHYSICS,          // Hand touching physics objects
    HAND_UI                // Hand touching UI elements
};

// Hand types for collision detection
enum class HandType {
    LEFT,
    RIGHT,
    BOTH,
    NEITHER
};

// Object types for collision system
enum class ObjectType {
    STATIC,                // Static objects (walls, floors)
    DYNAMIC,               // Dynamic objects (movable items)
    INTERACTIVE,           // Interactive objects (doors, switches)
    WEAPON,                // Weapon objects
    VEHICLE,               // Vehicle objects
    NPC,                   // Non-player character
    ENVIRONMENT,           // Environmental objects
    PUZZLE,                // Puzzle objects
    INVENTORY,             // Inventory items
    CRAFTING,              // Crafting materials
    HACKING,               // Hacking interfaces
    CYBERWARE,             // Cyberware implants
    PARTICLE,              // Particle effects
    AUDIO,                 // Audio sources
    LIGHTING,              // Light sources
    SHADOW,                // Shadow objects
    REFLECTION,            // Reflective surfaces
    TRANSPARENT,           // Transparent objects
    ANIMATED,              // Animated objects
    PHYSICS,               // Physics objects
    UI                     // UI elements
};

// Collision result structure
struct CollisionResult {
    bool collision_detected = false;
    ObjectType object_type = ObjectType::STATIC;
    CollisionType collision_type = CollisionType::HAND_OBJECT;
    glm::vec3 collision_point = glm::vec3(0.0f);
    glm::vec3 collision_normal = glm::vec3(0.0f);
    float collision_distance = 0.0f;
    float collision_intensity = 0.0f;
    bool can_interact = false;
    bool requires_animation = false;
    std::string interaction_hint = "";
    float haptic_intensity = 0.0f;
    bool visual_feedback = false;
    bool audio_feedback = false;
};

// Physics types for collision simulation
enum class PhysicsType {
    NONE,                  // No physics simulation
    STATIC,                // Static physics
    DYNAMIC,               // Dynamic physics
    KINEMATIC,             // Kinematic physics
    TRIGGER,               // Trigger physics
    PARTICLE,              // Particle physics
    FLUID,                 // Fluid physics
    CLOTH,                 // Cloth physics
    ROPE,                  // Rope physics
    JOINT,                 // Joint physics
    CONSTRAINT             // Constraint physics
};

// Haptic feedback types
enum class HapticType {
    NONE,                  // No haptic feedback
    LIGHT,                 // Light haptic feedback
    MEDIUM,                // Medium haptic feedback
    STRONG,                // Strong haptic feedback
    PULSE,                 // Pulsing haptic feedback
    VIBRATION,             // Vibration haptic feedback
    IMPACT,                // Impact haptic feedback
    TOUCH,                 // Touch haptic feedback
    GRAB,                  // Grab haptic feedback
    RELEASE,               // Release haptic feedback
    ERROR,                 // Error haptic feedback
    SUCCESS                // Success haptic feedback
};

// Interaction types for VR transformer
enum class InteractionType {
    NONE,                  // No interaction
    TOUCH,                 // Touch interaction
    GRAB,                  // Grab interaction
    PUSH,                  // Push interaction
    PULL,                  // Pull interaction
    ROTATE,                // Rotate interaction
    MOVE,                  // Move interaction
    ACTIVATE,              // Activate interaction
    DEACTIVATE,            // Deactivate interaction
    TOGGLE,                // Toggle interaction
    SELECT,                // Select interaction
    DESELECT,              // Deselect interaction
    OPEN,                  // Open interaction
    CLOSE,                 // Close interaction
    LOCK,                  // Lock interaction
    UNLOCK,                // Unlock interaction
    CRAFT,                 // Craft interaction
    HACK,                  // Hack interaction
    INVENTORY,             // Inventory interaction
    MENU                   // Menu interaction
};

// Interaction result structure
struct InteractionResult {
    bool success = false;
    InteractionType interaction_type = InteractionType::NONE;
    std::string message = "";
    bool requires_animation = false;
    float animation_duration = 0.0f;
    bool haptic_feedback = false;
    float haptic_intensity = 0.0f;
    bool visual_feedback = false;
    bool audio_feedback = false;
    bool game_state_changed = false;
    std::string game_state_message = "";
};

// Object ID type for collision system
using ObjectID = uint64_t;

// Performance metrics for collision system
struct CollisionPerformance {
    uint32_t total_collisions = 0;
    uint32_t active_collisions = 0;
    float collision_detection_time = 0.0f;
    float physics_simulation_time = 0.0f;
    float haptic_feedback_time = 0.0f;
    float visual_feedback_time = 0.0f;
    uint32_t objects_tracked = 0;
    uint32_t collision_layers = 0;
    float memory_usage_mb = 0.0f;
    float cpu_usage_percent = 0.0f;
    float gpu_usage_percent = 0.0f;
};

// Collision layer structure for organized collision detection
struct CollisionLayer {
    std::string name;
    uint32_t layer_id;
    bool enabled;
    std::vector<ObjectType> object_types;
    std::vector<CollisionType> collision_types;
    float collision_threshold;
    bool haptic_enabled;
    bool visual_enabled;
    bool audio_enabled;
};

// Main Full Aesthetic Collision Engine class
class FullAestheticCollisionEngine {
public:
    // Constructor and destructor
    FullAestheticCollisionEngine();
    virtual ~FullAestheticCollisionEngine();

    // Core initialization
    virtual bool initializeFullCollision();
    virtual bool shutdownFullCollision();
    virtual bool isInitialized() const;

    // Collision detection system
    virtual CollisionResult detectFullCollision(ObjectID object, HandType hand, CollisionType type);
    virtual std::vector<CollisionResult> detectAllCollisions(HandType hand);
    virtual bool updateCollisionSystem(float delta_time);

    // Physics simulation system
    virtual void simulateFullPhysics(ObjectID object, PhysicsType physics_type);
    virtual void simulateAllPhysics(float delta_time);
    virtual bool updatePhysicsSystem(float delta_time);

    // Haptic feedback system
    virtual void provideFullHapticFeedback(HandType hand, float intensity, HapticType haptic_type);
    virtual void updateHapticSystem(float delta_time);
    virtual bool isHapticEnabled() const;

    // Visual feedback system
    virtual void showFullCollisionHighlight(ObjectID object, CollisionType collision_type);
    virtual void updateVisualSystem(float delta_time);
    virtual bool isVisualEnabled() const;

    // Audio feedback system
    virtual void playCollisionAudio(ObjectID object, CollisionType collision_type, float volume);
    virtual void updateAudioSystem(float delta_time);
    virtual bool isAudioEnabled() const;

    // VR Transformer features - Door handle grabbing
    virtual bool enableDoorHandleGrabbing(ObjectID door, HandType hand);
    virtual bool grabDoorHandle(ObjectID door, HandType hand);
    virtual bool releaseDoorHandle(ObjectID door, HandType hand);
    virtual bool rotateDoorHandle(ObjectID door, HandType hand, float rotation);

    // VR Transformer features - Weapon interaction
    virtual bool enableWeaponInteraction(ObjectID weapon, HandType hand);
    virtual bool grabWeapon(ObjectID weapon, HandType hand);
    virtual bool releaseWeapon(ObjectID weapon, HandType hand);
    virtual bool aimWeapon(ObjectID weapon, HandType hand, const glm::vec3& direction);

    // VR Transformer features - Vehicle interaction
    virtual bool enableVehicleInteraction(ObjectID vehicle, HandType hand);
    virtual bool enterVehicle(ObjectID vehicle, HandType hand);
    virtual bool exitVehicle(ObjectID vehicle, HandType hand);
    virtual bool interactWithVehicle(ObjectID vehicle, HandType hand, InteractionType interaction);

    // VR Transformer features - NPC interaction
    virtual bool enableNPCInteraction(ObjectID npc, HandType hand);
    virtual bool touchNPC(ObjectID npc, HandType hand);
    virtual bool interactWithNPC(ObjectID npc, HandType hand, InteractionType interaction);

    // VR Transformer features - Environmental interaction
    virtual bool enableEnvironmentalInteraction(ObjectID env_object, HandType hand);
    virtual bool grabEnvironmentalObject(ObjectID env_object, HandType hand);
    virtual bool moveEnvironmentalObject(ObjectID env_object, HandType hand, const glm::vec3& position);
    virtual bool rotateEnvironmentalObject(ObjectID env_object, HandType hand, const glm::quat& rotation);

    // VR Transformer features - Puzzle interaction
    virtual bool enablePuzzleInteraction(ObjectID puzzle, HandType hand);
    virtual bool solvePuzzle(ObjectID puzzle, HandType hand, const std::string& solution);
    virtual bool interactWithPuzzle(ObjectID puzzle, HandType hand, InteractionType interaction);

    // VR Transformer features - Inventory manipulation
    virtual bool enableInventoryManipulation(ObjectID item, HandType hand);
    virtual bool selectInventoryItem(ObjectID item, HandType hand);
    virtual bool useInventoryItem(ObjectID item, HandType hand);
    virtual bool dropInventoryItem(ObjectID item, HandType hand);

    // VR Transformer features - Crafting system
    virtual bool enableCraftingInteraction(ObjectID crafting_station, HandType hand);
    virtual bool craftItem(ObjectID crafting_station, HandType hand, const std::string& recipe);
    virtual bool interactWithCrafting(ObjectID crafting_station, HandType hand, InteractionType interaction);

    // VR Transformer features - Hacking system
    virtual bool enableHackingInteraction(ObjectID hacking_interface, HandType hand);
    virtual bool hackSystem(ObjectID hacking_interface, HandType hand, const std::string& hack_type);
    virtual bool interactWithHacking(ObjectID hacking_interface, HandType hand, InteractionType interaction);

    // VR Transformer features - Cyberware interaction
    virtual bool enableCyberwareInteraction(ObjectID cyberware, HandType hand);
    virtual bool activateCyberware(ObjectID cyberware, HandType hand);
    virtual bool interactWithCyberware(ObjectID cyberware, HandType hand, InteractionType interaction);

    // Collision layer management
    virtual bool addCollisionLayer(const CollisionLayer& layer);
    virtual bool removeCollisionLayer(uint32_t layer_id);
    virtual bool enableCollisionLayer(uint32_t layer_id);
    virtual bool disableCollisionLayer(uint32_t layer_id);
    virtual std::vector<CollisionLayer> getCollisionLayers() const;

    // Object management
    virtual bool registerObject(ObjectID object, ObjectType type, const glm::vec3& position, const glm::quat& rotation);
    virtual bool unregisterObject(ObjectID object);
    virtual bool updateObjectTransform(ObjectID object, const glm::vec3& position, const glm::quat& rotation);
    virtual bool setObjectCollisionEnabled(ObjectID object, bool enabled);
    virtual bool setObjectPhysicsEnabled(ObjectID object, bool enabled);

    // Performance monitoring
    virtual CollisionPerformance getPerformanceMetrics() const;
    virtual void resetPerformanceMetrics();
    virtual bool optimizePerformance();

    // Configuration
    virtual bool loadConfiguration(const std::string& config_file);
    virtual bool saveConfiguration(const std::string& config_file);
    virtual bool setCollisionThreshold(float threshold);
    virtual bool setHapticIntensity(float intensity);
    virtual bool setVisualIntensity(float intensity);
    virtual bool setAudioVolume(float volume);

    // Game-specific collision features
    virtual bool enableGameSpecificCollision(const std::string& game_name);
    virtual bool disableGameSpecificCollision();
    virtual std::string getCurrentGame() const;

    // Horror-specific features (for RE7)
    virtual bool enableHorrorCollision();
    virtual bool disableHorrorCollision();
    virtual bool setHorrorIntensity(float intensity);

    // Combat-specific features (for MHW)
    virtual bool enableCombatCollision();
    virtual bool disableCombatCollision();
    virtual bool setCombatIntensity(float intensity);

    // Cyberpunk-specific features
    virtual bool enableCyberpunkCollision();
    virtual bool disableCyberpunkCollision();
    virtual bool setCyberpunkIntensity(float intensity);

    // Error handling and recovery
    virtual bool handleError(const std::string& error_message);
    virtual bool recoverFromError();
    virtual std::string getLastError() const;
    virtual bool isErrorState() const;

    // Debug and development
    virtual bool enableDebugMode();
    virtual bool disableDebugMode();
    virtual bool isDebugMode() const;
    virtual void logDebugInfo(const std::string& message);
    virtual std::vector<std::string> getDebugLog() const;

private:
    // Private implementation details
    class Impl;
    std::unique_ptr<Impl> m_impl;

    // Internal helper methods
    bool initializeCollisionLayers();
    bool initializePhysicsSystem();
    bool initializeHapticSystem();
    bool initializeVisualSystem();
    bool initializeAudioSystem();
    
    CollisionResult processCollision(ObjectID object, HandType hand, CollisionType type);
    bool validateCollision(const CollisionResult& collision);
    bool applyCollisionResponse(const CollisionResult& collision);
    
    void updatePerformanceMetrics(float delta_time);
    bool checkPerformanceThresholds();
    void optimizeCollisionSystem();
    
    bool handleGameSpecificCollision(ObjectID object, HandType hand, CollisionType type);
    bool handleHorrorCollision(ObjectID object, HandType hand, CollisionType type);
    bool handleCombatCollision(ObjectID object, HandType hand, CollisionType type);
    bool handleCyberpunkCollision(ObjectID object, HandType hand, CollisionType type);
};

} // namespace uevr