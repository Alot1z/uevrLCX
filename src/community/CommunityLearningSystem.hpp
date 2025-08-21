#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <future>

// Network and API includes
#include <curl/curl.h>
#include <json/json.h>
#include <sqlite3.h>

// VR-specific includes
#include "../vr/VRSystem.hpp"
#include "../vr/OpenXRIntegration.hpp"
#include "../ai/AIPoweredVROptimizer.hpp"
#include "../adapters/AdvancedEngineAdapters.hpp"

namespace uevr {

enum class CommunitySource {
    REDDIT,
    GITHUB,
    DISCORD,
    FORUMS,
    YOUTUBE,
    TWITTER,
    BLOG,
    WIKI,
    CUSTOM_API,
    LOCAL_DATABASE
};

enum class SolutionType {
    VR_INJECTION,
    DEPTH_EXTRACTION,
    SHADER_MODIFICATION,
    INPUT_MAPPING,
    PERFORMANCE_OPTIMIZATION,
    COLLISION_DETECTION,
    AUDIO_SPATIALIZATION,
    HAPTIC_FEEDBACK,
    EYE_TRACKING,
    FULL_BODY_TRACKING,
    CUSTOM_SOLUTION
};

struct CommunitySolution {
    std::string solutionId;
    std::string title;
    std::string description;
    std::string author;
    std::string source;
    CommunitySource sourceType;
    SolutionType solutionType;
    
    // Game compatibility
    std::string gameName;
    std::string engineType;
    std::string gameVersion;
    std::vector<std::string> compatibleGames;
    
    // Technical details
    std::string implementationDetails;
    std::string codeSnippet;
    std::string configuration;
    std::vector<std::string> requirements;
    std::vector<std::string> dependencies;
    
    // Performance metrics
    float performanceImprovement;
    float compatibilityScore;
    float stabilityScore;
    float userRating;
    int downloadCount;
    int successCount;
    int failureCount;
    
    // Community feedback
    std::vector<std::string> userComments;
    std::vector<float> userRatings;
    std::vector<std::string> reportedIssues;
    std::vector<std::string> solutions;
    
    // Metadata
    std::string creationDate;
    std::string lastUpdated;
    std::string version;
    bool isVerified;
    bool isOfficial;
    bool isDeprecated;
};

struct CommunityProfile {
    std::string profileId;
    std::string gameName;
    std::string engineType;
    std::string profileName;
    std::string author;
    
    // Profile settings
    struct ProfileSettings {
        bool enableVRInjection;
        bool enableDepthExtraction;
        bool enableShaderModification;
        bool enableInputMapping;
        bool enablePerformanceOptimization;
        bool enableCollisionDetection;
        bool enableAudioSpatialization;
        bool enableHapticFeedback;
        
        // Advanced settings
        float qualityLevel;
        float performanceLevel;
        float stabilityLevel;
        std::string customConfiguration;
    } settings;
    
    // Applied solutions
    std::vector<std::string> appliedSolutions;
    std::vector<std::string> customModifications;
    std::vector<std::string> performanceTweaks;
    
    // Results
    float averageFrameRate;
    float averageLatency;
    float vrCompatibilityScore;
    float userSatisfaction;
    int totalUsers;
    int successfulUsers;
    
    // Community data
    std::vector<std::string> userFeedback;
    std::vector<float> userRatings;
    std::vector<std::string> reportedIssues;
    std::vector<std::string> improvements;
    
    // Metadata
    std::string creationDate;
    std::string lastUpdated;
    std::string version;
    bool isVerified;
    bool isRecommended;
    int downloadCount;
};

struct CommunityMetrics {
    int totalSolutions;
    int totalProfiles;
    int totalUsers;
    int activeUsers;
    float averageSuccessRate;
    float averageUserRating;
    float communityGrowth;
    
    // Source metrics
    std::unordered_map<CommunitySource, int> sourceContributions;
    std::unordered_map<SolutionType, int> solutionTypeDistribution;
    std::unordered_map<std::string, int> gameCompatibility;
    
    // Performance metrics
    float averageProcessingTime;
    float averageResponseTime;
    float dataSyncRate;
    float cacheHitRate;
    
    // Quality metrics
    float solutionAccuracy;
    float profileEffectiveness;
    float communityEngagement;
    float knowledgeRetention;
};

class CommunityLearningSystem {
public:
    CommunityLearningSystem();
    ~CommunityLearningSystem();
    
    // Initialization and configuration
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }
    
    // Community data management
    bool syncCommunityData();
    bool updateLocalDatabase();
    bool backupCommunityData();
    bool restoreCommunityData(const std::string& backupPath);
    
    // Solution management
    std::vector<CommunitySolution> searchSolutions(const std::string& query);
    std::vector<CommunitySolution> getSolutionsByGame(const std::string& gameName);
    std::vector<CommunitySolution> getSolutionsByType(SolutionType type);
    std::vector<CommunitySolution> getVerifiedSolutions();
    
    bool addSolution(const CommunitySolution& solution);
    bool updateSolution(const std::string& solutionId, const CommunitySolution& solution);
    bool removeSolution(const std::string& solutionId);
    bool verifySolution(const std::string& solutionId);
    
    // Profile management
    std::vector<CommunityProfile> searchProfiles(const std::string& query);
    std::vector<CommunityProfile> getProfilesByGame(const std::string& gameName);
    std::vector<CommunityProfile> getRecommendedProfiles();
    std::vector<CommunityProfile> getVerifiedProfiles();
    
    bool addProfile(const CommunityProfile& profile);
    bool updateProfile(const std::string& profileId, const CommunityProfile& profile);
    bool removeProfile(const std::string& profileId);
    bool verifyProfile(const std::string& profileId);
    
    // Reddit integration
    bool syncRedditSolutions();
    bool parseRedditThread(const std::string& threadUrl);
    std::vector<CommunitySolution> getRedditSolutions();
    bool submitToReddit(const std::string& subreddit, const std::string& title, const std::string& content);
    
    // Community learning
    bool learnFromCommunity();
    bool applyCommunityKnowledge(const std::string& gameName);
    bool optimizeFromCommunity(const std::string& gameName);
    bool validateCommunitySolution(const std::string& solutionId);
    
    // Real-time updates
    void startCommunityMonitoring();
    void stopCommunityMonitoring();
    bool isMonitoring() const { return m_monitoring.load(); }
    
    void updateCommunityData();
    void processCommunityEvent(const std::string& eventType, const std::string& eventData);
    void handleCommunityFeedback(const std::string& solutionId, const std::string& feedback);
    
    // Advanced features
    bool enableAutoLearning(bool enable);
    bool enableCommunityValidation(bool enable);
    bool enableKnowledgeSharing(bool enable);
    
    // API integration
    bool connectToAPI(const std::string& apiUrl, const std::string& apiKey);
    bool disconnectFromAPI();
    bool isAPIConnected() const;
    
    // Data export and import
    bool exportCommunityData(const std::string& exportPath);
    bool importCommunityData(const std::string& importPath);
    bool exportSolution(const std::string& solutionId, const std::string& exportPath);
    bool importSolution(const std::string& importPath);
    
    // Performance monitoring
    CommunityMetrics getCommunityMetrics() const;
    void enableMetricsCollection(bool enable);
    void setMetricsCallback(std::function<void(const CommunityMetrics&)> callback);
    
    // Debug and diagnostics
    void enableDebugMode(bool enable);
    void setDebugCallback(std::function<void(const std::string&, const std::string&)> callback);
    std::vector<std::string> getDebugInfo() const;
    
    // Community interaction
    bool submitFeedback(const std::string& solutionId, const std::string& feedback, float rating);
    bool reportIssue(const std::string& solutionId, const std::string& issue);
    bool suggestImprovement(const std::string& solutionId, const std::string& improvement);
    bool voteSolution(const std::string& solutionId, bool upvote);

private:
    // Community data sources
    class RedditIntegration;
    std::unique_ptr<RedditIntegration> m_redditIntegration;
    
    class GitHubIntegration;
    std::unique_ptr<GitHubIntegration> m_githubIntegration;
    
    class DiscordIntegration;
    std::unique_ptr<DiscordIntegration> m_discordIntegration;
    
    class ForumIntegration;
    std::unique_ptr<ForumIntegration> m_forumIntegration;
    
    class APIIntegration;
    std::unique_ptr<APIIntegration> m_apiIntegration;
    
    // Database management
    class CommunityDatabase;
    std::unique_ptr<CommunityDatabase> m_database;
    
    // Common members
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_monitoring{false};
    std::atomic<bool> m_shutdown{false};
    
    std::unordered_map<std::string, CommunitySolution> m_solutions;
    mutable std::mutex m_solutionsMutex;
    
    std::unordered_map<std::string, CommunityProfile> m_profiles;
    mutable std::mutex m_profilesMutex;
    
    CommunityMetrics m_metrics;
    mutable std::mutex m_metricsMutex;
    bool m_metricsCollectionEnabled{false};
    std::function<void(const CommunityMetrics&)> m_metricsCallback;
    
    std::thread m_monitoringThread;
    std::condition_variable m_monitoringCV;
    std::mutex m_monitoringMutex;
    
    bool m_debugModeEnabled{false};
    std::function<void(const std::string&, const std::string&)> m_debugCallback;
    std::vector<std::string> m_debugInfo;
    mutable std::mutex m_debugMutex;
    
    // Advanced features
    bool m_autoLearningEnabled{true};
    bool m_communityValidationEnabled{true};
    bool m_knowledgeSharingEnabled{true};
    
    // API configuration
    std::string m_apiUrl;
    std::string m_apiKey;
    bool m_apiConnected{false};
    
    // Private methods
    void monitoringLoop();
    void updateMetrics();
    void processCommunityData();
    void syncDataSources();
    void validateSolutions();
    void optimizeProfiles();
    bool initializeCommunitySources();
    void cleanupCommunitySources();
    void updateCommunityKnowledge();
    void logCommunityError(const std::string& error);
    void saveCommunityData();
    void loadCommunityData();
    void clearCommunityCache();
    void optimizeMemoryUsage();
};

} // namespace uevr
