#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include "../core/SystemState.hpp"

namespace UEVRLCX {

// ============================================================================
// Forward Declarations
// ============================================================================

class D3D11Hook;
class D3D12Hook;
class OpenGLHook;
class VulkanHook;
class EngineHook;
class SecurityHook;

// ============================================================================
// Hook Data Structures
// ============================================================================

enum class HookType {
    D3D11,
    D3D12,
    OPENGL,
    VULKAN,
    ENGINE,
    SECURITY,
    CUSTOM
};

enum class HookStatus {
    INACTIVE,
    ACTIVE,
    ERROR,
    DISABLED
};

struct HookInfo {
    std::string name;
    HookType type;
    HookStatus status;
    std::string targetFunction;
    void* originalFunction;
    void* hookedFunction;
    std::chrono::steady_clock::time_point installTime;
    std::chrono::steady_clock::time_point lastCall;
    int callCount;
    
    HookInfo() : type(HookType::CUSTOM), status(HookStatus::INACTIVE),
                  originalFunction(nullptr), hookedFunction(nullptr), callCount(0) {
        installTime = std::chrono::steady_clock::now();
        lastCall = std::chrono::steady_clock::now();
    }
    
    HookInfo(const std::string& n, HookType t, const std::string& target)
        : name(n), type(t), status(HookStatus::INACTIVE), targetFunction(target),
          originalFunction(nullptr), hookedFunction(nullptr), callCount(0) {
        installTime = std::chrono::steady_clock::now();
        lastCall = std::chrono::steady_clock::now();
    }
};

struct HookStatistics {
    int totalHooks;
    int activeHooks;
    int errorHooks;
    int disabledHooks;
    int totalCalls;
    double averageCallTime;
    std::chrono::steady_clock::time_point lastUpdate;
    
    HookStatistics() : totalHooks(0), activeHooks(0), errorHooks(0),
                       disabledHooks(0), totalCalls(0), averageCallTime(0.0) {
        lastUpdate = std::chrono::steady_clock::now();
    }
};

// ============================================================================
// Hook Manager Interface
// ============================================================================

class IHooksManager {
public:
    virtual ~IHooksManager() = default;
    
    // ========================================================================
    // Initialization and Management
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ========================================================================
    // Hook Installation and Management
    // ========================================================================
    
    virtual bool InstallHook(const std::string& name, HookType type, 
                           const std::string& targetFunction, void* hookFunction) = 0;
    virtual bool UninstallHook(const std::string& name) = 0;
    virtual bool IsHookInstalled(const std::string& name) const = 0;
    virtual bool EnableHook(const std::string& name, bool enable) = 0;
    virtual bool IsHookEnabled(const std::string& name) const = 0;
    
    // ========================================================================
    // Graphics API Hooks
    // ========================================================================
    
    virtual bool InstallD3D11Hook() = 0;
    virtual bool InstallD3D12Hook() = 0;
    virtual bool InstallOpenGLHook() = 0;
    virtual bool InstallVulkanHook() = 0;
    virtual bool UninstallGraphicsHooks() = 0;
    
    // ========================================================================
    // Engine-Specific Hooks
    // ========================================================================
    
    virtual bool InstallEngineHook(const std::string& engineName) = 0;
    virtual bool UninstallEngineHook(const std::string& engineName) = 0;
    virtual std::vector<std::string> GetInstalledEngineHooks() const = 0;
    
    // ========================================================================
    // Security Hooks
    // ========================================================================
    
    virtual bool InstallSecurityHook(const std::string& securityType) = 0;
    virtual bool UninstallSecurityHook(const std::string& securityType) = 0;
    virtual std::vector<std::string> GetInstalledSecurityHooks() const = 0;
    
    // ========================================================================
    // Hook Information and Statistics
    // ========================================================================
    
    virtual HookInfo* GetHookInfo(const std::string& name) = 0;
    virtual std::vector<HookInfo> GetAllHooks() const = 0;
    virtual HookStatistics GetHookStatistics() const = 0;
    virtual bool UpdateHookStatistics() = 0;
    
    // ========================================================================
    // Safety and Validation
    // ========================================================================
    
    virtual bool ValidateHook(const std::string& name) = 0;
    virtual bool ValidateAllHooks() = 0;
    virtual bool IsHookSafe(const std::string& name) const = 0;
    virtual std::vector<std::string> GetUnsafeHooks() const = 0;
    
    // ========================================================================
    // Configuration and Settings
    // ========================================================================
    
    virtual bool SetHookTimeout(int milliseconds) = 0;
    virtual int GetHookTimeout() const = 0;
    virtual bool EnableHookLogging(bool enable) = 0;
    virtual bool IsHookLoggingEnabled() const = 0;
    
    // ========================================================================
    // Emergency and Recovery
    // ========================================================================
    
    virtual bool EmergencyUninstallAll() = 0;
    virtual bool RecoverHooks() = 0;
    virtual bool IsRecoveryMode() const = 0;
    virtual std::string GetLastError() const = 0;
};

// ============================================================================
// Hook Manager Implementation
// ============================================================================

class HooksManager : public IHooksManager {
private:
    // Core Components
    std::unique_ptr<D3D11Hook> m_d3d11Hook;
    std::unique_ptr<D3D12Hook> m_d3d12Hook;
    std::unique_ptr<OpenGLHook> m_openGLHook;
    std::unique_ptr<VulkanHook> m_vulkanHook;
    std::unique_ptr<EngineHook> m_engineHook;
    std::unique_ptr<SecurityHook> m_securityHook;
    
    // Hook Management
    std::unordered_map<std::string, std::unique_ptr<HookInfo>> m_hooks;
    std::vector<std::string> m_activeHooks;
    std::vector<std::string> m_disabledHooks;
    
    // Configuration
    int m_hookTimeout;
    bool m_hookLoggingEnabled;
    bool m_isInitialized;
    bool m_recoveryMode;
    
    // Statistics
    HookStatistics m_statistics;
    std::chrono::steady_clock::time_point m_lastStatisticsUpdate;
    
    // Error Handling
    std::string m_lastError;
    std::vector<std::string> m_errorHistory;
    
public:
    HooksManager();
    ~HooksManager();
    
    // ========================================================================
    // IHooksManager Implementation
    // ========================================================================
    
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_isInitialized; }
    
    bool InstallHook(const std::string& name, HookType type, 
                    const std::string& targetFunction, void* hookFunction) override;
    bool UninstallHook(const std::string& name) override;
    bool IsHookInstalled(const std::string& name) const override;
    bool EnableHook(const std::string& name, bool enable) override;
    bool IsHookEnabled(const std::string& name) const override;
    
    bool InstallD3D11Hook() override;
    bool InstallD3D12Hook() override;
    bool InstallOpenGLHook() override;
    bool InstallVulkanHook() override;
    bool UninstallGraphicsHooks() override;
    
    bool InstallEngineHook(const std::string& engineName) override;
    bool UninstallEngineHook(const std::string& engineName) override;
    std::vector<std::string> GetInstalledEngineHooks() const override;
    
    bool InstallSecurityHook(const std::string& securityType) override;
    bool UninstallSecurityHook(const std::string& securityType) override;
    std::vector<std::string> GetInstalledSecurityHooks() const override;
    
    HookInfo* GetHookInfo(const std::string& name) override;
    std::vector<HookInfo> GetAllHooks() const override;
    HookStatistics GetHookStatistics() const override { return m_statistics; }
    bool UpdateHookStatistics() override;
    
    bool ValidateHook(const std::string& name) override;
    bool ValidateAllHooks() override;
    bool IsHookSafe(const std::string& name) const override;
    std::vector<std::string> GetUnsafeHooks() const override;
    
    bool SetHookTimeout(int milliseconds) override;
    int GetHookTimeout() const override { return m_hookTimeout; }
    bool EnableHookLogging(bool enable) override;
    bool IsHookLoggingEnabled() const override { return m_hookLoggingEnabled; }
    
    bool EmergencyUninstallAll() override;
    bool RecoverHooks() override;
    bool IsRecoveryMode() const override { return m_recoveryMode; }
    std::string GetLastError() const override { return m_lastError; }
    
private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================
    
    bool InitializeGraphicsHooks();
    bool InitializeEngineHooks();
    bool InitializeSecurityHooks();
    
    bool ValidateHookFunction(void* hookFunction);
    bool ValidateTargetFunction(const std::string& targetFunction);
    bool CheckHookCompatibility(const std::string& name, HookType type);
    
    bool InstallHookInternal(const std::string& name, HookType type,
                           const std::string& targetFunction, void* hookFunction);
    bool UninstallHookInternal(const std::string& name);
    void UpdateHookStatus(const std::string& name, HookStatus status);
    
    void UpdateHookCallCount(const std::string& name);
    void UpdateHookLastCall(const std::string& name);
    void CalculateHookStatistics();
    
    bool IsHookNameValid(const std::string& name);
    bool IsHookTypeSupported(HookType type);
    std::string GenerateHookName(HookType type, const std::string& targetFunction);
    
    void LogHookOperation(const std::string& operation, const std::string& hookName);
    void LogHookError(const std::string& hookName, const std::string& error);
    void LogHookStatistics();
    
    void SetLastError(const std::string& error);
    void AddErrorToHistory(const std::string& error);
    void ClearErrorHistory();
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<IHooksManager> CreateHooksManager();
std::unique_ptr<IHooksManager> CreateHooksManagerWithConfig(const std::string& configPath);

} // namespace UEVRLCX
