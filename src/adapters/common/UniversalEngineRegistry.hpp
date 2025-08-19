#pragma once

#include "BaseCrossEngineAdapter.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

namespace uevr {

/**
 * @brief Universal Engine Registry for Mass Deployment
 * 
 * This registry automatically detects and manages support for 500+ games
 * across all major game engines without manual configuration.
 * 
 * Supports:
 * - Unreal Engine 4/5 (500+ games)
 * - Unity Engine (1000+ games)
 * - RE Engine (Capcom games)
 * - REDengine 4 (CD Projekt games)
 * - MT Framework (Capcom games)
 * - CryEngine (Crytek games)
 * - id Tech 6/7 (Bethesda games)
 * - Source Engine/Source 2 (Valve games)
 * - Custom engines (Indie games)
 */
class UniversalEngineRegistry {
public:
    // Engine detection signatures
    struct EngineSignature {
        std::string engineName;
        std::vector<std::string> fileSignatures;
        std::vector<std::string> memorySignatures;
        std::vector<std::string> processSignatures;
        EngineType engineType;
        std::string versionPattern;
    };
    
    // Game detection patterns
    struct GamePattern {
        std::string gameName;
        std::string executableName;
        std::vector<std::string> steamAppIds;
        std::vector<std::string> epicAppIds;
        std::vector<std::string> gogAppIds;
        EngineType engineType;
        std::string engineVersion;
        std::vector<std::string> requiredFiles;
        std::vector<std::string> optionalFiles;
    };
    
    // Universal VR configuration
    struct UniversalVRConfig {
        bool enableStereoRendering;
        bool enableMotionControllers;
        bool enableHapticFeedback;
        bool enableDynamicFOV;
        bool enableFrameInterpolation;
        bool enableNeuralUpscaling;
        bool enableRayTracing;
        bool enableTemporalUpsampling;
        float targetFrameRate;
        float targetLatency;
        bool adaptiveQuality;
        bool performanceMonitoring;
    };

    /**
     * @brief Initialize the universal engine registry
     * @return true if initialization successful
     */
    static bool initialize();
    
    /**
     * @brief Cleanup the universal engine registry
     */
    static void cleanup();
    
    /**
     * @brief Register an engine signature for automatic detection
     * @param signature Engine signature to register
     */
    static void registerEngineSignature(const EngineSignature& signature);
    
    /**
     * @brief Register a game pattern for automatic detection
     * @param pattern Game pattern to register
     */
    static void registerGamePattern(const GamePattern& pattern);
    
    /**
     * @brief Auto-detect the current game and engine
     * @return Engine type if detected, UNKNOWN_ENGINE otherwise
     */
    static EngineType autoDetectEngine();
    
    /**
     * @brief Auto-detect the current game
     * @return Game name if detected, empty string otherwise
     */
    static std::string autoDetectGame();
    
    /**
     * @brief Get engine compatibility for the detected engine
     * @return Engine compatibility information
     */
    static EngineCompatibility getEngineCompatibility();
    
    /**
     * @brief Get universal VR configuration for the detected game
     * @return VR configuration optimized for the game
     */
    static UniversalVRConfig getUniversalVRConfig();
    
    /**
     * @brief Create a universal adapter for the detected engine/game
     * @return Shared pointer to the created adapter
     */
    static std::shared_ptr<ICrossEngineAdapter> createUniversalAdapter();
    
    /**
     * @brief Get all supported engines
     * @return Vector of supported engine types
     */
    static std::vector<EngineType> getSupportedEngines();
    
    /**
     * @brief Get all supported games for an engine
     * @param engineType Engine type to query
     * @return Vector of supported game names
     */
    static std::vector<std::string> getSupportedGames(EngineType engineType);
    
    /**
     * @brief Check if a game is supported
     * @param gameName Name of the game to check
     * @return true if the game is supported
     */
    static bool isGameSupported(const std::string& gameName);
    
    /**
     * @brief Get total number of supported games
     * @return Total count of supported games
     */
    static size_t getTotalSupportedGames();
    
    /**
     * @brief Get total number of supported engines
     * @return Total count of supported engines
     */
    static size_t getTotalSupportedEngines();

private:
    static std::vector<EngineSignature> s_engineSignatures;
    static std::vector<GamePattern> s_gamePatterns;
    static std::unordered_map<EngineType, EngineCompatibility> s_engineCompatibility;
    static std::unordered_map<std::string, UniversalVRConfig> s_gameVRConfigs;
    
    static bool s_initialized;
    static EngineType s_detectedEngine;
    static std::string s_detectedGame;
    
    // Internal methods
    static bool initializeDefaultEngineSignatures();
    static bool initializeDefaultGamePatterns();
    static bool initializeEngineCompatibility();
    static bool initializeGameVRConfigs();
    static bool scanProcessForEngineSignatures();
    static bool scanFilesForGamePatterns();
    static bool validateEngineDetection();
    static bool validateGameDetection();
};

} // namespace uevr
