#pragma once

#include "common/UniversalAdapterFactory.hpp"
#include "common/UniversalEngineDetector.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

namespace uevr {

/**
 * @brief Universal VR Deployment System
 * 
 * Main orchestrator for the entire universal VR system
 * Automatically detects, creates, and manages VR adapters for 500+ games
 * Provides unified interface for all supported game engines
 */
class UniversalVRDeploymentSystem {
public:
    // System configuration
    struct SystemConfig {
        bool autoDetectionEnabled;
        bool hotReloadEnabled;
        bool performanceMonitoringEnabled;
        bool errorReportingEnabled;
        uint32_t maxConcurrentAdapters;
        uint32_t detectionIntervalMs;
        std::string logLevel;
        std::string configPath;
    };
    
    // Deployment status
    enum class DeploymentStatus {
        UNINITIALIZED,
        INITIALIZING,
        RUNNING,
        PAUSED,
        ERROR,
        SHUTTING_DOWN
    };
    
    // Game deployment info
    struct GameDeploymentInfo {
        std::string gameName;
        std::string executablePath;
        UniversalEngineDetector::EngineType engineType;
        std::string engineVersion;
        DeploymentStatus status;
        std::string vrAdapterName;
        bool isVREnabled;
        std::vector<std::string> activeFeatures;
        std::vector<std::string> errors;
        double performanceScore;
        uint64_t lastUpdateTime;
    };

public:
    UniversalVRDeploymentSystem();
    ~UniversalVRDeploymentSystem();
    
    // System lifecycle
    bool initialize(const SystemConfig& config = SystemConfig{});
    void shutdown();
    bool start();
    void stop();
    void pause();
    void resume();
    
    // Main deployment methods
    bool deployVRForGame(const std::string& gameExecutable);
    bool deployVRForAllDetectedGames();
    bool deployVRForEngine(UniversalEngineDetector::EngineType engineType);
    bool redeployVRForGame(const std::string& gameExecutable);
    
    // Game management
    std::vector<GameDeploymentInfo> getDeployedGames() const;
    GameDeploymentInfo getGameDeploymentInfo(const std::string& gameExecutable) const;
    bool isGameDeployed(const std::string& gameExecutable) const;
    bool removeGameDeployment(const std::string& gameExecutable);
    
    // Engine management
    std::vector<UniversalEngineDetector::EngineType> getSupportedEngines() const;
    bool isEngineSupported(UniversalEngineDetector::EngineType engineType) const;
    std::vector<std::string> getGamesForEngine(UniversalEngineDetector::EngineType engineType) const;
    
    // Detection and monitoring
    void startAutoDetection();
    void stopAutoDetection();
    bool isAutoDetectionRunning() const;
    std::vector<UniversalEngineDetector::GameInfo> scanForNewGames();
    
    // Performance monitoring
    struct SystemMetrics {
        uint32_t totalGamesDeployed;
        uint32_t activeDeployments;
        uint32_t successfulDeployments;
        uint32_t failedDeployments;
        double averageDeploymentTime;
        double systemPerformanceScore;
        std::vector<std::string> systemErrors;
        std::unordered_map<UniversalEngineDetector::EngineType, uint32_t> engineDeploymentCounts;
    };
    
    SystemMetrics getSystemMetrics() const;
    void resetMetrics();
    
    // Configuration management
    bool loadConfiguration(const std::string& configPath);
    bool saveConfiguration(const std::string& configPath);
    SystemConfig getConfiguration() const;
    bool updateConfiguration(const SystemConfig& newConfig);
    
    // Error handling and reporting
    std::vector<std::string> getSystemErrors() const;
    std::vector<std::string> getGameErrors(const std::string& gameExecutable) const;
    void clearErrors();
    void reportError(const std::string& error, bool critical = false);
    
    // Hot reload and updates
    bool reloadConfiguration();
    bool updateEngineAdapters();
    bool updateGameDatabase();
    bool performSystemMaintenance();

private:
    // Core components
    std::unique_ptr<UniversalAdapterFactory> m_adapterFactory;
    std::unique_ptr<UniversalEngineDetector> m_engineDetector;
    
    // System state
    SystemConfig m_config;
    DeploymentStatus m_status;
    std::atomic<bool> m_autoDetectionRunning;
    
    // Game deployments
    std::unordered_map<std::string, GameDeploymentInfo> m_gameDeployments;
    std::unordered_map<std::string, std::unique_ptr<BaseCrossEngineAdapter>> m_activeAdapters;
    
    // Performance tracking
    SystemMetrics m_metrics;
    std::chrono::high_resolution_clock::time_point m_startTime;
    
    // Threading and synchronization
    std::thread m_autoDetectionThread;
    std::thread m_maintenanceThread;
    mutable std::mutex m_deploymentsMutex;
    mutable std::mutex m_metricsMutex;
    
    // Internal methods
    void autoDetectionWorker();
    void maintenanceWorker();
    bool deploySingleGame(const UniversalEngineDetector::GameInfo& gameInfo);
    bool validateGameDeployment(const GameDeploymentInfo& deploymentInfo);
    void updateGameDeploymentStatus(const std::string& gameExecutable, DeploymentStatus status);
    void updateSystemMetrics();
    
    // Configuration helpers
    bool validateConfiguration(const SystemConfig& config) const;
    SystemConfig getDefaultConfiguration() const;
    
    // Error handling
    void logSystemError(const std::string& error, bool critical = false);
    void logGameError(const std::string& gameExecutable, const std::string& error, bool critical = false);
    
    // Performance optimization
    bool optimizeSystemPerformance();
    bool cleanupUnusedResources();
    bool balanceSystemLoad();
    
    // Monitoring and reporting
    void generateSystemReport();
    void generateGameReport(const std::string& gameExecutable);
    void exportMetrics(const std::string& filePath);
    
    // Utility functions
    std::string getStatusString(DeploymentStatus status) const;
    std::string getEngineTypeString(UniversalEngineDetector::EngineType engineType) const;
    double calculatePerformanceScore(const GameDeploymentInfo& deploymentInfo) const;
    bool shouldAutoDeploy(const UniversalEngineDetector::GameInfo& gameInfo) const;
};

} // namespace uevr

