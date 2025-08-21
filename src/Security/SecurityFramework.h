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

class VulnerabilityScanner;
class ThreatDetector;
class SecurityMonitor;
class EncryptionManager;
class AccessController;
class SecurityAuditor;

// ============================================================================
// Security Data Structures
// ============================================================================

enum class SecurityLevel {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL,
    MAXIMUM
};

enum class ThreatLevel {
    NONE,
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

enum class SecurityEventType {
    THREAT_DETECTED,
    VULNERABILITY_FOUND,
    ACCESS_ATTEMPT,
    ENCRYPTION_EVENT,
    AUDIT_EVENT,
    SYSTEM_COMPROMISE,
    RECOVERY_ATTEMPT
};

struct SecurityEvent {
    SecurityEventType type;
    std::string description;
    std::string source;
    ThreatLevel threatLevel;
    std::chrono::steady_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    bool isResolved;
    
    SecurityEvent() : type(SecurityEventType::AUDIT_EVENT), threatLevel(ThreatLevel::NONE),
                      isResolved(false) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    SecurityEvent(SecurityEventType t, const std::string& desc, const std::string& src, ThreatLevel level)
        : type(t), description(desc), source(src), threatLevel(level), isResolved(false) {
        timestamp = std::chrono::steady_clock::now();
    }
};

struct SecurityThreat {
    std::string id;
    std::string name;
    std::string description;
    ThreatLevel level;
    std::string category;
    std::vector<std::string> indicators;
    std::vector<std::string> mitigationSteps;
    bool isActive;
    std::chrono::steady_clock::time_point firstDetected;
    std::chrono::steady_clock::time_point lastSeen;
    
    SecurityThreat() : level(ThreatLevel::NONE), isActive(false) {
        firstDetected = std::chrono::steady_clock::now();
        lastSeen = std::chrono::steady_clock::now();
    }
    
    SecurityThreat(const std::string& i, const std::string& n, const std::string& desc, ThreatLevel l)
        : id(i), name(n), description(desc), level(l), isActive(false) {
        firstDetected = std::chrono::steady_clock::now();
        lastSeen = std::chrono::steady_clock::now();
    }
};

struct VulnerabilityReport {
    std::string id;
    std::string name;
    std::string description;
    std::string severity;
    std::string affectedComponent;
    std::vector<std::string> cveReferences;
    std::vector<std::string> remediationSteps;
    bool isPatched;
    std::chrono::steady_clock::time_point discoveryTime;
    std::chrono::steady_clock::time_point patchTime;
    
    VulnerabilityReport() : isPatched(false) {
        discoveryTime = std::chrono::steady_clock::now();
        patchTime = std::chrono::steady_clock::now();
    }
    
    VulnerabilityReport(const std::string& i, const std::string& n, const std::string& desc, const std::string& sev)
        : id(i), name(n), description(desc), severity(sev), isPatched(false) {
        discoveryTime = std::chrono::steady_clock::now();
        patchTime = std::chrono::steady_clock::now();
    }
};

// ============================================================================
// Security Framework Interface
// ============================================================================

class ISecurityFramework {
public:
    virtual ~ISecurityFramework() = default;
    
    // ========================================================================
    // Initialization and Management
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ========================================================================
    // Threat Detection and Prevention
    // ========================================================================
    
    virtual bool EnableThreatDetection(bool enable) = 0;
    virtual bool IsThreatDetectionEnabled() const = 0;
    virtual bool ScanForThreats() = 0;
    virtual std::vector<SecurityThreat> GetActiveThreats() const = 0;
    virtual bool MitigateThreat(const std::string& threatId) = 0;
    
    // ========================================================================
    // Vulnerability Scanning
    // ========================================================================
    
    virtual bool EnableVulnerabilityScanning(bool enable) = 0;
    virtual bool IsVulnerabilityScanningEnabled() const = 0;
    virtual bool ScanForVulnerabilities() = 0;
    virtual std::vector<VulnerabilityReport> GetVulnerabilities() const = 0;
    virtual bool PatchVulnerability(const std::string& vulnerabilityId) = 0;
    
    // ========================================================================
    // Security Monitoring
    // ========================================================================
    
    virtual bool EnableSecurityMonitoring(bool enable) = 0;
    virtual bool IsSecurityMonitoringEnabled() const = 0;
    virtual bool StartMonitoring() = 0;
    virtual bool StopMonitoring() = 0;
    virtual std::vector<SecurityEvent> GetSecurityEvents() const = 0;
    
    // ========================================================================
    // Access Control
    // ========================================================================
    
    virtual bool SetSecurityLevel(SecurityLevel level) = 0;
    virtual SecurityLevel GetSecurityLevel() const = 0;
    virtual bool AuthenticateUser(const std::string& username, const std::string& password) = 0;
    virtual bool AuthorizeAccess(const std::string& username, const std::string& resource) = 0;
    virtual bool RevokeAccess(const std::string& username, const std::string& resource) = 0;
    
    // ========================================================================
    // Encryption and Data Protection
    // ========================================================================
    
    virtual bool EnableEncryption(bool enable) = 0;
    virtual bool IsEncryptionEnabled() const = 0;
    virtual bool EncryptData(const std::string& data, std::string& encryptedData) = 0;
    virtual bool DecryptData(const std::string& encryptedData, std::string& decryptedData) = 0;
    virtual bool RotateEncryptionKeys() = 0;
    
    // ========================================================================
    // Security Auditing
    // ========================================================================
    
    virtual bool EnableSecurityAuditing(bool enable) = 0;
    virtual bool IsSecurityAuditingEnabled() const = 0;
    virtual bool PerformSecurityAudit() = 0;
    virtual std::string GenerateSecurityReport() const = 0;
    virtual bool ExportSecurityLogs(const std::string& filePath) = 0;
    
    // ========================================================================
    // Real-time Security
    // ========================================================================
    
    virtual bool EnableRealTimeProtection(bool enable) = 0;
    virtual bool IsRealTimeProtectionEnabled() const = 0;
    virtual bool ProcessSecurityEvent(const SecurityEvent& event) = 0;
    virtual bool IsSystemSecure() const = 0;
    
    // ========================================================================
    // Configuration and Settings
    // ========================================================================
    
    virtual bool SetScanInterval(int milliseconds) = 0;
    virtual int GetScanInterval() const = 0;
    virtual bool SetMaxThreats(int maxThreats) = 0;
    virtual int GetMaxThreats() const = 0;
    virtual bool EnableAutoMitigation(bool enable) = 0;
    virtual bool IsAutoMitigationEnabled() const = 0;
    
    // ========================================================================
    // Statistics and Monitoring
    // ========================================================================
    
    virtual int GetTotalThreats() const = 0;
    virtual int GetActiveThreats() const = 0;
    virtual int GetTotalVulnerabilities() const = 0;
    virtual int GetPatchedVulnerabilities() const = 0;
    virtual std::string GetSecurityStatus() const = 0;
    virtual double GetSecurityScore() const = 0;
};

// ============================================================================
// Security Framework Implementation
// ============================================================================

class SecurityFramework : public ISecurityFramework {
private:
    // Core Components
    std::unique_ptr<VulnerabilityScanner> m_vulnerabilityScanner;
    std::unique_ptr<ThreatDetector> m_threatDetector;
    std::unique_ptr<SecurityMonitor> m_securityMonitor;
    std::unique_ptr<EncryptionManager> m_encryptionManager;
    std::unique_ptr<AccessController> m_accessController;
    std::unique_ptr<SecurityAuditor> m_securityAuditor;
    
    // Security State
    SecurityLevel m_currentSecurityLevel;
    bool m_threatDetectionEnabled;
    bool m_vulnerabilityScanningEnabled;
    bool m_securityMonitoringEnabled;
    bool m_encryptionEnabled;
    bool m_securityAuditingEnabled;
    bool m_realTimeProtectionEnabled;
    bool m_autoMitigationEnabled;
    bool m_isInitialized;
    
    // Data Storage
    std::vector<SecurityThreat> m_activeThreats;
    std::vector<VulnerabilityReport> m_vulnerabilities;
    std::vector<SecurityEvent> m_securityEvents;
    std::unordered_map<std::string, std::string> m_userPermissions;
    
    // Configuration
    int m_scanInterval;
    int m_maxThreats;
    std::chrono::steady_clock::time_point m_lastScan;
    std::chrono::steady_clock::time_point m_lastAudit;
    
    // Statistics
    int m_totalThreats;
    int m_totalVulnerabilities;
    int m_patchedVulnerabilities;
    double m_securityScore;
    
public:
    SecurityFramework();
    ~SecurityFramework();
    
    // ========================================================================
    // ISecurityFramework Implementation
    // ========================================================================
    
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_isInitialized; }
    
    bool EnableThreatDetection(bool enable) override;
    bool IsThreatDetectionEnabled() const override { return m_threatDetectionEnabled; }
    bool ScanForThreats() override;
    std::vector<SecurityThreat> GetActiveThreats() const override { return m_activeThreats; }
    bool MitigateThreat(const std::string& threatId) override;
    
    bool EnableVulnerabilityScanning(bool enable) override;
    bool IsVulnerabilityScanningEnabled() const override { return m_vulnerabilityScanningEnabled; }
    bool ScanForVulnerabilities() override;
    std::vector<VulnerabilityReport> GetVulnerabilities() const override { return m_vulnerabilities; }
    bool PatchVulnerability(const std::string& vulnerabilityId) override;
    
    bool EnableSecurityMonitoring(bool enable) override;
    bool IsSecurityMonitoringEnabled() const override { return m_securityMonitoringEnabled; }
    bool StartMonitoring() override;
    bool StopMonitoring() override;
    std::vector<SecurityEvent> GetSecurityEvents() const override { return m_securityEvents; }
    
    bool SetSecurityLevel(SecurityLevel level) override;
    SecurityLevel GetSecurityLevel() const override { return m_currentSecurityLevel; }
    bool AuthenticateUser(const std::string& username, const std::string& password) override;
    bool AuthorizeAccess(const std::string& username, const std::string& resource) override;
    bool RevokeAccess(const std::string& username, const std::string& resource) override;
    
    bool EnableEncryption(bool enable) override;
    bool IsEncryptionEnabled() const override { return m_encryptionEnabled; }
    bool EncryptData(const std::string& data, std::string& encryptedData) override;
    bool DecryptData(const std::string& encryptedData, std::string& decryptedData) override;
    bool RotateEncryptionKeys() override;
    
    bool EnableSecurityAuditing(bool enable) override;
    bool IsSecurityAuditingEnabled() const override { return m_securityAuditingEnabled; }
    bool PerformSecurityAudit() override;
    std::string GenerateSecurityReport() const override;
    bool ExportSecurityLogs(const std::string& filePath) override;
    
    bool EnableRealTimeProtection(bool enable) override;
    bool IsRealTimeProtectionEnabled() const override { return m_realTimeProtectionEnabled; }
    bool ProcessSecurityEvent(const SecurityEvent& event) override;
    bool IsSystemSecure() const override;
    
    bool SetScanInterval(int milliseconds) override;
    int GetScanInterval() const override { return m_scanInterval; }
    bool SetMaxThreats(int maxThreats) override;
    int GetMaxThreats() const override { return m_maxThreats; }
    bool EnableAutoMitigation(bool enable) override;
    bool IsAutoMitigationEnabled() const override { return m_autoMitigationEnabled; }
    
    int GetTotalThreats() const override { return m_totalThreats; }
    int GetActiveThreats() const override { return static_cast<int>(m_activeThreats.size()); }
    int GetTotalVulnerabilities() const override { return m_totalVulnerabilities; }
    int GetPatchedVulnerabilities() const override { return m_patchedVulnerabilities; }
    std::string GetSecurityStatus() const override;
    double GetSecurityScore() const override { return m_securityScore; }
    
private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================
    
    bool InitializeVulnerabilityScanner();
    bool InitializeThreatDetector();
    bool InitializeSecurityMonitor();
    bool InitializeEncryptionManager();
    bool InitializeAccessController();
    bool InitializeSecurityAuditor();
    
    bool ValidateThreat(const SecurityThreat& threat);
    bool ValidateVulnerability(const VulnerabilityReport& vulnerability);
    bool ValidateSecurityEvent(const SecurityEvent& event);
    
    bool ProcessThreatDetection();
    bool ProcessVulnerabilityScanning();
    bool ProcessSecurityMonitoring();
    bool ProcessAccessControl();
    
    void UpdateSecurityStatistics();
    void CalculateSecurityScore();
    void LogSecurityEvent(const SecurityEvent& event);
    void LogSecurityOperation(const std::string& operation, const std::string& details);
    
    bool IsThreatMitigationSafe(const std::string& threatId);
    bool IsVulnerabilityPatchSafe(const std::string& vulnerabilityId);
    void HandleSecurityError(const std::string& operation, const std::string& error);
    
    void CleanupOldThreats();
    void CleanupOldVulnerabilities();
    void CleanupOldSecurityEvents();
    
    bool PerformThreatAnalysis();
    bool PerformVulnerabilityAnalysis();
    bool PerformSecurityAnalysis();
    
    std::string GenerateThreatReport() const;
    std::string GenerateVulnerabilityReport() const;
    std::string GenerateSecurityEventReport() const;
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<ISecurityFramework> CreateSecurityFramework();
std::unique_ptr<ISecurityFramework> CreateSecurityFrameworkWithConfig(const std::string& configPath);

} // namespace UEVRLCX
