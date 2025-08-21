#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "uevr/vr/FullAestheticCollisionEngine.hpp"
#include "uevr/vr/FullPhysicsIntegration.hpp"
#include "uevr/vr/VRTransformer.hpp"

namespace uevr {

// Forward declarations
class VRSystem;
class MTFrameworkInterface;

// Monster Hunter World specific enums
enum class MHWObjectType {
    WEAPON,                 // Weapons and equipment
    ARMOR,                  // Armor pieces
    ITEM,                   // Consumable items
    MATERIAL,               // Crafting materials
    MONSTER,                // Monster entities
    NPC,                    // Non-player characters
    ENVIRONMENT,            // Environmental objects
    CRAFTING_STATION,       // Crafting stations
    SHOP,                   // Shop interfaces
    QUEST_BOARD,            // Quest board
    CANTEEN,                // Canteen interface
    TENT,                   // Tent and camp
    MAP,                    // Map objects
    GATHERING_POINT,        // Resource gathering points
    MINING_SPOT,            // Mining locations
    BONE_PILE,              // Bone pile locations
    PLANT,                  // Plant resources
    INSECT,                 // Insect resources
    FISH,                   // Fish resources
    TRACKS                  // Monster tracks
};

// Monster Hunter World interaction types
enum class MHWInteractionType {
    GATHER,                 // Gather resource
    MINE,                   // Mine resource
    CRAFT,                  // Craft item
    UPGRADE,                // Upgrade equipment
    EQUIP,                  // Equip item
    UNEQUIP,                // Unequip item
    USE,                    // Use item
    COMBINE,                // Combine items
    BUY,                    // Buy item
    SELL,                   // Sell item
    ACCEPT_QUEST,           // Accept quest
    COMPLETE_QUEST,         // Complete quest
    TALK,                   // Talk to NPC
    COOK,                   // Cook food
    EAT,                    // Eat food
    SLEEP,                  // Sleep at camp
    FAST_TRAVEL,            // Fast travel
    MARK_LOCATION,          // Mark location on map
    TRACK_MONSTER,          // Track monster
    CAPTURE_MONSTER         // Capture monster
};

// Monster Hunter World game state
enum class MHWGameState {
    MAIN_MENU,              // Main menu
    IN_GAME,                // In game
    INVENTORY,              // Inventory menu
    MAP,                    // Map view
    DIALOGUE,               // Dialogue system
    COMBAT,                 // Combat mode
    EXPLORATION,            // Exploration mode
    CRAFTING,               // Crafting mode
    SHOPPING,               // Shopping mode
    QUEST_MENU,             // Quest menu
    CANTEEN,                // Canteen menu
    CAMP,                   // Camp menu
    PAUSED,                 // Game paused
    LOADING,                // Loading screen
    SAVING,                 // Saving game
    ERROR                   // Error state
};

// Monster Hunter World combat settings
enum class MHWCombatLevel {
    NONE,                   // No combat
    LIGHT,                  // Light combat
    MEDIUM,                 // Medium combat
    HEAVY,                  // Heavy combat
    EXTREME,                // Extreme combat
    ADAPTIVE                // Adaptive combat
};

// Monster Hunter World object data structure
struct MHWObjectData {
    uint64_t id = 0;
    MHWObjectType type = MHWObjectType::ENVIRONMENT;
    std::string name = "";
    std::string description = "";
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool is_interactable = false;
    bool is_gatherable = false;
    bool is_craftable = false;
    bool is_equippable = false;
    bool is_usable = false;
    bool is_breakable = false;
    float health = 100.0f;
    float max_health = 100.0f;
    float value = 0.0f;
    float rarity = 0.0f;
    std::string rarity_tier = "";
    std::string element = "";
    std::string weapon_type = "";
    std::string armor_type = "";
    std::vector<std::string> skills;
    std::vector<std::string> tags;
    std::unordered_map<std::string, float> stats;
    bool is_quest_item = false;
    bool is_story_item = false;
    bool is_collectible = false;
    std::string icon_path = "";
    std::string model_path = "";
    std::string audio_path = "";
};

// Monster Hunter World game profile
struct MHWGameProfile {
    std::string version = "1.0";
    std::string save_file = "";
    std::string difficulty = "Normal";
    MHWGameState current_state = MHWGameState::MAIN_MENU;
    MHWCombatLevel combat_level = MHWCombatLevel::MEDIUM;
    bool adaptive_combat = true;
    bool comfort_features = true;
    float graphics_quality = 0.5f;
    float audio_quality = 0.5f;
    float haptic_intensity = 0.5f;
    float vr_comfort_level = 0.5f;
    bool subtitles_enabled = true;
    std::string language = "English";
    std::unordered_map<std::string, bool> game_settings;
    std::unordered_map<std::string, float> vr_settings;
};

// Main Monster Hunter World Adapter class
class MonsterHunterWorldAdapter {
public:
    // Constructor and destructor
    MonsterHunterWorldAdapter();
    virtual ~MonsterHunterWorldAdapter();

    // Core initialization
    virtual bool initializeAdapter();
    virtual bool shutdownAdapter();
    virtual bool isInitialized() const;

    // Game detection and connection
    virtual bool detectGame();
    virtual bool connectToGame();
    virtual bool disconnectFromGame();
    virtual bool isGameRunning() const;
    virtual bool isGameConnected() const;

    // Game profile management
    virtual bool loadGameProfile(const std::string& profile_path);
    virtual bool saveGameProfile(const std::string& profile_path);
    virtual bool createGameProfile(const MHWGameProfile& profile);
    virtual bool updateGameProfile(const MHWGameProfile& profile);
    virtual MHWGameProfile getCurrentGameProfile() const;

    // Object management
    virtual uint64_t registerMHWObject(const MHWObjectData& object_data);
    virtual bool unregisterMHWObject(uint64_t object_id);
    virtual bool updateMHWObject(uint64_t object_id, const MHWObjectData& object_data);
    virtual MHWObjectData getMHWObject(uint64_t object_id) const;
    virtual std::vector<uint64_t> getAllMHWObjects() const;
    virtual std::vector<uint64_t> getObjectsByType(MHWObjectType type) const;

    // VR integration
    virtual bool enableVRCollision(uint64_t object_id);
    virtual bool disableVRCollision(uint64_t object_id);
    virtual bool enableVRPhysics(uint64_t object_id);
    virtual bool disableVRPhysics(uint64_t object_id);
    virtual bool enableVRHaptics(uint64_t object_id);
    virtual bool disableVRHaptics(uint64_t object_id);

    // Game-specific interactions
    virtual bool gatherResource(uint64_t object_id, HandType hand);
    virtual bool mineResource(uint64_t object_id, HandType hand);
    virtual bool craftItem(uint64_t object_id, HandType hand, const std::string& recipe);
    virtual bool upgradeEquipment(uint64_t object_id, HandType hand);
    virtual bool equipItem(uint64_t object_id, HandType hand);
    virtual bool unequipItem(uint64_t object_id, HandType hand);
    virtual bool useItem(uint64_t object_id, HandType hand);
    virtual bool combineItems(uint64_t object_id, uint64_t other_item, HandType hand);
    virtual bool buyItem(uint64_t object_id, HandType hand);
    virtual bool sellItem(uint64_t object_id, HandType hand);

    // Combat-specific features
    virtual bool setCombatLevel(MHWCombatLevel level);
    virtual bool enableAdaptiveCombat(bool enable);
    virtual bool setComfortFeatures(bool enable);
    virtual bool adjustCombatIntensity(float intensity);
    virtual bool applyCombatSettings();
    virtual bool handleCombatCollision(uint64_t object_id, HandType hand, CollisionType collision);

    // Performance optimization
    virtual bool setGraphicsQuality(float quality);
    virtual bool setAudioQuality(float quality);
    virtual bool optimizeForQuest3();
    virtual bool enableAdaptivePerformance(bool enable);
    virtual bool setFrameRateLimit(uint32_t fps);

    // VR comfort features
    virtual bool setVRComfortLevel(float level);
    virtual bool enableVRComfortFeatures(bool enable);
    virtual bool setHapticIntensity(float intensity);
    virtual bool enableVRSpatialAudio(bool enable);
    virtual bool enableVRVisualEffects(bool enable);
    virtual bool enableSubtitles(bool enable);

    // Error handling and recovery
    virtual bool handleAdapterError(const std::string& error_message);
    virtual bool recoverFromError();
    virtual std::string getLastError() const;
    virtual bool isErrorState() const;

    // Debug and development
    virtual bool enableDebugMode(bool enable);
    virtual bool isDebugMode() const;
    virtual void logDebugInfo(const std::string& message);
    virtual std::vector<std::string> getDebugLog() const;

private:
    // Private implementation details
    class Impl;
    std::unique_ptr<Impl> m_impl;

    // Internal helper methods
    bool initializeMTFrameworkInterface();
    bool initializeVRSystems();
    bool initializeGameObjects();
    
    void updateGameState();
    void updateGameObjects();
    void updateVRIntegration();
    
    bool validateObjectData(const MHWObjectData& object_data);
    bool applyVRSettings(const MHWGameProfile& profile);
    bool optimizeForCombat(MHWCombatLevel level);
    
    void updatePerformanceMetrics();
    bool checkPerformanceThresholds();
    void optimizeAdapterSystem();
    
    bool handleGameSpecificFeatures(uint64_t object_id, HandType hand, InteractionType interaction);
    bool handleMHWInteractions(uint64_t object_id, HandType hand, MHWInteractionType interaction);
    bool handleMTFrameworkOptimizations();
};

} // namespace uevr
