#pragma once

#include "UniversalEngineDetector.hpp"
#include "BaseCrossEngineAdapter.hpp"
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>

namespace uevr {

// Forward declarations for all engine adapters
class UnrealEngineAdapter;
class UnityEngineAdapter;
class CryEngineAdapter;
class IdTechAdapter;
class SourceEngineAdapter;
class REEngineAdapter;
class REDengine4Adapter;
class MTFrameworkAdapter;

/**
 * @brief Universal Adapter Factory
 * 
 * Creates the appropriate VR adapter for any detected game engine
 * Supports 500+ games across all major engine families
 */
class UniversalAdapterFactory {
public:
    // Adapter creation function type
    using AdapterCreator = std::function<std::unique_ptr<BaseCrossEngineAdapter>(const std::string&, const std::string&, const std::string&)>;
    
    // Adapter information
    struct AdapterInfo {
        std::string engineName;
        std::string description;
        std::string version;
        std::vector<std::string> supportedGames;
        std::vector<std::string> supportedFeatures;
        bool isActive;
        uint32_t gameCount;
    };

public:
    UniversalAdapterFactory();
    ~UniversalAdapterFactory() = default;

    // Main factory methods
    std::unique_ptr<BaseCrossEngineAdapter> createAdapter(const UniversalEngineDetector::GameInfo& gameInfo);
    std::unique_ptr<BaseCrossEngineAdapter> createAdapterByEngineType(UniversalEngineDetector::EngineType engineType, 
                                                                     const std::string& gameExecutable);
    std::unique_ptr<BaseCrossEngineAdapter> autoDetectAndCreate();
    
    // Adapter management
    void registerEngineAdapter(UniversalEngineDetector::EngineType engineType, AdapterCreator creator);
    void unregisterEngineAdapter(UniversalEngineDetector::EngineType engineType);
    bool isEngineSupported(UniversalEngineDetector::EngineType engineType) const;
    
    // Information and statistics
    std::vector<UniversalEngineDetector::EngineType> getSupportedEngines() const;
    std::vector<AdapterInfo> getAdapterInfo() const;
    AdapterInfo getAdapterInfo(UniversalEngineDetector::EngineType engineType) const;
    
    // Game database management
    void loadGameDatabase();
    void addGameToDatabase(const UniversalEngineDetector::GameInfo& gameInfo);
    std::vector<UniversalEngineDetector::GameInfo> getGamesForEngine(UniversalEngineDetector::EngineType engineType) const;
    
    // Performance monitoring
    struct FactoryMetrics {
        uint32_t totalAdaptersCreated;
        uint32_t successfulCreations;
        uint32_t failedCreations;
        double averageCreationTime;
        std::vector<std::string> creationErrors;
        std::unordered_map<UniversalEngineDetector::EngineType, uint32_t> engineUsageCounts;
    };
    
    FactoryMetrics getFactoryMetrics() const;
    void resetMetrics();

private:
    // Engine detection system
    std::unique_ptr<UniversalEngineDetector> m_engineDetector;
    
    // Registered adapter creators
    std::unordered_map<UniversalEngineDetector::EngineType, AdapterCreator> m_adapterCreators;
    
    // Adapter information database
    std::unordered_map<UniversalEngineDetector::EngineType, AdapterInfo> m_adapterInfo;
    
    // Performance tracking
    FactoryMetrics m_metrics;
    
    // Internal methods
    void initializeDefaultAdapters();
    void initializeAdapterInfo();
    bool validateGameInfo(const UniversalEngineDetector::GameInfo& gameInfo) const;
    std::string getEngineDescription(UniversalEngineDetector::EngineType engineType) const;
    std::vector<std::string> getEngineFeatures(UniversalEngineDetector::EngineType engineType) const;
    
    // Adapter creation helpers
    std::unique_ptr<BaseCrossEngineAdapter> createUnrealEngineAdapter(const std::string& name, 
                                                                     const std::string& engineType, 
                                                                     const std::string& gameExecutable);
    std::unique_ptr<BaseCrossEngineAdapter> createUnityEngineAdapter(const std::string& name, 
                                                                   const std::string& engineType, 
                                                                   const std::string& gameExecutable);
    std::unique_ptr<BaseCrossEngineAdapter> createCryEngineAdapter(const std::string& name, 
                                                                  const std::string& engineType, 
                                                                  const std::string& gameExecutable);
    std::unique_ptr<BaseCrossEngineAdapter> createIdTechAdapter(const std::string& name, 
                                                               const std::string& engineType, 
                                                               const std::string& gameExecutable);
    std::unique_ptr<BaseCrossEngineAdapter> createSourceEngineAdapter(const std::string& name, 
                                                                     const std::string& engineType, 
                                                                     const std::string& gameExecutable);
    std::unique_ptr<BaseCrossEngineAdapter> createREEngineAdapter(const std::string& name, 
                                                                 const std::string& engineType, 
                                                                 const std::string& gameExecutable);
    std::unique_ptr<BaseCrossEngineAdapter> createREDengine4Adapter(const std::string& name, 
                                                                   const std::string& engineType, 
                                                                   const std::string& gameExecutable);
    std::unique_ptr<BaseCrossEngineAdapter> createMTFrameworkAdapter(const std::string& name, 
                                                                    const std::string& engineType, 
                                                                    const std::string& gameExecutable);
    
    // Error handling
    void logFactoryError(const std::string& error, bool critical = false);
    void updateMetrics(bool success, double creationTime, UniversalEngineDetector::EngineType engineType);
};

} // namespace uevr

