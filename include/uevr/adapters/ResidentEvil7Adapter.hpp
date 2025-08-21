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
class REEngineInterface;

// Resident Evil 7 specific enums
enum class RE7ObjectType {
    WEAPON,                 // Weapons and firearms
    AMMO,                   // Ammunition
    MEDICAL,                // Medical items and herbs
    KEY_ITEM,               // Key items and puzzle pieces
    PUZZLE,                 // Puzzle objects
    DOOR,                   // Doors and entrances
    CONTAINER,              // Containers and storage
    NPC,                    // Non-player characters
    ENEMY,                  // Enemies and monsters
    ENVIRONMENT,            // Environmental objects
    LIGHT_SOURCE,           // Light sources and switches
    AUDIO_SOURCE,           // Audio sources and triggers
    TRIGGER,                // Game triggers and events
    COLLECTIBLE,            // Collectible items
    SAVE_POINT,             // Save points
    TYPEWRITER,             // Typewriter save stations
    MERCHANT,               // Merchant interface
    CRAFTING,               // Crafting interface
    INVENTORY,              // Inventory items
    STORY_OBJECT            // Story-related objects
};

// Resident Evil 7 interaction types
enum class RE7InteractionType {
    EXAMINE,                // Examine object
    PICK_UP,                // Pick up item
    USE,                    // Use item
    COMBINE,                // Combine items
    CRAFT,                  // Craft item
    SAVE,                   // Save game
    BUY,                    // Buy item
    SELL,                   // Sell item
    REPAIR,                 // Repair item
    UPGRADE,                // Upgrade item
    OPEN,                   // Open container/door
    CLOSE,                  // Close container/door
    LOCK,                   // Lock object
    UNLOCK,                 // Unlock object
    BREAK,                  // Break object
    FIX,                    // Fix object
    ACTIVATE,               // Activate mechanism
    DEACTIVATE,             // Deactivate mechanism
    MARK,                   // Mark location
    SEARCH                  // Search area
};

// Resident Evil 7 game state
enum class RE7GameState {
    MAIN_MENU,              // Main menu
    IN_GAME,                // In game
    INVENTORY,              // Inventory menu
    MAP,                    // Map view
    DIALOGUE,               // Dialogue system
    COMBAT,                 // Combat mode
    STEALTH,                // Stealth mode
    PUZZLE,                 // Puzzle solving
    CRAFTING,               // Crafting mode
    SHOPPING,               // Shopping mode
    SAVING,                 // Saving game
    LOADING,                // Loading screen
    PAUSED,                 // Game paused
    CUTSCENE,               // Cutscene playing
    ERROR                   // Error state
};

// Resident Evil 7 horror settings
enum class RE7HorrorLevel {
    NONE,                   // No horror elements
    LIGHT,                  // Light horror
    MEDIUM,                 // Medium horror
    HIGH,                   // High horror
    EXTREME,                // Extreme horror
    ADAPTIVE                // Adaptive horror
};

// Resident Evil 7 object data structure
struct RE7ObjectData {
    uint64_t id = 0;
    RE7ObjectType type = RE7ObjectType::ENVIRONMENT;
    std::string name = "";
    std::string description = "";
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool is_interactable = false;
    bool is_pickable = false;
    bool is_usable = false;
    bool is_breakable = false;
    bool is_locked = false;
    bool is_hidden = false;
    float health = 100.0f;
    float max_health = 100.0f;
    float value = 0.0f;
    float rarity = 0.0f;
    std::string faction = "";
    std::string quality = "";
    std::vector<std::string> tags;
    std::unordered_map<std::string, float> stats;
    bool is_quest_item = false;
    bool is_story_item = false;
    bool is_collectible = false;
    std::string icon_path = "";
    std::string model_path = "";
    std::string audio_path = "";
};

// Resident Evil 7 game profile
struct RE7GameProfile {
    std::string version = "1.0";
    std::string save_file = "";
    std::string difficulty = "Normal";
    RE7GameState current_state = RE7GameState::MAIN_MENU;
    RE7HorrorLevel horror_level = RE7HorrorLevel::MEDIUM;
    bool adaptive_horror = true;
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

// Main Resident Evil 7 Adapter class
class ResidentEvil7Adapter {
public:
    // Constructor and destructor
    ResidentEvil7Adapter();
    virtual ~ResidentEvil7Adapter();

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
    virtual bool createGameProfile(const RE7GameProfile& profile);
    virtual bool updateGameProfile(const RE7GameProfile& profile);
    virtual RE7GameProfile getCurrentGameProfile() const;

    // Object management
    virtual uint64_t registerRE7Object(const RE7ObjectData& object_data);
    virtual bool unregisterRE7Object(uint64_t object_id);
    virtual bool updateRE7Object(uint64_t object_id, const RE7ObjectData& object_data);
    virtual RE7ObjectData getRE7Object(uint64_t object_id) const;
    virtual std::vector<uint64_t> getAllRE7Objects() const;
    virtual std::vector<uint64_t> getObjectsByType(RE7ObjectType type) const;

    // VR integration
    virtual bool enableVRCollision(uint64_t object_id);
    virtual bool disableVRCollision(uint64_t object_id);
    virtual bool enableVRPhysics(uint64_t object_id);
    virtual bool disableVRPhysics(uint64_t object_id);
    virtual bool enableVRHaptics(uint64_t object_id);
    virtual bool disableVRHaptics(uint64_t object_id);

    // Game-specific interactions
    virtual bool examineObject(uint64_t object_id, HandType hand);
    virtual bool pickUpItem(uint64_t object_id, HandType hand);
    virtual bool useItem(uint64_t object_id, HandType hand);
    virtual bool combineItems(uint64_t object_id, uint64_t other_item, HandType hand);
    virtual bool craftItem(uint64_t object_id, HandType hand, const std::string& recipe);
    virtual bool saveGame(uint64_t save_point, HandType hand);
    virtual bool buyItem(uint64_t object_id, HandType hand);
    virtual bool sellItem(uint64_t object_id, HandType hand);
    virtual bool repairItem(uint64_t object_id, HandType hand);
    virtual bool upgradeItem(uint64_t object_id, HandType hand);

    // Horror-specific features
    virtual bool setHorrorLevel(RE7HorrorLevel level);
    virtual bool enableAdaptiveHorror(bool enable);
    virtual bool setComfortFeatures(bool enable);
    virtual bool adjustHorrorIntensity(float intensity);
    virtual bool applyHorrorComfortSettings();
    virtual bool handleHorrorCollision(uint64_t object_id, HandType hand, CollisionType collision);

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
    bool initializeREEngineInterface();
    bool initializeVRSystems();
    bool initializeGameObjects();
    
    void updateGameState();
    void updateGameObjects();
    void updateVRIntegration();
    
    bool validateObjectData(const RE7ObjectData& object_data);
    bool applyVRSettings(const RE7GameProfile& profile);
    bool optimizeForHorror(RE7HorrorLevel level);
    
    void updatePerformanceMetrics();
    bool checkPerformanceThresholds();
    void optimizeAdapterSystem();
    
    bool handleGameSpecificFeatures(uint64_t object_id, HandType hand, InteractionType interaction);
    bool handleRE7Interactions(uint64_t object_id, HandType hand, RE7InteractionType interaction);
    bool handleREEngineOptimizations();
};

} // namespace uevr
