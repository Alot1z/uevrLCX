#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <functional>

namespace UEVRLCX {

// ============================================================================
// Forward Declarations
// ============================================================================

class KnowledgeBase;
class DataFetcher;
class CommunityAPI;
class KnowledgeProcessor;

// ============================================================================
// Knowledge Data Structures
// ============================================================================

struct KnowledgeEntry {
    std::string id;
    std::string title;
    std::string content;
    std::string source;
    std::string category;
    std::vector<std::string> tags;
    std::chrono::steady_clock::time_point timestamp;
    double relevance;
    double confidence;
    
    KnowledgeEntry() : relevance(0.0), confidence(0.0) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    KnowledgeEntry(const std::string& i, const std::string& t, const std::string& c, const std::string& s)
        : id(i), title(t), content(c), source(s), relevance(0.0), confidence(0.0) {
        timestamp = std::chrono::steady_clock::now();
    }
};

struct DataSource {
    std::string name;
    std::string url;
    std::string type;
    bool isActive;
    bool isAuthenticated;
    std::chrono::steady_clock::time_point lastSync;
    std::chrono::steady_clock::time_point nextSync;
    
    DataSource() : isActive(false), isAuthenticated(false) {
        lastSync = std::chrono::steady_clock::now();
        nextSync = std::chrono::steady_clock::now();
    }
    
    DataSource(const std::string& n, const std::string& u, const std::string& t)
        : name(n), url(u), type(t), isActive(false), isAuthenticated(false) {
        lastSync = std::chrono::steady_clock::now();
        nextSync = std::chrono::steady_clock::now();
    }
};

struct SyncResult {
    bool success;
    int entriesAdded;
    int entriesUpdated;
    int entriesRemoved;
    std::string message;
    std::chrono::steady_clock::time_point timestamp;
    
    SyncResult() : success(false), entriesAdded(0), entriesUpdated(0), entriesRemoved(0) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    SyncResult(bool s, int added, int updated, int removed, const std::string& msg)
        : success(s), entriesAdded(added), entriesUpdated(updated), entriesRemoved(removed), message(msg) {
        timestamp = std::chrono::steady_clock::now();
    }
};

// ============================================================================
// DeepWiki Connector Interface
// ============================================================================

class IDeepWikiConnector {
public:
    virtual ~IDeepWikiConnector() = default;
    
    // ========================================================================
    // Initialization and Management
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ========================================================================
    // Knowledge Base Management
    // ========================================================================
    
    virtual bool AddKnowledgeEntry(const KnowledgeEntry& entry) = 0;
    virtual bool UpdateKnowledgeEntry(const std::string& id, const KnowledgeEntry& entry) = 0;
    virtual bool RemoveKnowledgeEntry(const std::string& id) = 0;
    virtual KnowledgeEntry* GetKnowledgeEntry(const std::string& id) = 0;
    virtual std::vector<KnowledgeEntry> SearchKnowledge(const std::string& query) = 0;
    
    // ========================================================================
    // Data Source Management
    // ========================================================================
    
    virtual bool AddDataSource(const DataSource& source) = 0;
    virtual bool RemoveDataSource(const std::string& name) = 0;
    virtual bool EnableDataSource(const std::string& name, bool enable) = 0;
    virtual std::vector<DataSource> GetDataSources() const = 0;
    virtual bool IsDataSourceActive(const std::string& name) const = 0;
    
    // ========================================================================
    // Synchronization and Updates
    // ========================================================================
    
    virtual SyncResult SyncDataSource(const std::string& sourceName) = 0;
    virtual SyncResult SyncAllDataSources() = 0;
    virtual bool IsSyncInProgress() const = 0;
    virtual std::chrono::steady_clock::time_point GetLastSyncTime(const std::string& sourceName) const = 0;
    
    // ========================================================================
    // Real-time Processing
    // ========================================================================
    
    virtual bool EnableRealTimeSync(bool enable) = 0;
    virtual bool IsRealTimeSyncEnabled() const = 0;
    virtual bool ProcessRealTimeUpdate(const std::string& source, const std::string& data) = 0;
    
    // ========================================================================
    // Community Integration
    // ========================================================================
    
    virtual bool ConnectToCommunity(const std::string& communityName) = 0;
    virtual bool DisconnectFromCommunity(const std::string& communityName) = 0;
    virtual std::vector<std::string> GetConnectedCommunities() const = 0;
    virtual bool IsCommunityConnected(const std::string& communityName) const = 0;
    
    // ========================================================================
    // Configuration and Settings
    // ========================================================================
    
    virtual bool SetSyncInterval(int milliseconds) = 0;
    virtual int GetSyncInterval() const = 0;
    virtual bool SetMaxEntries(size_t maxEntries) = 0;
    virtual size_t GetMaxEntries() const = 0;
    
    // ========================================================================
    // Statistics and Monitoring
    // ========================================================================
    
    virtual size_t GetTotalEntries() const = 0;
    virtual size_t GetActiveSources() const = 0;
    virtual std::chrono::steady_clock::time_point GetLastGlobalSync() const = 0;
    virtual std::string GetSystemHealth() const = 0;
};

// ============================================================================
// DeepWiki Connector Implementation
// ============================================================================

class DeepWikiConnector : public IDeepWikiConnector {
private:
    // Core Components
    std::unique_ptr<KnowledgeBase> m_knowledgeBase;
    std::unique_ptr<DataFetcher> m_dataFetcher;
    std::unique_ptr<CommunityAPI> m_communityAPI;
    std::unique_ptr<KnowledgeProcessor> m_processor;
    
    // Data Sources
    std::unordered_map<std::string, DataSource> m_dataSources;
    std::vector<std::string> m_activeSources;
    
    // Configuration
    int m_syncInterval;
    size_t m_maxEntries;
    bool m_realTimeSyncEnabled;
    bool m_isInitialized;
    
    // Synchronization State
    std::atomic<bool> m_syncInProgress;
    std::chrono::steady_clock::time_point m_lastGlobalSync;
    
    // Community Connections
    std::vector<std::string> m_connectedCommunities;
    
public:
    DeepWikiConnector();
    ~DeepWikiConnector();
    
    // ========================================================================
    // IDeepWikiConnector Implementation
    // ========================================================================
    
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_isInitialized; }
    
    bool AddKnowledgeEntry(const KnowledgeEntry& entry) override;
    bool UpdateKnowledgeEntry(const std::string& id, const KnowledgeEntry& entry) override;
    bool RemoveKnowledgeEntry(const std::string& id) override;
    KnowledgeEntry* GetKnowledgeEntry(const std::string& id) override;
    std::vector<KnowledgeEntry> SearchKnowledge(const std::string& query) override;
    
    bool AddDataSource(const DataSource& source) override;
    bool RemoveDataSource(const std::string& name) override;
    bool EnableDataSource(const std::string& name, bool enable) override;
    std::vector<DataSource> GetDataSources() const override;
    bool IsDataSourceActive(const std::string& name) const override;
    
    SyncResult SyncDataSource(const std::string& sourceName) override;
    SyncResult SyncAllDataSources() override;
    bool IsSyncInProgress() const override { return m_syncInProgress.load(); }
    std::chrono::steady_clock::time_point GetLastSyncTime(const std::string& sourceName) const override;
    
    bool EnableRealTimeSync(bool enable) override;
    bool IsRealTimeSyncEnabled() const override { return m_realTimeSyncEnabled; }
    bool ProcessRealTimeUpdate(const std::string& source, const std::string& data) override;
    
    bool ConnectToCommunity(const std::string& communityName) override;
    bool DisconnectFromCommunity(const std::string& communityName) override;
    std::vector<std::string> GetConnectedCommunities() const override { return m_connectedCommunities; }
    bool IsCommunityConnected(const std::string& communityName) const override;
    
    bool SetSyncInterval(int milliseconds) override;
    int GetSyncInterval() const override { return m_syncInterval; }
    bool SetMaxEntries(size_t maxEntries) override;
    size_t GetMaxEntries() const override { return m_maxEntries; }
    
    size_t GetTotalEntries() const override;
    size_t GetActiveSources() const override { return m_activeSources.size(); }
    std::chrono::steady_clock::time_point GetLastGlobalSync() const override { return m_lastGlobalSync; }
    std::string GetSystemHealth() const override;
    
private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================
    
    bool InitializeKnowledgeBase();
    bool InitializeDataFetcher();
    bool InitializeCommunityAPI();
    bool InitializeKnowledgeProcessor();
    
    bool ValidateDataSource(const DataSource& source);
    bool AuthenticateDataSource(DataSource& source);
    void UpdateDataSourceSyncTime(const std::string& sourceName);
    
    SyncResult PerformDataSourceSync(const std::string& sourceName);
    bool ProcessSyncResult(const SyncResult& result);
    void HandleSyncError(const std::string& sourceName, const std::string& error);
    
    bool ValidateKnowledgeEntry(const KnowledgeEntry& entry);
    void ProcessKnowledgeEntry(KnowledgeEntry& entry);
    void CleanupOldEntries();
    
    void LogSyncOperation(const std::string& sourceName, const SyncResult& result);
    void LogKnowledgeOperation(const std::string& operation, const std::string& entryId);
    void LogCommunityOperation(const std::string& operation, const std::string& communityName);
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<IDeepWikiConnector> CreateDeepWikiConnector();
std::unique_ptr<IDeepWikiConnector> CreateDeepWikiConnectorWithConfig(const std::string& configPath);

} // namespace UEVRLCX
