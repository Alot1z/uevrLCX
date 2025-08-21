#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono> // Added for std::chrono

namespace UEVRLCX {
namespace Context7AI {

// ============================================================================
// Context7 AI Database - Structured VR Conversion Knowledge
// ============================================================================

struct ComponentContext {
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> dependencies;
    std::vector<std::string> features;
    std::unordered_map<std::string, std::string> metadata;
    double complexity;
    double importance;
    bool isImplemented;
    
    ComponentContext() : complexity(0.0), importance(0.0), isImplemented(false) {}
};

struct SystemLayer {
    std::string name;
    std::string description;
    std::vector<std::string> components;
    std::vector<std::string> subLayers;
    double priority;
    bool isActive;
    
    SystemLayer() : priority(0.0), isActive(false) {}
};

struct KnowledgeNode {
    std::string id;
    std::string title;
    std::string content;
    std::vector<std::string> tags;
    std::vector<std::string> relatedNodes;
    std::chrono::steady_clock::time_point lastUpdated;
    double relevance;
    
    KnowledgeNode() : relevance(0.0) {}
};

class Context7AIDatabase {
public:
    Context7AIDatabase();
    ~Context7AIDatabase();

    // ========================================================================
    // Core Database Operations
    // ========================================================================
    
    bool Initialize();
    bool LoadKnowledgeBase(const std::string& knowledgePath);
    bool SaveKnowledgeBase(const std::string& knowledgePath);
    bool UpdateKnowledgeBase();
    void Shutdown();

    // ========================================================================
    // Component Context Management
    // ========================================================================
    
    bool AddComponentContext(const ComponentContext& context);
    bool UpdateComponentContext(const std::string& name, const ComponentContext& context);
    bool RemoveComponentContext(const std::string& name);
    ComponentContext GetComponentContext(const std::string& name) const;
    std::vector<ComponentContext> GetAllComponentContexts() const;
    std::vector<ComponentContext> GetComponentContextsByCategory(const std::string& category) const;

    // ========================================================================
    // System Layer Management
    // ========================================================================
    
    bool AddSystemLayer(const SystemLayer& layer);
    bool UpdateSystemLayer(const std::string& name, const SystemLayer& layer);
    bool RemoveSystemLayer(const std::string& name);
    SystemLayer GetSystemLayer(const std::string& name) const;
    std::vector<SystemLayer> GetAllSystemLayers() const;
    std::vector<SystemLayer> GetActiveSystemLayers() const;

    // ========================================================================
    // Knowledge Node Management
    // ========================================================================
    
    bool AddKnowledgeNode(const KnowledgeNode& node);
    bool UpdateKnowledgeNode(const std::string& id, const KnowledgeNode& node);
    bool RemoveKnowledgeNode(const std::string& id);
    KnowledgeNode GetKnowledgeNode(const std::string& id) const;
    std::vector<KnowledgeNode> GetAllKnowledgeNodes() const;
    std::vector<KnowledgeNode> GetKnowledgeNodesByTag(const std::string& tag) const;

    // ========================================================================
    // Contextual Analysis and Relationships
    // ========================================================================
    
    std::vector<std::string> GetComponentDependencies(const std::string& componentName) const;
    std::vector<std::string> GetRelatedComponents(const std::string& componentName) const;
    std::vector<std::string> GetSystemDependencies(const std::string& systemName) const;
    double CalculateComponentComplexity(const std::string& componentName) const;
    double CalculateSystemPriority(const std::string& systemName) const;

    // ========================================================================
    // AI-Powered Analysis
    // ========================================================================
    
    bool AnalyzeSystemArchitecture();
    bool OptimizeComponentRelationships();
    bool GenerateImplementationPlan();
    bool ValidateSystemConsistency();
    std::vector<std::string> GetRecommendedComponents(const std::string& context) const;
    std::vector<std::string> GetOptimizationSuggestions() const;

    // ========================================================================
    // Search and Query
    // ========================================================================
    
    std::vector<ComponentContext> SearchComponents(const std::string& query) const;
    std::vector<SystemLayer> SearchSystems(const std::string& query) const;
    std::vector<KnowledgeNode> SearchKnowledge(const std::string& query) const;
    std::vector<std::string> GetAutocompleteSuggestions(const std::string& partial) const;

    // ========================================================================
    // Integration with VR Conversion System
    // ========================================================================
    
    bool IntegrateWithVRSystem();
    bool UpdateVRComponentStatus(const std::string& componentName, bool isImplemented);
    bool AddVRConversionContext(const std::string& gameEngine, const std::string& renderingAPI);
    std::vector<ComponentContext> GetVRConversionComponents() const;
    bool ValidateVRConversionPlan(const std::vector<std::string>& components) const;

    // ========================================================================
    // Performance and Monitoring
    // ========================================================================
    
    bool StartMonitoring();
    bool StopMonitoring();
    bool IsMonitoring() const;
    std::string GetDatabaseStats() const;
    bool OptimizeDatabase();
    bool BackupDatabase(const std::string& backupPath);

private:
    bool m_isInitialized;
    bool m_isMonitoring;
    
    // Database storage
    std::unordered_map<std::string, ComponentContext> m_componentContexts;
    std::unordered_map<std::string, SystemLayer> m_systemLayers;
    std::unordered_map<std::string, KnowledgeNode> m_knowledgeNodes;
    
    // Relationships and dependencies
    std::unordered_map<std::string, std::vector<std::string>> m_componentDependencies;
    std::unordered_map<std::string, std::vector<std::string>> m_systemDependencies;
    std::unordered_map<std::string, std::vector<std::string>> m_knowledgeRelationships;
    
    // AI analysis cache
    std::unordered_map<std::string, double> m_complexityCache;
    std::unordered_map<std::string, double> m_priorityCache;
    std::vector<std::string> m_optimizationSuggestions;
    
    // Private helper methods
    bool LoadComponentContexts();
    bool LoadSystemLayers();
    bool LoadKnowledgeNodes();
    bool SaveComponentContexts();
    bool SaveSystemLayers();
    bool SaveKnowledgeNodes();
    
    bool AnalyzeComponentRelationships();
    bool UpdateDependencyGraph();
    bool ValidateDataConsistency();
    
    std::vector<std::string> TokenizeQuery(const std::string& query) const;
    double CalculateRelevance(const std::string& query, const std::string& content) const;
    bool IsComponentCompatible(const std::string& component1, const std::string& component2) const;
};

} // namespace Context7AI
} // namespace UEVRLCX
