#include "UniversalVRConversionSystem.hpp"
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

namespace uevrLCX {

class GameAnalyzer : public IGameAnalyzer {
private:
    GameAnalyzerConfig m_config;
    GameAnalyzerMetrics m_metrics;
    bool m_initialized;
    bool m_analysisActive;
    
    // Game information storage
    std::unordered_map<std::string, GameProfile> m_analyzedGames;
    
    // Engine detection patterns
    struct EnginePattern {
        std::string engineName;
        std::vector<std::string> signatures;
        float confidence;
    };
    std::vector<EnginePattern> m_enginePatterns;
    
    // Reddit community solutions
    struct RedditSolution {
        std::string gameName;
        std::string engineType;
        std::string solutionType;
        std::string description;
        float successRate;
    };
    std::vector<RedditSolution> m_redditSolutions;
    
    // Analysis thread
    std::thread m_analysisThread;
    std::mutex m_analysisMutex;
    bool m_analysisThreadRunning;

public:
    GameAnalyzer() : m_initialized(false), m_analysisActive(false), 
                     m_analysisThreadRunning(false) {
        spdlog::info("GameAnalyzer: Constructor called");
    }
    
    ~GameAnalyzer() {
        Shutdown();
    }

    bool Initialize(const GameAnalyzerConfig& config) override {
        std::lock_guard<std::mutex> lock(m_analysisMutex);
        
        spdlog::info("GameAnalyzer: Initializing with config");
        
        m_config = config;
        m_metrics = GameAnalyzerMetrics{};
        
        // Initialize engine patterns
        InitializeEnginePatterns();
        
        // Load Reddit solutions
        LoadRedditSolutions();
        
        m_initialized = true;
        spdlog::info("GameAnalyzer: Initialization completed successfully");
        return true;
    }

    bool AnalyzeGame(const std::string& processName) override {
        std::lock_guard<std::mutex> lock(m_analysisMutex);
        
        if (!m_initialized) {
            spdlog::error("GameAnalyzer: Cannot analyze game - not initialized");
            return false;
        }
        
        spdlog::info("GameAnalyzer: Analyzing game: {}", processName);
        
        // Create game profile
        GameProfile profile;
        profile.gameName = ExtractGameName(processName);
        profile.processName = processName;
        profile.executablePath = GetProcessExecutablePath(processName);
        
        // Detect engine
        DetectEngine(profile);
        
        // Detect graphics APIs
        DetectGraphicsAPIs(profile);
        
        // Analyze compatibility
        AnalyzeCompatibility(profile);
        
        // Find Reddit solutions
        FindRedditSolutions(profile);
        
        // Store profile
        m_analyzedGames[processName] = profile;
        
        spdlog::info("GameAnalyzer: Successfully analyzed game: {} (Engine: {}, Compatibility: {})", 
                     processName, profile.engineType, static_cast<int>(profile.compatibilityLevel));
        return true;
    }

    GameProfile GetGameProfile(const std::string& processName) const override {
        std::lock_guard<std::mutex> lock(m_analysisMutex);
        
        auto it = m_analyzedGames.find(processName);
        if (it == m_analyzedGames.end()) {
            return GameProfile{};
        }
        
        return it->second;
    }

    std::vector<std::string> GetCompatibleGames() const override {
        std::lock_guard<std::mutex> lock(m_analysisMutex);
        
        std::vector<std::string> compatibleGames;
        for (const auto& [processName, profile] : m_analyzedGames) {
            if (profile.compatibilityLevel >= GameCompatibilityLevel::Compatible) {
                compatibleGames.push_back(processName);
            }
        }
        
        return compatibleGames;
    }

    std::vector<std::string> GetRedditSolutions(const std::string& gameName) const override {
        std::lock_guard<std::mutex> lock(m_analysisMutex);
        
        auto it = m_analyzedGames.find(gameName);
        if (it == m_analyzedGames.end()) {
            return {};
        }
        
        return it->second.redditSolutions;
    }

    bool StartContinuousAnalysis() override {
        std::lock_guard<std::mutex> lock(m_analysisMutex);
        
        if (!m_initialized) {
            spdlog::error("GameAnalyzer: Cannot start continuous analysis - not initialized");
            return false;
        }
        
        if (m_analysisActive) {
            spdlog::warn("GameAnalyzer: Continuous analysis already active");
            return true;
        }
        
        spdlog::info("GameAnalyzer: Starting continuous analysis");
        
        StartAnalysisThread();
        m_analysisActive = true;
        
        spdlog::info("GameAnalyzer: Continuous analysis started");
        return true;
    }

    bool StopContinuousAnalysis() override {
        std::lock_guard<std::mutex> lock(m_analysisMutex);
        
        if (!m_analysisActive) {
            spdlog::warn("GameAnalyzer: Continuous analysis not active");
            return true;
        }
        
        spdlog::info("GameAnalyzer: Stopping continuous analysis");
        
        if (m_analysisThreadRunning) {
            m_analysisThreadRunning = false;
            if (m_analysisThread.joinable()) {
                m_analysisThread.join();
            }
        }
        
        m_analysisActive = false;
        spdlog::info("GameAnalyzer: Continuous analysis stopped");
        return true;
    }

    GameAnalyzerMetrics GetMetrics() const override {
        return m_metrics;
    }

    void Shutdown() override {
        std::lock_guard<std::mutex> lock(m_analysisMutex);
        
        spdlog::info("GameAnalyzer: Shutting down");
        
        if (m_analysisActive) {
            StopContinuousAnalysis();
        }
        
        m_analyzedGames.clear();
        m_enginePatterns.clear();
        m_redditSolutions.clear();
        
        m_initialized = false;
        m_analysisActive = false;
        
        spdlog::info("GameAnalyzer: Shutdown completed");
    }

private:
    void InitializeEnginePatterns() {
        spdlog::info("GameAnalyzer: Initializing engine detection patterns");
        
        // Unreal Engine
        m_enginePatterns.push_back({
            "Unreal Engine",
            {"UE4", "UE5", "UnrealEngine", "Engine\\Classes"},
            0.95f
        });
        
        // Unity
        m_enginePatterns.push_back({
            "Unity",
            {"UnityEngine", "Assembly-CSharp", "UnityPlayer"},
            0.95f
        });
        
        // Source Engine
        m_enginePatterns.push_back({
            "Source Engine",
            {"source", "hl2", "counter-strike"},
            0.90f
        });
        
        spdlog::info("GameAnalyzer: Initialized {} engine patterns", m_enginePatterns.size());
    }
    
    void LoadRedditSolutions() {
        spdlog::info("GameAnalyzer: Loading Reddit community solutions");
        
        // Add solutions from the Reddit community resource
        m_redditSolutions = {
            {"Cyberpunk 2077", "REDengine", "VRGIN + Depth3D", 
             "Use VRGIN for injection and Depth3D for depth extraction", 0.85f},
            {"Monster Hunter World", "MT Framework", "VRGIN + Reshade", 
             "MT Framework games work well with VRGIN injection", 0.90f},
            {"Resident Evil 7", "RE Engine", "VRGIN + Physics", 
             "RE Engine games require special physics handling", 0.80f},
            {"Generic Unreal Engine", "Unreal Engine", "VRGIN + Depth3D + Reshade", 
             "Standard approach for Unreal Engine games", 0.95f},
            {"Generic Unity", "Unity", "VRGIN + Custom Shaders", 
             "Unity games need custom shader injection", 0.88f}
        };
        
        spdlog::info("GameAnalyzer: Loaded {} Reddit community solutions", m_redditSolutions.size());
    }
    
    DWORD FindProcessId(const std::string& processName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }
        
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(snapshot, &processEntry)) {
            do {
                std::string currentProcessName = processEntry.szExeFile;
                if (currentProcessName.find(processName) != std::string::npos) {
                    CloseHandle(snapshot);
                    return processEntry.th32ProcessID;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        
        CloseHandle(snapshot);
        return 0;
    }
    
    std::string GetProcessExecutablePath(DWORD processId) {
        HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (!process) {
            return "";
        }
        
        char path[MAX_PATH];
        DWORD pathSize = MAX_PATH;
        
        if (QueryFullProcessImageNameA(process, 0, path, &pathSize)) {
            CloseHandle(process);
            return std::string(path);
        }
        
        CloseHandle(process);
        return "";
    }
    
    std::string ExtractGameName(const std::string& processName) {
        std::string gameName = processName;
        
        std::vector<std::string> suffixes = {
            ".exe", "-Win64-Shipping", "-Win32-Shipping", 
            "Editor", "Launcher", "Client", "Server"
        };
        
        for (const auto& suffix : suffixes) {
            size_t pos = gameName.find(suffix);
            if (pos != std::string::npos) {
                gameName = gameName.substr(0, pos);
                break;
            }
        }
        
        return gameName;
    }
    
    void DetectEngine(GameProfile& profile) {
        spdlog::info("GameAnalyzer: Detecting engine for: {}", profile.gameName);
        
        float bestConfidence = 0.0f;
        std::string bestEngine = "Unknown";
        
        DWORD processId = FindProcessId(profile.processName);
        if (processId != 0) {
            HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
            if (process) {
                HMODULE modules[1024];
                DWORD cbNeeded;
                
                if (EnumProcessModules(process, modules, sizeof(modules), &cbNeeded)) {
                    for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                        char moduleName[MAX_PATH];
                        if (GetModuleBaseNameA(process, modules[i], moduleName, sizeof(moduleName))) {
                            std::string moduleStr(moduleName);
                            
                            for (const auto& pattern : m_enginePatterns) {
                                for (const auto& signature : pattern.signatures) {
                                    if (moduleStr.find(signature) != std::string::npos) {
                                        if (pattern.confidence > bestConfidence) {
                                            bestConfidence = pattern.confidence;
                                            bestEngine = pattern.engineName;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                CloseHandle(process);
            }
        }
        
        profile.engineType = bestEngine;
        spdlog::info("GameAnalyzer: Detected engine: {} (confidence: {:.2f})", bestEngine, bestConfidence);
    }
    
    void DetectGraphicsAPIs(GameProfile& profile) {
        spdlog::info("GameAnalyzer: Detecting graphics APIs for: {}", profile.gameName);
        
        DWORD processId = FindProcessId(profile.processName);
        if (processId == 0) {
            return;
        }
        
        HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (!process) {
            return;
        }
        
        HMODULE modules[1024];
        DWORD cbNeeded;
        
        if (EnumProcessModules(process, modules, sizeof(modules), &cbNeeded)) {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                char moduleName[MAX_PATH];
                if (GetModuleBaseNameA(process, modules[i], moduleName, sizeof(moduleName))) {
                    std::string moduleStr(moduleName);
                    std::transform(moduleStr.begin(), moduleStr.end(), moduleStr.begin(), ::tolower);
                    
                    if (moduleStr.find("d3d11") != std::string::npos) {
                        profile.detectedAPIs.push_back("DirectX 11");
                    }
                    if (moduleStr.find("d3d12") != std::string::npos) {
                        profile.detectedAPIs.push_back("DirectX 12");
                    }
                    if (moduleStr.find("opengl") != std::string::npos) {
                        profile.detectedAPIs.push_back("OpenGL");
                    }
                    if (moduleStr.find("vulkan") != std::string::npos) {
                        profile.detectedAPIs.push_back("Vulkan");
                    }
                }
            }
        }
        
        CloseHandle(process);
        
        // Remove duplicates
        std::sort(profile.detectedAPIs.begin(), profile.detectedAPIs.end());
        profile.detectedAPIs.erase(std::unique(profile.detectedAPIs.begin(), profile.detectedAPIs.end()), 
                                   profile.detectedAPIs.end());
        
        spdlog::info("GameAnalyzer: Detected APIs: {}", 
                     profile.detectedAPIs.empty() ? "None" : 
                     std::accumulate(std::next(profile.detectedAPIs.begin()), profile.detectedAPIs.end(),
                                   profile.detectedAPIs[0],
                                   [](const std::string& a, const std::string& b) { return a + ", " + b; }));
    }
    
    void AnalyzeCompatibility(GameProfile& profile) {
        spdlog::info("GameAnalyzer: Analyzing compatibility for: {}", profile.gameName);
        
        GameCompatibilityLevel compatibility = GameCompatibilityLevel::Unknown;
        
        // Check engine compatibility
        if (profile.engineType == "Unreal Engine") {
            compatibility = GameCompatibilityLevel::HighlyCompatible;
        } else if (profile.engineType == "Unity") {
            compatibility = GameCompatibilityLevel::Compatible;
        } else if (profile.engineType == "Source Engine") {
            compatibility = GameCompatibilityLevel::ModeratelyCompatible;
        } else {
            compatibility = GameCompatibilityLevel::Experimental;
        }
        
        // Check graphics API compatibility
        for (const auto& api : profile.detectedAPIs) {
            if (api == "DirectX 11" || api == "DirectX 12") {
                profile.detectedFeatures.push_back("Depth Buffer Support");
                profile.hasDepthBuffer = true;
            }
        }
        
        profile.compatibilityLevel = compatibility;
        
        spdlog::info("GameAnalyzer: Compatibility level: {} for {}", 
                     static_cast<int>(compatibility), profile.gameName);
    }
    
    void FindRedditSolutions(GameProfile& profile) {
        spdlog::info("GameAnalyzer: Finding Reddit solutions for: {}", profile.gameName);
        
        for (const auto& solution : m_redditSolutions) {
            bool matches = false;
            
            // Direct game name match
            if (solution.gameName.find(profile.gameName) != std::string::npos ||
                profile.gameName.find(solution.gameName) != std::string::npos) {
                matches = true;
            }
            
            // Engine type match
            if (solution.engineType.find(profile.engineType) != std::string::npos ||
                profile.engineType.find(solution.engineType) != std::string::npos) {
                matches = true;
            }
            
            if (matches) {
                std::string solutionInfo = solution.solutionType + " - " + solution.description;
                profile.redditSolutions.push_back(solutionInfo);
            }
        }
        
        spdlog::info("GameAnalyzer: Found {} Reddit solutions for: {}", 
                     profile.redditSolutions.size(), profile.gameName);
    }
    
    void StartAnalysisThread() {
        if (m_analysisThreadRunning) {
            return;
        }
        
        m_analysisThreadRunning = true;
        m_analysisThread = std::thread([this]() {
            spdlog::info("GameAnalyzer: Analysis thread started");
            
            while (m_analysisThreadRunning) {
                ScanForNewGames();
                std::this_thread::sleep_for(std::chrono::seconds(30));
            }
            
            spdlog::info("GameAnalyzer: Analysis thread stopped");
        });
    }
    
    void ScanForNewGames() {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return;
        }
        
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(snapshot, &processEntry)) {
            do {
                std::string processName = processEntry.szExeFile;
                
                if (IsGameProcess(processName) && 
                    m_analyzedGames.find(processName) == m_analyzedGames.end()) {
                    
                    spdlog::info("GameAnalyzer: Found new game process: {}", processName);
                    AnalyzeGame(processName);
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        
        CloseHandle(snapshot);
    }
    
    bool IsGameProcess(const std::string& processName) {
        std::vector<std::string> nonGameIndicators = {
            "svchost", "explorer", "chrome", "firefox", "steam", "discord",
            "nvidia", "amd", "intel", "windows", "system"
        };
        
        std::string lowerProcessName = processName;
        std::transform(lowerProcessName.begin(), lowerProcessName.end(), 
                      lowerProcessName.begin(), ::tolower);
        
        for (const auto& indicator : nonGameIndicators) {
            if (lowerProcessName.find(indicator) != std::string::npos) {
                return false;
            }
        }
        
        return lowerProcessName.find(".exe") != std::string::npos;
    }
};

// Factory function
std::unique_ptr<IGameAnalyzer> CreateGameAnalyzer() {
    return std::make_unique<GameAnalyzer>();
}

} // namespace uevrLCX
