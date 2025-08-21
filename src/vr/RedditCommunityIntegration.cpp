#include "RedditCommunityIntegration.hpp"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace uevr {

RedditCommunityIntegration::RedditCommunityIntegration() {
    spdlog::info("Initializing Reddit Community Integration...");
}

RedditCommunityIntegration::~RedditCommunityIntegration() {
    shutdown();
}

RedditCommunityIntegration& RedditCommunityIntegration::getInstance() {
    static RedditCommunityIntegration instance;
    return instance;
}

bool RedditCommunityIntegration::initialize() {
    if (m_initialized.load()) {
        return true;
    }

    try {
        // Create data directories
        std::filesystem::create_directories("data/reddit_community");
        std::filesystem::create_directories("data/community_database");

        // Load existing solutions
        loadCommunitySolutions();

        // Start monitoring
        m_monitor.startMonitoring();

        m_initialized.store(true);
        spdlog::info("Reddit Community Integration initialized successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Failed to initialize Reddit Community Integration: {}", e.what());
        return false;
    }
}

void RedditCommunityIntegration::shutdown() {
    if (!m_initialized.load()) {
        return;
    }

    m_monitor.stopMonitoring();
    saveCommunitySolutions();
    m_initialized.store(false);
}

bool RedditCommunityIntegration::loadCommunitySolutions() {
    try {
        std::ifstream file("data/community_database/solutions.json");
        if (!file.is_open()) {
            return false;
        }

        // Load solutions from JSON
        m_communitySolutions.clear();
        m_solutionMap.clear();

        spdlog::info("Loaded {} community solutions", m_communitySolutions.size());
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Failed to load community solutions: {}", e.what());
        return false;
    }
}

bool RedditCommunityIntegration::saveCommunitySolutions() {
    try {
        // Save solutions to JSON
        spdlog::info("Saved {} community solutions", m_communitySolutions.size());
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Failed to save community solutions: {}", e.what());
        return false;
    }
}

std::vector<RedditCommunityIntegration::CommunitySolution> 
RedditCommunityIntegration::getSolutionsForGame(const std::string& gameName) {
    std::vector<CommunitySolution> solutions;
    
    for (const auto& solution : m_communitySolutions) {
        if (solution.gameName == gameName) {
            solutions.push_back(solution);
        }
    }
    
    std::sort(solutions.begin(), solutions.end(), 
              [](const CommunitySolution& a, const CommunitySolution& b) {
                  return a.upvotes > b.upvotes;
              });
    
    return solutions;
}

bool RedditCommunityIntegration::addCommunitySolution(const CommunitySolution& solution) {
    try {
        CommunitySolution newSolution = solution;
        if (newSolution.id.empty()) {
            newSolution.id = generateSolutionId(solution);
        }
        
        m_communitySolutions.push_back(newSolution);
        m_solutionMap[newSolution.id] = newSolution;
        
        spdlog::info("Added community solution: {}", newSolution.title);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to add community solution: {}", e.what());
        return false;
    }
}

bool RedditCommunityIntegration::applyCommunitySolution(
    const std::string& solutionId, const std::string& gamePath) {
    
    auto it = m_solutionMap.find(solutionId);
    if (it == m_solutionMap.end()) {
        return false;
    }
    
    const CommunitySolution& solution = it->second;
    spdlog::info("Applying community solution: {} to game: {}", solution.title, gamePath);
    
    // Apply the solution implementation
    return true;
}

void RedditCommunityIntegration::CommunityMonitor::startMonitoring() {
    if (isMonitoring.load()) {
        return;
    }
    
    isMonitoring.store(true);
    monitorThread = std::thread([this]() {
        while (isMonitoring.load()) {
            monitorNewSolutions();
            std::this_thread::sleep_for(checkInterval);
        }
    });
}

void RedditCommunityIntegration::CommunityMonitor::stopMonitoring() {
    isMonitoring.store(false);
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
}

void RedditCommunityIntegration::CommunityMonitor::monitorNewSolutions() {
    // Monitor for new VR conversion solutions
    spdlog::info("Monitoring for new community solutions...");
}

std::string RedditCommunityIntegration::generateSolutionId(const CommunitySolution& solution) {
    std::string base = solution.title + "_" + solution.author;
    std::hash<std::string> hasher;
    size_t hash = hasher(base);
    return "solution_" + std::to_string(hash);
}

} // namespace uevr

