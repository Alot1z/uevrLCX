#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

namespace uevr {

/**
 * @brief Universal Engine Detection System
 * 
 * Automatically detects game engines and creates appropriate VR adapters
 * Supports 500+ games across major engine families
 */
class UniversalEngineDetector {
public:
    // Engine types supported
    enum class EngineType {
        UNKNOWN,
        REDENGINE_4,      // CD Projekt RED
        RE_ENGINE,        // Capcom
        UNREAL_ENGINE_4,  // Epic Games
        UNREAL_ENGINE_5,  // Epic Games
        UNITY,            // Unity Technologies
        CRYENGINE,        // Crytek
        ID_TECH,          // id Software
        SOURCE_ENGINE,    // Valve
        SOURCE_2,         // Valve
        MT_FRAMEWORK,     // Capcom (legacy)
        CUSTOM            // Custom/unknown engines
    };

    // Game detection info
    struct GameInfo {
        std::string executableName;
        std::string displayName;
        EngineType engineType;
        std::string engineVersion;
        std::string supportedVRFeatures;
        bool isVRReady;
        std::vector<std::string> requiredHooks;
    };

    // Engine detection strategy
    struct EngineDetectionStrategy {
        std::string engineName;
        std::vector<std::string> processNames;
        std::vector<std::string> moduleNames;
        std::vector<std::string> registryKeys;
        std::vector<std::string> fileSignatures;
        std::function<bool(const std::string&)> customDetector;
    };

public:
    UniversalEngineDetector();
    ~UniversalEngineDetector() = default;

    // Main detection methods
    EngineType detectEngine(const std::string& processName);
    std::vector<GameInfo> scanRunningGames();
    std::unique_ptr<class ICrossEngineAdapter> createAdapterForGame(const GameInfo& gameInfo);
    
    // Engine-specific detection
    bool detectREDengine4(const std::string& processName);
    bool detectREEngine(const std::string& processName);
    bool detectUnrealEngine(const std::string& processName);
    bool detectUnity(const std::string& processName);
    bool detectCryEngine(const std::string& processName);
    bool detectIdTech(const std::string& processName);
    bool detectSourceEngine(const std::string& processName);
    bool detectMTFramework(const std::string& processName);

    // Game database
    void loadGameDatabase();
    void addGameToDatabase(const GameInfo& gameInfo);
    std::vector<GameInfo> getGamesForEngine(EngineType engineType);

    // Performance monitoring
    struct DetectionMetrics {
        uint32_t totalGamesScanned;
        uint32_t enginesDetected;
        uint32_t vrReadyGames;
        double averageDetectionTime;
        std::vector<std::string> detectionErrors;
    };
    
    DetectionMetrics getDetectionMetrics() const;

private:
    // Detection strategies for each engine
    std::unordered_map<EngineType, EngineDetectionStrategy> m_detectionStrategies;
    
    // Game database
    std::vector<GameInfo> m_gameDatabase;
    
    // Performance tracking
    DetectionMetrics m_metrics;
    
    // Helper methods
    bool scanProcessModules(HANDLE processHandle, const std::vector<std::string>& targetModules);
    bool checkRegistryKeys(const std::vector<std::string>& keys);
    bool scanFileSignatures(const std::string& processPath, const std::vector<std::string>& signatures);
    std::string getEngineVersionFromProcess(HANDLE processHandle, EngineType engineType);
    
    // Initialize detection strategies
    void initializeDetectionStrategies();
};

} // namespace uevr
