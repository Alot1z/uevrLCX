#include "UniversalEngineDetector.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace uevr {

UniversalEngineDetector::UniversalEngineDetector() {
    initializeDetectionStrategies();
    loadGameDatabase();
}

void UniversalEngineDetector::initializeDetectionStrategies() {
    // REDengine 4 detection strategy
    m_detectionStrategies[EngineType::REDENGINE_4] = {
        "REDengine 4",
        {"Cyberpunk2077.exe", "cp2077.exe", "witcher3.exe"},
        {"REDengine4.dll", "REDengine4_x64.dll", "REDengine4_x86.dll"},
        {"SOFTWARE\\CD Projekt RED\\REDengine4"},
        {"REDengine4", "REDengine4_x64", "REDengine4_x86"},
        [this](const std::string& processName) { return detectREDengine4(processName); }
    };

    // RE Engine detection strategy
    m_detectionStrategies[EngineType::RE_ENGINE] = {
        "RE Engine",
        {"re4.exe", "re8.exe", "mhw.exe", "mhr.exe", "dmc5.exe", "sf6.exe", "dd2.exe"},
        {"REEngine.dll", "REEngine_x64.dll", "REEngine_x86.dll"},
        {"SOFTWARE\\Capcom\\RE Engine"},
        {"REEngine", "REEngine_x64", "REEngine_x86"},
        [this](const std::string& processName) { return detectREEngine(processName); }
    };

    // Unreal Engine 4 detection strategy
    m_detectionStrategies[EngineType::UNREAL_ENGINE_4] = {
        "Unreal Engine 4",
        {"FortniteClient-Win64-Shipping.exe", "Gears5.exe", "Borderlands3.exe"},
        {"UE4Game.dll", "UE4Editor.dll", "Engine.dll"},
        {"SOFTWARE\\Epic Games\\Unreal Engine\\4.0"},
        {"UE4", "UnrealEngine4"},
        [this](const std::string& processName) { return detectUnrealEngine(processName); }
    };

    // Unreal Engine 5 detection strategy
    m_detectionStrategies[EngineType::UNREAL_ENGINE_5] = {
        "Unreal Engine 5",
        {"FortniteClient-Win64-Shipping.exe", "Gears6.exe"},
        {"UE5Game.dll", "UE5Editor.dll", "Engine.dll"},
        {"SOFTWARE\\Epic Games\\Unreal Engine\\5.0"},
        {"UE5", "UnrealEngine5"},
        [this](const std::string& processName) { return detectUnrealEngine(processName); }
    };

    // Unity detection strategy
    m_detectionStrategies[EngineType::UNITY] = {
        "Unity",
        {"Ori.exe", "Cuphead.exe", "Cities.exe"},
        {"UnityPlayer.dll", "libunity.dll", "GameAssembly.dll"},
        {"SOFTWARE\\Unity Technologies\\Unity"},
        {"Unity", "UnityPlayer"},
        [this](const std::string& processName) { return detectUnity(processName); }
    };

    // CryEngine detection strategy
    m_detectionStrategies[EngineType::CRYENGINE] = {
        "CryEngine",
        {"Hunt.exe", "KingdomCome.exe", "Crysis.exe"},
        {"CrySystem.dll", "CryRenderD3D11.dll", "CryRenderD3D12.dll"},
        {"SOFTWARE\\Crytek\\CryEngine"},
        {"CryEngine", "CrySystem"},
        [this](const std::string& processName) { return detectCryEngine(processName); }
    };

    // id Tech detection strategy
    m_detectionStrategies[EngineType::ID_TECH] = {
        "id Tech",
        {"DOOM.exe", "DOOMEternal.exe", "Wolfenstein.exe"},
        {"idTech.dll", "idRenderSystem.dll", "idGame.dll"},
        {"SOFTWARE\\id Software\\id Tech"},
        {"idTech", "idRenderSystem"},
        [this](const std::string& processName) { return detectIdTech(processName); }
    };

    // Source Engine detection strategy
    m_detectionStrategies[EngineType::SOURCE_ENGINE] = {
        "Source Engine",
        {"hl2.exe", "csgo.exe", "dota2.exe"},
        {"engine.dll", "client.dll", "server.dll"},
        {"SOFTWARE\\Valve\\Source"},
        {"Source", "engine"},
        [this](const std::string& processName) { return detectSourceEngine(processName); }
    };

    // Source 2 detection strategy
    m_detectionStrategies[EngineType::SOURCE_2] = {
        "Source 2",
        {"hlvr.exe", "cs2.exe", "dota2.exe"},
        {"engine2.dll", "client2.dll", "server2.dll"},
        {"SOFTWARE\\Valve\\Source2"},
        {"Source2", "engine2"},
        [this](const std::string& processName) { return detectSourceEngine(processName); }
    };

    // MT Framework detection strategy
    m_detectionStrategies[EngineType::MT_FRAMEWORK] = {
        "MT Framework",
        {"mhw.exe", "mhr.exe", "dd.exe"},
        {"MTFramework.dll", "MTFramework_x64.dll"},
        {"SOFTWARE\\Capcom\\MT Framework"},
        {"MTFramework", "MTFramework_x64"},
        [this](const std::string& processName) { return detectMTFramework(processName); }
    };
}

EngineType UniversalEngineDetector::detectEngine(const std::string& processName) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "[UniversalEngineDetector] Detecting engine for process: " << processName << std::endl;
    
    // Try each detection strategy
    for (const auto& [engineType, strategy] : m_detectionStrategies) {
        if (strategy.customDetector && strategy.customDetector(processName)) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            
            m_metrics.totalGamesScanned++;
            m_metrics.enginesDetected++;
            m_metrics.averageDetectionTime = 
                (m_metrics.averageDetectionTime * (m_metrics.totalGamesScanned - 1) + duration.count()) / 
                m_metrics.totalGamesScanned;
            
            std::cout << "[UniversalEngineDetector] Detected " << strategy.engineName 
                      << " in " << duration.count() << " μs" << std::endl;
            
            return engineType;
        }
    }
    
    m_metrics.totalGamesScanned++;
    std::cout << "[UniversalEngineDetector] Unknown engine for process: " << processName << std::endl;
    return EngineType::UNKNOWN;
}

std::vector<UniversalEngineDetector::GameInfo> UniversalEngineDetector::scanRunningGames() {
    std::vector<GameInfo> runningGames;
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        m_metrics.detectionErrors.push_back("Failed to create process snapshot");
        return runningGames;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring processName = pe32.szExeFile;
            std::string processNameStr(processName.begin(), processName.end());
            
            // Skip system processes
            if (processNameStr.find("System") != std::string::npos ||
                processNameStr.find("svchost") != std::string::npos ||
                processNameStr.find("explorer") != std::string::npos) {
                continue;
            }
            
            EngineType engineType = detectEngine(processNameStr);
            if (engineType != EngineType::UNKNOWN) {
                GameInfo gameInfo;
                gameInfo.executableName = processNameStr;
                gameInfo.displayName = processNameStr.substr(0, processNameStr.find_last_of('.'));
                gameInfo.engineType = engineType;
                gameInfo.engineVersion = getEngineVersionFromProcess(pe32.th32ProcessID, engineType);
                gameInfo.isVRReady = true; // Assume VR ready for detected engines
                
                // Add engine-specific VR features
                switch (engineType) {
                    case EngineType::REDENGINE_4:
                        gameInfo.supportedVRFeatures = "Neural frame interpolation, Temporal upsampling, Ray-tracing fallback";
                        break;
                    case EngineType::RE_ENGINE:
                        gameInfo.supportedVRFeatures = "Dynamic FOV, Motion-to-photon optimization, Haptic feedback";
                        break;
                    case EngineType::UNREAL_ENGINE_4:
                    case EngineType::UNREAL_ENGINE_5:
                        gameInfo.supportedVRFeatures = "Blueprint injection, Scalable LOD, Cross-platform compatibility";
                        break;
                    case EngineType::UNITY:
                        gameInfo.supportedVRFeatures = "Component-based VR, Universal shader replacement";
                        break;
                    case EngineType::CRYENGINE:
                        gameInfo.supportedVRFeatures = "Advanced lighting, Vegetation optimization";
                        break;
                    case EngineType::ID_TECH:
                        gameInfo.supportedVRFeatures = "High-performance rendering, Advanced culling";
                        break;
                    case EngineType::SOURCE_ENGINE:
                    case EngineType::SOURCE_2:
                        gameInfo.supportedVRFeatures = "Legacy compatibility, Modern VR integration";
                        break;
                    case EngineType::MT_FRAMEWORK:
                        gameInfo.supportedVRFeatures = "Legacy engine support, Basic VR hooks";
                        break;
                    default:
                        gameInfo.supportedVRFeatures = "Unknown";
                        break;
                }
                
                runningGames.push_back(gameInfo);
                
                if (gameInfo.isVRReady) {
                    m_metrics.vrReadyGames++;
                }
            }
            
        } while (Process32NextW(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    
    std::cout << "[UniversalEngineDetector] Found " << runningGames.size() << " games with supported engines" << std::endl;
    return runningGames;
}

// Engine-specific detection implementations
bool UniversalEngineDetector::detectREDengine4(const std::string& processName) {
    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
    
    // Check process name
    if (processName.find("cyberpunk") != std::string::npos ||
        processName.find("cp2077") != std::string::npos ||
        processName.find("witcher") != std::string::npos) {
        return true;
    }
    
    // Check for REDengine modules
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (processHandle) {
        std::vector<std::string> modules = {"REDengine4.dll", "REDengine4_x64.dll"};
        if (scanProcessModules(processHandle, modules)) {
            CloseHandle(processHandle);
            return true;
        }
        CloseHandle(processHandle);
    }
    
    return false;
}

bool UniversalEngineDetector::detectREEngine(const std::string& processName) {
    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
    
    // Check process name
    if (processName.find("re") != std::string::npos ||
        processName.find("resident") != std::string::npos ||
        processName.find("mhw") != std::string::npos ||
        processName.find("monster") != std::string::npos ||
        processName.find("dmc") != std::string::npos ||
        processName.find("sf") != std::string::npos ||
        processName.find("dragon") != std::string::npos) {
        return true;
    }
    
    // Check for RE Engine modules
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (processHandle) {
        std::vector<std::string> modules = {"REEngine.dll", "REEngine_x64.dll"};
        if (scanProcessModules(processHandle, modules)) {
            CloseHandle(processHandle);
            return true;
        }
        CloseHandle(processHandle);
    }
    
    return false;
}

bool UniversalEngineDetector::detectUnrealEngine(const std::string& processName) {
    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
    
    // Check process name
    if (processName.find("fortnite") != std::string::npos ||
        processName.find("gears") != std::string::npos ||
        processName.find("borderlands") != std::string::npos) {
        return true;
    }
    
    // Check for Unreal Engine modules
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (processHandle) {
        std::vector<std::string> modules = {"UE4Game.dll", "UE5Game.dll", "Engine.dll"};
        if (scanProcessModules(processHandle, modules)) {
            CloseHandle(processHandle);
            return true;
        }
        CloseHandle(processHandle);
    }
    
    return false;
}

bool UniversalEngineDetector::detectUnity(const std::string& processName) {
    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
    
    // Check process name
    if (processName.find("ori") != std::string::npos ||
        processName.find("cuphead") != std::string::npos ||
        processName.find("cities") != std::string::npos) {
        return true;
    }
    
    // Check for Unity modules
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (processHandle) {
        std::vector<std::string> modules = {"UnityPlayer.dll", "GameAssembly.dll"};
        if (scanProcessModules(processHandle, modules)) {
            CloseHandle(processHandle);
            return true;
        }
        CloseHandle(processHandle);
    }
    
    return false;
}

bool UniversalEngineDetector::detectCryEngine(const std::string& processName) {
    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
    
    // Check process name
    if (processName.find("hunt") != std::string::npos ||
        processName.find("kingdom") != std::string::npos ||
        processName.find("crysis") != std::string::npos) {
        return true;
    }
    
    // Check for CryEngine modules
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (processHandle) {
        std::vector<std::string> modules = {"CrySystem.dll", "CryRenderD3D11.dll"};
        if (scanProcessModules(processHandle, modules)) {
            CloseHandle(processHandle);
            return true;
        }
        CloseHandle(processHandle);
    }
    
    return false;
}

bool UniversalEngineDetector::detectIdTech(const std::string& processName) {
    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
    
    // Check process name
    if (processName.find("doom") != std::string::npos ||
        processName.find("wolfenstein") != std::string::npos) {
        return true;
    }
    
    // Check for id Tech modules
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (processHandle) {
        std::vector<std::string> modules = {"idTech.dll", "idRenderSystem.dll"};
        if (scanProcessModules(processHandle, modules)) {
            CloseHandle(processHandle);
            return true;
        }
        CloseHandle(processHandle);
    }
    
    return false;
}

bool UniversalEngineDetector::detectSourceEngine(const std::string& processName) {
    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
    
    // Check process name
    if (processName.find("hl") != std::string::npos ||
        processName.find("cs") != std::string::npos ||
        processName.find("dota") != std::string::npos) {
        return true;
    }
    
    // Check for Source Engine modules
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (processHandle) {
        std::vector<std::string> modules = {"engine.dll", "client.dll", "engine2.dll"};
        if (scanProcessModules(processHandle, modules)) {
            CloseHandle(processHandle);
            return true;
        }
        CloseHandle(processHandle);
    }
    
    return false;
}

bool UniversalEngineDetector::detectMTFramework(const std::string& processName) {
    std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
    
    // Check process name
    if (processName.find("mhw") != std::string::npos ||
        processName.find("mhr") != std::string::npos ||
        processName.find("dd") != std::string::npos) {
        return true;
    }
    
    // Check for MT Framework modules
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (processHandle) {
        std::vector<std::string> modules = {"MTFramework.dll", "MTFramework_x64.dll"};
        if (scanProcessModules(processHandle, modules)) {
            CloseHandle(processHandle);
            return true;
        }
        CloseHandle(processHandle);
    }
    
    return false;
}

// Helper methods
bool UniversalEngineDetector::scanProcessModules(HANDLE processHandle, const std::vector<std::string>& targetModules) {
    HMODULE modules[1024];
    DWORD cbNeeded;
    
    if (EnumProcessModules(processHandle, modules, sizeof(modules), &cbNeeded)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR moduleName[MAX_PATH];
            if (GetModuleBaseName(processHandle, modules[i], moduleName, sizeof(moduleName) / sizeof(TCHAR))) {
                std::wstring moduleNameW = moduleName;
                std::string moduleNameStr(moduleNameW.begin(), moduleNameW.end());
                
                for (const auto& targetModule : targetModules) {
                    if (moduleNameStr.find(targetModule) != std::string::npos) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

std::string UniversalEngineDetector::getEngineVersionFromProcess(HANDLE processHandle, EngineType engineType) {
    // This is a simplified version - in reality you'd extract version info from modules
    switch (engineType) {
        case EngineType::REDENGINE_4:
            return "4.0";
        case EngineType::RE_ENGINE:
            return "2.0";
        case EngineType::UNREAL_ENGINE_4:
            return "4.27";
        case EngineType::UNREAL_ENGINE_5:
            return "5.3";
        case EngineType::UNITY:
            return "2022.3 LTS";
        case EngineType::CRYENGINE:
            return "5.7";
        case EngineType::ID_TECH:
            return "7.0";
        case EngineType::SOURCE_ENGINE:
            return "1.0";
        case EngineType::SOURCE_2:
            return "2.0";
        case EngineType::MT_FRAMEWORK:
            return "1.0";
        default:
            return "Unknown";
    }
}

void UniversalEngineDetector::loadGameDatabase() {
    // Load pre-configured game database
    // In a real implementation, this would load from a JSON/XML file
    std::cout << "[UniversalEngineDetector] Loading game database..." << std::endl;
    
    // Add some known games
    GameInfo cyberpunk = {
        "Cyberpunk2077.exe",
        "Cyberpunk 2077",
        EngineType::REDENGINE_4,
        "4.0",
        "Neural frame interpolation, Temporal upsampling, Ray-tracing fallback",
        true,
        {"REDengine4.dll", "REDscript.dll"}
    };
    m_gameDatabase.push_back(cyberpunk);
    
    GameInfo re4 = {
        "re4.exe",
        "Resident Evil 4 Remake",
        EngineType::RE_ENGINE,
        "2.0",
        "Dynamic FOV, Motion-to-photon optimization, Haptic feedback",
        true,
        {"REEngine.dll", "REEngine_x64.dll"}
    };
    m_gameDatabase.push_back(re4);
    
    std::cout << "[UniversalEngineDetector] Loaded " << m_gameDatabase.size() << " games" << std::endl;
}

std::unique_ptr<class ICrossEngineAdapter> UniversalEngineDetector::createAdapterForGame(const GameInfo& gameInfo) {
    // This would create the appropriate adapter based on engine type
    // For now, return nullptr as the adapter creation is handled elsewhere
    std::cout << "[UniversalEngineDetector] Creating adapter for " << gameInfo.displayName 
              << " (Engine: " << static_cast<int>(gameInfo.engineType) << ")" << std::endl;
    
    return nullptr;
}

UniversalEngineDetector::DetectionMetrics UniversalEngineDetector::getDetectionMetrics() const {
    return m_metrics;
}

} // namespace uevr
