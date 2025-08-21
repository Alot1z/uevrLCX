#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <fstream>
#include <sstream>
#include <regex>

// Core system includes
#include "../core/Framework.hpp"
#include "../core/PluginManager.hpp"

namespace uevr {

/**
 * @brief Reddit Community Integration System
 * 
 * Integrates community-driven VR conversion solutions from Reddit communities:
 * - r/oculus: https://www.reddit.com/r/oculus/comments/am9tyv/best_solution_to_turn_any_game_into_vr_game/
 * - r/VRGaming: VR gaming community solutions
 * - r/SteamVR: SteamVR development techniques
 * - r/Unity3D: Unity VR conversion methods
 * - r/unrealengine: Unreal Engine VR modding
 * 
 * This system provides real-world, community-tested VR conversion techniques
 * that have been proven to work across various games and engines.
 */
class RedditCommunityIntegration {
public:
    // Singleton pattern
    static RedditCommunityIntegration& getInstance();
    
    // Disable copy and assignment
    RedditCommunityIntegration(const RedditCommunityIntegration&) = delete;
    RedditCommunityIntegration& operator=(const RedditCommunityIntegration&) = delete;

    // Core system lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }

    // Community solution management
    struct CommunitySolution {
        std::string id;
        std::string title;
        std::string description;
        std::string gameName;
        std::string engineType;
        std::string redditUrl;
        std::string author;
        std::chrono::system_clock::time_point datePosted;
        int upvotes;
        int comments;
        bool isVerified;
        std::vector<std::string> tags;
        std::string implementationCode;
        std::string configuration;
        std::unordered_map<std::string, std::string> parameters;
        
        CommunitySolution() : upvotes(0), comments(0), isVerified(false) {}
    };

    // Solution database management
    bool loadCommunitySolutions();
    bool saveCommunitySolutions();
    std::vector<CommunitySolution> getSolutionsForGame(const std::string& gameName);
    std::vector<CommunitySolution> getSolutionsForEngine(const std::string& engineType);
    std::vector<CommunitySolution> getTopRatedSolutions(int limit = 10);
    bool addCommunitySolution(const CommunitySolution& solution);
    bool updateCommunitySolution(const std::string& id, const CommunitySolution& solution);
    bool verifySolution(const std::string& id, bool verified = true);

    // Reddit API integration
    struct RedditAPI {
        std::string baseUrl = "https://www.reddit.com";
        std::string userAgent = "uevrLCX/1.0";
        std::string accessToken;
        std::chrono::system_clock::time_point tokenExpiry;
        
        bool authenticate();
        std::string fetchSubredditPosts(const std::string& subreddit, const std::string& query = "");
        std::string fetchPostComments(const std::string& postId);
        std::string fetchUserPosts(const std::string& username);
    };

    // Community solution categories
    enum class SolutionCategory {
        INJECTION_METHODS,
        DEPTH_EXTRACTION,
        SHADER_MODIFICATION,
        INPUT_MAPPING,
        PERFORMANCE_OPTIMIZATION,
        COLLISION_DETECTION,
        AUDIO_SPATIALIZATION,
        UI_ADAPTATION,
        MOTION_CONTROLLERS,
        ROOM_SCALE,
        COMFORT_SETTINGS,
        ENGINE_SPECIFIC,
        LEGACY_SUPPORT,
        EXPERIMENTAL,
        VERIFIED_WORKING
    };

    // Solution filtering and search
    std::vector<CommunitySolution> searchSolutions(const std::string& query);
    std::vector<CommunitySolution> filterByCategory(SolutionCategory category);
    std::vector<CommunitySolution> filterByDateRange(std::chrono::system_clock::time_point start, 
                                                    std::chrono::system_clock::time_point end);
    std::vector<CommunitySolution> filterByUpvotes(int minUpvotes);

    // Community learning and AI integration
    struct CommunityLearning {
        std::unordered_map<std::string, double> successRates;
        std::unordered_map<std::string, std::vector<std::string>> commonPatterns;
        std::unordered_map<std::string, std::vector<std::string>> failureModes;
        std::unordered_map<std::string, std::vector<std::string>> optimizationTips;
        
        void analyzeSolutionSuccess(const std::string& solutionId, bool success);
        void updatePatterns(const std::string& gameType, const std::vector<std::string>& patterns);
        std::vector<std::string> getRecommendedSolutions(const std::string& gameName);
        double getSuccessRate(const std::string& solutionId);
    };

    // Real-time community monitoring
    struct CommunityMonitor {
        std::atomic<bool> isMonitoring{false};
        std::thread monitorThread;
        std::chrono::seconds checkInterval{300}; // 5 minutes
        
        void startMonitoring();
        void stopMonitoring();
        void monitorNewSolutions();
        void monitorTrendingTopics();
        void alertNewHighRatedSolutions();
    };

    // Community solution application
    bool applyCommunitySolution(const std::string& solutionId, const std::string& gamePath);
    bool testCommunitySolution(const std::string& solutionId, const std::string& gamePath);
    std::string generateSolutionReport(const std::string& solutionId);
    bool shareSolutionResults(const std::string& solutionId, bool success, const std::string& notes);

    // Community contribution system
    struct Contribution {
        std::string contributorId;
        std::string solutionId;
        std::string contributionType; // "test", "improvement", "bug_fix", "new_solution"
        std::string description;
        std::string codeChanges;
        std::chrono::system_clock::time_point timestamp;
        int communityScore;
    };

    bool submitContribution(const Contribution& contribution);
    std::vector<Contribution> getContributionsForSolution(const std::string& solutionId);
    bool approveContribution(const std::string& contributionId);
    bool rejectContribution(const std::string& contributionId, const std::string& reason);

    // Community solution validation
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
        double confidenceScore;
        std::string validationReport;
    };

    ValidationResult validateSolution(const CommunitySolution& solution);
    bool runAutomatedTests(const std::string& solutionId);
    std::string generateValidationReport(const std::string& solutionId);

    // Community solution optimization
    struct OptimizationResult {
        std::string originalSolutionId;
        std::string optimizedSolutionId;
        double performanceImprovement;
        double compatibilityImprovement;
        std::vector<std::string> optimizations;
        std::string optimizationReport;
    };

    OptimizationResult optimizeSolution(const std::string& solutionId);
    bool applyOptimizations(const std::string& solutionId, const std::vector<std::string>& optimizations);

    // Community solution compatibility matrix
    struct CompatibilityMatrix {
        std::unordered_map<std::string, std::unordered_map<std::string, double>> gameEngineCompatibility;
        std::unordered_map<std::string, std::unordered_map<std::string, double>> solutionCompatibility;
        std::unordered_map<std::string, std::vector<std::string>> knownConflicts;
        std::unordered_map<std::string, std::vector<std::string>> recommendedCombinations;
        
        double getCompatibilityScore(const std::string& gameName, const std::string& engineType);
        double getSolutionCompatibility(const std::string& solutionId, const std::string& gameName);
        std::vector<std::string> getCompatibleSolutions(const std::string& gameName);
        std::vector<std::string> getConflictingSolutions(const std::string& solutionId);
    };

    // Community solution performance tracking
    struct PerformanceMetrics {
        std::string solutionId;
        double averageFPS;
        double averageLatency;
        double memoryUsage;
        double cpuUsage;
        double gpuUsage;
        int totalTests;
        int successfulTests;
        double successRate;
        std::chrono::system_clock::time_point lastUpdated;
        
        PerformanceMetrics() : averageFPS(0.0), averageLatency(0.0), memoryUsage(0.0),
                              cpuUsage(0.0), gpuUsage(0.0), totalTests(0), successfulTests(0),
                              successRate(0.0) {}
    };

    bool updatePerformanceMetrics(const std::string& solutionId, const PerformanceMetrics& metrics);
    PerformanceMetrics getPerformanceMetrics(const std::string& solutionId);
    std::vector<PerformanceMetrics> getTopPerformingSolutions(int limit = 10);

    // Community solution documentation
    struct Documentation {
        std::string solutionId;
        std::string installationGuide;
        std::string configurationGuide;
        std::string troubleshootingGuide;
        std::string faq;
        std::vector<std::string> screenshots;
        std::vector<std::string> videos;
        std::string changelog;
        std::string credits;
    };

    bool updateDocumentation(const std::string& solutionId, const Documentation& doc);
    Documentation getDocumentation(const std::string& solutionId);
    std::string generateDocumentation(const std::string& solutionId);

    // Community solution backup and sync
    bool backupCommunityDatabase();
    bool restoreCommunityDatabase(const std::string& backupPath);
    bool syncWithRemoteDatabase();
    bool exportCommunityData(const std::string& exportPath);
    bool importCommunityData(const std::string& importPath);

private:
    // Internal data structures
    std::atomic<bool> m_initialized{false};
    std::mutex m_mutex;
    std::condition_variable m_cv;
    
    // Core components
    std::unique_ptr<Framework> m_framework;
    std::unique_ptr<PluginManager> m_pluginManager;
    
    // Community data
    std::vector<CommunitySolution> m_communitySolutions;
    std::unordered_map<std::string, CommunitySolution> m_solutionMap;
    RedditAPI m_redditAPI;
    CommunityLearning m_learning;
    CommunityMonitor m_monitor;
    CompatibilityMatrix m_compatibilityMatrix;
    std::unordered_map<std::string, PerformanceMetrics> m_performanceMetrics;
    std::unordered_map<std::string, Documentation> m_documentation;
    
    // Internal methods
    bool initializeRedditAPI();
    bool initializeCommunityDatabase();
    bool initializeMonitoring();
    void parseRedditPost(const std::string& postData, CommunitySolution& solution);
    void parseRedditComment(const std::string& commentData, CommunitySolution& solution);
    std::string extractSolutionCode(const std::string& text);
    std::vector<std::string> extractTags(const std::string& text);
    bool validateSolutionFormat(const CommunitySolution& solution);
    void updateCompatibilityMatrix();
    void updateLearningPatterns();
    void monitorCommunityActivity();
    std::string generateSolutionId(const CommunitySolution& solution);
    bool saveSolutionToDatabase(const CommunitySolution& solution);
    bool loadSolutionFromDatabase(const std::string& id);
    void cleanupExpiredData();
    void performDatabaseMaintenance();
};

} // namespace uevr
