#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>
#include "../core/SystemState.hpp"

namespace UEVRLCX {

// ============================================================================
// Forward Declarations
// ============================================================================

class ModLoader;
class ModCreator;
class ModMarketplace;
class ModValidator;
class ModSecurity;
class ModCompatibility;

// ============================================================================
// Mod Data Structures
// ============================================================================

enum class ModType {
    VR_ENHANCEMENT,
    PERFORMANCE_OPTIMIZATION,
    VISUAL_IMPROVEMENT,
    GAMEPLAY_MODIFICATION,
    AUDIO_ENHANCEMENT,
    CONTENT_ADDITION,
    UTILITY,
    CUSTOM
};

enum class ModStatus {
    INSTALLED,
    ENABLED,
    DISABLED,
    UPDATING,
    ERROR,
    INCOMPATIBLE,
    OUTDATED
};

enum class ModPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

enum class ModCompatibilityLevel {
    FULLY_COMPATIBLE,
    MOSTLY_COMPATIBLE,
    PARTIALLY_COMPATIBLE,
    INCOMPATIBLE,
    UNKNOWN
};

struct ModInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    ModType type;
    ModStatus status;
    ModPriority priority;
    ModCompatibilityLevel compatibility;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflicts;
    std::vector<std::string> tags;
    std::string filePath;
    std::string iconPath;
    std::string readmePath;
    std::chrono::steady_clock::time_point installDate;
    std::chrono::steady_clock::time_point lastUpdate;
    bool isVerified;
    bool isOfficial;
    
    ModInfo() : type(ModType::CUSTOM), status(ModStatus::INSTALLED),
                 priority(ModPriority::NORMAL), compatibility(ModCompatibilityLevel::UNKNOWN),
                 isVerified(false), isOfficial(false) {
        installDate = std::chrono::steady_clock::now();
        lastUpdate = std::chrono::steady_clock::now();
    }
    
    ModInfo(const std::string& i, const std::string& n, const std::string& v, const std::string& a)
        : id(i), name(n), version(v), author(a), type(ModType::CUSTOM),
          status(ModStatus::INSTALLED), priority(ModPriority::NORMAL),
          compatibility(ModCompatibilityLevel::UNKNOWN), isVerified(false), isOfficial(false) {
        installDate = std::chrono::steady_clock::now();
        lastUpdate = std::chrono::steady_clock::now();
    }
};

struct ModInstallationResult {
    bool success;
    std::string modId;
    std::string message;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::chrono::steady_clock::time_point timestamp;
    
    ModInstallationResult() : success(false) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    ModInstallationResult(bool s, const std::string& id, const std::string& msg)
        : success(s), modId(id), message(msg) {
        timestamp = std::chrono::steady_clock::now();
    }
};

struct ModDependency {
    std::string modId;
    std::string version;
    bool isRequired;
    bool isInstalled;
    bool isCompatible;
    
    ModDependency() : isRequired(true), isInstalled(false), isCompatible(false) {}
    
    ModDependency(const std::string& id, const std::string& ver, bool required = true)
        : modId(id), version(ver), isRequired(required), isInstalled(false), isCompatible(false) {}
};

// ============================================================================
// Mod Manager Interface
// ============================================================================

class IModManager {
public:
    virtual ~IModManager() = default;
    
    // ========================================================================
    // Initialization and Management
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ========================================================================
    // Mod Installation and Management
    // ========================================================================
    
    virtual ModInstallationResult InstallMod(const std::string& modFilePath) = 0;
    virtual bool UninstallMod(const std::string& modId) = 0;
    virtual bool EnableMod(const std::string& modId) = 0;
    virtual bool DisableMod(const std::string& modId) = 0;
    virtual bool UpdateMod(const std::string& modId) = 0;
    
    // ========================================================================
    // Mod Information and Status
    // ========================================================================
    
    virtual ModInfo* GetModInfo(const std::string& modId) = 0;
    virtual std::vector<ModInfo> GetAllMods() const = 0;
    virtual std::vector<ModInfo> GetEnabledMods() const = 0;
    virtual std::vector<ModInfo> GetModsByType(ModType type) const = 0;
    virtual bool IsModInstalled(const std::string& modId) const = 0;
    virtual bool IsModEnabled(const std::string& modId) const = 0;
    
    // ========================================================================
    // Mod Dependencies and Compatibility
    // ========================================================================
    
    virtual std::vector<ModDependency> GetModDependencies(const std::string& modId) const = 0;
    virtual bool CheckModCompatibility(const std::string& modId) = 0;
    virtual bool ResolveModConflicts(const std::string& modId) = 0;
    virtual std::vector<std::string> GetConflictingMods(const std::string& modId) const = 0;
    
    // ========================================================================
    // Mod Loading and Execution
    // ========================================================================
    
    virtual bool LoadMod(const std::string& modId) = 0;
    virtual bool UnloadMod(const std::string& modId) = 0;
    virtual bool IsModLoaded(const std::string& modId) const = 0;
    virtual bool ExecuteModFunction(const std::string& modId, const std::string& functionName) = 0;
    
    // ========================================================================
    // Mod Creation and Development
    // ========================================================================
    
    virtual bool CreateMod(const std::string& name, const std::string& description, ModType type) = 0;
    virtual bool EditMod(const std::string& modId, const std::string& field, const std::string& value) = 0;
    virtual bool ValidateMod(const std::string& modId) = 0;
    virtual bool PackageMod(const std::string& modId, const std::string& outputPath) = 0;
    
    // ========================================================================
    // Mod Marketplace Integration
    // ========================================================================
    
    virtual bool ConnectToMarketplace(const std::string& marketplaceUrl) = 0;
    virtual bool DisconnectFromMarketplace() = 0;
    virtual bool IsConnectedToMarketplace() const = 0;
    virtual std::vector<ModInfo> SearchMarketplace(const std::string& query) = 0;
    virtual bool DownloadModFromMarketplace(const std::string& modId) = 0;
    
    // ========================================================================
    // Mod Security and Validation
    // ========================================================================
    
    virtual bool EnableModSecurity(bool enable) = 0;
    virtual bool IsModSecurityEnabled() const = 0;
    virtual bool ScanModForThreats(const std::string& modId) = 0;
    virtual bool VerifyModSignature(const std::string& modId) = 0;
    virtual std::vector<std::string> GetModSecurityIssues(const std::string& modId) const = 0;
    
    // ========================================================================
    // Mod Performance and Optimization
    // ========================================================================
    
    virtual bool EnableModOptimization(bool enable) = 0;
    virtual bool IsModOptimizationEnabled() const = 0;
    virtual bool OptimizeModPerformance(const std::string& modId) = 0;
    virtual double GetModPerformanceImpact(const std::string& modId) const = 0;
    
    // ========================================================================
    // Configuration and Settings
    // ========================================================================
    
    virtual bool SetModLoadOrder(const std::vector<std::string>& modIds) = 0;
    virtual std::vector<std::string> GetModLoadOrder() const = 0;
    virtual bool SetModPriority(const std::string& modId, ModPriority priority) = 0;
    virtual ModPriority GetModPriority(const std::string& modId) const = 0;
    virtual bool EnableAutoModUpdates(bool enable) = 0;
    virtual bool IsAutoModUpdatesEnabled() const = 0;
    
    // ========================================================================
    // Monitoring and Statistics
    // ========================================================================
    
    virtual int GetTotalMods() const = 0;
    virtual int GetEnabledMods() const = 0;
    virtual int GetDisabledMods() const = 0;
    virtual std::string GetModManagerStatus() const = 0;
    virtual std::chrono::steady_clock::time_point GetLastModUpdate() const = 0;
};

// ============================================================================
// Mod Manager Implementation
// ============================================================================

class ModManager : public IModManager {
private:
    // Core Components
    std::unique_ptr<ModLoader> m_modLoader;
    std::unique_ptr<ModCreator> m_modCreator;
    std::unique_ptr<ModMarketplace> m_modMarketplace;
    std::unique_ptr<ModValidator> m_modValidator;
    std::unique_ptr<ModSecurity> m_modSecurity;
    std::unique_ptr<ModCompatibility> m_modCompatibility;
    
    // Mod Management
    std::unordered_map<std::string, std::unique_ptr<ModInfo>> m_mods;
    std::vector<std::string> m_enabledMods;
    std::vector<std::string> m_disabledMods;
    std::vector<std::string> m_modLoadOrder;
    
    // Configuration
    bool m_modSecurityEnabled;
    bool m_modOptimizationEnabled;
    bool m_autoModUpdatesEnabled;
    bool m_isInitialized;
    bool m_marketplaceConnected;
    
    // Statistics
    int m_totalMods;
    int m_enabledMods;
    int m_disabledMods;
    std::chrono::steady_clock::time_point m_lastModUpdate;
    
    // Security and Validation
    std::vector<std::string> m_verifiedMods;
    std::vector<std::string> m_suspiciousMods;
    std::unordered_map<std::string, std::vector<std::string>> m_modSecurityIssues;
    
public:
    ModManager();
    ~ModManager();
    
    // ========================================================================
    // IModManager Implementation
    // ========================================================================
    
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_isInitialized; }
    
    ModInstallationResult InstallMod(const std::string& modFilePath) override;
    bool UninstallMod(const std::string& modId) override;
    bool EnableMod(const std::string& modId) override;
    bool DisableMod(const std::string& modId) override;
    bool UpdateMod(const std::string& modId) override;
    
    ModInfo* GetModInfo(const std::string& modId) override;
    std::vector<ModInfo> GetAllMods() const override;
    std::vector<ModInfo> GetEnabledMods() const override;
    std::vector<ModInfo> GetModsByType(ModType type) const override;
    bool IsModInstalled(const std::string& modId) const override;
    bool IsModEnabled(const std::string& modId) const override;
    
    std::vector<ModDependency> GetModDependencies(const std::string& modId) const override;
    bool CheckModCompatibility(const std::string& modId) override;
    bool ResolveModConflicts(const std::string& modId) override;
    std::vector<std::string> GetConflictingMods(const std::string& modId) const override;
    
    bool LoadMod(const std::string& modId) override;
    bool UnloadMod(const std::string& modId) override;
    bool IsModLoaded(const std::string& modId) const override;
    bool ExecuteModFunction(const std::string& modId, const std::string& functionName) override;
    
    bool CreateMod(const std::string& name, const std::string& description, ModType type) override;
    bool EditMod(const std::string& modId, const std::string& field, const std::string& value) override;
    bool ValidateMod(const std::string& modId) override;
    bool PackageMod(const std::string& modId, const std::string& outputPath) override;
    
    bool ConnectToMarketplace(const std::string& marketplaceUrl) override;
    bool DisconnectFromMarketplace() override;
    bool IsConnectedToMarketplace() const override { return m_marketplaceConnected; }
    std::vector<ModInfo> SearchMarketplace(const std::string& query) override;
    bool DownloadModFromMarketplace(const std::string& modId) override;
    
    bool EnableModSecurity(bool enable) override;
    bool IsModSecurityEnabled() const override { return m_modSecurityEnabled; }
    bool ScanModForThreats(const std::string& modId) override;
    bool VerifyModSignature(const std::string& modId) override;
    std::vector<std::string> GetModSecurityIssues(const std::string& modId) const override;
    
    bool EnableModOptimization(bool enable) override;
    bool IsModOptimizationEnabled() const override { return m_modOptimizationEnabled; }
    bool OptimizeModPerformance(const std::string& modId) override;
    double GetModPerformanceImpact(const std::string& modId) const override;
    
    bool SetModLoadOrder(const std::vector<std::string>& modIds) override;
    std::vector<std::string> GetModLoadOrder() const override { return m_modLoadOrder; }
    bool SetModPriority(const std::string& modId, ModPriority priority) override;
    ModPriority GetModPriority(const std::string& modId) const override;
    bool EnableAutoModUpdates(bool enable) override;
    bool IsAutoModUpdatesEnabled() const override { return m_autoModUpdatesEnabled; }
    
    int GetTotalMods() const override { return m_totalMods; }
    int GetEnabledMods() const override { return m_enabledMods; }
    int GetDisabledMods() const override { return m_disabledMods; }
    std::string GetModManagerStatus() const override;
    std::chrono::steady_clock::time_point GetLastModUpdate() const override { return m_lastModUpdate; }
    
private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================
    
    bool InitializeModLoader();
    bool InitializeModCreator();
    bool InitializeModMarketplace();
    bool InitializeModValidator();
    bool InitializeModSecurity();
    bool InitializeModCompatibility();
    
    bool ValidateModFile(const std::string& modFilePath);
    bool ValidateModInfo(const ModInfo& modInfo);
    bool CheckModDependencies(const std::string& modId);
    bool ResolveModDependencies(const std::string& modId);
    
    bool ProcessModInstallation(const std::string& modFilePath, ModInstallationResult& result);
    bool ProcessModUninstallation(const std::string& modId);
    bool ProcessModEnabling(const std::string& modId);
    bool ProcessModDisabling(const std::string& modId);
    
    void UpdateModStatistics();
    void UpdateModLoadOrder();
    void LogModOperation(const std::string& operation, const std::string& modId);
    void LogModInstallationResult(const ModInstallationResult& result);
    
    bool IsModCompatible(const std::string& modId);
    bool HasModConflicts(const std::string& modId);
    void HandleModConflict(const std::string& modId, const std::string& conflictingModId);
    
    void CleanupUnusedMods();
    void CleanupModCache();
    void ValidateAllMods();
    
    bool PerformModSecurityScan(const std::string& modId);
    bool PerformModCompatibilityCheck(const std::string& modId);
    bool PerformModPerformanceAnalysis(const std::string& modId);
    
    std::string GenerateModReport(const std::string& modId) const;
    std::string GenerateModManagerReport() const;
    std::string GenerateModCompatibilityReport() const;
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<IModManager> CreateModManager();
std::unique_ptr<IModManager> CreateModManagerWithConfig(const std::string& configPath);

} // namespace UEVRLCX
