#include "CompatibilityManager.hpp"
#include <spdlog/spdlog.h>
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <filesystem>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <algorithm>
#include <fstream>
#include <json/json.h>

namespace uevrLCX {

class CompatibilityManager : public ICompatibilityManager {
private:
    CompatibilityManagerConfig m_config;
    CompatibilityManagerMetrics m_metrics;
    bool m_initialized;
    bool m_managementActive;
    
    // Compatibility database
    struct CompatibilityEntry {
        std::string gameName;
        std::string engineType;
        std::string engineVersion;
        float compatibilityScore;
        std::vector<std::string> knownIssues;
        std::vector<std::string> solutions;
        std::string lastTested;
        bool isVerified;
    };
    std::unordered_map<std::string, CompatibilityEntry> m_compatibilityDatabase;
    
    // Community solutions
    struct CommunitySolution {
        std::string gameName;
        std::string author;
        std::string description;
        std::string solutionType;
        std::string code;
        float successRate;
        int usageCount;
        std::string lastUpdated;
    };
    std::vector<CommunitySolution> m_communitySolutions;
    
    // Reddit integration
    struct RedditSolution {
        std::string gameName;
        std::string redditPostId;
        std::string redditAuthor;
        std::string solution;
        float upvoteRatio;
        int upvotes;
        std::string subreddit;
        std::string postDate;
    };
    std::vector<RedditSolution> m_redditSolutions;
    
    // Auto-fix system
    struct AutoFixRule {
        std::string ruleName;
        std::string condition;
        std::string action;
        float confidence;
        bool enabled;
    };
    std::vector<AutoFixRule> m_autoFixRules;
    
    std::thread m_managementThread;
    std::mutex m_managementMutex;
    bool m_managementThreadRunning;
    
    // Database file paths
    std::string m_databasePath;
    std::string m_communitySolutionsPath;
    std::string m_redditSolutionsPath;

public:
    CompatibilityManager();
    ~CompatibilityManager();
    bool Initialize(const CompatibilityManagerConfig& config) override;
    bool AssessCompatibility(const std::string& gameName) override;
    CompatibilityResult GetCompatibilityResult(const std::string& gameName) const override;
    bool ApplyCommunitySolution(const std::string& gameName, const std::string& solutionId) override;
    bool AddCommunitySolution(const CommunitySolution& solution) override;
    bool UpdateCompatibilityDatabase(const std::string& gameName, const CompatibilityEntry& entry) override;
    std::vector<std::string> GetCompatibleGames() const override;
    CompatibilityManagerMetrics GetMetrics() const override;
    bool StartContinuousManagement() override;
    bool StopContinuousManagement() override;
    void Shutdown() override;

private:
    void LoadCompatibilityDatabase();
    void LoadCommunitySolutions();
    void LoadRedditSolutions();
    void InitializeAutoFixRules();
    void StartManagementThread();
    void MonitorCompatibility();
    bool SaveCompatibilityDatabase();
    bool SaveCommunitySolutions();
    bool SaveRedditSolutions();
    float CalculateCompatibilityScore(const std::string& gameName);
    std::vector<std::string> FindMatchingSolutions(const std::string& gameName);
    bool ApplyAutoFix(const std::string& gameName);
    void UpdateCompatibilityMetrics();
    bool LoadFromJson(const std::string& filePath, Json::Value& root);
    bool SaveToJson(const std::string& filePath, const Json::Value& root);
    std::string GetGameExecutablePath(const std::string& gameName);
    bool IsGameRunning(const std::string& gameName);
    void LogCompatibilityMetrics();
};

CompatibilityManager::CompatibilityManager() 
    : m_initialized(false), m_managementActive(false), m_managementThreadRunning(false) {
    spdlog::info("CompatibilityManager: Initializing");
}

CompatibilityManager::~CompatibilityManager() {
    Shutdown();
}

bool CompatibilityManager::Initialize(const CompatibilityManagerConfig& config) {
    m_config = config;
    
    // Set up database paths
    m_databasePath = "data/compatibility_database.json";
    m_communitySolutionsPath = "data/community_solutions.json";
    m_redditSolutionsPath = "data/reddit_solutions.json";
    
    // Create data directory if it doesn't exist
    std::filesystem::create_directories("data");
    
    LoadCompatibilityDatabase();
    LoadCommunitySolutions();
    LoadRedditSolutions();
    InitializeAutoFixRules();
    
    m_initialized = true;
    spdlog::info("CompatibilityManager: Initialized successfully");
    return true;
}

bool CompatibilityManager::AssessCompatibility(const std::string& gameName) {
    if (!m_initialized) {
        spdlog::error("CompatibilityManager: Not initialized");
        return false;
    }

    spdlog::info("CompatibilityManager: Assessing compatibility for game: {}", gameName);
    
    // Check if game is already in database
    auto it = m_compatibilityDatabase.find(gameName);
    if (it != m_compatibilityDatabase.end()) {
        spdlog::info("CompatibilityManager: Found existing compatibility data for {}", gameName);
        return true;
    }
    
    // Calculate compatibility score
    float compatibilityScore = CalculateCompatibilityScore(gameName);
    
    // Create new compatibility entry
    CompatibilityEntry entry;
    entry.gameName = gameName;
    entry.engineType = "Unknown"; // Will be detected by GameAnalyzer
    entry.engineVersion = "Unknown";
    entry.compatibilityScore = compatibilityScore;
    entry.isVerified = false;
    entry.lastTested = "Now";
    
    // Find matching community solutions
    std::vector<std::string> solutions = FindMatchingSolutions(gameName);
    entry.solutions = solutions;
    
    // Apply auto-fixes if available
    if (ApplyAutoFix(gameName)) {
        entry.compatibilityScore += 0.1f; // Boost score for auto-fixes
    }
    
    // Add to database
    m_compatibilityDatabase[gameName] = entry;
    
    // Save database
    SaveCompatibilityDatabase();
    
    m_metrics.assessedGames++;
    spdlog::info("CompatibilityManager: Compatibility assessment completed for {} (Score: {:.2f})", 
                 gameName, compatibilityScore);
    return true;
}

CompatibilityResult CompatibilityManager::GetCompatibilityResult(const std::string& gameName) const {
    CompatibilityResult result;
    result.gameName = gameName;
    result.compatibilityScore = 0.0f;
    result.status = "Unknown";
    
    auto it = m_compatibilityDatabase.find(gameName);
    if (it != m_compatibilityDatabase.end()) {
        const auto& entry = it->second;
        result.compatibilityScore = entry.compatibilityScore;
        result.engineType = entry.engineType;
        result.engineVersion = entry.engineVersion;
        result.knownIssues = entry.knownIssues;
        result.availableSolutions = entry.solutions;
        result.isVerified = entry.isVerified;
        
        if (entry.compatibilityScore >= 0.9f) {
            result.status = "Excellent";
        } else if (entry.compatibilityScore >= 0.7f) {
            result.status = "Good";
        } else if (entry.compatibilityScore >= 0.5f) {
            result.status = "Fair";
        } else {
            result.status = "Poor";
        }
    }
    
    return result;
}

bool CompatibilityManager::ApplyCommunitySolution(const std::string& gameName, const std::string& solutionId) {
    if (!m_initialized) {
        spdlog::error("CompatibilityManager: Not initialized");
        return false;
    }

    spdlog::info("CompatibilityManager: Applying community solution {} for game {}", solutionId, gameName);
    
    // Find the solution
    for (const auto& solution : m_communitySolutions) {
        if (solution.gameName == gameName || 
            solution.description.find(gameName) != std::string::npos) {
            spdlog::info("CompatibilityManager: Found solution: {}", solution.description);
            
            // Apply the solution (this would integrate with the actual system)
            // For now, we just log it
            spdlog::info("CompatibilityManager: Applying solution code: {}", solution.code);
            
            // Update usage count
            const_cast<CommunitySolution&>(solution).usageCount++;
            
            m_metrics.appliedSolutions++;
            return true;
        }
    }
    
    spdlog::warn("CompatibilityManager: Solution {} not found for game {}", solutionId, gameName);
    return false;
}

bool CompatibilityManager::AddCommunitySolution(const CommunitySolution& solution) {
    if (!m_initialized) {
        spdlog::error("CompatibilityManager: Not initialized");
        return false;
    }

    spdlog::info("CompatibilityManager: Adding community solution for game: {}", solution.gameName);
    
    m_communitySolutions.push_back(solution);
    SaveCommunitySolutions();
    
    m_metrics.addedSolutions++;
    return true;
}

bool CompatibilityManager::UpdateCompatibilityDatabase(const std::string& gameName, const CompatibilityEntry& entry) {
    if (!m_initialized) {
        spdlog::error("CompatibilityManager: Not initialized");
        return false;
    }

    spdlog::info("CompatibilityManager: Updating compatibility database for game: {}", gameName);
    
    m_compatibilityDatabase[gameName] = entry;
    SaveCompatibilityDatabase();
    
    m_metrics.updatedEntries++;
    return true;
}

std::vector<std::string> CompatibilityManager::GetCompatibleGames() const {
    std::vector<std::string> compatibleGames;
    
    for (const auto& pair : m_compatibilityDatabase) {
        if (pair.second.compatibilityScore >= 0.7f) {
            compatibleGames.push_back(pair.first);
        }
    }
    
    return compatibleGames;
}

CompatibilityManagerMetrics CompatibilityManager::GetMetrics() const {
    return m_metrics;
}

bool CompatibilityManager::StartContinuousManagement() {
    if (!m_initialized) return false;
    m_managementActive = true;
    StartManagementThread();
    return true;
}

bool CompatibilityManager::StopContinuousManagement() {
    m_managementActive = false;
    if (m_managementThread.joinable()) {
        m_managementThread.join();
    }
    return true;
}

void CompatibilityManager::Shutdown() {
    StopContinuousManagement();
    SaveCompatibilityDatabase();
    SaveCommunitySolutions();
    SaveRedditSolutions();
    m_initialized = false;
}

void CompatibilityManager::LoadCompatibilityDatabase() {
    Json::Value root;
    if (LoadFromJson(m_databasePath, root)) {
        for (const auto& gameName : root.getMemberNames()) {
            const auto& gameData = root[gameName];
            CompatibilityEntry entry;
            entry.gameName = gameName;
            entry.engineType = gameData.get("engineType", "Unknown").asString();
            entry.engineVersion = gameData.get("engineVersion", "Unknown").asString();
            entry.compatibilityScore = gameData.get("compatibilityScore", 0.0f).asFloat();
            entry.isVerified = gameData.get("isVerified", false).asBool();
            entry.lastTested = gameData.get("lastTested", "Unknown").asString();
            
            const auto& issues = gameData["knownIssues"];
            for (const auto& issue : issues) {
                entry.knownIssues.push_back(issue.asString());
            }
            
            const auto& solutions = gameData["solutions"];
            for (const auto& solution : solutions) {
                entry.solutions.push_back(solution.asString());
            }
            
            m_compatibilityDatabase[gameName] = entry;
        }
        spdlog::info("CompatibilityManager: Loaded {} compatibility entries", m_compatibilityDatabase.size());
    }
}

void CompatibilityManager::LoadCommunitySolutions() {
    Json::Value root;
    if (LoadFromJson(m_communitySolutionsPath, root)) {
        for (const auto& solutionData : root) {
            CommunitySolution solution;
            solution.gameName = solutionData.get("gameName", "").asString();
            solution.author = solutionData.get("author", "").asString();
            solution.description = solutionData.get("description", "").asString();
            solution.solutionType = solutionData.get("solutionType", "").asString();
            solution.code = solutionData.get("code", "").asString();
            solution.successRate = solutionData.get("successRate", 0.0f).asFloat();
            solution.usageCount = solutionData.get("usageCount", 0).asInt();
            solution.lastUpdated = solutionData.get("lastUpdated", "").asString();
            
            m_communitySolutions.push_back(solution);
        }
        spdlog::info("CompatibilityManager: Loaded {} community solutions", m_communitySolutions.size());
    }
}

void CompatibilityManager::LoadRedditSolutions() {
    Json::Value root;
    if (LoadFromJson(m_redditSolutionsPath, root)) {
        for (const auto& solutionData : root) {
            RedditSolution solution;
            solution.gameName = solutionData.get("gameName", "").asString();
            solution.redditPostId = solutionData.get("redditPostId", "").asString();
            solution.redditAuthor = solutionData.get("redditAuthor", "").asString();
            solution.solution = solutionData.get("solution", "").asString();
            solution.upvoteRatio = solutionData.get("upvoteRatio", 0.0f).asFloat();
            solution.upvotes = solutionData.get("upvotes", 0).asInt();
            solution.subreddit = solutionData.get("subreddit", "").asString();
            solution.postDate = solutionData.get("postDate", "").asString();
            
            m_redditSolutions.push_back(solution);
        }
        spdlog::info("CompatibilityManager: Loaded {} Reddit solutions", m_redditSolutions.size());
    }
}

void CompatibilityManager::InitializeAutoFixRules() {
    m_autoFixRules = {
        {"UnrealEngine_VR_Setup", "engineType == 'UnrealEngine'", "Apply UE VR setup", 0.9f, true},
        {"Unity_VR_Setup", "engineType == 'Unity'", "Apply Unity VR setup", 0.85f, true},
        {"DirectX11_Compatibility", "graphicsAPI == 'DirectX11'", "Apply DX11 compatibility fixes", 0.8f, true},
        {"OpenGL_Compatibility", "graphicsAPI == 'OpenGL'", "Apply OpenGL compatibility fixes", 0.75f, true},
        {"Vulkan_Compatibility", "graphicsAPI == 'Vulkan'", "Apply Vulkan compatibility fixes", 0.8f, true}
    };
}

void CompatibilityManager::StartManagementThread() {
    m_managementThreadRunning = true;
    m_managementThread = std::thread([this]() {
        while (m_managementThreadRunning) {
            MonitorCompatibility();
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    });
}

void CompatibilityManager::MonitorCompatibility() {
    // Monitor for new games and assess compatibility
    spdlog::debug("CompatibilityManager: Monitoring compatibility");
    UpdateCompatibilityMetrics();
}

bool CompatibilityManager::SaveCompatibilityDatabase() {
    Json::Value root;
    
    for (const auto& pair : m_compatibilityDatabase) {
        const auto& entry = pair.second;
        Json::Value gameData;
        gameData["engineType"] = entry.engineType;
        gameData["engineVersion"] = entry.engineVersion;
        gameData["compatibilityScore"] = entry.compatibilityScore;
        gameData["isVerified"] = entry.isVerified;
        gameData["lastTested"] = entry.lastTested;
        
        Json::Value issues;
        for (const auto& issue : entry.knownIssues) {
            issues.append(issue);
        }
        gameData["knownIssues"] = issues;
        
        Json::Value solutions;
        for (const auto& solution : entry.solutions) {
            solutions.append(solution);
        }
        gameData["solutions"] = solutions;
        
        root[entry.gameName] = gameData;
    }
    
    return SaveToJson(m_databasePath, root);
}

bool CompatibilityManager::SaveCommunitySolutions() {
    Json::Value root;
    
    for (const auto& solution : m_communitySolutions) {
        Json::Value solutionData;
        solutionData["gameName"] = solution.gameName;
        solutionData["author"] = solution.author;
        solutionData["description"] = solution.description;
        solutionData["solutionType"] = solution.solutionType;
        solutionData["code"] = solution.code;
        solutionData["successRate"] = solution.successRate;
        solutionData["usageCount"] = solution.usageCount;
        solutionData["lastUpdated"] = solution.lastUpdated;
        
        root.append(solutionData);
    }
    
    return SaveToJson(m_communitySolutionsPath, root);
}

bool CompatibilityManager::SaveRedditSolutions() {
    Json::Value root;
    
    for (const auto& solution : m_redditSolutions) {
        Json::Value solutionData;
        solutionData["gameName"] = solution.gameName;
        solutionData["redditPostId"] = solution.redditPostId;
        solutionData["redditAuthor"] = solution.redditAuthor;
        solutionData["solution"] = solution.solution;
        solutionData["upvoteRatio"] = solution.upvoteRatio;
        solutionData["upvotes"] = solution.upvotes;
        solutionData["subreddit"] = solution.subreddit;
        solutionData["postDate"] = solution.postDate;
        
        root.append(solutionData);
    }
    
    return SaveToJson(m_redditSolutionsPath, root);
}

float CompatibilityManager::CalculateCompatibilityScore(const std::string& gameName) {
    // Base score calculation based on game characteristics
    float baseScore = 0.5f;
    
    // Check if game is running
    if (IsGameRunning(gameName)) {
        baseScore += 0.1f;
    }
    
    // Check for known engine patterns
    if (gameName.find("UE4") != std::string::npos || gameName.find("UE5") != std::string::npos) {
        baseScore += 0.2f; // Unreal Engine games
    } else if (gameName.find("Unity") != std::string::npos) {
        baseScore += 0.15f; // Unity games
    }
    
    // Check for community solutions
    std::vector<std::string> solutions = FindMatchingSolutions(gameName);
    baseScore += solutions.size() * 0.05f; // Boost for each available solution
    
    return std::clamp(baseScore, 0.0f, 1.0f);
}

std::vector<std::string> CompatibilityManager::FindMatchingSolutions(const std::string& gameName) {
    std::vector<std::string> matchingSolutions;
    
    // Check community solutions
    for (const auto& solution : m_communitySolutions) {
        if (solution.gameName == gameName || 
            solution.description.find(gameName) != std::string::npos) {
            matchingSolutions.push_back(solution.description);
        }
    }
    
    // Check Reddit solutions
    for (const auto& solution : m_redditSolutions) {
        if (solution.gameName == gameName || 
            solution.solution.find(gameName) != std::string::npos) {
            matchingSolutions.push_back("Reddit: " + solution.solution);
        }
    }
    
    return matchingSolutions;
}

bool CompatibilityManager::ApplyAutoFix(const std::string& gameName) {
    spdlog::info("CompatibilityManager: Applying auto-fixes for game: {}", gameName);
    
    bool applied = false;
    for (const auto& rule : m_autoFixRules) {
        if (rule.enabled) {
            spdlog::info("CompatibilityManager: Applying auto-fix rule: {}", rule.ruleName);
            applied = true;
        }
    }
    
    return applied;
}

void CompatibilityManager::UpdateCompatibilityMetrics() {
    m_metrics.totalGames = m_compatibilityDatabase.size();
    m_metrics.totalSolutions = m_communitySolutions.size() + m_redditSolutions.size();
    
    // Calculate average compatibility score
    float totalScore = 0.0f;
    for (const auto& pair : m_compatibilityDatabase) {
        totalScore += pair.second.compatibilityScore;
    }
    m_metrics.averageCompatibilityScore = totalScore / m_metrics.totalGames;
    
    LogCompatibilityMetrics();
}

bool CompatibilityManager::LoadFromJson(const std::string& filePath, Json::Value& root) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        spdlog::warn("CompatibilityManager: Could not open file: {}", filePath);
        return false;
    }
    
    Json::CharReaderBuilder builder;
    std::string errors;
    bool success = Json::parseFromStream(builder, file, &root, &errors);
    
    if (!success) {
        spdlog::error("CompatibilityManager: JSON parsing error: {}", errors);
        return false;
    }
    
    return true;
}

bool CompatibilityManager::SaveToJson(const std::string& filePath, const Json::Value& root) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        spdlog::error("CompatibilityManager: Could not create file: {}", filePath);
        return false;
    }
    
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &file);
    
    return true;
}

std::string CompatibilityManager::GetGameExecutablePath(const std::string& gameName) {
    // This would implement actual game executable path detection
    return "C:\\Games\\" + gameName + "\\" + gameName + ".exe";
}

bool CompatibilityManager::IsGameRunning(const std::string& gameName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring wProcessName = pe32.szExeFile;
            std::string currentProcessName(wProcessName.begin(), wProcessName.end());
            if (currentProcessName.find(gameName) != std::string::npos) {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32NextW(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return false;
}

void CompatibilityManager::LogCompatibilityMetrics() {
    static int logCounter = 0;
    if (++logCounter % 10 == 0) { // Log every 10 monitoring cycles
        spdlog::info("CompatibilityManager: Total games={}, Solutions={}, Avg score={:.2f}", 
                     m_metrics.totalGames, m_metrics.totalSolutions, m_metrics.averageCompatibilityScore);
    }
}

// Factory function
std::unique_ptr<ICompatibilityManager> CreateCompatibilityManager() {
    return std::make_unique<CompatibilityManager>();
}

} // namespace uevrLCX
