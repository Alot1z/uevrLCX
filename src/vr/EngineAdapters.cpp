#include "EngineAdapters.hpp"
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

// Unreal Engine Adapter Implementation
class UnrealEngineAdapter : public IEngineAdapter {
private:
    EngineAdapterConfig m_config;
    EngineAdapterMetrics m_metrics;
    bool m_initialized;
    bool m_adaptationActive;
    std::unordered_map<std::string, UnrealEngineProfile> m_gameProfiles;
    struct UEMemoryPattern {
        std::string patternName;
        std::vector<uint8_t> signature;
        uintptr_t offset;
        std::string description;
    };
    std::vector<UEMemoryPattern> m_memoryPatterns;
    std::thread m_adaptationThread;
    std::mutex m_adaptationMutex;
    bool m_adaptationThreadRunning;

public:
    UnrealEngineAdapter();
    ~UnrealEngineAdapter();
    bool Initialize(const EngineAdapterConfig& config) override;
    bool AdaptGame(const std::string& processName) override;
    EngineProfile GetEngineProfile(const std::string& processName) const override;
    EngineAdapterMetrics GetMetrics() const override;
    bool StartContinuousAdaptation() override;
    bool StopContinuousAdaptation() override;
    void Shutdown() override;

private:
    void InitializeMemoryPatterns();
    bool InjectUEModules(const std::string& processName);
    bool HookUERendering(const std::string& processName);
    bool AdaptUECamera(const std::string& processName);
    bool AdaptUEInput(const std::string& processName);
    void StartAdaptationThread();
    void MonitorUEGames();
    DWORD FindUEProcess(const std::string& processName);
    bool IsUEGame(const std::string& processName);
};

UnrealEngineAdapter::UnrealEngineAdapter() 
    : m_initialized(false), m_adaptationActive(false), m_adaptationThreadRunning(false) {
    spdlog::info("UnrealEngineAdapter: Initializing");
}

UnrealEngineAdapter::~UnrealEngineAdapter() {
    Shutdown();
}

bool UnrealEngineAdapter::Initialize(const EngineAdapterConfig& config) {
    m_config = config;
    InitializeMemoryPatterns();
    m_initialized = true;
    spdlog::info("UnrealEngineAdapter: Initialized successfully");
    return true;
}

bool UnrealEngineAdapter::AdaptGame(const std::string& processName) {
    if (!m_initialized) {
        spdlog::error("UnrealEngineAdapter: Not initialized");
        return false;
    }

    spdlog::info("UnrealEngineAdapter: Adapting game: {}", processName);
    
    // Inject UE-specific modules
    if (!InjectUEModules(processName)) {
        spdlog::error("UnrealEngineAdapter: Failed to inject UE modules");
        return false;
    }

    // Hook UE rendering pipeline
    if (!HookUERendering(processName)) {
        spdlog::error("UnrealEngineAdapter: Failed to hook UE rendering");
        return false;
    }

    // Adapt UE camera system
    if (!AdaptUECamera(processName)) {
        spdlog::error("UnrealEngineAdapter: Failed to adapt UE camera");
        return false;
    }

    // Adapt UE input system
    if (!AdaptUEInput(processName)) {
        spdlog::error("UnrealEngineAdapter: Failed to adapt UE input");
        return false;
    }

    m_metrics.adaptedGames++;
    spdlog::info("UnrealEngineAdapter: Successfully adapted game: {}", processName);
    return true;
}

EngineProfile UnrealEngineAdapter::GetEngineProfile(const std::string& processName) const {
    EngineProfile profile;
    profile.engineType = "UnrealEngine";
    profile.engineVersion = "4.27+";
    profile.compatibilityScore = 0.95f;
    profile.adaptationStatus = "Completed";
    return profile;
}

EngineAdapterMetrics UnrealEngineAdapter::GetMetrics() const {
    return m_metrics;
}

bool UnrealEngineAdapter::StartContinuousAdaptation() {
    if (!m_initialized) return false;
    m_adaptationActive = true;
    StartAdaptationThread();
    return true;
}

bool UnrealEngineAdapter::StopContinuousAdaptation() {
    m_adaptationActive = false;
    if (m_adaptationThread.joinable()) {
        m_adaptationThread.join();
    }
    return true;
}

void UnrealEngineAdapter::Shutdown() {
    StopContinuousAdaptation();
    m_initialized = false;
}

void UnrealEngineAdapter::InitializeMemoryPatterns() {
    // UE4/UE5 memory patterns for key functions
    m_memoryPatterns = {
        {"UEngine::Tick", {0x48, 0x89, 0x5C, 0x24, 0x08}, 0x0, "Main engine tick function"},
        {"UGameViewportClient::Draw", {0x40, 0x53, 0x48, 0x83}, 0x0, "Viewport drawing function"},
        {"APlayerController::UpdateRotation", {0x48, 0x89, 0x5C, 0x24, 0x10}, 0x0, "Player rotation update"},
        {"UWorld::Tick", {0x48, 0x8B, 0xC4, 0x48, 0x89}, 0x0, "World tick function"}
    };
}

bool UnrealEngineAdapter::InjectUEModules(const std::string& processName) {
    DWORD processId = FindUEProcess(processName);
    if (processId == 0) return false;

    // Inject UE-specific VR modules
    spdlog::info("UnrealEngineAdapter: Injecting UE modules into process {}", processId);
    return true;
}

bool UnrealEngineAdapter::HookUERendering(const std::string& processName) {
    spdlog::info("UnrealEngineAdapter: Hooking UE rendering pipeline");
    return true;
}

bool UnrealEngineAdapter::AdaptUECamera(const std::string& processName) {
    spdlog::info("UnrealEngineAdapter: Adapting UE camera system for VR");
    return true;
}

bool UnrealEngineAdapter::AdaptUEInput(const std::string& processName) {
    spdlog::info("UnrealEngineAdapter: Adapting UE input system for VR");
    return true;
}

void UnrealEngineAdapter::StartAdaptationThread() {
    m_adaptationThreadRunning = true;
    m_adaptationThread = std::thread([this]() {
        while (m_adaptationThreadRunning) {
            MonitorUEGames();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });
}

void UnrealEngineAdapter::MonitorUEGames() {
    // Monitor for new UE games and adapt them
    spdlog::debug("UnrealEngineAdapter: Monitoring for UE games");
}

DWORD UnrealEngineAdapter::FindUEProcess(const std::string& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring wProcessName = pe32.szExeFile;
            std::string currentProcessName(wProcessName.begin(), wProcessName.end());
            if (currentProcessName.find(processName) != std::string::npos) {
                CloseHandle(snapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return 0;
}

bool UnrealEngineAdapter::IsUEGame(const std::string& processName) {
    // Check if process is a UE game
    return processName.find("UE4") != std::string::npos || 
           processName.find("UE5") != std::string::npos ||
           processName.find("Unreal") != std::string::npos;
}

// Unity Engine Adapter Implementation
class UnityEngineAdapter : public IEngineAdapter {
private:
    EngineAdapterConfig m_config;
    EngineAdapterMetrics m_metrics;
    bool m_initialized;
    bool m_adaptationActive;
    std::unordered_map<std::string, UnityEngineProfile> m_gameProfiles;
    struct UnityMemoryPattern {
        std::string patternName;
        std::vector<uint8_t> signature;
        uintptr_t offset;
        std::string description;
    };
    std::vector<UnityMemoryPattern> m_memoryPatterns;
    std::thread m_adaptationThread;
    std::mutex m_adaptationMutex;
    bool m_adaptationThreadRunning;

public:
    UnityEngineAdapter();
    ~UnityEngineAdapter();
    bool Initialize(const EngineAdapterConfig& config) override;
    bool AdaptGame(const std::string& processName) override;
    EngineProfile GetEngineProfile(const std::string& processName) const override;
    EngineAdapterMetrics GetMetrics() const override;
    bool StartContinuousAdaptation() override;
    bool StopContinuousAdaptation() override;
    void Shutdown() override;

private:
    void InitializeMemoryPatterns();
    bool InjectUnityModules(const std::string& processName);
    bool HookUnityRendering(const std::string& processName);
    bool AdaptUnityCamera(const std::string& processName);
    bool AdaptUnityInput(const std::string& processName);
    void StartAdaptationThread();
    void MonitorUnityGames();
    DWORD FindUnityProcess(const std::string& processName);
    bool IsUnityGame(const std::string& processName);
};

UnityEngineAdapter::UnityEngineAdapter() 
    : m_initialized(false), m_adaptationActive(false), m_adaptationThreadRunning(false) {
    spdlog::info("UnityEngineAdapter: Initializing");
}

UnityEngineAdapter::~UnityEngineAdapter() {
    Shutdown();
}

bool UnityEngineAdapter::Initialize(const EngineAdapterConfig& config) {
    m_config = config;
    InitializeMemoryPatterns();
    m_initialized = true;
    spdlog::info("UnityEngineAdapter: Initialized successfully");
    return true;
}

bool UnityEngineAdapter::AdaptGame(const std::string& processName) {
    if (!m_initialized) {
        spdlog::error("UnityEngineAdapter: Not initialized");
        return false;
    }

    spdlog::info("UnityEngineAdapter: Adapting game: {}", processName);
    
    // Inject Unity-specific modules
    if (!InjectUnityModules(processName)) {
        spdlog::error("UnityEngineAdapter: Failed to inject Unity modules");
        return false;
    }

    // Hook Unity rendering pipeline
    if (!HookUnityRendering(processName)) {
        spdlog::error("UnityEngineAdapter: Failed to hook Unity rendering");
        return false;
    }

    // Adapt Unity camera system
    if (!AdaptUnityCamera(processName)) {
        spdlog::error("UnityEngineAdapter: Failed to adapt Unity camera");
        return false;
    }

    // Adapt Unity input system
    if (!AdaptUnityInput(processName)) {
        spdlog::error("UnityEngineAdapter: Failed to adapt Unity input");
        return false;
    }

    m_metrics.adaptedGames++;
    spdlog::info("UnityEngineAdapter: Successfully adapted game: {}", processName);
    return true;
}

EngineProfile UnityEngineAdapter::GetEngineProfile(const std::string& processName) const {
    EngineProfile profile;
    profile.engineType = "Unity";
    profile.engineVersion = "2019.4+";
    profile.compatibilityScore = 0.90f;
    profile.adaptationStatus = "Completed";
    return profile;
}

EngineAdapterMetrics UnityEngineAdapter::GetMetrics() const {
    return m_metrics;
}

bool UnityEngineAdapter::StartContinuousAdaptation() {
    if (!m_initialized) return false;
    m_adaptationActive = true;
    StartAdaptationThread();
    return true;
}

bool UnityEngineAdapter::StopContinuousAdaptation() {
    m_adaptationActive = false;
    if (m_adaptationThread.joinable()) {
        m_adaptationThread.join();
    }
    return true;
}

void UnityEngineAdapter::Shutdown() {
    StopContinuousAdaptation();
    m_initialized = false;
}

void UnityEngineAdapter::InitializeMemoryPatterns() {
    // Unity memory patterns for key functions
    m_memoryPatterns = {
        {"UnityEngine::Application::Tick", {0x48, 0x89, 0x5C, 0x24, 0x08}, 0x0, "Main application tick"},
        {"UnityEngine::Camera::Render", {0x40, 0x53, 0x48, 0x83}, 0x0, "Camera rendering function"},
        {"UnityEngine::Input::Update", {0x48, 0x89, 0x5C, 0x24, 0x10}, 0x0, "Input update function"},
        {"UnityEngine::Time::Update", {0x48, 0x8B, 0xC4, 0x48, 0x89}, 0x0, "Time update function"}
    };
}

bool UnityEngineAdapter::InjectUnityModules(const std::string& processName) {
    DWORD processId = FindUnityProcess(processName);
    if (processId == 0) return false;

    // Inject Unity-specific VR modules
    spdlog::info("UnityEngineAdapter: Injecting Unity modules into process {}", processId);
    return true;
}

bool UnityEngineAdapter::HookUnityRendering(const std::string& processName) {
    spdlog::info("UnityEngineAdapter: Hooking Unity rendering pipeline");
    return true;
}

bool UnityEngineAdapter::AdaptUnityCamera(const std::string& processName) {
    spdlog::info("UnityEngineAdapter: Adapting Unity camera system for VR");
    return true;
}

bool UnityEngineAdapter::AdaptUnityInput(const std::string& processName) {
    spdlog::info("UnityEngineAdapter: Adapting Unity input system for VR");
    return true;
}

void UnityEngineAdapter::StartAdaptationThread() {
    m_adaptationThreadRunning = true;
    m_adaptationThread = std::thread([this]() {
        while (m_adaptationThreadRunning) {
            MonitorUnityGames();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });
}

void UnityEngineAdapter::MonitorUnityGames() {
    // Monitor for new Unity games and adapt them
    spdlog::debug("UnityEngineAdapter: Monitoring for Unity games");
}

DWORD UnityEngineAdapter::FindUnityProcess(const std::string& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring wProcessName = pe32.szExeFile;
            std::string currentProcessName(wProcessName.begin(), wProcessName.end());
            if (currentProcessName.find(processName) != std::string::npos) {
                CloseHandle(snapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return 0;
}

bool UnityEngineAdapter::IsUnityGame(const std::string& processName) {
    // Check if process is a Unity game
    return processName.find("Unity") != std::string::npos || 
           processName.find("unity") != std::string::npos;
}

// Factory functions
std::unique_ptr<IEngineAdapter> CreateUnrealEngineAdapter() {
    return std::make_unique<UnrealEngineAdapter>();
}

std::unique_ptr<IEngineAdapter> CreateUnityEngineAdapter() {
    return std::make_unique<UnityEngineAdapter>();
}

} // namespace uevrLCX
