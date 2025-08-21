#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <filesystem>

// Core system includes
#include "../src/core/Framework.hpp"

namespace uevr {
namespace deployment {

/**
 * @brief Package Manager for uevrLCX Deployment
 * 
 * Handles all deployment and packaging operations:
 * - Installer creation
 * - Dependency management
 * - Update system
 * - Configuration management
 * - Distribution and deployment
 */
class PackageManager {
public:
    // Singleton pattern
    static PackageManager& getInstance();
    
    // Disable copy and assignment
    PackageManager(const PackageManager&) = delete;
    PackageManager& operator=(const PackageManager&) = delete;

    // Core system lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }

    // Package configuration
    struct PackageConfig {
        std::string packageName = "uevrLCX";
        std::string version = "1.0.0";
        std::string description = "Universal VR Conversion System";
        std::string author = "uevrLCX Team";
        std::string license = "MIT";
        
        // Installation settings
        std::string installDirectory = "C:\\Program Files\\uevrLCX";
        std::string dataDirectory = "C:\\ProgramData\\uevrLCX";
        std::string userDirectory = "%APPDATA%\\uevrLCX";
        
        // Component selection
        bool includeCoreSystem = true;
        bool includeVRComponents = true;
        bool includeCommunityFeatures = true;
        bool includeDocumentation = true;
        bool includeExamples = true;
        bool includeDevelopmentTools = false;
        
        // Dependencies
        std::vector<std::string> requiredDependencies;
        std::vector<std::string> optionalDependencies;
        std::vector<std::string> systemRequirements;
        
        PackageConfig() {
            requiredDependencies = {
                "Visual C++ Redistributable 2019",
                "DirectX 11 Runtime",
                "Windows 10/11 (64-bit)"
            };
            
            optionalDependencies = {
                "SteamVR",
                "Oculus Runtime",
                "OpenVR"
            };
            
            systemRequirements = {
                "Windows 10/11 (64-bit)",
                "8GB RAM minimum, 16GB recommended",
                "DirectX 11 compatible GPU",
                "VR headset (Oculus, HTC Vive, Valve Index, etc.)"
            };
        }
    };

    void setPackageConfig(const PackageConfig& config);
    PackageConfig getPackageConfig() const;

    // Package creation
    struct PackageInfo {
        std::string packageId;
        std::string packageName;
        std::string version;
        std::string architecture; // x64, x86
        std::string platform; // Windows, Linux, macOS
        size_t packageSize;
        std::string checksum;
        std::chrono::system_clock::time_point buildDate;
        std::vector<std::string> includedComponents;
        
        PackageInfo() : packageSize(0) {}
    };

    bool createPackage(const PackageConfig& config, PackageInfo& packageInfo);
    bool createInstaller(const PackageInfo& packageInfo, const std::string& outputPath);
    bool createPortablePackage(const PackageInfo& packageInfo, const std::string& outputPath);
    bool createUpdatePackage(const std::string& fromVersion, const std::string& toVersion, 
                           const std::string& outputPath);

    // Installation management
    struct InstallationInfo {
        std::string installPath;
        std::string version;
        std::chrono::system_clock::time_point installDate;
        std::vector<std::string> installedComponents;
        std::unordered_map<std::string, std::string> configuration;
        bool isPortable;
        
        InstallationInfo() : isPortable(false) {}
    };

    bool installPackage(const std::string& packagePath, InstallationInfo& installInfo);
    bool uninstallPackage(const std::string& installPath);
    bool repairInstallation(const std::string& installPath);
    bool updateInstallation(const std::string& installPath, const std::string& newVersion);
    std::vector<InstallationInfo> findInstallations();

    // Dependency management
    struct DependencyInfo {
        std::string name;
        std::string version;
        std::string type; // "required", "optional", "system"
        bool isInstalled;
        std::string installPath;
        std::string downloadUrl;
        size_t downloadSize;
        
        DependencyInfo() : isInstalled(false), downloadSize(0) {}
    };

    std::vector<DependencyInfo> checkDependencies();
    bool installDependency(const std::string& dependencyName);
    bool uninstallDependency(const std::string& dependencyName);
    bool updateDependency(const std::string& dependencyName);
    bool validateDependencies();

    // Update system
    struct UpdateInfo {
        std::string currentVersion;
        std::string latestVersion;
        std::string updateUrl;
        size_t updateSize;
        std::string changelog;
        bool isAvailable;
        bool isRequired;
        std::chrono::system_clock::time_point releaseDate;
        
        UpdateInfo() : updateSize(0), isAvailable(false), isRequired(false) {}
    };

    UpdateInfo checkForUpdates();
    bool downloadUpdate(const UpdateInfo& updateInfo, const std::string& downloadPath);
    bool installUpdate(const std::string& updatePath);
    bool rollbackUpdate(const std::string& installPath);
    bool enableAutoUpdates(bool enable);

    // Configuration management
    struct ConfigurationInfo {
        std::string configPath;
        std::unordered_map<std::string, std::string> settings;
        std::chrono::system_clock::time_point lastModified;
        bool isDefault;
        
        ConfigurationInfo() : isDefault(true) {}
    };

    bool loadConfiguration(const std::string& configPath, ConfigurationInfo& configInfo);
    bool saveConfiguration(const std::string& configPath, const ConfigurationInfo& configInfo);
    bool createDefaultConfiguration(const std::string& configPath);
    bool validateConfiguration(const ConfigurationInfo& configInfo);
    bool backupConfiguration(const std::string& configPath, const std::string& backupPath);
    bool restoreConfiguration(const std::string& backupPath, const std::string& configPath);

    // Distribution
    struct DistributionInfo {
        std::string distributionName;
        std::string distributionUrl;
        std::string distributionType; // "github", "website", "steam", "epic"
        std::vector<std::string> supportedPlatforms;
        std::vector<std::string> supportedArchitectures;
        std::string license;
        std::string pricing; // "free", "paid", "subscription"
        
        DistributionInfo() {}
    };

    bool createDistributionPackage(const PackageInfo& packageInfo, DistributionInfo& distInfo);
    bool uploadToDistribution(const std::string& packagePath, const DistributionInfo& distInfo);
    bool publishRelease(const std::string& version, const std::string& releaseNotes);
    bool generateDistributionManifest(const PackageInfo& packageInfo, const std::string& outputPath);

    // Deployment automation
    struct DeploymentConfig {
        std::string deploymentType; // "local", "network", "cloud", "docker"
        std::string targetEnvironment; // "development", "testing", "staging", "production"
        std::vector<std::string> targetMachines;
        std::unordered_map<std::string, std::string> environmentVariables;
        bool enableRollback;
        bool enableMonitoring;
        
        DeploymentConfig() : enableRollback(true), enableMonitoring(true) {}
    };

    bool deployPackage(const PackageInfo& packageInfo, const DeploymentConfig& deployConfig);
    bool rollbackDeployment(const std::string& deploymentId);
    bool monitorDeployment(const std::string& deploymentId);
    bool validateDeployment(const std::string& deploymentId);

    // Security and validation
    struct SecurityInfo {
        std::string packageSignature;
        std::string certificateInfo;
        bool isSigned;
        bool isVerified;
        std::string verificationStatus;
        
        SecurityInfo() : isSigned(false), isVerified(false) {}
    };

    SecurityInfo validatePackageSecurity(const std::string& packagePath);
    bool signPackage(const std::string& packagePath, const std::string& certificatePath);
    bool verifyPackageSignature(const std::string& packagePath);
    bool generatePackageChecksum(const std::string& packagePath, std::string& checksum);

    // Error handling
    struct PackageError {
        std::string errorCode;
        std::string errorMessage;
        std::string packageName;
        std::string operation;
        std::string timestamp;
        std::string suggestedFix;
        
        PackageError() {}
    };

    std::vector<PackageError> getRecentErrors() const;
    void clearErrors();
    bool hasErrors() const;

    // Performance monitoring
    struct PackageMetrics {
        int totalPackages;
        int successfulPackages;
        int failedPackages;
        double averagePackageTime;
        double averageInstallTime;
        double averageUpdateTime;
        size_t totalPackageSize;
        size_t totalDownloadSize;
        
        PackageMetrics() : totalPackages(0), successfulPackages(0), failedPackages(0),
                          averagePackageTime(0.0), averageInstallTime(0.0), averageUpdateTime(0.0),
                          totalPackageSize(0), totalDownloadSize(0) {}
    };

    PackageMetrics getMetrics() const;
    void resetMetrics();

private:
    PackageManager();
    ~PackageManager();

    // Internal data structures
    std::atomic<bool> m_initialized{false};
    std::mutex m_mutex;
    std::condition_variable m_cv;
    
    // Configuration
    PackageConfig m_packageConfig;
    
    // Package data
    std::vector<PackageInfo> m_createdPackages;
    std::vector<InstallationInfo> m_installations;
    std::vector<DependencyInfo> m_dependencies;
    std::vector<UpdateInfo> m_updates;
    
    // Error handling
    std::vector<PackageError> m_recentErrors;
    mutable std::mutex m_errorsMutex;
    
    // Performance tracking
    PackageMetrics m_metrics;
    mutable std::mutex m_metricsMutex;
    
    // Internal methods
    bool initializePackageSystem();
    bool initializeUpdateSystem();
    bool initializeSecuritySystem();
    
    // Package implementations
    bool collectPackageFiles(const PackageConfig& config, std::vector<std::filesystem::path>& files);
    bool createPackageManifest(const PackageConfig& config, const std::string& manifestPath);
    bool compressPackageFiles(const std::vector<std::filesystem::path>& files, const std::string& outputPath);
    
    // Installation implementations
    bool extractPackageFiles(const std::string& packagePath, const std::string& extractPath);
    bool installPackageFiles(const std::string& sourcePath, const std::string& targetPath);
    bool createInstallationRegistry(const InstallationInfo& installInfo);
    bool removeInstallationRegistry(const std::string& installPath);
    
    // Dependency implementations
    bool downloadDependency(const DependencyInfo& depInfo, const std::string& downloadPath);
    bool installDependencyFile(const std::string& filePath, const std::string& installPath);
    bool validateDependencyInstallation(const DependencyInfo& depInfo);
    
    // Update implementations
    bool checkUpdateServer(const std::string& updateUrl, UpdateInfo& updateInfo);
    bool downloadUpdateFile(const std::string& downloadUrl, const std::string& downloadPath);
    bool applyUpdatePatch(const std::string& updatePath, const std::string& installPath);
    
    // Configuration implementations
    bool parseConfigurationFile(const std::string& configPath, ConfigurationInfo& configInfo);
    bool writeConfigurationFile(const std::string& configPath, const ConfigurationInfo& configInfo);
    bool validateConfigurationSettings(const std::unordered_map<std::string, std::string>& settings);
    
    // Security implementations
    bool generatePackageSignature(const std::string& packagePath, std::string& signature);
    bool verifyPackageIntegrity(const std::string& packagePath);
    bool validatePackagePermissions(const std::string& packagePath);
    
    // Error handling
    void logError(const std::string& errorCode, const std::string& errorMessage, 
                  const std::string& packageName = "", const std::string& operation = "");
    void updateMetrics(const std::string& operation, double time, bool success, size_t size = 0);
    
    // Utility methods
    std::string generatePackageId();
    std::string calculateFileChecksum(const std::string& filePath);
    bool createDirectoryStructure(const std::string& basePath, const std::vector<std::string>& directories);
    void cleanupTemporaryFiles();
};

} // namespace deployment
} // namespace uevr
