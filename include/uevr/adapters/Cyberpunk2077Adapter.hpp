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
class REDengine4Interface;

// Cyberpunk 2077 specific enums
enum class CyberpunkObjectType {
    CYBERWARE,              // Cyberware implants
    WEAPON,                 // Weapons and firearms
    VEHICLE,                // Vehicles and transportation
    NPC,                    // Non-player characters
    ENVIRONMENT,            // Environmental objects
    HACKING_INTERFACE,      // Hacking terminals
    INVENTORY_ITEM,         // Inventory items
    CRAFTING_STATION,       // Crafting stations
    SHOP_INTERFACE,         // Shop interfaces
    MISSION_OBJECT,         // Mission-related objects
    LORE_OBJECT,            // Lore and story objects
    COLLECTIBLE,            // Collectible items
    AMMO,                   // Ammunition
    MEDICAL,                // Medical items
    FOOD,                   // Food and consumables
    CLOTHING,               // Clothing and armor
    TOOL,                   // Tools and utilities
    CONTAINER,              // Containers and storage
    DOOR,                   // Doors and entrances
    ELEVATOR                // Elevators and lifts
};

// Cyberpunk 2077 interaction types
enum class CyberpunkInteractionType {
    HACK,                   // Hacking interaction
    CRAFT,                  // Crafting interaction
    BUY,                    // Buying items
    SELL,                   // Selling items
    EQUIP,                  // Equipping items
    UNEQUIP,                // Unequipping items
    UPGRADE,                // Upgrading items
    INSTALL,                // Installing cyberware
    REMOVE,                 // Removing cyberware
    REPAIR,                 // Repairing items
    MODIFY,                 // Modifying items
    SCAN,                   // Scanning objects
    TALK,                   // Talking to NPCs
    BARTER,                 // Bartering with NPCs
    STEAL,                  // Stealing items
    BREAK_IN,               // Breaking into places
    HIDE,                   // Hiding objects
    SEARCH,                 // Searching areas
    MARK,                   // Marking locations
    TRACK                   // Tracking targets
};

// Cyberpunk 2077 game state
enum class CyberpunkGameState {
    MAIN_MENU,              // Main menu
    IN_GAME,                // In game
    INVENTORY,              // Inventory menu
    MAP,                    // Map view
    DIALOGUE,               // Dialogue system
    COMBAT,                 // Combat mode
    STEALTH,                // Stealth mode
    DRIVING,                // Driving mode
    HACKING,                // Hacking mode
    CRAFTING,               // Crafting mode
    SHOPPING,               // Shopping mode
    MISSION,                // Mission mode
    PAUSED,                 // Game paused
    LOADING,                // Loading screen
    SAVING,                 // Saving game
    ERROR                   // Error state
};

// Cyberpunk 2077 performance settings
enum class CyberpunkPerformanceMode {
    LOW,                    // Low performance
    MEDIUM,                 // Medium performance
    HIGH,                   // High performance
    ULTRA,                  // Ultra performance
    RAY_TRACING,            // Ray tracing enabled
    DLSS,                   // DLSS enabled
    FSR,                    // FSR enabled
    QUEST_3_OPTIMIZED       // Quest 3 optimized
};

// Cyberpunk 2077 object data structure
struct CyberpunkObjectData {
    uint64_t id = 0;
    CyberpunkObjectType type = CyberpunkObjectType::ENVIRONMENT;
    std::string name = "";
    std::string description = "";
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool is_interactable = false;
    bool is_hackable = false;
    bool is_craftable = false;
    bool is_buyable = false;
    bool is_equippable = false;
    bool is_upgradeable = false;
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

// Cyberpunk 2077 game profile
struct CyberpunkGameProfile {
    std::string version = "2.0";
    std::string save_file = "";
    std::string mod_folder = "";
    CyberpunkGameState current_state = CyberpunkGameState::MAIN_MENU;
    CyberpunkPerformanceMode performance_mode = CyberpunkPerformanceMode::MEDIUM;
    bool ray_tracing_enabled = false;
    bool dlss_enabled = false;
    bool fsr_enabled = false;
    float graphics_quality = 0.5f;
    float audio_quality = 0.5f;
    float haptic_intensity = 0.5f;
    float vr_comfort_level = 0.5f;
    std::vector<std::string> active_mods;
    std::unordered_map<std::string, bool> game_settings;
    std::unordered_map<std::string, float> vr_settings;
};

// Main Cyberpunk 2077 Adapter class
class Cyberpunk2077Adapter {
public:
    // Constructor and destructor
    Cyberpunk2077Adapter();
    virtual ~Cyberpunk2077Adapter();

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
    virtual bool createGameProfile(const CyberpunkGameProfile& profile);
    virtual bool updateGameProfile(const CyberpunkGameProfile& profile);
    virtual CyberpunkGameProfile getCurrentGameProfile() const;

    // Object management
    virtual uint64_t registerCyberpunkObject(const CyberpunkObjectData& object_data);
    virtual bool unregisterCyberpunkObject(uint64_t object_id);
    virtual bool updateCyberpunkObject(uint64_t object_id, const CyberpunkObjectData& object_data);
    virtual CyberpunkObjectData getCyberpunkObject(uint64_t object_id) const;
    virtual std::vector<uint64_t> getAllCyberpunkObjects() const;
    virtual std::vector<uint64_t> getObjectsByType(CyberpunkObjectType type) const;

    // VR integration
    virtual bool enableVRCollision(uint64_t object_id);
    virtual bool disableVRCollision(uint64_t object_id);
    virtual bool enableVRPhysics(uint64_t object_id);
    virtual bool disableVRPhysics(uint64_t object_id);
    virtual bool enableVRHaptics(uint64_t object_id);
    virtual bool disableVRHaptics(uint64_t object_id);

    // Game-specific interactions
    virtual bool hackObject(uint64_t object_id, HandType hand);
    virtual bool craftItem(uint64_t object_id, HandType hand, const std::string& recipe);
    virtual bool buyItem(uint64_t object_id, HandType hand);
    virtual bool sellItem(uint64_t object_id, HandType hand);
    virtual bool equipItem(uint64_t object_id, HandType hand);
    virtual bool unequipItem(uint64_t object_id, HandType hand);
    virtual bool upgradeItem(uint64_t object_id, HandType hand);
    virtual bool installCyberware(uint64_t object_id, HandType hand);
    virtual bool removeCyberware(uint64_t object_id, HandType hand);

    // Performance optimization
    virtual bool setPerformanceMode(CyberpunkPerformanceMode mode);
    virtual bool enableRayTracing(bool enable);
    virtual bool enableDLSS(bool enable);
    virtual bool enableFSR(bool enable);
    virtual bool optimizeForQuest3();
    virtual bool setGraphicsQuality(float quality);
    virtual bool setAudioQuality(float quality);

    // VR comfort features
    virtual bool setVRComfortLevel(float level);
    virtual bool enableVRComfortFeatures(bool enable);
    virtual bool setHapticIntensity(float intensity);
    virtual bool enableVRSpatialAudio(bool enable);
    virtual bool enableVRVisualEffects(bool enable);

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
    bool initializeREDengine4Interface();
    bool initializeVRSystems();
    bool initializeGameObjects();
    
    void updateGameState();
    void updateGameObjects();
    void updateVRIntegration();
    
    bool validateObjectData(const CyberpunkObjectData& object_data);
    bool applyVRSettings(const CyberpunkGameProfile& profile);
    bool optimizeForPerformance(CyberpunkPerformanceMode mode);
    
    void updatePerformanceMetrics();
    bool checkPerformanceThresholds();
    void optimizeAdapterSystem();
    
    bool handleGameSpecificFeatures(uint64_t object_id, HandType hand, InteractionType interaction);
    bool handleCyberpunkInteractions(uint64_t object_id, HandType hand, CyberpunkInteractionType interaction);
    bool handleREDengine4Optimizations();
};

} // namespace uevr
