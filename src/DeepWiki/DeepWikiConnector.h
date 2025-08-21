#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace UEVRLCX {
namespace DeepWiki {

// ============================================================================
// DeepWiki Connector - In-Depth Technical Documentation
// ============================================================================

struct TechnicalDocument {
    std::string id;
    std::string title;
    std::string content;
    std::string category;
    std::vector<std::string> tags;
    std::vector<std::string> references;
    std::vector<std::string> codeExamples;
    std::chrono::steady_clock::time_point lastUpdated;
    double technicalDepth;
    double complexity;
    bool isVerified;
    
    TechnicalDocument() : technicalDepth(0.0), complexity(0.0), isVerified(false) {}
};

struct ImplementationGuide {
    std::string id;
    std::string title;
    std::string description;
    std::vector<std::string> prerequisites;
    std::vector<std::string> steps;
    std::vector<std::string> codeSnippets;
    std::vector<std::string> troubleshooting;
    std::unordered_map<std::string, std::string> configuration;
    double difficulty;
    double estimatedTime;
    bool isComplete;
    
    ImplementationGuide() : difficulty(0.0), estimatedTime(0.0), isComplete(false) {}
};

struct PerformanceAnalysis {
    std::string id;
    std::string title;
    std::string description;
    std::vector<std::string> metrics;
    std::vector<std::string> benchmarks;
    std::vector<std::string> optimizations;
    std::unordered_map<std::string, double> performanceData;
    std::vector<std::string> recommendations;
    double baselinePerformance;
    double optimizedPerformance;
    double improvementFactor;
    
    PerformanceAnalysis() : baselinePerformance(0.0), optimizedPerformance(0.0), improvementFactor(0.0) {}
};

struct ArchitecturePattern {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> components;
    std::vector<std::string> interactions;
    std::vector<std::string> benefits;
    std::vector<std::string> drawbacks;
    std::vector<std::string> useCases;
    std::unordered_map<std::string, std::string> implementation;
    double complexity;
    double effectiveness;
    bool isRecommended;
    
    ArchitecturePattern() : complexity(0.0), effectiveness(0.0), isRecommended(false) {}
};

class DeepWikiConnector {
public:
    DeepWikiConnector();
    ~DeepWikiConnector();

    // ========================================================================
    // Core Documentation Operations
    // ========================================================================
    
    bool Initialize();
    bool LoadDocumentation(const std::string& docPath);
    bool SaveDocumentation(const std::string& docPath);
    bool UpdateDocumentation();
    void Shutdown();

    // ========================================================================
    // Technical Document Management
    // ========================================================================
    
    bool AddTechnicalDocument(const TechnicalDocument& document);
    bool UpdateTechnicalDocument(const std::string& id, const TechnicalDocument& document);
    bool RemoveTechnicalDocument(const std::string& id);
    TechnicalDocument GetTechnicalDocument(const std::string& id) const;
    std::vector<TechnicalDocument> GetAllTechnicalDocuments() const;
    std::vector<TechnicalDocument> GetTechnicalDocumentsByCategory(const std::string& category) const;
    std::vector<TechnicalDocument> GetTechnicalDocumentsByTag(const std::string& tag) const;

    // ========================================================================
    // Implementation Guide Management
    // ========================================================================
    
    bool AddImplementationGuide(const ImplementationGuide& guide);
    bool UpdateImplementationGuide(const std::string& id, const ImplementationGuide& guide);
    bool RemoveImplementationGuide(const std::string& id);
    ImplementationGuide GetImplementationGuide(const std::string& id) const;
    std::vector<ImplementationGuide> GetAllImplementationGuides() const;
    std::vector<ImplementationGuide> GetImplementationGuidesByDifficulty(double minDifficulty, double maxDifficulty) const;

    // ========================================================================
    // Performance Analysis Management
    // ========================================================================
    
    bool AddPerformanceAnalysis(const PerformanceAnalysis& analysis);
    bool UpdatePerformanceAnalysis(const std::string& id, const PerformanceAnalysis& analysis);
    bool RemovePerformanceAnalysis(const std::string& id);
    PerformanceAnalysis GetPerformanceAnalysis(const std::string& id) const;
    std::vector<PerformanceAnalysis> GetAllPerformanceAnalyses() const;
    std::vector<PerformanceAnalysis> GetPerformanceAnalysesByImprovement(double minImprovement) const;

    // ========================================================================
    // Architecture Pattern Management
    // ========================================================================
    
    bool AddArchitecturePattern(const ArchitecturePattern& pattern);
    bool UpdateArchitecturePattern(const std::string& id, const ArchitecturePattern& pattern);
    bool RemoveArchitecturePattern(const std::string& id);
    ArchitecturePattern GetArchitecturePattern(const std::string& id) const;
    std::vector<ArchitecturePattern> GetAllArchitecturePatterns() const;
    std::vector<ArchitecturePattern> GetRecommendedArchitecturePatterns() const;

    // ========================================================================
    // Cross-Reference and Linking
    // ========================================================================
    
    std::vector<std::string> GetRelatedDocuments(const std::string& documentId) const;
    std::vector<std::string> GetReferencedDocuments(const std::string& documentId) const;
    std::vector<std::string> GetDocumentReferences(const std::string& documentId) const;
    bool CreateCrossReference(const std::string& sourceId, const std::string& targetId);
    bool RemoveCrossReference(const std::string& sourceId, const std::string& targetId);

    // ========================================================================
    // Search and Query
    // ========================================================================
    
    std::vector<TechnicalDocument> SearchTechnicalDocuments(const std::string& query) const;
    std::vector<ImplementationGuide> SearchImplementationGuides(const std::string& query) const;
    std::vector<PerformanceAnalysis> SearchPerformanceAnalyses(const std::string& query) const;
    std::vector<ArchitecturePattern> SearchArchitecturePatterns(const std::string& query) const;
    std::vector<std::string> GetSearchSuggestions(const std::string& partial) const;

    // ========================================================================
    // Code Example Management
    // ========================================================================
    
    bool AddCodeExample(const std::string& documentId, const std::string& codeExample);
    bool UpdateCodeExample(const std::string& documentId, const std::string& oldCode, const std::string& newCode);
    bool RemoveCodeExample(const std::string& documentId, const std::string& codeExample);
    std::vector<std::string> GetCodeExamples(const std::string& documentId) const;
    std::vector<std::string> SearchCodeExamples(const std::string& query) const;

    // ========================================================================
    // Integration with VR Conversion System
    // ========================================================================
    
    bool IntegrateWithVRSystem();
    bool AddVRConversionDocumentation(const std::string& gameEngine, const std::string& renderingAPI);
    std::vector<TechnicalDocument> GetVRConversionDocuments() const;
    std::vector<ImplementationGuide> GetVRConversionGuides() const;
    std::vector<PerformanceAnalysis> GetVRPerformanceAnalyses() const;
    std::vector<ArchitecturePattern> GetVRArchitecturePatterns() const;

    // ========================================================================
    // Documentation Quality and Verification
    // ========================================================================
    
    bool VerifyDocumentation(const std::string& documentId);
    bool ValidateDocumentation(const std::string& documentId);
    double CalculateDocumentQuality(const std::string& documentId) const;
    std::vector<std::string> GetDocumentationIssues(const std::string& documentId) const;
    bool FixDocumentationIssues(const std::string& documentId, const std::vector<std::string>& fixes);

    // ========================================================================
    // Export and Import
    // ========================================================================
    
    bool ExportDocumentation(const std::string& exportPath, const std::string& format);
    bool ImportDocumentation(const std::string& importPath, const std::string& format);
    bool GenerateDocumentationReport(const std::string& reportPath);
    std::string GetDocumentationStats() const;

    // ========================================================================
    // Collaboration and Versioning
    // ========================================================================
    
    bool CreateDocumentVersion(const std::string& documentId, const std::string& version);
    bool RestoreDocumentVersion(const std::string& documentId, const std::string& version);
    std::vector<std::string> GetDocumentVersions(const std::string& documentId) const;
    bool CompareDocumentVersions(const std::string& documentId, const std::string& version1, const std::string& version2);

private:
    bool m_isInitialized;
    
    // Documentation storage
    std::unordered_map<std::string, TechnicalDocument> m_technicalDocuments;
    std::unordered_map<std::string, ImplementationGuide> m_implementationGuides;
    std::unordered_map<std::string, PerformanceAnalysis> m_performanceAnalyses;
    std::unordered_map<std::string, ArchitecturePattern> m_architecturePatterns;
    
    // Cross-references and relationships
    std::unordered_map<std::string, std::vector<std::string>> m_documentReferences;
    std::unordered_map<std::string, std::vector<std::string>> m_documentReferencedBy;
    std::unordered_map<std::string, std::vector<std::string>> m_documentCrossReferences;
    
    // Version control
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_documentVersions;
    
    // Quality metrics
    std::unordered_map<std::string, double> m_documentQuality;
    std::unordered_map<std::string, std::vector<std::string>> m_documentIssues;
    
    // Private helper methods
    bool LoadTechnicalDocuments();
    bool LoadImplementationGuides();
    bool LoadPerformanceAnalyses();
    bool LoadArchitecturePatterns();
    bool SaveTechnicalDocuments();
    bool SaveImplementationGuides();
    bool SavePerformanceAnalyses();
    bool SaveArchitecturePatterns();
    
    bool ValidateDocumentStructure(const TechnicalDocument& document) const;
    bool ValidateGuideStructure(const ImplementationGuide& guide) const;
    bool ValidateAnalysisStructure(const PerformanceAnalysis& analysis) const;
    bool ValidatePatternStructure(const ArchitecturePattern& pattern) const;
    
    std::vector<std::string> TokenizeContent(const std::string& content) const;
    double CalculateRelevance(const std::string& query, const std::string& content) const;
    bool IsDocumentCompatible(const std::string& document1, const std::string& document2) const;
    
    bool UpdateCrossReferences(const std::string& documentId);
    bool ValidateCrossReferences();
    bool OptimizeDocumentationStructure();
};

} // namespace DeepWiki
} // namespace UEVRLCX
