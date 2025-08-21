#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "FullAestheticCollisionEngine.hpp"
#include "FullPhysicsIntegration.hpp"

namespace uevr {

// Forward declarations
class VRSystem;
class GameAdapter;

// VR Transformer game types
enum class GameType {
    UNKNOWN,                // Unknown game type
    CYBERPUNK_2077,        // Cyberpunk 2077 (REDengine 4)
    RESIDENT_EVIL_7,       // Resident Evil 7 (RE Engine)
    MONSTER_HUNTER_WORLD,  // Monster Hunter World (MT Framework)
    UNREAL_ENGINE,         // Unreal Engine games
    UNITY_ENGINE,          // Unity Engine games
    FROSTBITE_ENGINE,      // Frostbite Engine games
    SOURCE_ENGINE          // Source Engine games
};

// VR Transformer interaction modes
enum class InteractionMode {
    NORMAL,                 // Normal interaction mode
    HORROR,                 // Horror game mode (RE7)
    COMBAT,                 // Combat game mode (MHW)
    CYBERPUNK,             // Cyberpunk mode (Cyberpunk 2077)
    SPORTS,                 // Sports game mode
    PUZZLE,                 // Puzzle game mode
    ADVENTURE,             // Adventure game mode
    SIMULATION             // Simulation game mode
};

// VR Transformer comfort settings
enum class ComfortLevel {
    NONE,                   // No comfort features
    LIGHT,                  // Light comfort features
    MEDIUM,                 // Medium comfort features
    HIGH,                   // High comfort features
    MAXIMUM                 // Maximum comfort features
};

// VR Transformer performance profiles
enum class PerformanceProfile {
    LOW_END,                // Low-end hardware
    MID_RANGE,              // Mid-range hardware
    HIGH_END,               // High-end hardware
    ULTRA,                  // Ultra hardware
    QUEST_3_OPTIMIZED      // Meta Quest 3 optimized
};

// VR Transformer game-specific features
enum class GameFeature {
    DOOR_HANDLES,           // Door handle grabbing
    WEAPON_INTERACTION,     // Weapon handling
    VEHICLE_CONTROLS,       // Vehicle interaction
    NPC_DIALOGUE,           // NPC interaction
    ENVIRONMENTAL_OBJECTS,  // Environmental manipulation
    INVENTORY_SYSTEM,       // Inventory management
    CRAFTING_SYSTEM,        // Crafting interface
    HACKING_INTERFACE,      // Hacking systems
    CYBERWARE_INTEGRATION,  // Cyberware systems
    HORROR_ATMOSPHERE,      // Horror game features
    COMBAT_MECHANICS,       // Combat system
    PUZZLE_SOLVING,         // Puzzle mechanics
    AUDIO_SPATIALIZATION,   // 3D audio
    HAPTIC_FEEDBACK,        // Haptic systems
    VISUAL_EFFECTS          // Visual enhancements
};

// VR Transformer game profile structure
struct GameProfile {
    GameType game_type = GameType::UNKNOWN;
    std::string game_name = "";
    std::string engine_name = "";
    std::string version = "";
    std::vector<GameFeature> supported_features;
    InteractionMode default_mode = InteractionMode::NORMAL;
    ComfortLevel comfort_level = ComfortLevel::MEDIUM;
    PerformanceProfile performance_profile = PerformanceProfile::MID_RANGE;
    bool horror_optimized = false;
    bool combat_optimized = false;
    bool cyberpunk_optimized = false;
    float interaction_intensity = 0.5f;
    float haptic_intensity = 0.5f;
    float visual_intensity = 0.5f;
    float audio_intensity = 0.5f;
    std::string config_file = "";
    std::string profile_file = "";
};

// VR Transformer interaction result
struct TransformerInteractionResult {
    bool success = false;
    GameType game_type = GameType::UNKNOWN;
    InteractionMode mode = InteractionMode::NORMAL;
    std::string message = "";
    bool requires_animation = false;
    float animation_duration = 0.0f;
    bool haptic_feedback = false;
    float haptic_intensity = 0.0f;
    bool visual_feedback = false;
    bool audio_feedback = false;
    bool game_state_changed = false;
    std::string game_state_message = "";
    std::vector<GameFeature> used_features;
    float performance_impact = 0.0f;
};

// VR Transformer performance metrics
struct TransformerPerformance {
    uint32_t active_interactions = 0;
    uint32_t total_interactions = 0;
    float interaction_time = 0.0f;
    float collision_time = 0.0f;
    float physics_time = 0.0f;
    float haptic_time = 0.0f;
    float visual_time = 0.0f;
    float audio_time = 0.0f;
    uint32_t games_supported = 0;
    uint32_t features_active = 0;
    float memory_usage_mb = 0.0f;
    float cpu_usage_percent = 0.0f;
    float gpu_usage_percent = 0.0f;
    uint32_t frames_per_second = 0;
    float average_frame_time = 0.0f;
    float min_frame_time = 0.0f;
    float max_frame_time = 0.0f;
};

// Main VR Transformer class
class VRTransformer {
public:
    // Constructor and destructor
    VRTransformer();
    virtual ~VRTransformer();

    // Core initialization
    virtual bool initializeTransformer();
    virtual bool shutdownTransformer();
    virtual bool isInitialized() const;

    // Game profile management
    virtual bool loadGameProfile(const std::string& game_name);
    virtual bool saveGameProfile(const std::string& game_name);
    virtual bool createGameProfile(const GameProfile& profile);
    virtual bool updateGameProfile(const GameProfile& profile);
    virtual bool deleteGameProfile(const std::string& game_name);
    virtual GameProfile getCurrentGameProfile() const;
    virtual std::vector<std::string> getAvailableGames() const;

    // Game detection and auto-configuration
    virtual bool detectGameType(const std::string& executable_name);
    virtual bool autoConfigureGame(const std::string& game_name);
    virtual bool validateGameConfiguration(const std::string& game_name);

    // VR Transformer features
    virtual bool enableGameFeature(GameFeature feature);
    virtual bool disableGameFeature(GameFeature feature);
    virtual bool isGameFeatureEnabled(GameFeature feature) const;
    virtual std::vector<GameFeature> getEnabledFeatures() const;

    // Interaction mode management
    virtual bool setInteractionMode(InteractionMode mode);
    virtual bool setComfortLevel(ComfortLevel level);
    virtual bool setPerformanceProfile(PerformanceProfile profile);
    virtual InteractionMode getCurrentInteractionMode() const;
    virtual ComfortLevel getCurrentComfortLevel() const;
    virtual PerformanceProfile getCurrentPerformanceProfile() const;

    // Game-specific VR interactions
    virtual TransformerInteractionResult interactWithGame(ObjectID object, HandType hand, InteractionType interaction);
    virtual bool handleGameSpecificCollision(ObjectID object, HandType hand, CollisionType collision);
    virtual bool handleGameSpecificPhysics(ObjectID object, InteractionType interaction);

    // Horror game features (RE7)
    virtual bool enableHorrorMode();
    virtual bool disableHorrorMode();
    virtual bool setHorrorIntensity(float intensity);
    virtual bool applyHorrorComfortSettings();
    virtual bool handleHorrorCollision(ObjectID object, HandType hand, CollisionType collision);

    // Combat game features (MHW)
    virtual bool enableCombatMode();
    virtual bool disableCombatMode();
    virtual bool setCombatIntensity(float intensity);
    virtual bool applyCombatSettings();
    virtual bool handleCombatCollision(ObjectID object, HandType hand, CollisionType collision);

    // Cyberpunk game features (Cyberpunk 2077)
    virtual bool enableCyberpunkMode();
    virtual bool disableCyberpunkMode();
    virtual bool setCyberpunkIntensity(float intensity);
    virtual bool applyCyberpunkSettings();
    virtual bool handleCyberpunkCollision(ObjectID object, HandType hand, CollisionType collision);

    // Engine-specific optimizations
    virtual bool optimizeForUnrealEngine();
    virtual bool optimizeForUnityEngine();
    virtual bool optimizeForFrostbiteEngine();
    virtual bool optimizeForSourceEngine();
    virtual bool optimizeForQuest3();

    // Performance monitoring and optimization
    virtual TransformerPerformance getTransformerPerformance() const;
    virtual void resetPerformanceMetrics();
    virtual bool optimizePerformance();
    virtual bool enableProfiling(bool enable);
    virtual bool enableDebugMode(bool enable);

    // Configuration and settings
    virtual bool loadTransformerConfiguration(const std::string& config_file);
    virtual bool saveTransformerConfiguration(const std::string& config_file);
    virtual bool setInteractionIntensity(float intensity);
    virtual bool setHapticIntensity(float intensity);
    virtual bool setVisualIntensity(float intensity);
    virtual bool setAudioIntensity(float intensity);

    // Error handling and recovery
    virtual bool handleTransformerError(const std::string& error_message);
    virtual bool recoverFromError();
    virtual std::string getLastError() const;
    virtual bool isErrorState() const;

    // Debug and development
    virtual bool enableTransformerDebugMode();
    virtual bool disableTransformerDebugMode();
    virtual bool isTransformerDebugMode() const;
    virtual void logTransformerDebugInfo(const std::string& message);
    virtual std::vector<std::string> getTransformerDebugLog() const;

private:
    // Private implementation details
    class Impl;
    std::unique_ptr<Impl> m_impl;

    // Internal helper methods
    bool initializeGameProfiles();
    bool initializeCollisionEngine();
    bool initializePhysicsEngine();
    bool initializeGameFeatures();
    
    void updateGameProfiles();
    void updateGameFeatures();
    void updatePerformanceMetrics();
    
    bool validateGameFeature(GameFeature feature, const GameProfile& profile);
    bool applyGameSettings(const GameProfile& profile);
    bool optimizeForGameType(GameType game_type);
    
    void updatePerformanceMetrics(float delta_time);
    bool checkPerformanceThresholds();
    void optimizeTransformerSystem();
    
    bool handleGameSpecificFeatures(ObjectID object, HandType hand, InteractionType interaction);
    bool handleEngineSpecificOptimizations(GameType game_type);
    bool handlePerformanceOptimizations(PerformanceProfile profile);
};

} // namespace uevr
